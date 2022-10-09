/*
  ==============================================================================

    CustomButtons.h
    Created: 8 Oct 2022 7:42:14pm
    Author:  Quessada

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

struct PowerButton : juce::ToggleButton { };

struct AnalyzerButton : juce::ToggleButton
{
    void resized() override;

    juce::Path randomPath;
};

