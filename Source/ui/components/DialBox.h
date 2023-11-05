/*
  ==============================================================================

    DialBox.h
    Created: 3 Nov 2023 9:10:35pm
    Author:  andre

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "Dial.h"
#include "TooltipBox.h"

//==============================================================================
/*
*/
class DialBox : public juce::GroupComponent
{
public:
    DialBox(const std::vector<juce::RangedAudioParameter*>& dials, TooltipBox& tooltipBox, const juce::String& label = "", const size_t& ncols = 2) :
        ncols(ncols),
        label(label)
    { 
        for (auto const dial : dials) {
            this->dials.emplace_back(std::make_unique<Dial>(dial, &tooltipBox));
            addAndMakeVisible(*this->dials.back());
        }

        setText(label);
        setTextLabelPosition(juce::Justification::centred);
    }

    ~DialBox() override
    {
    }

    void resized() override
    {
        size_t dialWidth = getWidth() / ncols;

        auto area = getLocalBounds();
        area.removeFromTop(juce::Label().getFont().getHeight());

        juce::FlexBox fb;
        fb.flexWrap = juce::FlexBox::Wrap::wrap;
        fb.justifyContent = juce::FlexBox::JustifyContent::spaceBetween;

        for (auto& d : dials) {
            fb.items.add(juce::FlexItem(*d).withMinWidth(dialWidth).withFlex(1));
        }

        fb.performLayout(area);
    }

    void setNcols(const size_t& n) { ncols = n; }
    size_t getNcols() { return ncols; }

private:
    juce::String label;
    std::vector<std::unique_ptr<Dial>> dials;
    size_t ncols;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (DialBox)
};
