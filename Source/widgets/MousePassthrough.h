/*
  ==============================================================================

    MousePassthrough.h
    Created: 20 Mar 2023 11:01:52am
    Author:  andre

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

//==============================================================================
/*
*/
class MousePassthrough  : public juce::MouseListener
{
public:
    MousePassthrough(Component& parent) : parent(parent) {}
    ~MousePassthrough() override {}

    void mouseEnter(const MouseEvent& ev) override {
        parent.mouseEnter(ev);
        MouseListener::mouseEnter(ev);
    }

protected:
    Component& parent;

private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MousePassthrough)
};
