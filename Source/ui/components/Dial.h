/*
  ==============================================================================

    Dial.h
    Created: 3 Nov 2023 9:10:35pm
    Author:  andre

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "HoverForTooltip.h"

//==============================================================================

class Dial : public HoverForTooltip
{
public:
    Dial(juce::RangedAudioParameter* rap, TooltipBox* tooltipBox) : 
        HoverForTooltip(rap->getParameterID().toStdString(), tooltipBox), 
        sliderAttachment(*rap, slider)
    {
        slider.setSliderStyle(juce::Slider::SliderStyle::Rotary);
        slider.setTextValueSuffix(rap->getLabel());
        slider.setTextBoxStyle(juce::Slider::TextBoxBelow, true, slider.getTextBoxWidth(), slider.getTextBoxHeight());
        
        addAndMakeVisible(slider);
        label.setText(rap->getParameterID(), juce::sendNotification);
        label.attachToComponent(&slider, false);
        label.setJustificationType(juce::Justification::centred);
    }

    ~Dial() override
    {
    }

    void paint(juce::Graphics& g) override
    {
        g.fillAll(getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId));   // clear the background // draw some placeholder text
    }

    void resized() override
    {
        int size = min(getWidth(), getHeight() - label.getHeight());
        auto area = getLocalBounds().reduced(padding);
        area.removeFromTop(label.getHeight());
        slider.setBounds(area);
    }

private:
    juce::Slider slider;
    juce::SliderParameterAttachment sliderAttachment;
    juce::Label label;

    int padding = 5;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Dial)
};
