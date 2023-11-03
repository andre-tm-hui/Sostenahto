/*
  ==============================================================================

    TabButtonLF.h
    Created: 26 Sep 2023 5:28:51pm
    Author:  andre

  ==============================================================================
*/

#pragma once

#include "CustomLookAndFeel.h"

//==============================================================================
/*
*/
class TabButtonLF  : public CustomLookAndFeel
{
public:
    TabButtonLF()
    {
    }

    ~TabButtonLF() override
    {
    }

    void drawButtonBackground(
        juce::Graphics& g,
        juce::Button&,
        const juce::Colour& backgroundColour,
        bool  	shouldDrawButtonAsHighlighted,
        bool  	shouldDrawButtonAsDown
    ) override {
        g.setColour(getCurrentColourScheme().getUIColour(ColourScheme::windowBackground));
        g.fillAll();
    }

    void drawButtonText(
        juce::Graphics& g, 
        juce::TextButton& tb, 
        bool shouldDrawButtonAsHighlighted, 
        bool shouldDrawButtonAsDown
    ) override {
        const bool state = tb.getToggleState();
        const juce::Colour onColour = findColour(juce::TextButton::textColourOnId),
            offColour = findColour(juce::TextButton::textColourOffId);

        auto colour = offColour.interpolatedWith(
            onColour,
            state 
                ? 1.f 
                : shouldDrawButtonAsHighlighted 
                    ? highlightColourInterp
                    : 0.f
        );

        g.setColour(colour);

        g.setFont(juce::Font(
            fontSize,
            shouldDrawButtonAsHighlighted && !shouldDrawButtonAsDown 
                ? juce::Font::FontStyleFlags::bold 
                : juce::Font::FontStyleFlags::plain
        ));

        g.drawText(tb.getButtonText(), g.getClipBounds().toFloat(), juce::Justification::centred);
    }

private:
    static constexpr float highlightColourInterp = 0.7f;
    static constexpr float fontSize = 14.f;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (TabButtonLF)
};
