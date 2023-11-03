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
class DialBox : public juce::Component
{
public:
    DialBox(const std::vector<juce::RangedAudioParameter*>& dials, TooltipBox& tooltipBox, const size_t& ncols = 2) :
        ncols(ncols)
    { 
        for (auto const dial : dials) {
            this->dials.emplace_back(std::make_unique<Dial>(dial, &tooltipBox));
            addAndMakeVisible(*this->dials.back());
        }
    }

    ~DialBox() override
    {
    }

    void paint (juce::Graphics& g) override
    {
        g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));   // clear the background
    }

    void resized() override
    {
        size_t dialWidth = getWidth() / ncols,
               dialHeight = getHeight() / ((dials.size() + 1) / 2);
        
        for (int i = 0; i < dials.size(); i++) {
            dials[i]->setBounds((i % 2) * dialWidth, (i / 2) * dialHeight, dialWidth, dialHeight);
        }
    }

    void setNcols(const size_t& n) { ncols = n; }
    size_t getNcols() { return ncols; }

private:
    std::vector<std::unique_ptr<Dial>> dials;
    size_t ncols;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (DialBox)
};
