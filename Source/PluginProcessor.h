/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once

/*
    DSP ARIANE TEST Roadmap
    1)figure out audio split in four bands > DONE
    2)create parameters to control where this split happens > DONE
    3)prove that splitting into 4 bands produces no audible artifacts > DONE
    4) create audio parameters for the 4 compressor bands. Need to be inside EACH band  >DONE
    5) add 3 remaining compressors >DONE
    6) add abillity to mute/solo/bypass each compressor > DONE
    7) add input and output gain to offset changes in output level > DONE
    8) clean up anything that needs cleaning > DONE

    GUI ROADMAP
    1)GLobal controls(xovers,gain slilders)
    2)Main band controls(attack, release, threshold, ratio)
    3) add solo/mute/bypass buttons
    4) Band Select
    5) Band select buttons reflect the solo/mute/bypass state
    6)Sliders design
    7)spectrum analysers
    8)data structure for spectrum analysers
    9)Fifo usage in pluginProcessor::processBlock
    10) implementation of the analyzer nrendering pre-compupted paths
    11)Drawing crossovers on top of the analyzer plot
    12)drawing gain reduction meters
    13) analyzer bypass button
    14) Global bypass button

    */

#include <JuceHeader.h>
#include "CompressorBand.h"
//==============================================================================
/**
*/
class SimpleMBCompAudioProcessor  : public juce::AudioProcessor
                            #if JucePlugin_Enable_ARA
                             , public juce::AudioProcessorARAExtension
                            #endif
{
public:
    //==============================================================================
    SimpleMBCompAudioProcessor();
    ~SimpleMBCompAudioProcessor() override;

    //==============================================================================
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

   #ifndef JucePlugin_PreferredChannelConfigurations
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
   #endif

    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    //==============================================================================
    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    //==============================================================================
    const juce::String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    //==============================================================================
    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram (int index) override;
    const juce::String getProgramName (int index) override;
    void changeProgramName (int index, const juce::String& newName) override;

    //==============================================================================
    void getStateInformation (juce::MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;

    using APVTS = juce::AudioProcessorValueTreeState;
    static APVTS::ParameterLayout createParameterLayout();

    APVTS apvts {*this, nullptr, "Parameters", createParameterLayout()};

private:

    //juce::dsp::Compressor<float> compressor;
    //juce::AudioParameterFloat* attack{ nullptr };
    //juce::AudioParameterFloat* release{ nullptr };
    //juce::AudioParameterFloat* threshold{ nullptr };
    //juce::AudioParameterChoice* ratio{ nullptr };
    //juce::AudioParameterBool* bypassed{ nullptr };

    std::array<CompressorBand, 4> compressors; 
    CompressorBand& LowBandComp = compressors[0];
    CompressorBand& LowMidBandComp = compressors[1];
    CompressorBand& MidHighBandComp = compressors[2];
    CompressorBand& HighBandComp = compressors[3];


    using Filter = juce::dsp::LinkwitzRileyFilter<float>;

    //two instances of filter creation for testing
    /*    Filter LP, HP;*/

    //All-pass filter for testing purposes
        //Filter AP;
        //juce::AudioBuffer<float> apBuffer;

    //lesson12 Multifilter
    // //      fc0  fc1
    //Filter  LP1, AP2,
    //        HP1, LP2,
    //             HP2;
    //      fc0  fc1  fc2
    Filter  LP1, LP2, LP3,
            HP1, HP2, HP3,
                 AP2, AP3,
                 AP1, AP4,
                      AP5;
    
    //Creating inverted allpass to each stage for testing
    Filter invAP1, invAP4, invAP5;

    // Creating buffer for these filters
    juce::AudioBuffer<float> invAPBuffer;
                
    //cache  audio parameter for crossover
        juce::AudioParameterFloat* BassCrossover{ nullptr };
        juce::AudioParameterFloat* MasterCrossover{ nullptr };
        juce::AudioParameterFloat* LowCrossover{ nullptr };
        juce::AudioParameterFloat* LowMidCrossover{ nullptr };
        juce::AudioParameterFloat* MidHighCrossover{ nullptr };
        juce::AudioParameterFloat* HighCrossover{ nullptr };

        //juce::AudioParameterFloat* LowMidCrossover{ nullptr };
        //juce::AudioParameterFloat* MidHighCrossover{ nullptr };



    // 4 (FOUR bands) buffer creation for filters (all audio spectrum to each filter!!!)
        std::array<juce::AudioBuffer<float>,10> filterBuffers; 

    //Declaring Gains
        juce::dsp::Gain<float> inputGain, outputGain;
        juce::AudioParameterFloat* inputGainParam{ nullptr };
        juce::AudioParameterFloat* outputGainParam{ nullptr };


        //Apllying a helper function using a template that figures out the correct functions
        template<typename T, typename U>
        void applyGain(T& buffer, U& gain)
        {
            auto block = juce::dsp::AudioBlock<float>(buffer);
            auto ctx = juce::dsp::ProcessContextReplacing<float>(block);
            gain.process(ctx);
        }






    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SimpleMBCompAudioProcessor)
};
