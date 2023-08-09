/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"
#include "widgets/CustomLookAndFeel.h"
#include "widgets/CustomDial.h"
#include "widgets/PedalWidget.h"
#include "widgets/SplashScreen.h"

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

    CustomDial* maxLayersDial,
        *riseDial,
        *tailDial,
        *wetDial,
        *dryDial,
        *periodDial,
        *autoGateDial,
        *autoSampleLengthDial;

    CustomToggleButton* forcePeriodToggle,
        *autoSustainToggle,
        *autoGateDirectionToggle;

    std::unique_ptr<SliderAttachment> maxLayersAttachment,
        riseAttachment,
        tailAttachment,
        wetAttachment,
        dryAttachment,
        periodAttachment,
        autoGateAttachment,
        autoSampleLengthAttachment;

    std::unique_ptr<ButtonAttachment> holdAttachment,
        forcePeriodAttachment,
        autoSustainAttachment,
        autoGateDirectionAttachment;

    PedalWidget* pedalWidget;

    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    SustainPedalAudioProcessor& audioProcessor;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SustainPedalAudioProcessorEditor)
};
