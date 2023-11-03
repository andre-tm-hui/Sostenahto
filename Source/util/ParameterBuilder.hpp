#pragma once

#include "JuceHeader.h"
#include "params/ParamIDs.h"

static AudioProcessorValueTreeState::ParameterLayout createParameterLayout()
{
    AudioProcessorValueTreeState::ParameterLayout layout;

    layout.add(std::make_unique<AudioParameterBool>(
        ParameterID{ Param::ID::sustainState, 1 },
        Param::ID::sustainState,
        false
    ));

    layout.add(std::make_unique<AudioParameterFloat>(
        ParameterID{ Param::ID::wet, 1 },
        Param::ID::wet,
        NormalisableRange<float> { 0.f, 100.f, 0.01f, 1.0f },
        50.f,
        "%",
        AudioProcessorParameter::genericParameter
    ));

    layout.add(std::make_unique<AudioParameterFloat>(
        ParameterID{ Param::ID::dry, 1 },
        Param::ID::dry,
        NormalisableRange<float> { 0.f, 100.f, 0.01f, 1.0f },
        50.f,
        "%",
        AudioProcessorParameter::genericParameter
    ));

    layout.add(std::make_unique<AudioParameterFloat>(
        ParameterID{ Param::ID::rise, 1 },
        Param::ID::rise,
        NormalisableRange<float> { 0.f, 5.f, 0.01f, 1.0f },
        0.5f,
        "s",
        AudioProcessorParameter::genericParameter
    ));

    layout.add(std::make_unique<AudioParameterFloat>(
        ParameterID{ Param::ID::tail, 1 },
        Param::ID::tail,
        NormalisableRange<float> { 0.f, 5.f, 0.01f, 1.0f },
        1.f,
        "s",
        AudioProcessorParameter::genericParameter
    ));

    layout.add(std::make_unique<AudioParameterFloat>(
        ParameterID{ Param::ID::period, 1 },
        Param::ID::period,
        NormalisableRange<float> { 0.01f, 1.5f, 0.01f, 1.0f },
        0.5f,
        "s",
        AudioProcessorParameter::genericParameter
    ));

    layout.add(std::make_unique<AudioParameterInt>(
        ParameterID{ Param::ID::maxLayers, 1 },
        Param::ID::maxLayers,
        1,
        10,
        1,
        String()
    ));

    layout.add(std::make_unique<AudioParameterBool>(
        ParameterID{ Param::ID::holdToggle, 1 },
        Param::ID::holdToggle,
        true
    ));

    layout.add(std::make_unique<AudioParameterInt>(
        ParameterID{ Param::ID::keycode, 1 },
        Param::ID::keycode,
        -1,
        INT_MAX,
        1,
        String()
    ));

    layout.add(std::make_unique<AudioParameterBool>(
        ParameterID{ Param::ID::forcePeriod, 1 },
        Param::ID::forcePeriod,
        true
    ));

    layout.add(std::make_unique<AudioParameterBool>(
        ParameterID{ Param::ID::autoSustain, 1 },
        Param::ID::autoSustain,
        false
    ));

    layout.add(std::make_unique<AudioParameterFloat>(
        ParameterID{ Param::ID::autoGate, 1 },
        Param::ID::autoGate,
        NormalisableRange<float> { 0.f, 1.f, 0.01f, 1.0f },
        0.2f,
        String(),
        AudioProcessorParameter::genericParameter
    ));

    layout.add(std::make_unique<AudioParameterBool>(
        ParameterID{ Param::ID::autoGateDirection, 1 },
        Param::ID::autoGateDirection,
        false
    ));

    layout.add(std::make_unique<AudioParameterFloat>(
        ParameterID{ Param::ID::autoSampleLength, 1 },
        Param::ID::autoSampleLength,
        NormalisableRange<float> { 0.f, 2.f, 0.01f, 1.0f },
        0.2f,
        "s",
        AudioProcessorParameter::genericParameter
    ));

    return layout;
}