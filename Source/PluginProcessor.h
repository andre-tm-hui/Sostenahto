/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "util/dsp/TransientDetect.h"
#include "util/dsp/SamplingUtil.h"
#include "util/SustainData.h"
#include "util/LicenseManager.h"

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
#if JUCE_DEBUG
    std::vector<float> getTransients() { return td->getFrameFlags(); }
#endif

    //==============================================================================
    void setPedal(bool val);

    bool getHoldToSustain() { return holdToggle->get(); }

    int getKeycode() { DBG(keycode->get()); return keycode->get(); }
    void setKeycode(int kc) { 
        DBG(kc); 
        keycode->beginChangeGesture(); 
        keycode->setValueNotifyingHost(kc); 
        keycode->endChangeGesture();
        DBG(keycode->get());
    }

    void setLicenseKey(std::string val) { licenseKey = val; LicenseManager::saveLicense(licenseKey); }
    std::string getLicenseKey() { return licenseKey; }

    //==============================================================================
    void start() { ready = true; }
    bool isReady() { return ready; }
    bool canRun() { return tailSignal.size() >= 4096; }

    //==============================================================================
#if JUCE_DEBUG
    std::vector<float> getSpectralFluxes() { return td->getFrameFlags(); }
#endif

private:
    void handleSustainStateChange();

    //==============================================================================
    AudioProcessorValueTreeState parameters;

    //==============================================================================
    TransientDetector* td;

    std::vector<float> tailSignal, sample;
    std::vector<SustainData*> layers;
    std::vector<SustainData*> fadingOut;
    std::vector<std::future<std::vector<float>>> generatedSamples;

    bool ready = false,
        pedalDown = false,
        gettingPhrase = false,
        phraseFound = false;

    int sr;

    AudioParameterFloat* rise,
        * tail,
        * wet,
        * dry,
        * period,
        * autoGate,
        * autoSampleLength; 

    AudioParameterInt* maxLayers,
        * keycode;

    AudioParameterBool* sustainState,
        * holdToggle,
        * forcePeriod,
        * autoSustain,
        * autoGateDirection;

    bool previousSustainState = false;
    int waitFor = INT_MIN;

    std::string licenseKey = "";

    std::mutex mtx;

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SustainPedalAudioProcessor)
};