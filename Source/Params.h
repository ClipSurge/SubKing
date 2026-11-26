#pragma once
// Single source of truth for all parameters (plain C++, no JUCE dependency).
namespace sk {
enum Kind { K_FLOAT = 0, K_CHOICE = 1, K_BOOL = 2, K_INT = 3 };
struct Def { const char *id, *grp, *name; int kind; float min, max, def, skew; const char *choices, *unit; int dec; };

#define WAVES "Saw|Square|Triangle|Sine"
// X(enum, id, group, label, kind, min, max, default, skew, choices, unit, decimals)
#define SK_PARAMS(X) \
 X(O1WAVE,  "o1wave", "Osc1", "Wave",    1, 0, 3, 0,     1,    WAVES, "", 0) \
 X(O1LVL,   "o1lvl",  "Osc1", "Level",   0, 0, 1, 0.8f,  1,    "", "", 2) \
 X(O2WAVE,  "o2wave", "Osc2", "Wave",    1, 0, 3, 0,     1,    WAVES, "", 0) \
 X(O2OCT,   "o2oct",  "Osc2", "Octave",  3, -2, 2, 0,    1,    "", "", 0) \
 X(O2DET,   "o2det",  "Osc2", "Detune",  0, -100, 100, 8, 1,   "", "ct", 1) \
 X(O2LVL,   "o2lvl",  "Osc2", "Level",   0, 0, 1, 0.6f,  1,    "", "", 2) \
 X(SUBWAVE, "subwave","Sub",  "Wave",    1, 0, 1, 0,     1,    "Sine|Square", "", 0) \
 X(SUBOCT,  "suboct", "Sub",  "Octave",  1, 0, 1, 0,     1,    "-1 Oct|-2 Oct", "", 0) \
 X(SUBLVL,  "sublvl", "Sub",  "Sub",     0, 0, 1, 0.8f,  1,    "", "", 2) \
 X(NOISE,   "noise",  "Noise","Noise",   0, 0, 1, 0,     1,    "", "", 2) \
 X(GLIDE,   "glide",  "Voice","Glide",   0, 0, 1000, 30, 0.4f, "", "ms", 0) \
 X(LEGATO,  "legato", "Voice","Legato",  2, 0, 1, 1,     1,    "", "", 0) \
 X(CUTOFF,  "cutoff", "Filter","Cutoff", 0, 20, 20000, 900, 0.3f, "", "Hz", 0) \
 X(RES,     "res",    "Filter","Reso",   0, 0, 1, 0.2f,  1,    "", "", 2) \
 X(FDRIVE,  "fdrive", "Filter","Drive",  0, 0, 1, 0.2f,  1,    "", "", 2) \
 X(FENV,    "fenv",   "Filter","Env Amt",0, -1, 1, 0.4f, 1,    "", "", 2) \
 X(KTRACK,  "ktrack", "Filter","Key Trk",0, 0, 1, 0.3f,  1,    "", "", 2) \
 X(FA,      "fa",     "FEnv",  "Atk",    0, 0.5f, 2000, 1,    0.35f, "", "ms", 1) \
 X(FD,      "fd",     "FEnv",  "Dec",    0, 5, 5000, 250,     0.35f, "", "ms", 0) \
 X(FS,      "fs",     "FEnv",  "Sus",    0, 0, 1, 0.3f,  1,    "", "", 2) \
 X(FR,      "fr",     "FEnv",  "Rel",    0, 5, 5000, 150,     0.35f, "", "ms", 0) \
 X(AA,      "aa",     "AEnv",  "Atk",    0, 0.5f, 2000, 2,    0.35f, "", "ms", 1) \
 X(AD,      "ad",     "AEnv",  "Dec",    0, 5, 5000, 300,     0.35f, "", "ms", 0) \
 X(AS,      "as",     "AEnv",  "Sus",    0, 0, 1, 0.9f,  1,    "", "", 2) \
 X(AR,      "ar",     "AEnv",  "Rel",    0, 5, 5000, 120,     0.35f, "", "ms", 0) \
 X(LFOSHAPE,"lfoshape","LFO",  "Shape",  1, 0, 3, 0,     1,    "Sine|Triangle|Saw|Square", "", 0) \
 X(LFORATE, "lforate","LFO",   "Rate",   0, 0.05f, 30, 2, 0.4f, "", "Hz", 2) \
 X(LFOSYNC, "lfosync","LFO",   "Sync",   1, 0, 6, 0,     1,    "Free|1/1|1/2|1/4|1/8|1/8T|1/16", "", 0) \
 X(LFOCUT,  "lfocut", "LFO",   "To Cutoff",0, 0, 1, 0,   1,    "", "", 2) \
 X(LFOPITCH,"lfopitch","LFO",  "To Pitch",0, 0, 100, 0,  1,    "", "ct", 0) \
 X(LFORETRIG,"lforetrig","LFO","Retrig", 2, 0, 1, 0,     1,    "", "", 0) \
 X(PENV,    "penv",   "PEnv",  "Amount", 0, 0, 48, 0,    1,    "", "st", 1) \
 X(PDEC,    "pdec",   "PEnv",  "Decay",  0, 5, 1000, 60, 0.4f, "", "ms", 0) \
 X(DDRIVE,  "ddrive", "Dist",  "Drive",  0, 0, 1, 0.2f,  1,    "", "", 2) \
 X(DMIX,    "dmix",   "Dist",  "Mix",    0, 0, 1, 0.3f,  1,    "", "", 2) \
 X(DTONE,   "dtone",  "Dist",  "Tone",   0, 500, 20000, 8000, 0.35f, "", "Hz", 0) \
 X(VELSENS, "velsens","Master","Velocity",0, 0, 1, 0.5f, 1,    "", "", 2) \
 X(LEVEL,   "level",  "Master","Volume", 0, -24, 6, 0,   1,    "", "dB", 1)

enum P {
#define X(e,...) P_##e,
  SK_PARAMS(X)
#undef X
  P_COUNT
};
static const Def kDefs[P_COUNT] = {
#define X(e,id,grp,name,kind,mn,mx,df,skew,ch,un,dec) { id,grp,name,kind,(float)(mn),(float)(mx),(float)(df),(float)(skew),ch,un,dec },
  SK_PARAMS(X)
#undef X
};
} // namespace sk
