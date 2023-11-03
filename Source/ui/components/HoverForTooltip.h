/*
  ==============================================================================

    HoverForTooltip.h
    Created: 22 Sep 2023 5:21:50pm
    Author:  andre

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "TooltipBox.h"

//==============================================================================
/*
*/
class HoverForTooltip : public juce::Component, public juce::ActionBroadcaster
{
public:
    HoverForTooltip(const std::string &id, TooltipBox* tooltipBox) : id(id) 
    {
        if (tooltipBox != nullptr) {
            addActionListener(tooltipBox);
        }
        
        setComponentID(id);
    }

    void mouseEnter(const MouseEvent& event) override 
    {
        sendActionMessage(id);
    }

protected:
    std::string id;

private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (HoverForTooltip)
};
