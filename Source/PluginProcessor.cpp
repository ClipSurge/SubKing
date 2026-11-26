#include "PluginProcessor.h"
#include "PluginEditor.h"

juce::AudioProcessorValueTreeState::ParameterLayout SubKingProcessor::createLayout() {
  using namespace juce;
  AudioProcessorValueTreeState::ParameterLayout layout;
  for (int i = 0; i < sk::P_COUNT; ++i) {
    const auto& d = sk::kDefs[i];
    String name = String(d.grp) + " " + d.name;
    ParameterID pid { d.id, 1 };
    switch (d.kind) {
      case sk::K_FLOAT: {
        int dec = d.dec; String unit = d.unit;
        layout.add(std::make_unique<AudioParameterFloat>(pid, name,
          NormalisableRange<float>(d.min, d.max, 0.f, d.skew), d.def,
          AudioParameterFloatAttributes().withStringFromValueFunction([dec, unit](float v, int) {
            return String(v, dec) + (unit.isEmpty() ? String() : " " + unit); })));
        break; }
      case sk::K_CHOICE:
        layout.add(std::make_unique<AudioParameterChoice>(pid, name, StringArray::fromTokens(d.choices, "|", ""), (int) d.def));
        break;
      case sk::K_BOOL:
        layout.add(std::make_unique<AudioParameterBool>(pid, name, d.def > 0.5f));
        break;
      default:
        layout.add(std::make_unique<AudioParameterInt>(pid, name, (int) d.min, (int) d.max, (int) d.def));
        break;
    }
  }
  return layout;
}

SubKingProcessor::SubKingProcessor()
  : AudioProcessor(BusesProperties().withOutput("Output", juce::AudioChannelSet::stereo(), true)),
    apvts(*this, nullptr, "STATE", createLayout()) {
  for (int i = 0; i < sk::P_COUNT; ++i) raw[i] = apvts.getRawParameterValue(sk::kDefs[i].id);
}

void SubKingProcessor::prepareToPlay(double sampleRate, int) { engine.prepare(sampleRate); }

bool SubKingProcessor::isBusesLayoutSupported(const BusesLayout& l) const {
  return l.getMainOutputChannelSet() == juce::AudioChannelSet::stereo()
      || l.getMainOutputChannelSet() == juce::AudioChannelSet::mono();
}

void SubKingProcessor::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midi) {
  juce::ScopedNoDenormals noDenormals;
  buffer.clear();
  const int n = buffer.getNumSamples();

  float p[sk::P_COUNT];
  for (int i = 0; i < sk::P_COUNT; ++i) p[i] = raw[i]->load();

  double bpm = 120.0;
  if (auto* ph = getPlayHead())
    if (auto pos = ph->getPosition())
      if (auto b = pos->getBpm()) bpm = *b;
  static const double cyclesPerBeat[] = { 0.0, 0.25, 0.5, 1.0, 2.0, 3.0, 4.0 };
  int sync = juce::jlimit(0, 6, (int) p[sk::P_LFOSYNC]);
  if (sync > 0) p[sk::P_LFORATE] = (float) (bpm / 60.0 * cyclesPerBeat[sync]);
  engine.setParams(p);

  float* L = buffer.getWritePointer(0);
  int pos = 0;
  for (const auto meta : midi) {
    int t = juce::jlimit(0, n, meta.samplePosition);
    if (t > pos) { engine.render(L + pos, t - pos); pos = t; }
    auto m = meta.getMessage();
    if (m.isNoteOn()) engine.noteOn(m.getNoteNumber(), m.getFloatVelocity());
    else if (m.isNoteOff()) engine.noteOff(m.getNoteNumber());
    else if (m.isAllNotesOff() || m.isAllSoundOff()) engine.allNotesOff();
    else if (m.isPitchWheel()) engine.setBend((m.getPitchWheelValue() - 8192) / 8192.f * 2.f);
  }
  if (pos < n) engine.render(L + pos, n - pos);
  for (int ch = 1; ch < buffer.getNumChannels(); ++ch) buffer.copyFrom(ch, 0, L, n);
}

void SubKingProcessor::setCurrentProgram(int index) {
  index = juce::jlimit(0, getNumPrograms() - 1, index);
  currentProgram = index;
  float vals[sk::P_COUNT];
  for (int i = 0; i < sk::P_COUNT; ++i) vals[i] = sk::kDefs[i].def;
  for (auto& kv : sk::presets()[(size_t) index].v) vals[kv.first] = kv.second;
  for (int i = 0; i < sk::P_COUNT; ++i)
    if (auto* prm = apvts.getParameter(sk::kDefs[i].id)) prm->setValueNotifyingHost(prm->convertTo0to1(vals[i]));
}

void SubKingProcessor::getStateInformation(juce::MemoryBlock& dest) {
  auto state = apvts.copyState();
  state.setProperty("program", currentProgram, nullptr);
  if (auto xml = state.createXml()) copyXmlToBinary(*xml, dest);
}

void SubKingProcessor::setStateInformation(const void* data, int size) {
  if (auto xml = getXmlFromBinary(data, size))
    if (xml->hasTagName(apvts.state.getType())) {
      auto st = juce::ValueTree::fromXml(*xml);
      currentProgram = juce::jlimit(0, getNumPrograms() - 1, (int) st.getProperty("program", 0));
      apvts.replaceState(st);
    }
}

juce::AudioProcessorEditor* SubKingProcessor::createEditor() { return new SubKingEditor(*this); }
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter() { return new SubKingProcessor(); }
