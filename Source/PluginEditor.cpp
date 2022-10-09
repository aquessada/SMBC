/*
  ==============================================================================
    This file contains the basic framework code for a JUCE plugin editor.
  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"





//==============================================================================

//==============================================================================

CompressorBandControls::CompressorBandControls(juce::AudioProcessorValueTreeState& apv) :
    apvts(apv),
    attackSlider(nullptr, "ms", "ATTACK"),
    releaseSlider(nullptr, "ms", "RELEASE"),
    thresholdSlider(nullptr, "dB", "THRESH"),
ratioSlider(nullptr, "")
{
    /* using namespace Params;
     const auto& params = GetParams();

     auto getParamHelper = [&params, &apvts = this->apvts](const auto& name) -> auto&
     {
         return getParam(apvts, params, name);
     };*/

     //attackSlider.changeParam(&getParamHelper(Names::Attack_Low_Mid_Band));
     //releaseSlider.changeParam(&getParamHelper(Names::Release_Low_Mid_Band));
     //thresholdSlider.changeParam(&getParamHelper(Names::Threshold_Low_Mid_Band));
     //ratioSlider.changeParam(&getParamHelper(Names::Ratio_Low_Mid_Band));

     //addLabelPairs(attackSlider.labels, getParamHelper(Names::Attack_Low_Mid_Band), "ms");
     //addLabelPairs(releaseSlider.labels, getParamHelper(Names::Release_Low_Mid_Band), "ms");
     //addLabelPairs(thresholdSlider.labels, getParamHelper(Names::Threshold_Low_Mid_Band), "dB");

     //ratioSlider.labels.clear();
     //ratioSlider.labels.add({ 0.f, "1:1" });
     //auto ratioParam = dynamic_cast<juce::AudioParameterChoice*>(&getParamHelper(Names::Ratio_Low_Mid_Band));
     //ratioSlider.labels.add({ 1.0f,
     //    juce::String(ratioParam->choices.getReference(ratioParam->choices.size() - 1).getIntValue()) + ":1" });

    /* auto makeAttachmentHelper = [&params, &apvts = this->apvts](auto& attachment,
         const auto& name,
         auto& slider)
     {
         makeAttachment(attachment, apvts, params, name, slider);
     };

     makeAttachmentHelper(attackSliderAttachment,
         Names::Attack_Low_Mid_Band,
         attackSlider);

     makeAttachmentHelper(releaseSliderAttachment,
         Names::Release_Low_Mid_Band,
         releaseSlider);

     makeAttachmentHelper(thresholdSliderAttachment,
         Names::Threshold_Low_Mid_Band,
         thresholdSlider);

     makeAttachmentHelper(ratioSliderAttachment,
         Names::Ratio_Low_Mid_Band,
         ratioSlider);*/

    addAndMakeVisible(attackSlider);
    addAndMakeVisible(releaseSlider);
    addAndMakeVisible(thresholdSlider);
    addAndMakeVisible(ratioSlider);

    bypassButton.addListener(this);
    soloButton.addListener(this);
    muteButton.addListener(this);

    bypassButton.setName("X");
    soloButton.setName("S");
    muteButton.setName("M");

    addAndMakeVisible(bypassButton);
    addAndMakeVisible(soloButton);
    addAndMakeVisible(muteButton);

    /*makeAttachmentHelper(bypassButtonAttachment, Names::Bypassed_Low_Mid_Band, bypassButton);
    makeAttachmentHelper(soloButtonAttachment,  Names::Solo_Low_Mid_Band, soloButton);
    makeAttachmentHelper(muteButtonAttachment, Names::Mute_Low_Mid_Band, muteButton);*/

    lowBand.setName("Low");
    lowMidBand.setName("Low-Mid");
    midHighBand.setName("Mid_High");
    highBand.setName("High");
    //grouping these buttons togheter so when you select one the others disappear
    lowBand.setRadioGroupId(1);
    lowMidBand.setRadioGroupId(1);
    midHighBand.setRadioGroupId(1);
    highBand.setRadioGroupId(1);

    auto buttonSwitcher = [safePtr = this->safePtr]()
    {
        if (auto* c = safePtr.getComponent())
        {
            c->updateAttachments();
        }
    };

    lowBand.onClick = buttonSwitcher;
    lowMidBand.onClick = buttonSwitcher;
    midHighBand.onClick = buttonSwitcher;
    highBand.onClick = buttonSwitcher;

    lowBand.setToggleState(true, juce::NotificationType::dontSendNotification);

    updateAttachments();

    addAndMakeVisible(lowBand);
    addAndMakeVisible(lowMidBand);
    addAndMakeVisible(midHighBand);
    addAndMakeVisible(highBand);
}

CompressorBandControls::~CompressorBandControls()
{
    bypassButton.removeListener(this);
    soloButton.removeListener(this);
    muteButton.removeListener(this);
}


void CompressorBandControls::resized()
{
    auto bounds = getLocalBounds().reduced(5);
    using namespace juce;

    auto createBandButtonControlBox = [](std::vector<Component*> comps) //for the right positioning boxes
    {
        FlexBox flexBox;
        flexBox.flexDirection = FlexBox::Direction::column;
        flexBox.flexWrap = FlexBox::Wrap::noWrap;

        auto spacer = FlexItem().withHeight(2);

        for (auto* comp : comps)
        {
            flexBox.items.add(spacer);
            flexBox.items.add(FlexItem(*comp).withFlex(1.f));
        }
            flexBox.items.add(spacer);

            return flexBox;        
    };
    //call lambdas now for buttons
    auto bandButtonControlBox = createBandButtonControlBox({&bypassButton, &soloButton, &muteButton});
    auto bandSelectControlBox = createBandButtonControlBox({ &lowBand, &lowMidBand, &midHighBand, &highBand });

    FlexBox flexBox;
    flexBox.flexDirection = FlexBox::Direction::row;
    flexBox.flexWrap = FlexBox::Wrap::noWrap;

    auto spacer = FlexItem().withWidth(4);
    auto endCap = FlexItem().withWidth(6);

    //flexBox.items.add(endCap);
    flexBox.items.add(spacer);
    flexBox.items.add(FlexItem(bandSelectControlBox).withWidth(60));
    flexBox.items.add(spacer);
    flexBox.items.add(FlexItem(attackSlider).withFlex(1.f));
    flexBox.items.add(spacer);
    flexBox.items.add(FlexItem(releaseSlider).withFlex(1.f));
    flexBox.items.add(spacer);
    flexBox.items.add(FlexItem(thresholdSlider).withFlex(1.f));
    flexBox.items.add(spacer);
    flexBox.items.add(FlexItem(ratioSlider).withFlex(1.f));
    //flexBox.items.add(endCap);
    flexBox.items.add(spacer);
    flexBox.items.add(FlexItem(bandButtonControlBox).withWidth(30));

    flexBox.performLayout(bounds);
}




//==============================================================================

//==============================================================================
SimpleMBCompAudioProcessorEditor::SimpleMBCompAudioProcessorEditor(SimpleMBCompAudioProcessor& p)
    : AudioProcessorEditor(&p), audioProcessor(p)
{
    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
    setLookAndFeel(&lnf);
    //    addAndMakeVisible(controlBar);
    //    addAndMakeVisible(analyzer);

    addAndMakeVisible(globalControls);
    addAndMakeVisible(bandControls);

    setSize(600, 500);
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