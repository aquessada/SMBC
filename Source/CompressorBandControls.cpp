/*
  ==============================================================================

    CompressorBandControls.cpp
    Created: 9 Oct 2022 11:30:58am
    Author:  Quessada

  ==============================================================================
*/

#include "CompressorBandControls.h"


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
