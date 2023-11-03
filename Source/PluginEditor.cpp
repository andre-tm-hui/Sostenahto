/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
SustainPedalAudioProcessorEditor::SustainPedalAudioProcessorEditor(SustainPedalAudioProcessor& p, AudioProcessorValueTreeState& vts) :
    AudioProcessorEditor(&p),
    KeyListener(),
    audioProcessor(p),
    vts(vts),
    Timer(),
    navbar({ "Home", "Auto-Trigger" })
{
    LookAndFeel::setDefaultLookAndFeel(&lf);
    addChildComponent(navbar);
    addAndMakeVisible(navbar);
    addChildComponent(tooltipBox);
    addAndMakeVisible(tooltipBox);
    
    dials.emplace_back(std::make_unique<Dial>(Param::ID::dry, vts, &tooltipBox));
    dials[0]->setBounds(210, 50, 100, 120);
    addAndMakeVisible(*dials[0]);

    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
#if JUCE_DEBUG
    setSize(1080, 680);
    startTimer(100);
#else
    setSize (540, 680);
#endif

    /*addAndMakeVisible(infoLabel);
    infoLabel.setBounds(10, 420, 180, 240);
    infoLabel.setFont(Font(14));
    infoLabel.setColour(Label::ColourIds::backgroundColourId, Colour(0xFF404040));
    infoLabel.setJustificationType(Justification::centredLeft);
    infoLabel.setBorderSize(BorderSize<int>(10));

    wetDial = new CustomDial(ParamIDs::wet, infoLabel, "The volume of the sustain.\n\n(0-100%)", "%");
    addAndMakeVisible(wetDial);
    wetDial->setTopLeftPosition(20, 50);
    wetAttachment.reset(new SliderAttachment(vts, ParamIDs::wet, *wetDial));
    
    dryDial = new CustomDial(ParamIDs::dry, infoLabel, "The volume of the original input signal.\n\n(0-100%)", "%");
    addAndMakeVisible(dryDial);
    dryDial->setTopLeftPosition(100, 50);
    dryAttachment.reset(new SliderAttachment(vts, ParamIDs::dry, *dryDial));
    
    riseDial = new CustomDial(ParamIDs::rise, infoLabel, "The fade-in time of the sustained sound.\n\n(0.0-5.0s)", "s");
    addAndMakeVisible(riseDial);
    riseDial->setTopLeftPosition(20, 170);
    riseAttachment.reset(new SliderAttachment(vts, ParamIDs::rise, *riseDial));
    
    tailDial = new CustomDial(ParamIDs::tail, infoLabel, "The fade-out time of the sustained sound.\n\n(0.0-5.0s)", "s");
    addAndMakeVisible(tailDial);
    tailDial->setTopLeftPosition(100, 170);
    tailAttachment.reset(new SliderAttachment(vts, ParamIDs::tail, *tailDial));
    
    maxLayersDial = new CustomDial(ParamIDs::maxLayers, infoLabel, "The maximum number of overlapping sustain layers.\n\n(1-10)");
    addAndMakeVisible(maxLayersDial);
    maxLayersDial->setTopLeftPosition(20, 290);
    maxLayersAttachment.reset(new SliderAttachment(vts, ParamIDs::maxLayers, *maxLayersDial));

    periodDial = new CustomDial(ParamIDs::period, infoLabel, "The target length of the looped signal.\n\nChanges the period of the oscilating effect commonly heard in polyphonic sounds (chords etc.).\n\n(0.0-1.5s)", "s");
    addAndMakeVisible(periodDial);
    periodDial->setTopLeftPosition(100, 290);
    periodAttachment.reset(new SliderAttachment(vts, ParamIDs::period, *periodDial));

    pedalWidget = new PedalWidget(audioProcessor, infoLabel);
    addAndMakeVisible(pedalWidget);
    pedalWidget->setTopLeftPosition(200, 0);
    holdAttachment.reset(new ButtonAttachment(vts, ParamIDs::holdToggle, pedalWidget->holdToggle));

    forcePeriodToggle = new CustomToggleButton(ParamIDs::forcePeriod, infoLabel, "When enabled, the pedal will take the recorded sample and stretch it to the length of the period if the sample is not long enough.\n\nOtherwise, the period will be based on the length of the recorded sample.");
    addAndMakeVisible(forcePeriodToggle);
    forcePeriodToggle->setClickingTogglesState(true);
    forcePeriodToggle->setTopLeftPosition(46, 384);
    forcePeriodAttachment.reset(new ButtonAttachment(vts, ParamIDs::forcePeriod, *forcePeriodToggle));

    addAndMakeVisible(experimentalLabel);
    experimentalLabel.setText("EXPERIMENTAL", dontSendNotification);
    experimentalLabel.setBounds(410, 10, 120, 660);
    experimentalLabel.setFont(Font(18, Font::bold));
    experimentalLabel.setColour(Label::ColourIds::backgroundColourId, Colour(0xFF181818));
    experimentalLabel.setJustificationType(Justification::topLeft);
    experimentalLabel.setBorderSize(BorderSize<int>(10));

    autoSustainToggle = new CustomToggleButton(ParamIDs::autoSustain, infoLabel, "When enabled, the pedal is dynamically triggered based on the attack of a sound.");
    addAndMakeVisible(autoSustainToggle);
    autoSustainToggle->setClickingTogglesState(true);
    autoSustainToggle->setBounds(415, 45, 110, 18);
    autoSustainAttachment.reset(new ButtonAttachment(vts, ParamIDs::autoSustain, *autoSustainToggle));

    autoGateDial = new CustomDial(ParamIDs::autoGate, infoLabel, "The attack cutoff value used by Automatic Sustain.");
    addAndMakeVisible(autoGateDial);
    autoGateDial->setBounds(415, 90, 110, 85);
    autoGateAttachment.reset(new SliderAttachment(vts, ParamIDs::autoGate, *autoGateDial));

    autoGateDirectionToggle = new CustomToggleButton(ParamIDs::autoGateDirection, infoLabel, "When enabled, sounds with a larger attack than this value will be sustained. When disabled, sounds with a smaller attack will be sustained.");
    addAndMakeVisible(autoGateDirectionToggle);
    autoGateDirectionToggle->setClickingTogglesState(true);
    autoGateDirectionToggle->setBounds(415, 185, 110, 18);
    autoGateDirectionAttachment.reset(new ButtonAttachment(vts, ParamIDs::autoGateDirection, *autoGateDirectionToggle));

    autoSampleLengthDial = new CustomDial(ParamIDs::autoSampleLength, infoLabel, "The length of the sampled clip. This corresponds to the delay before the sustain begins.", "s");
    addAndMakeVisible(autoSampleLengthDial);
    autoSampleLengthDial->setBounds(415, 230, 110, 85);
    autoSampleLengthAttachment.reset(new SliderAttachment(vts, ParamIDs::autoSampleLength, *autoSampleLengthDial));

    ss = new DonateSplashScreen(audioProcessor);
    if (!audioProcessor.isReady()) addAndMakeVisible(ss);

    addKeyListener(this);
    setWantsKeyboardFocus(true);*/
}

SustainPedalAudioProcessorEditor::~SustainPedalAudioProcessorEditor()
{
    if (!audioProcessor.isReady()) delete ss;
}

//==============================================================================
void SustainPedalAudioProcessorEditor::paint (Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll (getLookAndFeel().findColour (ResizableWindow::backgroundColourId));

    g.setColour (Colours::white);
    g.setFont (15.0f);

#if JUCE_DEBUG
    plot(g, audioProcessor.getSpectralFluxes(), 400, 0, 680, 680, 0.1, false);
#endif
}

void SustainPedalAudioProcessorEditor::resized()
{
    // This is generally where you'll want to lay out the positions of any
    // subcomponents in your editor..
    navbar.setBounds(0, 0, getWidth(), min(getHeight(), 40));
    tooltipBox.setBounds(0, 40, 200, 500);
    
}

bool SustainPedalAudioProcessorEditor::keyPressed(const KeyPress& key, Component*) {
    if (!audioProcessor.isReady() || !audioProcessor.canRun()) return true;

    /*if (pedalWidget->rebinding) {
        pedalWidget->bindButton.setButtonText(key.getTextDescription());
        //audioProcessor.setKeycode(key.getKeyCode());
        vts.state.getChildWithProperty("id", Param::ID::keycode).setProperty("value", var(key.getKeyCode()), nullptr);
        pedalWidget->rebinding = false;
        return true;
    }

    if (key.getKeyCode() == audioProcessor.getKeycode()) {
        if (audioProcessor.getHoldToSustain()) {
            pedalWidget->pedalToggle.setToggleState(true, juce::NotificationType::sendNotification);
        }
        else {
            pedalWidget->pedalToggle.setToggleState(!pedalWidget->pedalToggle.getToggleState(), juce::NotificationType::sendNotification);
        }
    }*/

    return true;
}

bool SustainPedalAudioProcessorEditor::keyStateChanged(bool, Component*) {
    if (audioProcessor.getHoldToSustain() && !KeyPress(audioProcessor.getKeycode()).isCurrentlyDown()) {
        //pedalWidget->pedalToggle.setToggleState(false, juce::NotificationType::sendNotification);
    }
    return true;
}

void SustainPedalAudioProcessorEditor::plot(Graphics& g, std::vector<float> data, int x, int y, int width, int height, double yScale, bool middle) {
    int nPoints = data.size();
    if (middle) y += height / 2; else y += height;
    for (int i = 0; i < nPoints - 1; i++) {
        g.drawLine(x + width * i / nPoints, y - yScale * data[i], x + width * (i + 1) / nPoints, y - yScale * data[i + 1], 1);
    }
}