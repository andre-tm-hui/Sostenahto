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
#include "TooltipBox.h"

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
        float paddingFactor = 1.f - 2.f * padding;
        label.setSize(size * paddingFactor, label.getHeight());
        slider.setSize(size * paddingFactor, size * paddingFactor);
        slider.setCentrePosition(getWidth() / 2, label.getHeight() / 2 + getHeight() / 2);
    }

private:
    juce::Slider slider;
    juce::SliderParameterAttachment sliderAttachment;
    juce::Label label;

    float padding = 0.05f;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Dial)
};
