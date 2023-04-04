/*
  ==============================================================================

    CustomDial.h
    Created: 19 Mar 2023 7:19:05pm
    Author:  andre

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "../widgets/MousePassthrough.h"

//==============================================================================
/*
*/

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
        double inc, 
        std::string labelText, 
        Label& infoBox, 
        std::string infoText = "", 
        std::string suffix = "", 
        double width = 80, 
        double height = 120
    ) : label(*this), 
        infoBox(infoBox), 
        infoText(infoText) {
        setSize(width, width);
        setTextValueSuffix(suffix);
        setTextBoxStyle(Slider::TextBoxBelow, false, 0.75 * width, (height - width) / 2.0);
        setSliderStyle(Slider::SliderStyle::Rotary);
        addAndMakeVisible(label);
        label.setText(labelText, dontSendNotification);
        label.attachToComponent(this, false);
        label.setJustificationType(Justification::centred);

        setComponentID(labelText);
    }

    ~CustomDial() override {}

    void mouseEnter(const MouseEvent& ev) override {
        infoBox.setText(infoText, dontSendNotification);
        getParentComponent()->repaint();
    };

    CustomLabel label;

private:
    Label& infoBox;

    std::string infoText;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (CustomDial)
};

class CustomToggleButton : public ToggleButton, public MousePassthrough {
public:
    CustomToggleButton(Component& parent) : ToggleButton(), MousePassthrough(parent) {}

    void mouseEnter(const MouseEvent& ev) override {
        parent.mouseEnter(ev);
    }

private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(CustomToggleButton)
};

class CustomTextButton : public TextButton, public MousePassthrough {
public:
    CustomTextButton(Component& parent) : TextButton(), MousePassthrough(parent) {}

    void mouseEnter(const MouseEvent& ev) override {
        parent.mouseEnter(ev);
    }

private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(CustomTextButton)
};