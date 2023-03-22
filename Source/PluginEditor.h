/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"
#include "CustomLookAndFeel.h"
#include "widgets/CustomDial.h"
#include "widgets/PedalWidget.h"

//==============================================================================
/**
*/

typedef AudioProcessorValueTreeState::SliderAttachment SliderAttachment;

class SustainPedalAudioProcessorEditor  : public AudioProcessorEditor, public Timer, public KeyListener
{
public:
    SustainPedalAudioProcessorEditor (SustainPedalAudioProcessor&, AudioProcessorValueTreeState&);
    ~SustainPedalAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;
    void timerCallback() override;

    bool keyPressed(const KeyPress& key, Component* originatingComponent) override;
    bool keyStateChanged(bool isKeyDown, Component*) override;

private:
    void plot(Graphics& g, std::vector<float> data, int x, int y, int width, int height, double yScale, bool middle = true);

    AudioProcessorValueTreeState& vts;

    CustomLookAndFeel lf;

    Label infoLabel;

    CustomDial* maxLayersDial,
        *riseDial,
        *tailDial,
        *wetDial,
        *dryDial,
        *periodDial;

    std::unique_ptr<SliderAttachment> maxLayersAttachment,
        riseAttachment,
        tailAttachment,
        wetAttachment,
        dryAttachment,
        periodAttachment;

    PedalWidget* pedalWidget;

    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    SustainPedalAudioProcessor& audioProcessor;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SustainPedalAudioProcessorEditor)
};
