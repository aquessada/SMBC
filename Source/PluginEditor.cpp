/*
  ==============================================================================
    This file contains the basic framework code for a JUCE plugin editor.
  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================



//==============================================================================
SimpleMBCompAudioProcessorEditor::SimpleMBCompAudioProcessorEditor(SimpleMBCompAudioProcessor& p)
    : AudioProcessorEditor(&p), audioProcessor(p)
{
    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
    setLookAndFeel(&lnf);
    //    addAndMakeVisible(controlBar);
    addAndMakeVisible(analyzer);

    addAndMakeVisible(globalControls);
    addAndMakeVisible(bandControls);

    setSize(600, 500);

    //set timer construct for the rms level meters
    startTimerHz(60);
}

SimpleMBCompAudioProcessorEditor::~SimpleMBCompAudioProcessorEditor()
{
    setLookAndFeel(nullptr);
}

//==============================================================================
void SimpleMBCompAudioProcessorEditor::paint(juce::Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
//    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));
//
//    g.setColour (juce::Colours::white);
//    g.setFont (15.0f);
//    g.drawFittedText ("Hello World!", getLocalBounds(), juce::Justification::centred, 1);
    g.fillAll(juce::Colours::black);
}

void SimpleMBCompAudioProcessorEditor::resized()
{
    // This is generally where you'll want to lay out the positions of any
    // subcomponents in your editor..

    auto bounds = getLocalBounds();

    controlBar.setBounds(bounds.removeFromTop(32));

    bandControls.setBounds(bounds.removeFromBottom(135));

    analyzer.setBounds(bounds.removeFromTop(225));

    globalControls.setBounds(bounds);
}

void SimpleMBCompAudioProcessorEditor::timerCallback()
{ 
    std::vector<float> values
    {
        audioProcessor.LowBandComp.getRMSInputLevelDb(),
        audioProcessor.LowBandComp.getRMSOutputLevelDb(),
        audioProcessor.LowMidBandComp.getRMSInputLevelDb(),
        audioProcessor.LowMidBandComp.getRMSOutputLevelDb(),
        audioProcessor.MidHighBandComp.getRMSInputLevelDb(),
        audioProcessor.MidHighBandComp.getRMSOutputLevelDb(),
        audioProcessor.HighBandComp.getRMSInputLevelDb(),
        audioProcessor.HighBandComp.getRMSOutputLevelDb()
    };
    analyzer.update(values);
};
