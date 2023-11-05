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

#include "ui/root/home/HomeMenu.h"

//==============================================================================


class SustainPedalAudioProcessorEditor  : public AudioProcessorEditor, public KeyListener, public Timer, public juce::ActionListener
{
public:
    SustainPedalAudioProcessorEditor (SustainPedalAudioProcessor&, juce::AudioProcessorValueTreeState&);
    ~SustainPedalAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;
    void timerCallback() override { }//repaint(); }

    bool keyPressed(const KeyPress& key, Component* originatingComponent) override;
    bool keyStateChanged(bool isKeyDown, Component*) override;

private:
    void plot(Graphics& g, std::vector<float> data, int x, int y, int width, int height, double yScale, bool middle = true);

    void actionListenerCallback(const String&) override;

    void navTo(const String&);

    void shiftMenus(const int&);

    AudioProcessorValueTreeState& vts;

    CustomLookAndFeel lf;

    DonateSplashScreen* ss;

    Label infoLabel, experimentalLabel;

    ComponentAnimator animator;
    Navbar navbar;
    Home home;
    TooltipBox tooltipBox;

    size_t navbarHeight = 32,
        tooltipHeight = 120,
        menuWidth = 160,
        pedalWidth = 216;

    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    SustainPedalAudioProcessor& audioProcessor;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SustainPedalAudioProcessorEditor)
};
