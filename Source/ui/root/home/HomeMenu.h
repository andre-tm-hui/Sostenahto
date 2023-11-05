/*
  ==============================================================================

    Home.h
    Created: 3 Nov 2023 9:14:15pm
    Author:  andre

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "../../components/DialBox.h"
#include "../../components/Switch.h"
#include "../../components/TooltipBox.h"

//==============================================================================
/*
*/
class Home  : public juce::Component
{
public:
    Home(juce::AudioProcessorValueTreeState& vts, TooltipBox& tooltipBox) :
        forcePeriodSwitch(vts.getParameter(Param::ID::forcePeriod), &tooltipBox)
    {
        mainSettings = std::make_unique<DialBox>(
            std::vector<juce::RangedAudioParameter*>{
                vts.getParameter(Param::ID::wet),
                vts.getParameter(Param::ID::dry),
            },
            tooltipBox,
            "Mix"
        );
        addAndMakeVisible(*mainSettings);
        //mainLabel.setText("Mix", juce::sendNotification);
        //mainLabel.attachToComponent(mainSettings.get(), false);

        timeSettings = std::make_unique<DialBox>(
            std::vector<juce::RangedAudioParameter*>{
                vts.getParameter(Param::ID::rise),
                vts.getParameter(Param::ID::tail),
                vts.getParameter(Param::ID::period),
                vts.getParameter(Param::ID::maxLayers),
        },
            tooltipBox,
            "Sustain"
        );
        addAndMakeVisible(*timeSettings);
        //timeLabel.setText("Sustain", juce::sendNotification);
        //timeLabel.attachToComponent(timeSettings.get(), false);

        // TODO: add checkbox/switch for forcePeriod
        addAndMakeVisible(forcePeriodSwitch);

        //addAndMakeVisible(tooltipBox);
    }

    ~Home() override
    {
    }

    void paint (juce::Graphics& g) override
    {
        g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));   // clear the background // draw some placeholder text
    }

    void resized() override
    {
        auto area = getLocalBounds().reduced(padding);

        size_t width = area.getWidth(),
            height = area.getHeight(),
            labelHeight = juce::Label().getFont().getHeight(),
            dialSpaceHeight = height - 2 * (spacing + labelHeight) - switchHeight,
            dialHeight = min(dialSpaceHeight / (mainNrows + timeNrows), maxDialHeight),
            mainHeight = labelHeight + dialHeight * mainNrows,
            timeHeight = labelHeight + dialHeight * timeNrows;

        mainSettings->setBounds(area.removeFromTop(mainHeight + mainLabel.getHeight()).reduced(5));
        area.removeFromTop(spacing);
        timeSettings->setBounds(area.removeFromTop(timeHeight + timeLabel.getHeight()).reduced(5));
        forcePeriodSwitch.setSize(width, switchHeight);
        forcePeriodSwitch.setBounds(area.removeFromTop(forcePeriodSwitch.getHeight()));
    }

private:
    std::unique_ptr<DialBox> mainSettings,
                             timeSettings;
    Switch forcePeriodSwitch;
    juce::Label mainLabel,
                timeLabel;

    const size_t
        maxDialHeight = 120,
        switchHeight = 36,
        spacing = 24,
        padding = 5,
        mainNrows = 1,
        timeNrows = 2;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Home)
};
