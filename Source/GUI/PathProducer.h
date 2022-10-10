/*
  ==============================================================================

    PathProducer.h
    Created: 10 Oct 2022 12:18:06pm
    Author:  Quessada

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "DSP/SingleChannelSampleFifo.h"
#include "GUI/Utilities.h"
#include "GUI/FFTDataGenerator.h"
#include "AnalyzerPathGenerator.h"
#include "PluginProcessor.h"


struct PathProducer
{
    PathProducer(SingleChannelSampleFifo<SimpleMBCompAudioProcessor::BlockType>& scsf) :
        leftChannelFifo(&scsf)
    {
        leftChannelFFTDataGenerator.changeOrder(FFTOrder::order2048);
        monoBuffer.setSize(1, leftChannelFFTDataGenerator.getFFTSize());
    }
    void process(juce::Rectangle<float> fftBounds, double sampleRate);
    juce::Path getPath() { return leftChannelFFTPath; }

    void updateNegativeInfinity(float nf) { negativeInfinity = nf; }
private:
    SingleChannelSampleFifo<SimpleMBCompAudioProcessor::BlockType>* leftChannelFifo;

    juce::AudioBuffer<float> monoBuffer;

    FFTDataGenerator<std::vector<float>> leftChannelFFTDataGenerator;

    AnalyzerPathGenerator<juce::Path> pathProducer;

    juce::Path leftChannelFFTPath;

    float negativeInfinity{ -48.f };


};