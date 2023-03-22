/*
  ==============================================================================

    PedalWidget.h
    Created: 19 Mar 2023 7:19:35pm
    Author:  andre

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "../Source/PluginEditor.h"
#include "CustomDial.h"

//==============================================================================
/*
*/
class PedalWidget  : public juce::Component
{
public:
    PedalWidget(SustainPedalAudioProcessor& audioProcessor, Label& infoText, double width = 200.0, double height = 680.0) : 
        audioProcessor(audioProcessor), infoText(infoText), pedalToggle(*this), bindButton(*this), holdToggle(*this), keybindLabel(*this) {
        setSize(width, height);

        hold = audioProcessor.getHoldToSustain();
        
        addAndMakeVisible(pedalToggle);
        pedalToggle.setClickingTogglesState(true);
        pedalToggle.setBounds(0, 0, 200, 650);
        pedalToggle.onStateChange = [this] { this->audioProcessor.setPedal(pedalToggle.getToggleState()); repaint(); };
        pedalToggle.setColour(pedalToggle.tickColourId, Colour());
        pedalToggle.setColour(pedalToggle.tickDisabledColourId, Colour());
        pedalUp = ImageCache::getFromMemory(BinaryData::pedalUp_png, BinaryData::pedalUp_pngSize);
        pedalDown = ImageCache::getFromMemory(BinaryData::pedalDown_png, BinaryData::pedalDown_pngSize);
        addAndMakeVisible(bindButton);
        bindButton.setButtonText(audioProcessor.getKeycode() == -1 ? "None" : KeyPress(audioProcessor.getKeycode()).getTextDescription());
        bindButton.setBounds(50, 60, 100, 24);
        bindButton.onClick = [this] { rebinding = true; };
        keybindLabel.setText("Keybind", dontSendNotification);
        keybindLabel.attachToComponent(&bindButton, false);
        keybindLabel.setSize(100, 24);
        keybindLabel.setJustificationType(Justification::centred);
        addAndMakeVisible(holdToggle);
        holdToggle.setClickingTogglesState(true);
        holdToggle.setToggleState(hold, dontSendNotification);
        holdToggle.setBounds(35, 640, 150, 20);
        holdToggle.setButtonText("Hold to sustain");
        holdToggle.onClick = [this] { hold = holdToggle.getToggleState(); };
    }
    ~PedalWidget() override {}

    void paint(juce::Graphics& g) override {
        g.drawImageWithin(pedalToggle.getToggleState() ? pedalDown : pedalUp, 0, 0, 200, 650, RectanglePlacement::Flags::stretchToFit);
    }
    void resized() override {}

    void mouseEnter(const MouseEvent& ev) override {
        infoText.setText(std::string("The \"Pedal\" that enables the sustain. To use, play a chord/note, and then press the pedal.") + 
            "\n\nKeybind: setup a keybind for your keyboard or a footswitch." + 
            "\n\nHold to sustain : change whether the pedal toggles or holds the sustain.", 
            dontSendNotification);
    }

    bool rebinding = false;
    bool hold;

    CustomToggleButton pedalToggle;
    CustomTextButton bindButton;

private:
    CustomToggleButton holdToggle;
    CustomLabel keybindLabel;
    Label& infoText;
    Image pedalUp, pedalDown;

    SustainPedalAudioProcessor& audioProcessor;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PedalWidget)
};
