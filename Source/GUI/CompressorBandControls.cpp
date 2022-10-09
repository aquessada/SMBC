/*
  ==============================================================================

    CompressorBandControls.cpp
    Created: 9 Oct 2022 11:30:58am
    Author:  Quessada

  ==============================================================================
*/

#include "CompressorBandControls.h"
#include "Utilities.h"
#include "Params.h"

CompressorBandControls::CompressorBandControls(juce::AudioProcessorValueTreeState& apv) :
    apvts(apv),
    attackSlider(nullptr, "ms", "ATTACK"),
    releaseSlider(nullptr, "ms", "RELEASE"),
    thresholdSlider(nullptr, "dB", "THRESH"),
    ratioSlider(nullptr, "")
{
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
    auto bandButtonControlBox = createBandButtonControlBox({ &bypassButton, &soloButton, &muteButton });
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

void CompressorBandControls::paint(juce::Graphics& g)
{
    auto bounds = getLocalBounds();
    drawModuleBackground(g, bounds);
}

void CompressorBandControls::buttonClicked(juce::Button* button)
{
    updateSliderEnablements();
    updateSoloMuteBypassToggleStates(*button);
}

void CompressorBandControls::updateSliderEnablements()
{
    auto disabled = muteButton.getToggleState() || bypassButton.getToggleState();
    attackSlider.setEnabled(!disabled);
    releaseSlider.setEnabled(!disabled);
    thresholdSlider.setEnabled(!disabled);
    ratioSlider.setEnabled(!disabled);
}

void CompressorBandControls::updateSoloMuteBypassToggleStates(juce::Button& clickedButton)
{
    if (&clickedButton == &soloButton && soloButton.getToggleState())
    {
        bypassButton.setToggleState(false, juce::NotificationType::sendNotification);
        muteButton.setToggleState(false, juce::NotificationType::sendNotification);
    }
    else if (&clickedButton == &muteButton && muteButton.getToggleState())
    {
        bypassButton.setToggleState(false, juce::NotificationType::sendNotification);
        soloButton.setToggleState(false, juce::NotificationType::sendNotification);
    }
    else if (&clickedButton == &bypassButton && bypassButton.getToggleState())
    {
        muteButton.setToggleState(false, juce::NotificationType::sendNotification);
        soloButton.setToggleState(false, juce::NotificationType::sendNotification);
    }
}

void CompressorBandControls::updateAttachments()
{   //figuring out which button was clicked
    enum BandType
    {
        Low,
        LowMid,
        MidHigh,
        High
    };

    BandType bandType = [this]()
    {
        if (lowBand.getToggleState())
            return BandType::Low;
        if (lowMidBand.getToggleState())
            return BandType::LowMid;
        if (midHighBand.getToggleState())
            return BandType::MidHigh;

        return BandType::High;
    }();

    using namespace Params;
    std::vector<Names> names;

    switch (bandType)
    {
    case Low:
    {
        names = std::vector<Names>
        {
            Names::Attack_Low_Band,
                    Release_Low_Band,
                    Threshold_Low_Band,
                    Ratio_Low_Band,
                    Mute_Low_Band,
                    Solo_Low_Band,
                    Bypassed_Low_Band
        };
        break;
    }

    case LowMid:
    {
        names = std::vector<Names>
        {
            Names::Attack_Low_Mid_Band,
                    Release_Low_Mid_Band,
                    Threshold_Low_Mid_Band,
                    Ratio_Low_Mid_Band,
                    Mute_Low_Mid_Band,
                    Solo_Low_Mid_Band,
                    Bypassed_Low_Mid_Band
        };
        break;
    }

    case MidHigh:
    {
        names = std::vector<Names>
        {
            Names::Attack_Mid_High_Band,
                    Release_Mid_High_Band,
                    Threshold_Mid_High_Band,
                    Ratio_Mid_High_Band,
                    Mute_Mid_High_Band,
                    Solo_Mid_High_Band,
                    Bypassed_Mid_High_Band
        };
        break;
    }

    case High:
    {
        names = std::vector<Names>
        {
            Names::Attack_High_Band,
                    Release_High_Band,
                    Threshold_High_Band,
                    Ratio_High_Band,
                    Mute_High_Band,
                    Solo_High_Band,
                    Bypassed_High_Band
        };
        break;
    }
    }

    enum Pos
    {
        Attack,
        Release,
        Threshold,
        Ratio,
        Mute,
        Solo,
        Bypass
    };

    const auto& params = GetParams();

    auto getParamHelper = [&params, &apvts = this->apvts, names](const auto& pos) -> auto&
    {
        return getParam(apvts, params, names.at(pos));
    };

    attackSliderAttachment.reset();
    releaseSliderAttachment.reset();
    thresholdSliderAttachment.reset();
    ratioSliderAttachment.reset();
    bypassButtonAttachment.reset();
    soloButtonAttachment.reset();
    muteButtonAttachment.reset();

    auto& attackParam = getParamHelper(Pos::Attack);
    addLabelPairs(attackSlider.labels, attackParam, "ms");
    attackSlider.changeParam(&attackParam);

    auto& releaseParam = getParamHelper(Pos::Release);
    addLabelPairs(releaseSlider.labels, releaseParam, "ms");
    releaseSlider.changeParam(&releaseParam);

    auto& threshParam = getParamHelper(Pos::Threshold);
    addLabelPairs(thresholdSlider.labels, threshParam, "dB");
    thresholdSlider.changeParam(&threshParam);

    auto& ratioParamRap = getParamHelper(Pos::Ratio);
    ratioSlider.labels.clear();
    ratioSlider.labels.add({ 0.f, "1:1" });
    auto ratioParam = dynamic_cast<juce::AudioParameterChoice*>(&getParamHelper(Pos::Ratio));//Wasnt previous in Names! Wrongly
    ratioSlider.labels.add({ 1.0f,
        juce::String(ratioParam->choices.getReference(ratioParam->choices.size() - 1).getIntValue()) + ":1" });
    ratioSlider.changeParam(ratioParam);

    auto makeAttachmentHelper = [&params, &apvts = this->apvts](auto& attachment,
        const auto& name,
        auto& slider)
    {
        makeAttachment(attachment, apvts, params, name, slider);
    };

    makeAttachmentHelper(attackSliderAttachment, names[Pos::Attack], attackSlider);
    makeAttachmentHelper(releaseSliderAttachment, names[Pos::Release], releaseSlider);
    makeAttachmentHelper(thresholdSliderAttachment, names[Pos::Threshold], thresholdSlider);
    makeAttachmentHelper(ratioSliderAttachment, names[Pos::Ratio], ratioSlider);
    makeAttachmentHelper(bypassButtonAttachment, names[Pos::Bypass], bypassButton);
    makeAttachmentHelper(soloButtonAttachment, names[Pos::Solo], soloButton);
    makeAttachmentHelper(muteButtonAttachment, names[Pos::Mute], muteButton);
}
