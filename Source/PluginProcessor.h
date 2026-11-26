#pragma once
#include <juce_audio_utils/juce_audio_utils.h>
#include "BassEngine.h"
#include "Presets.h"

class SubKingProcessor : public juce::AudioProcessor {
public:
  SubKingProcessor();
  ~SubKingProcessor() override = default;

  void prepareToPlay(double sampleRate, int samplesPerBlock) override;
  void releaseResources() override {}
  bool isBusesLayoutSupported(const BusesLayout& layouts) const override;
  void processBlock(juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

  juce::AudioProcessorEditor* createEditor() override;
  bool hasEditor() const override { return true; }

  const juce::String getName() const override { return "SubKing"; }
  bool acceptsMidi() const override { return true; }
  bool producesMidi() const override { return false; }
  bool isMidiEffect() const override { return false; }
  double getTailLengthSeconds() const override { return 5.0; }

  int getNumPrograms() override { return (int) sk::presets().size(); }
  int getCurrentProgram() override { return currentProgram; }
  void setCurrentProgram(int index) override;
  const juce::String getProgramName(int index) override { return sk::presets()[(size_t) index].name; }
  void changeProgramName(int, const juce::String&) override {}

  void getStateInformation(juce::MemoryBlock& destData) override;
  void setStateInformation(const void* data, int sizeInBytes) override;

  juce::AudioProcessorValueTreeState apvts;

private:
  static juce::AudioProcessorValueTreeState::ParameterLayout createLayout();
  sk::Engine engine;
  std::atomic<float>* raw[sk::P_COUNT] {};
  int currentProgram = 0;
  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SubKingProcessor)
};
