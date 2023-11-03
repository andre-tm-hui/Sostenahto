/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"
#include "ui/root/navbar/Navbar.h"
#include "ui/lookAndFeel/CustomLookAndFeel.h"
#include "widgets/CustomDial.h"
#include "widgets/PedalWidget.h"
#include "widgets/SplashScreen.h"

#include "ui/components/Dial.h"

//==============================================================================


class SustainPedalAudioProcessorEditor  : public AudioProcessorEditor, public KeyListener, public Timer
{
public:
    SustainPedalAudioProcessorEditor (SustainPedalAudioProcessor&, AudioProcessorValueTreeState&);
    ~SustainPedalAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;
    void timerCallback() override { }//repaint(); }

    bool keyPressed(const KeyPress& key, Component* originatingComponent) override;
    bool keyStateChanged(bool isKeyDown, Component*) override;

private:
    void plot(Graphics& g, std::vector<float> data, int x, int y, int width, int height, double yScale, bool middle = true);

    AudioProcessorValueTreeState& vts;

    CustomLookAndFeel lf;

    DonateSplashScreen* ss;

    Label infoLabel, experimentalLabel;

    Navbar navbar;
    TooltipBox tooltipBox;
    std::vector<std::unique_ptr<Dial>> dials;

    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    SustainPedalAudioProcessor& audioProcessor;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SustainPedalAudioProcessorEditor)
};
