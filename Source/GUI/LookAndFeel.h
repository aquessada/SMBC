/*
  ==============================================================================

    LookAndFeel.h
    Created: 8 Oct 2022 10:28:24am
    Author:  Quessada

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "RotarySliderWithLabels.h"

#define USE_LIVE_CONSTANT false
#if USE_LIVE_CONSTANT
#define colorHelper(c) JUCE_LIVE_CONSTANT(c);
#else
#define colorHelper(c) c;
#endif

namespace ColorScheme
{
    inline juce::Colour getSliderBorderColor()
    {
        //return colorHelper(juce::Colours::blue);
        //when you find out the hexa colour you want:
        return colorHelper(juce::Colour(0xffffffff));
    }
    
    inline juce::Colour getModuleBorderColor()
    {
        return colorHelper(juce::Colours::blueviolet);
    }


}


struct LookAndFeel : juce::LookAndFeel_V4
{
    void drawRotarySlider(juce::Graphics&,
        int x, int y, int width, int height,
        float sliderPosProportional,
        float rotaryStartAngle,
        float rotaryEndAngle,
        juce::Slider&) override;

    void drawToggleButton(juce::Graphics& g,
        juce::ToggleButton& toggleButton,
        bool shouldDrawButtonAsHighlighted,
        bool shouldDrawButtonAsDown) override;
};