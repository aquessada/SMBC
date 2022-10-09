/*
  ==============================================================================

    GlobalControls.h
    Created: 9 Oct 2022 11:52:26am
    Author:  Quessada

  ==============================================================================
*/

#pragma once

#include<JuceHeader.h>

struct GlobalControls : juce::Component
{




    GlobalControls(juce::AudioProcessorValueTreeState& apvts);

    void paint(juce::Graphics& g) override;


    void resized() override;


private:

    using RSWL = RotarySliderWithLabels;
    std::unique_ptr<RSWL> inGainSlider, lowMidXoverSlider, midHighXoverSlider, outGainSlider;


    using Attachment = juce::AudioProcessorValueTreeState::SliderAttachment;
    std::unique_ptr<Attachment> lowMidXoverSliderAttachment,
        midHighXoverSliderAttachment,
        inGainSliderAttachment,
        outGainSliderAttachment;
};