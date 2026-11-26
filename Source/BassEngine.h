#pragma once
#include "Params.h"
#include <algorithm>
#include <cmath>
#include <cstring>
#include <vector>

namespace sk {
constexpr float kPi = 3.14159265358979f;

inline float polyBlep(float t, float dt) {
  if (t < dt) { t /= dt; return t + t - t * t - 1.f; }
  if (t > 1.f - dt) { t = (t - 1.f) / dt; return t * t + t + t + 1.f; }
  return 0.f;
}

struct Osc {
  double phase = 0.0;
  // wave: 0 saw, 1 square, 2 triangle, 3 sine
  float next(int wave, float freq, float sr) {
    float dt = std::min(freq / sr, 0.45f);
    float t = (float) phase, v;
    switch (wave) {
      case 0:  v = 2.f * t - 1.f; v -= polyBlep(t, dt); break;
      case 1:  v = t < 0.5f ? 1.f : -1.f; v += polyBlep(t, dt); v -= polyBlep(t + 0.5f >= 1.f ? t - 0.5f : t + 0.5f, dt); break;
      case 2:  v = 1.f - 4.f * std::fabs(t - 0.5f); break;
      default: v = std::sin(2.f * kPi * t); break;
    }
    phase += dt; if (phase >= 1.0) phase -= 1.0;
    return v;
  }
};

struct Env {
  enum S { Idle, Att, Dec, Sus, Rel };
  S st = Idle; float level = 0, aInc = 0.01f, dCoef = 0.01f, rCoef = 0.01f, sus = 1.f;
  void set(float a, float d, float s, float r, float sr) {
    aInc = 1.f / std::max(1.f, a * 0.001f * sr);
    dCoef = 1.f - std::exp(-4.6f / std::max(1.f, d * 0.001f * sr));
    rCoef = 1.f - std::exp(-4.6f / std::max(1.f, r * 0.001f * sr));
    sus = s;
  }
  void gateOn() { st = Att; }
  void gateOff() { if (st != Idle) st = Rel; }
  float tick() {
    switch (st) {
      case Att: level += aInc; if (level >= 1.f) { level = 1.f; st = Dec; } break;
      case Dec: level += (sus - level) * dCoef; if (std::fabs(level - sus) < 1e-5f) { level = sus; st = Sus; } break;
      case Sus: level = sus; break;
      case Rel: level -= level * rCoef; if (level < 1e-4f) { level = 0.f; st = Idle; } break;
      default: break;
    }
    return level;
  }
};

struct Ladder {
  float s[4] = {0, 0, 0, 0};
  void reset() { s[0] = s[1] = s[2] = s[3] = 0.f; }
  float process(float x, float g, float k) {
    float G = g / (1.f + g), G2 = G * G, G4 = G2 * G2, beta = 1.f / (1.f + g);
    float S = beta * (G2 * G * s[0] + G2 * s[1] + G * s[2] + s[3]);
    float u = std::tanh((x - k * S) / (1.f + k * G4));
    float y = u;
    for (int i = 0; i < 4; ++i) { float v = (y - s[i]) * G; float yo = v + s[i]; s[i] = yo + v; y = yo; }
    return y;
  }
};

class Engine {
public:
  Engine() { for (int i = 0; i < P_COUNT; ++i) p[i] = kDefs[i].def; prepare(44100.0); }

  void prepare(double sampleRate) {
    sr = (float) sampleRate;
    sm = 1.f - std::exp(-1.f / (0.005f * sr));
    allNotesOff(); ampEnv = Env(); filtEnv = Env(); ladder.reset(); setParams(p);
    cut = p[P_CUTOFF]; res = p[P_RES]; fdr = p[P_FDRIVE]; o1l = p[P_O1LVL]; o2l = p[P_O2LVL]; sbl = p[P_SUBLVL]; nsl = p[P_NOISE];
    gain = std::pow(10.f, p[P_LEVEL] / 20.f);
  }

  void setParams(const float* src) {
    std::memcpy(p, src, sizeof(p));
    ampEnv.set(p[P_AA], p[P_AD], p[P_AS], p[P_AR], sr);
    filtEnv.set(p[P_FA], p[P_FD], p[P_FS], p[P_FR], sr);
    pdCoef = std::exp(-4.6f / std::max(1.f, p[P_PDEC] * 0.001f * sr));
    glideCoef = p[P_GLIDE] < 0.5f ? 1.f : 1.f - std::exp(-4.6f / (p[P_GLIDE] * 0.001f * sr));
    toneA = 1.f - std::exp(-2.f * kPi * std::min(p[P_DTONE], 0.45f * sr) / sr);
  }

  void setBend(float semis) { bend = semis; }

  void noteOn(int note, float vel) {
    bool held = !stack.empty();
    removeNote(note);
    stack.push_back({note, vel});
    tgtNote = (float) note;
    bool legato = p[P_LEGATO] > 0.5f && held;
    if (ampEnv.level < 0.002f) {  // voice silent: hard-set pitch + phases for consistent punch
      curNote = tgtNote; osc1.phase = osc2.phase = subOsc.phase = 0.0; ladder.reset();
    }
    curVel = vel;
    if (!legato) trigger();
  }

  void noteOff(int note) {
    if (stack.empty()) return;
    bool wasTop = stack.back().note == note;
    removeNote(note);
    if (stack.empty()) { ampEnv.gateOff(); filtEnv.gateOff(); return; }
    if (wasTop) {
      tgtNote = (float) stack.back().note; curVel = stack.back().vel;
      if (p[P_LEGATO] < 0.5f) trigger();
    }
  }

  void allNotesOff() { stack.clear(); ampEnv.gateOff(); filtEnv.gateOff(); }

  void render(float* out, int n) {
    const float mixSm = sm;
    for (int i = 0; i < n; ++i) {
      cut += (p[P_CUTOFF] - cut) * mixSm; res += (p[P_RES] - res) * mixSm; fdr += (p[P_FDRIVE] - fdr) * mixSm;
      o1l += (p[P_O1LVL] - o1l) * mixSm; o2l += (p[P_O2LVL] - o2l) * mixSm;
      sbl += (p[P_SUBLVL] - sbl) * mixSm; nsl += (p[P_NOISE] - nsl) * mixSm;
      gain += (std::pow(10.f, p[P_LEVEL] / 20.f) - gain) * mixSm;
      curNote += (tgtNote - curNote) * glideCoef;
      float a = ampEnv.tick(), fe = filtEnv.tick();

      lfoPhase += p[P_LFORATE] / sr; if (lfoPhase >= 1.f) lfoPhase -= 1.f;
      float lfo;
      switch ((int) p[P_LFOSHAPE]) {
        case 0: lfo = std::sin(2.f * kPi * lfoPhase); break;
        case 1: lfo = 1.f - 4.f * std::fabs(lfoPhase - 0.5f); break;
        case 2: lfo = 1.f - 2.f * lfoPhase; break;
        default: lfo = std::tanh(5.f * std::sin(2.f * kPi * lfoPhase)); break;
      }
      pe *= pdCoef;

      if (ampEnv.st == Env::Idle) { out[i] = 0.f; ladder.reset(); dlp = 0.f; continue; }

      float semis = curNote + bend + p[P_PENV] * pe + lfo * p[P_LFOPITCH] * 0.01f;
      float maxF = 0.45f * sr;
      float f1 = std::min(8.175799f * std::exp2(semis / 12.f), maxF);
      float f2 = std::min(8.175799f * std::exp2((semis + p[P_O2OCT] * 12.f + p[P_O2DET] * 0.01f) / 12.f), maxF);
      float fs = f1 * (p[P_SUBOCT] < 0.5f ? 0.5f : 0.25f);

      float s1 = osc1.next((int) p[P_O1WAVE], f1, sr);
      float s2 = osc2.next((int) p[P_O2WAVE], f2, sr);
      float ss = subOsc.next(p[P_SUBWAVE] < 0.5f ? 3 : 1, fs, sr);
      rng = rng * 1664525u + 1013904223u;
      float nz = ((float) (rng >> 8) / 8388608.f) - 1.f;
      float mix = 0.5f * (s1 * o1l + s2 * o2l + ss * sbl + nz * nsl * 0.6f);

      float mod = fe * p[P_FENV] * 6.f + lfo * p[P_LFOCUT] * 4.f
                + (curNote + bend - 48.f) / 12.f * p[P_KTRACK] + (curVel - 0.7f) * p[P_VELSENS] * 2.f;
      float fc = std::min(std::max(cut * std::exp2(mod), 20.f), std::min(20000.f, 0.45f * sr));
      float g = std::tan(kPi * fc / sr);
      float k = res * 4.1f;
      float y = ladder.process(mix * (1.f + fdr * 5.f), g, k) / (1.f + fdr * 1.5f);
      if (!std::isfinite(y)) { ladder.reset(); y = 0.f; }

      float dr = p[P_DDRIVE], dm = p[P_DMIX];
      float w = std::tanh(y * (1.f + dr * 18.f) + 0.2f * dr) - std::tanh(0.2f * dr);
      dlp += toneA * (w - dlp);
      float wet = dlp * (0.55f / (1.f + dr * 0.6f));
      y = y * (1.f - dm) + wet * dm * 1.2f;

      float velGain = (1.f - p[P_VELSENS]) + p[P_VELSENS] * curVel;
      float o = y * a * velGain * gain * 1.15f;
      // DC blocker
      float dc = o - xPrev + 0.995f * yPrev; xPrev = o; yPrev = dc; o = dc;
      // transparent soft clip above 0.8
      float ao = std::fabs(o);
      if (ao > 0.8f) o = (o < 0 ? -1.f : 1.f) * (0.8f + 0.2f * std::tanh((ao - 0.8f) / 0.2f));
      out[i] = o;
    }
  }

private:
  struct N { int note; float vel; };
  void removeNote(int n) { stack.erase(std::remove_if(stack.begin(), stack.end(), [n](const N& x) { return x.note == n; }), stack.end()); }
  void trigger() {
    ampEnv.gateOn(); filtEnv.gateOn(); pe = 1.f;
    if (p[P_LFORETRIG] > 0.5f) lfoPhase = 0.f;
  }

  float p[P_COUNT];
  float sr = 44100.f, sm = 0.01f, pdCoef = 0.99f, glideCoef = 1.f, toneA = 0.5f;
  float cut = 900, res = 0.2f, fdr = 0.2f, o1l = 0, o2l = 0, sbl = 0, nsl = 0, gain = 1.f;
  float curNote = 36, tgtNote = 36, bend = 0, curVel = 0.8f, pe = 0.f, lfoPhase = 0.f;
  float dlp = 0.f, xPrev = 0.f, yPrev = 0.f;
  unsigned rng = 12345u;
  Osc osc1, osc2, subOsc; Env ampEnv, filtEnv; Ladder ladder;
  std::vector<N> stack;
};
} // namespace sk
