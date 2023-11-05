/*
  ==============================================================================

	CustomLookAndFeel.h
	Created: 18 Mar 2023 11:04:25pm
	Author:  andre

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

class CustomLookAndFeel : public LookAndFeel_V4 {
public:
	CustomLookAndFeel() {
		ColourScheme cs = getCurrentColourScheme();
		cs.setUIColour(ColourScheme::windowBackground, Colour(0xFF101010));
		cs.setUIColour(ColourScheme::widgetBackground, Colour(0xFF202020));
		cs.setUIColour(ColourScheme::menuBackground, Colour(0xFF101010));
		cs.setUIColour(ColourScheme::outline, Colour(0xFFB0B0B0));
		cs.setUIColour(ColourScheme::defaultText, Colour(0xFFD0D0D0));
		cs.setUIColour(ColourScheme::defaultFill, Colour(0xFFA0A0A0));
		cs.setUIColour(ColourScheme::highlightedText, Colour(0xFFF7F2A8));
		cs.setUIColour(ColourScheme::highlightedFill, Colour(0xFFF7F2A8));
		cs.setUIColour(ColourScheme::menuText, Colour(0xFFD0D0D0));
		setColourScheme(cs);
	}

	void drawGroupComponentOutline(Graphics& g, int w, int h, const String& text, const Justification& justification, GroupComponent&) override {
		float textHeight = juce::Label().getFont().getHeight(),
			textWidth = juce::Label().getFont().getStringWidth(text),
			lineY = textHeight / 2;
		juce::Rectangle<float> textBox((w - textWidth) / 2.f, 0.f, textWidth, textHeight);
		juce::Colour textColour = getCurrentColourScheme().getUIColour(ColourScheme::defaultText);

		// TODO: set colour using lookAndFeel
		g.setColour(textColour);
		// draw divider line
		g.drawHorizontalLine(lineY, 0, w);
		// clear out an area for the text
		g.setColour(findColour(juce::ResizableWindow::backgroundColourId));
		g.fillRect(textBox);
		// draw text
		g.setColour(textColour);
		g.drawText(text, textBox, justification);
	}
};