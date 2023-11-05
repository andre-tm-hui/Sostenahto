/*
  ==============================================================================

    Switch.h
    Created: 5 Nov 2023 3:02:47pm
    Author:  andre

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "HoverForTooltip.h"

//==============================================================================
/*
*/
class Switch  : public HoverForTooltip
{
public:
    Switch(juce::RangedAudioParameter* rap, TooltipBox* tooltipBox) :
        HoverForTooltip(rap->getParameterID().toStdString(), tooltipBox)
    {
        toggle.setButtonText(rap->getParameterID());
        addAndMakeVisible(toggle);
    }

    ~Switch() override
    {
    }

    void paint (juce::Graphics& g) override
    {
        g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));   // clear the background // draw some placeholder text
    }

    void resized() override
    {
        toggle.setBounds(getLocalBounds().reduced(padding));
    }

    void setPadding(const size_t& padding) { this->padding = padding; }

private:
    juce::ToggleButton toggle;

    size_t padding = 5;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Switch)
};
