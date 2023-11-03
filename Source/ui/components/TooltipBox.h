/*
  ==============================================================================

    TooltipBox.h
    Created: 22 Sep 2023 12:53:12pm
    Author:  andre

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "../../util/ParameterBuilder.hpp"

constexpr int margin = 15;

//==============================================================================
class TooltipBox : public juce::Component, public juce::ActionListener
{
public:
    TooltipBox() {}

    ~TooltipBox() override {}

    void paint (juce::Graphics& g) override
    {
        g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));   // clear the background

        g.setColour (juce::Colours::grey);
        g.fillRect(getLocalBounds());

        g.setColour(juce::Colours::white);
        g.drawFittedText(tooltip, getLocalBounds().expanded(-margin), juce::Justification::centred, 10000, 1.f);
    }

    void resized() override
    {
        repaint();
    }

    void actionListenerCallback(const String& message) override {
        tooltip = Param::tooltip.at(message.toStdString());
        repaint();
    }

private:
    std::string tooltip = "Welcome to Sohstenahto Pedal!";

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (TooltipBox)
};