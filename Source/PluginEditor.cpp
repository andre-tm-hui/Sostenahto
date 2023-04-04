/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
SustainPedalAudioProcessorEditor::SustainPedalAudioProcessorEditor(SustainPedalAudioProcessor& p, AudioProcessorValueTreeState& vts)
    : AudioProcessorEditor(&p), Timer(), KeyListener(), audioProcessor(p), vts(vts)
{
    LookAndFeel::setDefaultLookAndFeel(&lf);

    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
    setSize (400, 680);

    addAndMakeVisible(infoLabel);
    infoLabel.setBounds(10, 420, 180, 240);
    infoLabel.setColour(Label::ColourIds::backgroundColourId, Colour(0xFF404040));
    infoLabel.setJustificationType(Justification::centredLeft);
    infoLabel.setBorderSize(BorderSize<int>(10));

    wetDial = new CustomDial(1, "Wet", infoLabel, "The volume of the sustain.\n\n(0-100%)", "%");
    addAndMakeVisible(wetDial);
    wetDial->setTopLeftPosition(20, 50);
    wetAttachment.reset(new SliderAttachment(vts, "wet", *wetDial));
    
    dryDial = new CustomDial(1, "Dry", infoLabel, "The volume of the original input signal.\n\n(0-100%)", "%");
    addAndMakeVisible(dryDial);
    dryDial->setTopLeftPosition(100, 50);
    dryAttachment.reset(new SliderAttachment(vts, "dry", *dryDial));
    
    riseDial = new CustomDial(0.05, "Rise", infoLabel, "The fade-in time of the sustained sound.\n\n(0.0-5.0s)", "s");
    addAndMakeVisible(riseDial);
    riseDial->setTopLeftPosition(20, 180);
    riseAttachment.reset(new SliderAttachment(vts, "rise", *riseDial));
    
    tailDial = new CustomDial(0.05, "Tail", infoLabel, "The fade-out time of the sustained sound.\n\n(0.0-5.0s)", "s");
    addAndMakeVisible(tailDial);
    tailDial->setTopLeftPosition(100, 180);
    tailAttachment.reset(new SliderAttachment(vts, "tail", *tailDial));
    
    maxLayersDial = new CustomDial(1, "Max Layers", infoLabel, "The maximum number of overlapping sustain layers.\n\n(1-10)");
    addAndMakeVisible(maxLayersDial);
    maxLayersDial->setTopLeftPosition(20, 310);
    maxLayersAttachment.reset(new SliderAttachment(vts, "maxLayers", *maxLayersDial));

    periodDial = new CustomDial(0.1, "Period", infoLabel, "The target length of the looped signal.\n\nChanges the period of the oscilating effect commonly heard in polyphonic sounds (chords etc.).\n\n(0.0-3.0s)", "s");
    addAndMakeVisible(periodDial);
    periodDial->setTopLeftPosition(100, 310);
    periodAttachment.reset(new SliderAttachment(vts, "period", *periodDial));

    pedalWidget = new PedalWidget(audioProcessor, infoLabel);
    addAndMakeVisible(pedalWidget);
    pedalWidget->setTopLeftPosition(200, 0);

    ss = new DonateSplashScreen(audioProcessor);
    if (!audioProcessor.isReady()) addAndMakeVisible(ss);

    addKeyListener(this);
    setWantsKeyboardFocus(true);
}

SustainPedalAudioProcessorEditor::~SustainPedalAudioProcessorEditor()
{
    maxLayersAttachment.reset();
    riseAttachment.reset();
    tailAttachment.reset();
    wetAttachment.reset();
    dryAttachment.reset();
    periodAttachment.reset();
    delete maxLayersDial;
    delete riseDial;
    delete tailDial;
    delete wetDial;
    delete dryDial;
    delete periodDial;
    delete pedalWidget;
    if (!audioProcessor.isReady()) delete ss;
}

//==============================================================================
void SustainPedalAudioProcessorEditor::paint (Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll (getLookAndFeel().findColour (ResizableWindow::backgroundColourId));

    g.setColour (Colours::white);
    g.setFont (15.0f);

    //g.drawImageWithin(pedalToggle.getToggleState() ? pedalDown : pedalUp, 190, 0, 200, 650, RectanglePlacement::Flags::stretchToFit);
}

void SustainPedalAudioProcessorEditor::resized()
{
    // This is generally where you'll want to lay out the positions of any
    // subcomponents in your editor..
}

void SustainPedalAudioProcessorEditor::timerCallback() {
    repaint();
}

bool SustainPedalAudioProcessorEditor::keyPressed(const KeyPress& key, Component*) {
    if (!audioProcessor.isReady()) return true;

    if (pedalWidget->rebinding) {
        pedalWidget->bindButton.setButtonText(key.getTextDescription());
        vts.state.getChildWithProperty("id", "keycode").setProperty("value", var(key.getKeyCode()), nullptr);
        pedalWidget->rebinding = false;
        return true;
    }

    if (key.getKeyCode() == audioProcessor.getKeycode()) {
        if (pedalWidget->hold) {
            pedalWidget->pedalToggle.setToggleState(true, juce::NotificationType::sendNotification);
        }
        else {
            pedalWidget->pedalToggle.setToggleState(!pedalWidget->pedalToggle.getToggleState(), juce::NotificationType::sendNotification);
        }
    }

    return true;
}

bool SustainPedalAudioProcessorEditor::keyStateChanged(bool isKeyDown, Component*) {
    if (pedalWidget->hold && !KeyPress(audioProcessor.getKeycode()).isCurrentlyDown()) {
        pedalWidget->pedalToggle.setToggleState(false, juce::NotificationType::sendNotification);
    }
    return true;
}


void SustainPedalAudioProcessorEditor::plot(Graphics& g, std::vector<float> data, int x, int y, int width, int height, double yScale, bool middle) {
    int nPoints = data.size();
    if (middle) y += height / 2; else y += height;
    for (int i = 0; i < nPoints - 1; i++) {
        g.drawLine(x + width * i / nPoints, y - yScale * data[i], x + width * (i + 1) / nPoints, y - yScale * data[i + 1], 1);
    }
}