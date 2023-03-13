/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
SustainPedalAudioProcessor::SustainPedalAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
    : AudioProcessor(BusesProperties()
#if ! JucePlugin_IsMidiEffect
#if ! JucePlugin_IsSynth
        .withInput("Input", AudioChannelSet::stereo(), true)
#endif
        .withOutput("Output", AudioChannelSet::stereo(), true)
#endif
    )
#endif
{
}

SustainPedalAudioProcessor::~SustainPedalAudioProcessor()
{
}

//==============================================================================
const String SustainPedalAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool SustainPedalAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool SustainPedalAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool SustainPedalAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double SustainPedalAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int SustainPedalAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int SustainPedalAudioProcessor::getCurrentProgram()
{
    return 0;
}

void SustainPedalAudioProcessor::setCurrentProgram (int index)
{
}

const String SustainPedalAudioProcessor::getProgramName (int index)
{
    return {};
}

void SustainPedalAudioProcessor::changeProgramName (int index, const String& newName)
{
}

//==============================================================================
void SustainPedalAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    // Use this method as the place to do any pre-playback
    // initialisation that you need..
    this->sampleRate = sampleRate;
    td = new TransientDetector(sampleRate, samplesPerBlock);
}

void SustainPedalAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool SustainPedalAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    ignoreUnused (layouts);
    return true;
  #else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    // Some plugin hosts, such as certain GarageBand versions, will only
    // load plugins that support stereo bus layouts.
    if (layouts.getMainOutputChannelSet() != AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != AudioChannelSet::stereo())
        return false;

    // This checks if the input layout matches the output layout
   #if ! JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
   #endif

    return true;
  #endif
}
#endif

void SustainPedalAudioProcessor::processBlock (AudioBuffer<float>& buffer, MidiBuffer& midiMessages)
{
    ScopedNoDenormals noDenormals;
    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear (i, 0, buffer.getNumSamples());
    int nSamples = buffer.getNumSamples();

    auto* channelData = buffer.getWritePointer(1);
    auto* channel2Data = buffer.getWritePointer(0);
    td->process(std::vector<float>(channelData, channelData + nSamples), tailSignal);
    // Record the tail of a transient
    if (tailSignal.size() < sampleRate * 5) {
        tailSignal.resize(tailSignal.size() + nSamples);
        memcpy(&tailSignal[tailSignal.size() - nSamples], channelData, sizeof(float) * nSamples);
    }

    double dryDecimal = dry / 100.0;
    for (int i = 0; i < nSamples; i++) {
        channelData[i] *= dryDecimal;
        channel2Data[i] = channelData[i];
    }

    double wetDecimal = wet / 100.0;
    // If the pedal is triggered
    for (int i = 0; i < layers.size();) {
        auto sustain = layers[i]->getSample(nSamples, wetDecimal, sampleRate);
        if (sustain.empty()) {
            delete layers[i];
            layers.erase(layers.begin() + i);
            continue;
        }
        buffer.addFrom(0, 0, &sustain[0], nSamples);
        buffer.addFrom(1, 0, &sustain[0], nSamples);
        i++;
    }

    for (int i = 0; i < fadingOut.size();) {
        auto sustain = fadingOut[i]->getSample(nSamples, wetDecimal, sampleRate);
        if (sustain.empty()) {
            delete fadingOut[i];
            fadingOut.erase(fadingOut.begin() + i);
            continue;
        }
        buffer.addFrom(0, 0, &sustain[0], nSamples);
        buffer.addFrom(1, 0, &sustain[0], nSamples);
        i++;
    }
}

//==============================================================================
bool SustainPedalAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

AudioProcessorEditor* SustainPedalAudioProcessor::createEditor()
{
    return new SustainPedalAudioProcessorEditor (*this);
}

//==============================================================================
void SustainPedalAudioProcessor::getStateInformation (MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
}

void SustainPedalAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
}

void SustainPedalAudioProcessor::setPedal(bool val) {
    DBG((val ? "set pedal down" : "set pedal up"));
    pedalDown = val;
    if (pedalDown) {
        while (layers.size() >= maxLayers) {
            layers[0]->fade(false);
            fadingOut.push_back(layers[0]);
            layers.erase(layers.begin());
;        }
        for (auto layer : layers) {
            layer->fade(true);
        }
        sample = SamplingUtil::getSample(tailSignal, sampleRate * targetSampleLength, 2.0);
        layers.emplace_back(new SustainData(sample, &rise, &tail));
    }
    else {
        for (auto layer : layers) {
            layer->fade(false);
        }
    }
    DBG("done");
}

//==============================================================================
// This creates new instances of the plugin..
AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new SustainPedalAudioProcessor();
}