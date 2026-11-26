#include "PluginEditor.h"
using namespace juce;

static const Colour kBg(0xff10131a), kPanel(0xff1a1f29), kAccent(0xffff5a36), kText(0xffc9d1e0), kDim(0xff7a8499);
static constexpr int kCellW = 68, kCellH = 108, kPanelH = 138, kMargin = 14, kHeader = 64;

KnobLookAndFeel::KnobLookAndFeel() {
  setColour(Slider::textBoxTextColourId, kText);
  setColour(Slider::textBoxOutlineColourId, Colours::transparentBlack);
  setColour(Label::textColourId, kText);
  setColour(ComboBox::backgroundColourId, Colour(0xff252b37));
  setColour(ComboBox::textColourId, kText);
  setColour(ComboBox::outlineColourId, Colour(0xff39414f));
  setColour(ComboBox::arrowColourId, kAccent);
  setColour(PopupMenu::backgroundColourId, Colour(0xff1d222c));
  setColour(PopupMenu::textColourId, kText);
  setColour(PopupMenu::highlightedBackgroundColourId, kAccent);
  setColour(ToggleButton::tickColourId, kAccent);
  setColour(ToggleButton::tickDisabledColourId, kDim);
}

void KnobLookAndFeel::drawRotarySlider(Graphics& g, int x, int y, int w, int h, float pos, float start, float end, Slider&) {
  auto b = Rectangle<float>((float) x, (float) y, (float) w, (float) h).reduced(7.f);
  float r = jmin(b.getWidth(), b.getHeight()) * 0.5f;
  auto c = b.getCentre();
  Path track; track.addCentredArc(c.x, c.y, r, r, 0.f, start, end, true);
  g.setColour(Colour(0xff2b3140)); g.strokePath(track, PathStrokeType(4.f, PathStrokeType::curved, PathStrokeType::rounded));
  float ang = start + pos * (end - start);
  Path val; val.addCentredArc(c.x, c.y, r, r, 0.f, start, ang, true);
  g.setColour(kAccent); g.strokePath(val, PathStrokeType(4.f, PathStrokeType::curved, PathStrokeType::rounded));
  float ir = r - 7.f;
  g.setGradientFill(ColourGradient(Colour(0xff343b4b), c.x, c.y - ir, Colour(0xff161a22), c.x, c.y + ir, false));
  g.fillEllipse(c.x - ir, c.y - ir, ir * 2.f, ir * 2.f);
  Path ptr; ptr.addRoundedRectangle(-1.6f, -ir + 2.f, 3.2f, ir * 0.55f, 1.5f);
  g.setColour(Colours::white); g.fillPath(ptr, AffineTransform::rotation(ang).translated(c.x, c.y));
}

void SubKingEditor::makeCell(int i) {
  const auto& d = sk::kDefs[i];
  Cell c; c.index = i;
  c.label = std::make_unique<Label>(String(), d.name);
  c.label->setJustificationType(Justification::centred);
  c.label->setFont(FontOptions(12.f));
  c.label->setColour(Label::textColourId, kDim);
  addAndMakeVisible(*c.label);
  if (d.kind == sk::K_CHOICE) {
    c.combo = std::make_unique<ComboBox>();
    c.combo->addItemList(StringArray::fromTokens(d.choices, "|", ""), 1);
    addAndMakeVisible(*c.combo);
    c.ca = std::make_unique<AudioProcessorValueTreeState::ComboBoxAttachment>(proc.apvts, d.id, *c.combo);
  } else if (d.kind == sk::K_BOOL) {
    c.toggle = std::make_unique<ToggleButton>();
    addAndMakeVisible(*c.toggle);
    c.ba = std::make_unique<AudioProcessorValueTreeState::ButtonAttachment>(proc.apvts, d.id, *c.toggle);
  } else {
    c.slider = std::make_unique<Slider>(Slider::RotaryHorizontalVerticalDrag, Slider::TextBoxBelow);
    c.slider->setTextBoxStyle(Slider::TextBoxBelow, false, kCellW - 4, 15);
    c.slider->setRotaryParameters(MathConstants<float>::pi * 1.2f, MathConstants<float>::pi * 2.8f, true);
    c.slider->setLookAndFeel(&lnf);
    c.slider->setScrollWheelEnabled(false);
    addAndMakeVisible(*c.slider);
    c.sa = std::make_unique<AudioProcessorValueTreeState::SliderAttachment>(proc.apvts, d.id, *c.slider);
  }
  cells.push_back(std::move(c));
}

SubKingEditor::SubKingEditor(SubKingProcessor& p) : AudioProcessorEditor(&p), proc(p) {
  setLookAndFeel(&lnf);
  using namespace sk;
  sections = {
    { "OSC 1", { P_O1WAVE, P_O1LVL }, {}, 0 },
    { "OSC 2", { P_O2WAVE, P_O2OCT, P_O2DET, P_O2LVL }, {}, 0 },
    { "SUB / NOISE", { P_SUBWAVE, P_SUBOCT, P_SUBLVL, P_NOISE }, {}, 0 },
    { "VOICE", { P_GLIDE, P_LEGATO }, {}, 0 },
    { "FILTER", { P_CUTOFF, P_RES, P_FDRIVE, P_FENV, P_KTRACK }, {}, 1 },
    { "FILTER ENV", { P_FA, P_FD, P_FS, P_FR }, {}, 1 },
    { "AMP ENV", { P_AA, P_AD, P_AS, P_AR }, {}, 1 },
    { "LFO", { P_LFOSHAPE, P_LFORATE, P_LFOSYNC, P_LFOCUT, P_LFOPITCH, P_LFORETRIG }, {}, 2 },
    { "PITCH ENV", { P_PENV, P_PDEC }, {}, 2 },
    { "DISTORTION", { P_DDRIVE, P_DMIX, P_DTONE }, {}, 2 },
    { "MASTER", { P_VELSENS, P_LEVEL }, {}, 2 },
  };
  cells.reserve(P_COUNT);
  for (auto& s : sections) for (int id : s.ids) makeCell(id);

  presetLabel.setText("PRESET", dontSendNotification);
  presetLabel.setColour(Label::textColourId, kDim);
  addAndMakeVisible(presetLabel);
  for (int i = 0; i < proc.getNumPrograms(); ++i) presetBox.addItem(proc.getProgramName(i), i + 1);
  presetBox.setSelectedId(proc.getCurrentProgram() + 1, dontSendNotification);
  presetBox.onChange = [this] { proc.setCurrentProgram(presetBox.getSelectedId() - 1); };
  addAndMakeVisible(presetBox);
  setSize(1000, kHeader + 3 * kPanelH + 34);
}

SubKingEditor::~SubKingEditor() {
  for (auto& c : cells) if (c.slider) c.slider->setLookAndFeel(nullptr);
  setLookAndFeel(nullptr);
}

void SubKingEditor::resized() {
  presetLabel.setBounds(getWidth() - 330, 20, 60, 24);
  presetBox.setBounds(getWidth() - 270, 20, 256, 26);
  int rowX[3] = { kMargin, kMargin, kMargin };
  size_t ci = 0;
  for (auto& s : sections) {
    int w = (int) s.ids.size() * kCellW + 16;
    int y = kHeader + s.row * kPanelH;
    s.bounds = { rowX[s.row], y, w, kPanelH - 8 };
    for (size_t k = 0; k < s.ids.size(); ++k, ++ci) {
      auto& c = cells[ci];
      int cx = s.bounds.getX() + 8 + (int) k * kCellW, cy = y + 24;
      if (c.slider) c.slider->setBounds(cx, cy, kCellW, kCellH - 18);
      if (c.combo) c.combo->setBounds(cx + 2, cy + 22, kCellW - 4, 24);
      if (c.toggle) c.toggle->setBounds(cx + kCellW / 2 - 12, cy + 22, 26, 26);
      c.label->setBounds(cx, cy + kCellH - 18, kCellW, 16);
    }
    rowX[s.row] += w + 8;
  }
}

void SubKingEditor::paint(Graphics& g) {
  g.fillAll(kBg);
  g.setColour(kAccent);
  g.setFont(FontOptions(30.f, Font::bold));
  g.drawText("SUBKING", kMargin + 4, 12, 200, 36, Justification::centredLeft);
  g.setColour(kDim);
  g.setFont(FontOptions(12.f));
  g.drawText("MONO BASS SYNTH", kMargin + 150, 22, 200, 20, Justification::centredLeft);
  for (auto& s : sections) {
    auto b = s.bounds.toFloat();
    g.setColour(kPanel); g.fillRoundedRectangle(b, 8.f);
    g.setColour(Colour(0xff2a3140)); g.drawRoundedRectangle(b, 8.f, 1.f);
    g.setColour(kAccent.withAlpha(0.9f)); g.setFont(FontOptions(12.f, Font::bold));
    g.drawText(s.title, s.bounds.getX() + 10, s.bounds.getY() + 5, s.bounds.getWidth() - 20, 16, Justification::centredLeft);
  }
}
