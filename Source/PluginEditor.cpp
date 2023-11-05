/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
SustainPedalAudioProcessorEditor::SustainPedalAudioProcessorEditor(SustainPedalAudioProcessor& p, juce::AudioProcessorValueTreeState& vts) :
    AudioProcessorEditor(&p),
    audioProcessor(p),
    vts(vts),
    navbar({ "Home", "Auto-Trigger" }, this),
    home(vts, tooltipBox),
    tooltipBox()
{
    LookAndFeel::setDefaultLookAndFeel(&lf);
    addAndMakeVisible(navbar);
    addAndMakeVisible(home);
    addAndMakeVisible(tooltipBox);

    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
#if JUCE_DEBUG
    setSize(360, 680);
    startTimer(100);
#else
    setSize (540, 680);
#endif

    /*

    ss = new DonateSplashScreen(audioProcessor);
    if (!audioProcessor.isReady()) addAndMakeVisible(ss);

    addKeyListener(this);
    setWantsKeyboardFocus(true);*/
}

SustainPedalAudioProcessorEditor::~SustainPedalAudioProcessorEditor()
{
    //if (!audioProcessor.isReady()) delete ss;
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
    auto area = getLocalBounds();
    
    navbar.setBounds(area.removeFromTop(navbarHeight));
    tooltipBox.setBounds(area.removeFromBottom(tooltipHeight));
    home.setBounds(area.removeFromLeft(menuWidth));

    //navbar.setBounds(0, 0, getWidth(), getHeight() * navbarHeight);
    //home.setBounds(0, getHeight() * navbarHeight, getWidth() * menuWidth, getHeight() * (1.f - navbarHeight - tooltipHeight));
    //tooltipBox.setBounds(0, getHeight() * (1.f - tooltipHeight), getWidth(), getHeight() * tooltipHeight);
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

void SustainPedalAudioProcessorEditor::actionListenerCallback(const String& message) {
    size_t splitter = message.indexOf(":");
    String category = message.substring(0, splitter),
        body = message.substring(splitter + 1, message.length());

    if (category == "nav") {
        navTo(body);
    }
}

void SustainPedalAudioProcessorEditor::navTo(const String& target) {
    if (target == "Home") {
        shiftMenus(menuWidth);
    }
    else if (target == "Auto-Trigger") {
        shiftMenus(-menuWidth);
    }
}

void SustainPedalAudioProcessorEditor::shiftMenus(const int& offset) {
    auto homeBounds = home.getBounds();//,
        //pedalBounds = pedal.getBounds(),
        //autoTriggerBounds = autoTrigger.getBounds();

    homeBounds.setX(homeBounds.getX() + offset);
    //pedalBounds.setX(pedalBounds.getX() + offset);
    //autoTriggerBounds.setX(autoTriggerBounds.getX() + offset);

    // TODO: timeout the nav bar while animating
    animator.animateComponent(
        &home,
        homeBounds,
        1.f,
        200,
        false,
        1,
        1
    );
}