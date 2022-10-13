/*
52 11:59:24am
    Author:  Quessada

  ==============================================================================
*/

#include "CompressorBand.h"

void CompressorBand::prepare(const juce::dsp::ProcessSpec& spec)
{
    compressor.prepare(spec);
}

void CompressorBand::updateCompressorSettings()
{
    compressor.setAttack(attack->get());
    compressor.setRelease(release->get());
    compressor.setThreshold(threshold->get());
    compressor.setRatio(ratio->getCurrentChoiceName().getFloatValue());
}

void CompressorBand::process(juce::AudioBuffer<float>& buffer)
{
    //getting the PRE rms level of the buffer (this is no decibels!)
    auto preRMS = computeRMSLevel(buffer);


    auto block = juce::dsp::AudioBlock<float>(buffer);
    auto context = juce::dsp::ProcessContextReplacing<float>(block);

    context.isBypassed = bypassed->get();

    compressor.process(context);

    //getting the POST rms level of the buffer (this is no decibels!)
    auto postRMS = computeRMSLevel(buffer);

    //transforming the rms levels reading to decibels!
    auto convertToDb = [](auto input)
    {
        return juce::Decibels::gainToDecibels(input);
    };
    //Storing the values then..
    rmsInputLevelDb.store(convertToDb(preRMS));
    rmsOutputLevelDb.store(convertToDb(postRMS));

}
