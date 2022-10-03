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
    5) add 3 remaining compressors
    6) add abillity to mute/solo/bypass each compressor
    7) add input and output gain to offset changes in output level
    8) clean up anything that needs
    */

#include <JuceHeader.h>

namespace Params
{
enum Names
{       
        Bass_Crossover_Freq,
        Master_Crossover_Freq,
        Low_Crossover_Freq,
        Low_Mid_Crossover_Freq,
        Mid_High_Crossover_Freq,
        High_Crossover_Freq,

        Threshold_Low_Band,
        Threshold_Low_Mid_Band,
        Threshold_Mid_High_Band,
        Threshold_High_Band,

        Attack_Low_Band,
        Attack_Low_Mid_Band,
        Attack_Mid_High_Band,
        Attack_High_Band,

        Release_Low_Band,
        Release_Low_Mid_Band,
        Release_Mid_High_Band,
        Release_High_Band,

        Ratio_Low_Band,
        Ratio_Low_Mid_Band,
        Ratio_Mid_High_Band,
        Ratio_High_Band,

        
        Bypassed_Low_Band,
        Bypassed_Low_Mid_Band,
        Bypassed_Mid_High_Band,
        Bypassed_High_Band,
    };
inline const std::map<Names, juce::String>& GetParams()
{
    static std::map<Names, juce::String> params = {

    {Bass_Crossover_Freq,"Bass Crossover Freq"},
    {Master_Crossover_Freq,"Master Crossover Freq"},        
    {Low_Crossover_Freq,"Low Crossover Freq"},
    {Low_Mid_Crossover_Freq, "Low Mid Crossover Freq"},
    {Mid_High_Crossover_Freq, "Mid High Crossover Freq"},
    {High_Crossover_Freq, "High Crossover Freq"},
    {Threshold_Low_Band, "Threshold Low Band"},
    {Threshold_Low_Mid_Band, "Threshold Low Mid Band"},
    {Threshold_Mid_High_Band, "Threshold Mid High Band"},
    {Threshold_High_Band, "Threshold High Band"},
    {Attack_Low_Band, "Attack Low Band"},
    {Attack_Low_Mid_Band, "Attack Low Mid Band"},
    {Attack_Mid_High_Band, "Attack Mid High Band"},
    {Attack_High_Band, "Attack High Band"},
    {Release_Low_Band, "Release Low Band"},
    {Release_Low_Mid_Band, "Release Low Mid Band"},
    {Release_Mid_High_Band, "Release Mid High Band"},
    {Release_High_Band, "Release High Band"},
    {Ratio_Low_Band, "Ratio Low Band"},
    {Ratio_Low_Mid_Band, "Ratio Low Mid Band"},
    {Ratio_Mid_High_Band, "Ratio Mid High Band"},
    {Ratio_High_Band, "Ratio High Band"},
    {Bypassed_Low_Band, "Bypassed Low Band"},
    {Bypassed_Low_Mid_Band, "Bypassed Low Mid Band"},
    {Bypassed_Mid_High_Band, "Bypassed Mid High Band"},
    {Bypassed_High_Band, "Bypassed_High_Band"},
    };
    return params;
}
}



struct CompressorBand
{
    juce::AudioParameterFloat* attack{ nullptr };
    juce::AudioParameterFloat* release{ nullptr };
    juce::AudioParameterFloat* threshold{ nullptr };
    juce::AudioParameterChoice* ratio{ nullptr };
    juce::AudioParameterBool* bypassed{ nullptr };

    void prepare(const juce::dsp::ProcessSpec& spec)
    {
        compressor.prepare(spec);
    }

    void updateCompressorSettings()
    {
        compressor.setAttack(attack->get());
        compressor.setRelease(release->get());
        compressor.setThreshold(threshold->get());
        compressor.setRatio(ratio->getCurrentChoiceName().getFloatValue());
    }

    void process(juce::AudioBuffer<float>& buffer)
    {
        auto block = juce::dsp::AudioBlock<float>(buffer);
        auto context = juce::dsp::ProcessContextReplacing<float>(block);

        context.isBypassed = bypassed->get();

        compressor.process(context);
    }

private:
    juce::dsp::Compressor<float> compressor;
};

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






    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SimpleMBCompAudioProcessor)
};
