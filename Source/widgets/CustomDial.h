/*
  ==============================================================================

    CustomDial.h
    Created: 19 Mar 2023 7:19:05pm
    Author:  andre

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "MousePassthrough.h"

//==============================================================================

typedef AudioProcessorValueTreeState::SliderAttachment SliderAttachment;
typedef AudioProcessorValueTreeState::ButtonAttachment ButtonAttachment;

class CustomLabel : public juce::Label, public MousePassthrough {
public:
    CustomLabel(Component& parent) : Label(), MousePassthrough(parent) {

    }

    void mouseEnter(const MouseEvent& ev) override {
        parent.mouseEnter(ev);
    }

private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (CustomLabel)
};

class CustomDial  : public juce::Slider
{
public:
    CustomDial(
        std::string labelText, 
        Label& infoBox, 
        std::string infoText = "", 
        std::string suffix = "", 
        int width = 80, 
        int height = 120
    ) : label(*this), 
        infoBox(infoBox), 
        infoText(infoText) {
        setSize(width, width);
        setTextValueSuffix(suffix);
        setTextBoxStyle(Slider::TextBoxBelow, false, int(0.75 * width), int((height - width) / 2.0));
        setSliderStyle(Slider::SliderStyle::Rotary);
        addAndMakeVisible(label);
        label.setText(labelText, dontSendNotification);
        label.attachToComponent(this, false);
        label.setJustificationType(Justification::centred);

        setComponentID(labelText);
    }

    ~CustomDial() override {}

    void mouseEnter(const MouseEvent&) override {
        infoBox.setText(infoText, dontSendNotification);
        getParentComponent()->repaint();
    };

    CustomLabel label;

private:
    Label& infoBox;

    std::string infoText;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (CustomDial)
};

class CustomToggleButton : public ToggleButton {
public:
    CustomToggleButton(
        std::string labelText,
        Label& infoBox,
        std::string infoText = "",
        int width = 160,
        int height = 24
    ) : ToggleButton(labelText),
        infoBox(infoBox),
        infoText(infoText) {
        setSize(width, height);
        setComponentID(labelText);
    }

    ~CustomToggleButton() override {}

    void mouseEnter(const MouseEvent&) override {
        infoBox.setText(infoText, dontSendNotification);
        getParentComponent()->repaint();
    };

private:
    Label& infoBox;

    std::string infoText;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(CustomToggleButton)
};

class PedalToggleButton : public ToggleButton, public MousePassthrough {
public:
    PedalToggleButton(Component& parent) : ToggleButton(), MousePassthrough(parent) {}

    void mouseEnter(const MouseEvent& ev) override {
        parent.mouseEnter(ev);
    }

private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PedalToggleButton)
};

class PedalTextButton : public TextButton, public MousePassthrough {
public:
    PedalTextButton(Component& parent) : TextButton(), MousePassthrough(parent) {}

    void mouseEnter(const MouseEvent& ev) override {
        parent.mouseEnter(ev);
    }

private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PedalTextButton)
};