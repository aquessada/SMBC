/*
  ==============================================================================

    UtilityComponents.h
    Created: 8 Oct 2022 7:51:31pm
    Author:  Quessada

  ==============================================================================
*/

#pragma once

#include<JuceHeader.h>


struct Placeholder : juce::Component
{
    Placeholder();
    
    void paint(juce::Graphics& g) override;
    
    juce::Colour customColor;
};

struct RotarySlider : juce::Slider
{
    RotarySlider();
       
};

