/*
  ==============================================================================

    CompressorBand.h
    Created: 9 Oct 2022 11:59:24am
    Author:  Quessada

  ==============================================================================
*/

#pragma once

#include<JuceHeader.h>
#include "GUI/Utilities.h"


struct CompressorBand
{
    juce::AudioParameterFloat* attack{ nullptr };
    juce::AudioParameterFloat* release{ nullptr };
    juce::AudioParameterFloat* threshold{ nullptr };
    juce::AudioParameterChoice* ratio{ nullptr };
    juce::AudioParameterBool* bypassed{ nullptr };
    juce::AudioParameterBool* mute{ nullptr };
    juce::AudioParameterBool* solo{ nullptr };

    void prepare(const juce::dsp::ProcessSpec& spec);

    void updateCompressorSettings();    

    void process(juce::AudioBuffer<float>& buffer);

    //getter functions to atomic part
    float getRMSOutputLevelDb() const { return rmsOutputLevelDb;    }
    float getRMSInputLevelDb() const { return rmsInputLevelDb; }


private:
    juce::dsp::Compressor<float> compressor;

    //we need to use something called atomic to store input, output and RMS
    std::atomic<float> rmsInputLevelDb{ NEGATIVE_INFINITY };
    std::atomic<float> rmsOutputLevelDb{ NEGATIVE_INFINITY };

    //As we have a lot of channels we can create a template to simplify tghe things when getting the RMS levels of each one and after
    template<typename T>
        float computeRMSLevel(const T& buffer)
    {
            int numChannels = static_cast<int>(buffer.getNumChannels());
            int numSamples = static_cast<int>(buffer.getNumSamples());
            auto rms = 0.f;
            for (int chan = 0; chan < numChannels; ++chan)
            {
                rms += buffer.getRMSLevel(chan, 0, numSamples);
            }
            rms /= static_cast<float>(numChannels);
            return rms;
    }

    
};