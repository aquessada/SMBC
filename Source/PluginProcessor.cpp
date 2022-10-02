/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
SimpleMBCompAudioProcessor::SimpleMBCompAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
                     #endif
                       )
#endif
{
    using namespace Params;
    const auto& params = GetParams();

    auto floatHelper = [&apvts = this->apvts, &params](auto& param, const auto& paramName)
    {
        param = dynamic_cast<juce::AudioParameterFloat*>(apvts.getParameter(params.at(paramName)));
        jassert(param != nullptr);
    };

    floatHelper(compressor.attack, Names::Attack_Low_Band);
    floatHelper(compressor.release, Names::Release_Low_Band);
    floatHelper(compressor.threshold, Names::Threshold_Low_Band);

    auto choiceHelper = [&apvts = this->apvts, &params](auto& param, const auto& paramName)
    {
        param = dynamic_cast<juce::AudioParameterChoice*>(apvts.getParameter(params.at(paramName)));
        jassert(param != nullptr);
    };

    choiceHelper(compressor.ratio, Names::Ratio_Low_Band);

    auto boolHelper = [&apvts = this->apvts, &params](auto& param, const auto& paramName)
    {
        param = dynamic_cast<juce::AudioParameterBool*>(apvts.getParameter(params.at(paramName)));
        jassert(param != nullptr);
    };

    boolHelper(compressor.bypassed, Names::Bypassed_Low_Band);

    //initializing filters parameters
    floatHelper(BassCrossover, Names::Bass_Crossover_Freq);
    floatHelper(MasterCrossover, Names::Master_Crossover_Freq);
    floatHelper(LowCrossover, Names::Low_Crossover_Freq);
    floatHelper(LowMidCrossover, Names::Low_Mid_Crossover_Freq);
    floatHelper(MidHighCrossover, Names::Mid_High_Crossover_Freq);
    floatHelper(HighCrossover, Names::High_Crossover_Freq);

    //Configure Filters types
    LP1.setType(juce::dsp::LinkwitzRileyFilterType::lowpass);
    HP1.setType(juce::dsp::LinkwitzRileyFilterType::highpass); 

    LP2.setType(juce::dsp::LinkwitzRileyFilterType::lowpass);
    HP2.setType(juce::dsp::LinkwitzRileyFilterType::highpass);

    LP3.setType(juce::dsp::LinkwitzRileyFilterType::lowpass);
    HP3.setType(juce::dsp::LinkwitzRileyFilterType::highpass);

    AP2.setType(juce::dsp::LinkwitzRileyFilterType::allpass);   
    AP3.setType(juce::dsp::LinkwitzRileyFilterType::allpass);

    AP1.setType(juce::dsp::LinkwitzRileyFilterType::allpass);
    AP4.setType(juce::dsp::LinkwitzRileyFilterType::allpass);
    AP5.setType(juce::dsp::LinkwitzRileyFilterType::allpass);

    invAP1.setType(juce::dsp::LinkwitzRileyFilterType::allpass);
    invAP4.setType(juce::dsp::LinkwitzRileyFilterType::allpass);
    invAP5.setType(juce::dsp::LinkwitzRileyFilterType::allpass);
}

SimpleMBCompAudioProcessor::~SimpleMBCompAudioProcessor()
{
}

//==============================================================================
const juce::String SimpleMBCompAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool SimpleMBCompAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool SimpleMBCompAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool SimpleMBCompAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double SimpleMBCompAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int SimpleMBCompAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int SimpleMBCompAudioProcessor::getCurrentProgram()
{
    return 0;
}

void SimpleMBCompAudioProcessor::setCurrentProgram (int index)
{
}

const juce::String SimpleMBCompAudioProcessor::getProgramName (int index)
{
    return {};
}

void SimpleMBCompAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}

//==============================================================================
void SimpleMBCompAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    // Use this method as the place to do any pre-playback
    // initialisation that you need..
    juce::dsp::ProcessSpec spec;
    spec.maximumBlockSize = samplesPerBlock;
    spec.numChannels = getTotalNumOutputChannels();
    spec.sampleRate = sampleRate;

    compressor.prepare(spec);

    //PreparetoPlay FILTERS
    LP1.prepare(spec);
    HP1.prepare(spec);

    LP2.prepare(spec);
    HP2.prepare(spec);

    LP3.prepare(spec);
    HP3.prepare(spec);

    AP2.prepare(spec);
    AP3.prepare(spec);

    AP4.prepare(spec);
    AP5.prepare(spec);
    
    invAP1.prepare(spec);
    invAP4.prepare(spec);
    invAP5.prepare(spec);

    invAPBuffer.setSize(spec.numChannels, samplesPerBlock);
 
    //prepare the buffers!!
    for (auto& buffer : filterBuffers)
    {
        buffer.setSize(spec.numChannels, samplesPerBlock);
    }
}

void SimpleMBCompAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool SimpleMBCompAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    juce::ignoreUnused (layouts);
    return true;
  #else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    // Some plugin hosts, such as certain GarageBand versions, will only
    // load plugins that support stereo bus layouts.
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

    // This checks if the input layout matches the output layout
   #if ! JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
   #endif

    return true;
  #endif
}
#endif

void SimpleMBCompAudioProcessor::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    // In case we have more outputs than inputs, this code clears any output
    // channels that didn't contain input data, (because these aren't
    // guaranteed to be empty - they may contain garbage).
    // This is here to avoid people getting screaming feedback
    // when they first compile a plugin, but obviously you don't need to keep
    // this code if your algorithm always overwrites all the output channels.
    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear(i, 0, buffer.getNumSamples());

    //compressor.setAttack(attack->get());
    //compressor.setRelease(release->get());
    //compressor.setThreshold(threshold->get());
    //compressor.setRatio(ratio->getCurrentChoiceName().getFloatValue());


    //auto block = juce::dsp::AudioBlock<float>(buffer);
    //auto context = juce::dsp::ProcessContextReplacing<float>(block);

    //context.isBypassed = bypassed->get();

    //compressor.process(context);

    //compressor.updateCompressorSettings();
    //compressor.process(buffer);

    //copying buffers!! the buffer is the audio FROM soundcard
    for (auto& fb : filterBuffers)
    {
        fb = buffer;
    }

    //Preparing the inverted testing filters
    invAPBuffer = buffer;


    //Updating filter cutoffs!!
    auto BasscutoffFreq = BassCrossover->get();
    LP1.setCutoffFrequency(BasscutoffFreq);
    HP1.setCutoffFrequency(BasscutoffFreq);
    invAP1.setCutoffFrequency(BasscutoffFreq);
  
    

    auto MastercutoffFreq = MasterCrossover->get();
    invAP4.setCutoffFrequency(MastercutoffFreq);

    
  //  LP2.setCutoffFrequency(MastercutoffFreq);
  //  HP2.setCutoffFrequency(MastercutoffFreq);

    auto LowcutoffFreq = LowCrossover->get();
    AP2.setCutoffFrequency(LowcutoffFreq);
    LP2.setCutoffFrequency(LowcutoffFreq);
    HP2.setCutoffFrequency(LowcutoffFreq);
    //invAP1.setCutoffFrequency(LowcutoffFreq);
    //LP1.setCutoffFrequency(LowcutoffFreq);
    //HP1.setCutoffFrequency(LowcutoffFreq);


   auto LowMidcutoffFreq = LowMidCrossover->get();
  
   //HP2.setCutoffFrequency(LowMidcutoffFreq);
   // LP1.setCutoffFrequency(LowMidcutoffFreq);
   // HP1.setCutoffFrequency(LowMidcutoffFreq);
   //AP2.setCutoffFrequency(LowMidcutoffFreq);
   //AP3.setCutoffFrequency(LowMidcutoffFreq);
   // //AP5.setCutoffFrequency(LowMidcutoffFreq);
   // //AP4.setCutoffFrequency(LowMidcutoffFreq);
   // // invAP1.setCutoffFrequency(LowMidcutoffFreq);

   auto MidHighcutoffFreq = MidHighCrossover->get();
    //LP3.setCutoffFrequency(MidHighcutoffFreq);
    //HP3.setCutoffFrequency(MidHighcutoffFreq);
   //AP3.setCutoffFrequency(MidHighcutoffFreq);
   // //AP3.setCutoffFrequency(MidHighcutoffFreq);
   // //AP4.setCutoffFrequency(MidHighcutoffFreq);
   // //AP5.setCutoffFrequency(MidHighcutoffFreq);
   // //AP6.setCutoffFrequency(MidHighcutoffFreq);
   // //invAP2.setCutoffFrequency(MidHighcutoffFreq);

   // 
   auto HighcutoffFreq = HighCrossover->get();
   AP3.setCutoffFrequency(HighcutoffFreq);
   LP3.setCutoffFrequency(HighcutoffFreq);
   HP3.setCutoffFrequency(HighcutoffFreq);
   
   //// AP3.setCutoffFrequency(HighcutoffFreq);
   //invAP4.setCutoffFrequency(HighcutoffFreq);
   // LP3.setCutoffFrequency(HighcutoffFreq);
   // HP3.setCutoffFrequency(HighcutoffFreq);
    
    //invAP3.setCutoffFrequency(HighcutoffFreq); //after this we need to create filters biffers and context to work  

    //Creating blocks and contexts for the filters. One buffer of audio created FOUR BAND output buffers to be processed
    auto fb0Block = juce::dsp::AudioBlock<float>(filterBuffers[0]);
    auto fb1Block = juce::dsp::AudioBlock<float>(filterBuffers[1]);
    auto fb2Block = juce::dsp::AudioBlock<float>(filterBuffers[2]);
    auto fb3Block = juce::dsp::AudioBlock<float>(filterBuffers[3]);
    auto fb4Block = juce::dsp::AudioBlock<float>(filterBuffers[4]);
    auto fb5Block = juce::dsp::AudioBlock<float>(filterBuffers[5]);

    auto fb0Ctx = juce::dsp::ProcessContextReplacing<float>(fb0Block);
    auto fb1Ctx = juce::dsp::ProcessContextReplacing<float>(fb1Block);
    auto fb2Ctx = juce::dsp::ProcessContextReplacing<float>(fb2Block);
    auto fb3Ctx = juce::dsp::ProcessContextReplacing<float>(fb3Block);
    auto fb4Ctx = juce::dsp::ProcessContextReplacing<float>(fb4Block);
    auto fb5Ctx = juce::dsp::ProcessContextReplacing<float>(fb5Block);

    //Now we can process our audio using filters!

    //Bass Filter Stage
    LP1.process(fb0Ctx);
    AP3.process(fb0Ctx);
    filterBuffers[2] = filterBuffers[0];
    filterBuffers[4] = filterBuffers[0];

    //Master Filter Stage
    HP1.process(fb1Ctx);
    AP2.process(fb1Ctx);
    filterBuffers[3] = filterBuffers[1];
    filterBuffers[5] = filterBuffers[1];

    //LOW and MIDLOW filtering
    LP2.process(fb2Ctx);
    HP2.process(fb4Ctx);

    //MIDHIGH and HIGH filtering
    LP3.process(fb3Ctx);
    HP3.process(fb5Ctx);
    
    //AP2.process(fb0Ctx);

    //HP1.process(fb1Ctx);
   // filterBuffers[2] = filterBuffers[1];
    //LP2.process(fb0Ctx);
    

    ////Band1 - Low
    //LP1.process(fb0Ctx);
    //HP1.process(fb1Ctx);
    //LP2.process(fb0Ctx);
   // //AP2.process(fb0Ctx);
   //// AP4.process(fb0Ctx);
   // 
   // 
   // ////we needt o filter HP1 to be able to split it to other bands
   // HP1.process(fb1Ctx);
   // 
   // //// copying output of HP1 to others filters...
   // filterBuffers[2] = filterBuffers[1];
   // filterBuffers[3] = filterBuffers[1];

   // //Band2 - LowMid
   //// AP4.process(fb1Ctx); 
   // LP2.process(fb1Ctx);
   // AP5.process(fb1Ctx);
   // 
  
   // ////Band3 - MidHigh
   // //AP3.process(fb2Ctx);
   // HP2.process(fb2Ctx);
   // LP3.process(fb2Ctx);

   // ////Band4 - High
   // AP3.process(fb3Ctx);
   // AP6.process(fb3Ctx);
   // HP3.process(fb3Ctx);
   // 
   // 
   //
    
    

    auto invAPBlock = juce::dsp::AudioBlock<float>(invAPBuffer);
    auto invAPCtx = juce::dsp::ProcessContextReplacing<float>(invAPBlock);

    invAP1.process(invAPCtx);
    invAP4.process(invAPCtx);
    //invAP3.process(invAPCtx);

    //Caching details of the input buffers
    auto numSamples = buffer.getNumSamples();
    auto numChannels = buffer.getNumChannels();

    ////IF the compressor is bypassed dont clear, leave the buffer / testing
    //if (compressor.bypassed->get())
    //    return;

    //Cleaning buffers before adding into filters
    buffer.clear();

    //Each channel of filter buffer needs to be copied back to input buffer
    auto addFilterBand = [nc = numChannels, ns = numSamples](auto& inputBuffer, const auto& source)  //(goes to, copy from)
    {
        for (auto i = 0; i < nc; ++i) //loop thru all the channel of the input buffer and copy from the source buffer into that
        {
         inputBuffer.addFrom(i, 0, source, i, 0, ns);
        }
    };

    // filters summing  
     addFilterBand(buffer, filterBuffers[0]); //BASS
     addFilterBand(buffer, filterBuffers[1]); //MASTER

    //addFilterBand(buffer, filterBuffers[2]);  //LOW
    //addFilterBand(buffer, filterBuffers[4]); //MIDLOW
    // addFilterBand(buffer, filterBuffers[3]);  //MIDHIGH
    // addFilterBand(buffer, filterBuffers[5]); //HIGH
    //addFilterBand(buffer, filterBuffers[3]);

    //Testing filter linearity inverting the ALLpass filter to zero the audio
    if (compressor.bypassed->get())
    {
        for (auto ch = 0; ch < numChannels; ++ch)
        {
            juce::FloatVectorOperations::multiply(invAPBuffer.getWritePointer(ch), -1.f, numSamples);
       }
        addFilterBand(buffer, invAPBuffer);
    }
}

//==============================================================================
bool SimpleMBCompAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* SimpleMBCompAudioProcessor::createEditor()
{
 //   return new SimpleMBCompAudioProcessorEditor (*this);
      return new juce::GenericAudioProcessorEditor (*this);
}

//==============================================================================
void SimpleMBCompAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
    juce::MemoryOutputStream mos(destData, true);
    apvts.state.writeToStream(mos);
}

void SimpleMBCompAudioProcessor::setStateInformation(const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
    auto tree = juce::ValueTree::readFromData(data, sizeInBytes);
    if ( tree.isValid())
    {
        apvts.replaceState(tree);
    }
}
   
juce::AudioProcessorValueTreeState::ParameterLayout SimpleMBCompAudioProcessor::createParameterLayout()
{
    APVTS::ParameterLayout layout;

    using namespace juce;
    using namespace Params;
    const auto& params = GetParams();

    layout.add(std::make_unique<AudioParameterFloat>(params.at(Names::Threshold_Low_Band), params.at(Names::Threshold_Low_Band), NormalisableRange<float>(-80, 12, 1, 1), 0));

    auto attackReleaseRange = NormalisableRange<float>(5, 500, 1, 1);

    layout.add(std::make_unique<AudioParameterFloat>(params.at(Names::Attack_Low_Band), params.at(Names::Attack_Low_Band), attackReleaseRange, 50));

    layout.add(std::make_unique<AudioParameterFloat>(params.at(Names::Release_Low_Band), params.at(Names::Release_Low_Band), attackReleaseRange, 250));

    auto choices = std::vector<double>{ 1, 1.5, 2, 3, 4, 5, 6, 7, 8, 10, 15, 20, 50, 100 };
    juce::StringArray sa;
    for ( auto choice : choices )
    {
        sa.add( juce::String(choice, 1) );
    }

    layout.add(std::make_unique<AudioParameterChoice>(params.at(Names::Ratio_Low_Band), params.at(Names::Ratio_Low_Band), sa, 3));

    layout.add(std::make_unique<AudioParameterBool>(params.at(Names::Bypassed_Low_Band), params.at(Names::Bypassed_Low_Band), false));

    //Creating Filters parameters!!!
    //    layout.add(std::make_unique<AudioParameterFloat>(params.at(Names::Low_Mid_Crossover_Freq),
    //                                                 params.at(Names::Low_Mid_Crossover_Freq),
    //                                                 NormalisableRange<float>(20, 999, 1, 1),
    //                                                 400));
    //
    //layout.add(std::make_unique<AudioParameterFloat>(params.at(Names::Mid_High_Crossover_Freq),
    //                                                 params.at(Names::Mid_High_Crossover_Freq),
    //                                                 NormalisableRange<float>(1000, 6299, 1, 1),
    //                                                 2000));

    //layout.add(std::make_unique<AudioParameterFloat>(params.at(Names::High_Crossover_Freq),
    //                                                params.at(Names::High_Crossover_Freq),
    //                                                 NormalisableRange<float>(6300, 20000, 1, 1),
    //                                                    8000));


    layout.add(std::make_unique<AudioParameterFloat>(params.at(Names::Bass_Crossover_Freq), params.at(Names::Bass_Crossover_Freq), NormalisableRange<float>(20, 650, 1, 1), 650));
    
    layout.add(std::make_unique<AudioParameterFloat>(params.at(Names::Master_Crossover_Freq), params.at(Names::Master_Crossover_Freq), NormalisableRange<float>(650, 20000, 1, 1), 650));

    layout.add(std::make_unique<AudioParameterFloat>(params.at(Names::Low_Crossover_Freq), params.at(Names::Low_Crossover_Freq), NormalisableRange<float>(20, 130, 1, 1), 130));

    layout.add(std::make_unique<AudioParameterFloat>(params.at(Names::Low_Mid_Crossover_Freq), params.at(Names::Low_Mid_Crossover_Freq), NormalisableRange<float>(130, 650, 1, 1), 130));

    layout.add(std::make_unique<AudioParameterFloat>(params.at(Names::Mid_High_Crossover_Freq), params.at(Names::Mid_High_Crossover_Freq), NormalisableRange<float>(650, 1600, 1, 1), 650));

    layout.add(std::make_unique<AudioParameterFloat>(params.at(Names::High_Crossover_Freq), params.at(Names::High_Crossover_Freq), NormalisableRange<float>(1600, 20000, 1, 1), 1600));


    return layout;
}
      

    


//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new SimpleMBCompAudioProcessor();
}
