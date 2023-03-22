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
};