/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
SustainPedalAudioProcessorEditor::SustainPedalAudioProcessorEditor (SustainPedalAudioProcessor& p)
    : AudioProcessorEditor(&p), Timer(), KeyListener(), audioProcessor(p), pedalToggle()
{
    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
    setSize (1200, 800);

    // Setup wet and dry sliders
    setupSlider(wetSlider, audioProcessor.getWet(), 0, 100, 1, wetLabel, "Wet", "%");
    wetSlider.onValueChange = [this] { audioProcessor.setWet(wetSlider.getValue()); };
    wetSlider.setTopLeftPosition(30, 40);
    setupSlider(drySlider, audioProcessor.getDry(), 0, 100, 1, dryLabel, "Dry", "%");
    drySlider.onValueChange = [this] { audioProcessor.setDry(drySlider.getValue()); };
    drySlider.setTopLeftPosition(110, 40);

    // Setup rise/tail sliders
    setupSlider(riseSlider, audioProcessor.getRise(), 0, 5, 0.05, riseLabel, "Rise", "s");
    riseSlider.onValueChange = [this] { audioProcessor.setRise(riseSlider.getValue()); };
    riseSlider.setTopLeftPosition(30, 160);
    setupSlider(tailSlider, audioProcessor.getTail(), 0, 5, 0.05, tailLabel, "Tail", "s");
    tailSlider.onValueChange = [this] { audioProcessor.setTail(tailSlider.getValue()); };
    tailSlider.setTopLeftPosition(110, 160);

    // Setup max-layers slider
    setupSlider(maxLayersSlider, audioProcessor.getMaxLayers(), 1, 10, 1, maxLayersLabel, "Layers");
    maxLayersSlider.onValueChange = [this] { audioProcessor.setMaxLayers(maxLayersSlider.getValue()); };
    maxLayersSlider.setTopLeftPosition(30, 300);
    
    // Setup advanced settings sliders
    setupSlider(targetSampleLengthSlider, audioProcessor.getTargetSampleLength(), 0, 3, 0.1, targetSampleLengthLabel, "Target Sample Length", "s");
    targetSampleLengthSlider.onValueChange = [this] { audioProcessor.setTargetSampleLength(targetSampleLengthSlider.getValue()); };
    targetSampleLengthSlider.setTopLeftPosition(30, 440);

    // Setup pedal settings
    addAndMakeVisible(pedalToggle);
    pedalToggle.setClickingTogglesState(true);
    pedalToggle.setBounds(210, 40, 160, 20);
    pedalToggle.onStateChange = [this] { audioProcessor.setPedal(pedalToggle.getToggleState()); };
    addAndMakeVisible(bindButton);
    bindButton.setButtonText("None");
    bindButton.setBounds(275, 70, 100, 24);
    bindButton.onClick = [this] { rebinding = true; };
    keybindLabel.setText("Keybind:", dontSendNotification);
    keybindLabel.attachToComponent(&bindButton, true);
    keybindLabel.setSize(80, 24);
    addAndMakeVisible(holdToggle);
    holdToggle.setClickingTogglesState(true);
    holdToggle.setToggleState(true, dontSendNotification);
    holdToggle.setBounds(210, 100, 160, 20);
    holdToggle.setButtonText("Hold to sustain");
    holdToggle.onClick = [this] { hold = holdToggle.getToggleState(); };

    addKeyListener(this);
    setWantsKeyboardFocus(true);

    //startTimer(0.1);
}

SustainPedalAudioProcessorEditor::~SustainPedalAudioProcessorEditor()
{
}

//==============================================================================
void SustainPedalAudioProcessorEditor::paint (Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll (getLookAndFeel().findColour (ResizableWindow::backgroundColourId));

    g.setColour (Colours::white);
    g.setFont (15.0f);

    //plot(g, audioProcessor.getLatestSample(), 400, 260, 800, 200, 500);
    //plot(g, audioProcessor.getLatestRecording(), 400, 40, 800, 200, 500);
    //plot(g, audioProcessor.getTransients(), 400, 480, 800, 200, 0.1, false);
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
    if (rebinding) {
        bindButton.setButtonText(key.getTextDescription());
        keycode = key.getKeyCode();
        rebinding = false;
        return true;
    }

    if (key.getKeyCode() == keycode) {
        if (hold) {
            pedalToggle.setToggleState(true, juce::NotificationType::sendNotification);
        }
        else {
            pedalToggle.setToggleState(!pedalToggle.getToggleState(), juce::NotificationType::sendNotification);
        }
    }

    return true;
}

bool SustainPedalAudioProcessorEditor::keyStateChanged(bool isKeyDown, Component*) {
    if (hold && !KeyPress(keycode).isCurrentlyDown()) {
        pedalToggle.setToggleState(false, juce::NotificationType::sendNotification);
    }
    return true;
}

void SustainPedalAudioProcessorEditor::setupSlider(Slider& slider, double defaultValue, double min, double max, double inc, Label& label, std::string labelText, std::string suffix, double width, double height) {
    addAndMakeVisible(slider);
    slider.setSize(width, width);
    slider.setRange(min, max, inc);
    slider.setValue(defaultValue, juce::dontSendNotification);
    slider.setTextValueSuffix(suffix);
    slider.setTextBoxStyle(Slider::TextBoxBelow, false, 0.75 * width, (height - width) / 2.0);
    slider.setSliderStyle(Slider::SliderStyle::Rotary);
    addAndMakeVisible(slider);
    label.setText(labelText, juce::dontSendNotification);
    label.attachToComponent(&slider, false);
    label.setSize(width, (height - width) / 2.0);
    label.setJustificationType(Justification::centred);
}

void SustainPedalAudioProcessorEditor::plot(Graphics& g, std::vector<float> data, int x, int y, int width, int height, double yScale, bool middle) {
    int nPoints = data.size();
    if (middle) y += height / 2; else y += height;
    for (int i = 0; i < nPoints - 1; i++) {
        g.drawLine(x + width * i / nPoints, y - yScale * data[i], x + width * (i + 1) / nPoints, y - yScale * data[i + 1], 1);
    }
}