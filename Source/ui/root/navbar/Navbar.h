/*
  ==============================================================================

    Navbar.h
    Created: 26 Sep 2023 5:07:02pm
    Author:  andre

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include <assert.h>

#include "../../lookAndFeel/TabButtonLF.h"

//==============================================================================
/*
*/
class Navbar  : public juce::Component, juce::ActionBroadcaster
{
public:
    Navbar(const std::vector<std::string>& tabLabels)
    {
        assert(tabLabels.size() > 0);

        for (auto const& label : tabLabels) {
            tabButtons.emplace_back(std::make_unique<juce::TextButton>(label));
            tabButtons.back()->setRadioGroupId(1);
            tabButtons.back()->setToggleable(true);
            tabButtons.back()->setClickingTogglesState(true);
            tabButtons.back()->setLookAndFeel(&tblf);
            tabButtons.back()->onStateChange = [=] { sendActionMessage("Change Tab " + label); };
            addAndMakeVisible(*tabButtons.back());
        }

        tabButtons[0]->setToggleState(true, juce::sendNotification);
    }

    ~Navbar() override
    {
    }

    void paint (juce::Graphics& g) override
    {
        g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));   // clear the background
    }

    void resized() override
    {
        int buttonWidth = getWidth() / tabButtons.size();
        for (size_t i = 0; i < tabButtons.size(); i++) {
            tabButtons[i]->setBounds(i * buttonWidth, 0, buttonWidth, getHeight());
        }
    }

private:
    TabButtonLF tblf;
    std::vector<std::unique_ptr<juce::TextButton>> tabButtons;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Navbar)
};
