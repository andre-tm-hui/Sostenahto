/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "TransientDetector.h"
#include "SustainData.h"
#include "SamplingUtil.h"

//==============================================================================
/**
*/
class SustainPedalAudioProcessor  : public AudioProcessor
                            #if JucePlugin_Enable_ARA
                             , public AudioProcessorARAExtension
                            #endif
{
public:
    //==============================================================================
    SustainPedalAudioProcessor();
    ~SustainPedalAudioProcessor() override;

    //==============================================================================
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

   #ifndef JucePlugin_PreferredChannelConfigurations
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
   #endif

    void processBlock (AudioBuffer<float>&, MidiBuffer&) override;

    //==============================================================================
    AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    //==============================================================================
    const String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    //==============================================================================
    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram (int index) override;
    const String getProgramName (int index) override;
    void changeProgramName (int index, const String& newName) override;

    //==============================================================================
    void getStateInformation (MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;

    //==============================================================================
    std::vector<float> getLatestSample() { return sample; }
    std::vector<float> getLatestRecording() { return tailSignal; }
    std::vector<float> getTransients() { return td->getSpectralFluxes(); }

    //==============================================================================
    void setPedal(bool val);

    void setMaxLayers(int val) { maxLayers = val; }
    int getMaxLayers() { return maxLayers; }

    void setRise(double val) { rise = val; }
    double getRise() { return rise; }

    void setTail(double val) { tail = val; }
    double getTail() { return tail; }

    void setWet(double val) { wet = val; }
    double getWet() { return wet; }

    void setDry(double val) { dry = val; }
    double getDry() { return dry; }

    void setTargetSampleLength(double val) { targetSampleLength = val; }
    double getTargetSampleLength() { return targetSampleLength; }

private:
    //==============================================================================
    TransientDetector* td;

    std::vector<float> tailSignal, sample;
    std::vector<SustainData*> layers;
    std::vector<SustainData*> fadingOut;

    bool pedalDown = false,
        gettingPhrase = false,
        phraseFound = false;

    int maxLayers = 1, sampleRate;

    double rise = 0.5,
        tail = 1.0,
        wet = 70.0,
        dry = 100.0,
        targetSampleLength = 1.0;

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SustainPedalAudioProcessor)
};