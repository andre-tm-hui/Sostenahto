/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"

//==============================================================================
/**
*/
class SustainPedalAudioProcessorEditor  : public AudioProcessorEditor, public Timer, public KeyListener
{
public:
    SustainPedalAudioProcessorEditor (SustainPedalAudioProcessor&);
    ~SustainPedalAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;
    void timerCallback() override;

    bool keyPressed(const KeyPress& key, Component* originatingComponent) override;
    bool keyStateChanged(bool isKeyDown, Component*) override;

private:
    void plot(Graphics& g, std::vector<float> data, int x, int y, int width, int height, double yScale, bool middle = true);
    void setupSlider(Slider& slider, double defaultValue, double min, double max, double inc, Label& label, std::string labelText, std::string suffix = "", double width = 80, double height = 120);

    Slider  maxLayersSlider,
        riseSlider,
        tailSlider,
        wetSlider,
        drySlider,
        targetSampleLengthSlider;

    Label maxLayersLabel,
        riseLabel,
        tailLabel,
        wetLabel,
        dryLabel,
        targetSampleLengthLabel,
        keybindLabel;

    ToggleButton pedalToggle, holdToggle;
    TextButton bindButton;

    int keycode = -1;
    bool rebinding = false,
        hold = true;

    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    SustainPedalAudioProcessor& audioProcessor;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SustainPedalAudioProcessorEditor)
};
