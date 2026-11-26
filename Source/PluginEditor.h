#pragma once
#include "PluginProcessor.h"

class KnobLookAndFeel : public juce::LookAndFeel_V4 {
public:
  KnobLookAndFeel();
  void drawRotarySlider(juce::Graphics&, int x, int y, int w, int h, float pos, float start, float end, juce::Slider&) override;
};

class SubKingEditor : public juce::AudioProcessorEditor {
public:
  explicit SubKingEditor(SubKingProcessor&);
  ~SubKingEditor() override;
  void paint(juce::Graphics&) override;
  void resized() override;

private:
  struct Cell {
    int index = 0;
    std::unique_ptr<juce::Slider> slider;
    std::unique_ptr<juce::ComboBox> combo;
    std::unique_ptr<juce::ToggleButton> toggle;
    std::unique_ptr<juce::Label> label;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> sa;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ComboBoxAttachment> ca;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> ba;
  };
  struct Section { juce::String title; std::vector<int> ids; juce::Rectangle<int> bounds; int row; };

  SubKingProcessor& proc;
  KnobLookAndFeel lnf;
  std::vector<Cell> cells;
  std::vector<Section> sections;
  juce::ComboBox presetBox;
  juce::Label presetLabel;
  void makeCell(int paramIndex);
  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SubKingEditor)
};
