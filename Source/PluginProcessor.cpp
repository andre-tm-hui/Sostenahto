/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
SustainPedalAudioProcessor::SustainPedalAudioProcessor() :
    parameters(*this, nullptr, Identifier("APVTS"), {
            std::make_unique<AudioParameterFloat> ("wet", "Wet", 0.f, 100.f, 70.f),
            std::make_unique<AudioParameterFloat> ("dry", "Dry", 0.f, 100.f, 100.f),
            std::make_unique<AudioParameterFloat>("rise", "Rise", 0.f, 5.f, 0.5f),
            std::make_unique<AudioParameterFloat>("tail", "Tail", 0.f, 5.f, 1.f),
            std::make_unique<AudioParameterFloat>("period", "Period", 0.01f, 1.5f, 0.5f),
            std::make_unique<AudioParameterInt>("maxLayers", "Max Layers", 1, 10, 1),
            std::make_unique<AudioParameterBool>("holdToggle", "Hold to sustain", true),
            std::make_unique<AudioParameterInt>("keycode", "Keycode", -1, INT_MAX, -1),
            std::make_unique<AudioParameterBool>("forcePeriod", "Force Period", true)
        })
#ifndef JucePlugin_PreferredChannelConfigurations
    , AudioProcessor(BusesProperties()
#if ! JucePlugin_IsMidiEffect
#if ! JucePlugin_IsSynth
        .withInput("Input", AudioChannelSet::stereo(), true)
#endif
        .withOutput("Output", AudioChannelSet::stereo(), true)
#endif
    )
#endif
{
    wet = parameters.getRawParameterValue("wet");
    dry = parameters.getRawParameterValue("dry");
    rise = parameters.getRawParameterValue("rise");
    tail = parameters.getRawParameterValue("tail");
    period = parameters.getRawParameterValue("period");
    maxLayers = parameters.getRawParameterValue("maxLayers");
    holdToggle = parameters.getRawParameterValue("holdToggle");
    keycode = parameters.getRawParameterValue("keycode");
    forcePeriod = parameters.getRawParameterValue("forcePeriod");

    licenseKey = LicenseManager::loadLicense();
}

SustainPedalAudioProcessor::~SustainPedalAudioProcessor()
{
    delete td;
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

void SustainPedalAudioProcessor::setCurrentProgram (int)
{
}

const String SustainPedalAudioProcessor::getProgramName (int)
{
    return {};
}

void SustainPedalAudioProcessor::changeProgramName (int, const String&)
{
}

//==============================================================================
void SustainPedalAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    // Use this method as the place to do any pre-playback
    // initialisation that you need..
    sr = (int)sampleRate;
    td = new TransientDetector(sr, samplesPerBlock);
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

void SustainPedalAudioProcessor::processBlock (AudioBuffer<float>& buffer, MidiBuffer&)
{
    if (!ready) return;

    ScopedNoDenormals noDenormals;
    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    if (totalNumInputChannels == 0 || totalNumOutputChannels == 0) return;

    int nSamples = buffer.getNumSamples();

    auto* channelData = buffer.getWritePointer(0);
    mtx.lock();
    td->process(std::vector<float>(channelData, channelData + nSamples), tailSignal);
    // Record the tail of a transient
    if (tailSignal.size() < sr * 5) {
        tailSignal.resize(tailSignal.size() + nSamples);
        memcpy(&tailSignal[tailSignal.size() - nSamples], channelData, sizeof(float) * nSamples);
    }
    mtx.unlock();

    float dryDecimal = *dry / 100.f;
    for (int i = 0; i < nSamples; i++) {
        channelData[i] *= dryDecimal;
    }

    int nGeneratedSamples = generatedSamples.size();
    for (int i = nGeneratedSamples - 1; i >= 0; i--) {
        if (generatedSamples[i]._Is_ready()) {
            layers.emplace_back(new SustainData(generatedSamples[i].get(), *rise, *tail));
            generatedSamples.erase(generatedSamples.begin() + i);
        }
    }

    float wetDecimal = *wet / 100.f;
    // If the pedal is triggered
    for (int i = 0; i < layers.size();) {
        if (layers[i] == nullptr) {
            layers.erase(layers.begin() + i);
            continue;
        }
        auto sustain = layers[i]->getSample(nSamples, wetDecimal, sr);
        if (sustain.empty()) {
            delete layers[i];
            layers.erase(layers.begin() + i);
            continue;
        }
        buffer.addFrom(0, 0, &sustain[0], nSamples);
        //buffer.addFrom(1, 0, &sustain[0], nSamples);
        i++;
    }

    for (int i = 0; i < fadingOut.size();) {
        if (fadingOut[i] == nullptr) {
            fadingOut.erase(fadingOut.begin() + i);
            continue;
        }
        auto sustain = fadingOut[i]->getSample(nSamples, wetDecimal, sr);
        if (sustain.empty()) {
            delete fadingOut[i];
            fadingOut.erase(fadingOut.begin() + i);
            continue;
        }
        buffer.addFrom(0, 0, &sustain[0], nSamples);
        //buffer.addFrom(1, 0, &sustain[0], nSamples);
        i++;
    }
    
    for (int i = 1; i < totalNumOutputChannels; i++) {
        buffer.copyFrom(i, 0, channelData, nSamples);
    }

    /*for (int i = 1; i < totalNumOutputChannels; i++) {
        buffer.addFrom(i, 0, channelData, nSamples);
    }*/
}

//==============================================================================
bool SustainPedalAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

AudioProcessorEditor* SustainPedalAudioProcessor::createEditor()
{
    return new SustainPedalAudioProcessorEditor (*this, parameters);
}

//==============================================================================
void SustainPedalAudioProcessor::getStateInformation (MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
    auto state = parameters.copyState();
    state.getChildWithProperty("id", "keycode").setProperty("value", var(*keycode), nullptr);
    std::unique_ptr<XmlElement> xml(state.createXml());

    copyXmlToBinary(*xml, destData);
    DBG(xml->toString());
}

void SustainPedalAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
    std::unique_ptr<XmlElement> xmlState(getXmlFromBinary(data, sizeInBytes));
    DBG(xmlState.get()->toString());

    if (xmlState.get() != nullptr)
        if (xmlState->hasTagName(parameters.state.getType()))
            parameters.replaceState(juce::ValueTree::fromXml(*xmlState));

    licenseKey = LicenseManager::loadLicense();
}

void SustainPedalAudioProcessor::setPedal(bool val) {    
    pedalDown = val;
    if (pedalDown) {
        while (layers.size() >= *maxLayers) {
            layers[0]->fade(false);
            fadingOut.push_back(layers[0]);
            layers.erase(layers.begin());
        }
        for (int i = 0; i < layers.size(); i++) {
            if (layers[i] != nullptr) {
                layers[i]->fade(true);
            }
        }
        mtx.lock();
        generatedSamples.emplace_back(
            std::async(
                std::launch::async, 
                SamplingUtil::getSample, 
                tailSignal, 
                int(sr * *period), 
                *forcePeriod > 0.5f, 
                (size_t)sr
            )
        );
        mtx.unlock();
    }
    else {
        for (int i = 0; i < layers.size(); i++) {
            if (layers[i] != nullptr) {
                layers[i]->fade(false);
            }
        }
    }
}

//==============================================================================
// This creates new instances of the plugin..
AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new SustainPedalAudioProcessor();
}