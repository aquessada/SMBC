/*
  ==============================================================================
    This file contains the basic framework code for a JUCE plugin editor.
  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

template<typename T>
bool truncateKiloValue(T& value)
{
    if (value > static_cast<T>(999))
    {
        value /= static_cast<T>(1000);
        return true;
    }

    return false;
}

juce::String getValString(const juce::RangedAudioParameter& param,
    bool getLow,
    juce::String suffix)
{
    juce::String str;

    auto val = getLow ? param.getNormalisableRange().start :
        param.getNormalisableRange().end;

    bool useK = truncateKiloValue(val);
    str << val;

    if (useK)
        str << "k";

    str << suffix;

    return str;
}

//==============================================================================
Placeholder::Placeholder()
{
    juce::Random r;
    customColor = juce::Colour(r.nextInt(255), r.nextInt(255), r.nextInt(255));
}
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

void drawModuleBackground(juce::Graphics& g,
    juce::Rectangle<int> bounds)
{
    using namespace juce;
    g.setColour(Colours::blueviolet);
    g.fillAll();

    auto localBounds = bounds;

    bounds.reduce(3, 3);
    g.setColour(Colours::black);
    g.fillRoundedRectangle(bounds.toFloat(), 3);

    g.drawRect(localBounds);
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
    if (&clickedButton == &soloButton &&soloButton.getToggleState())
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

//==============================================================================
GlobalControls::GlobalControls(juce::AudioProcessorValueTreeState& apvts)
{
    using namespace Params;
    const auto& params = GetParams();

    auto getParamHelper = [&params, &apvts](const auto& name) -> auto&
    {
        return getParam(apvts, params, name);
    };

    auto& gainInParam = getParamHelper(Names::Gain_In);
    auto& lowMidParam = getParamHelper(Names::Low_Mid_Crossover_Freq);
    auto& midHighParam = getParamHelper(Names::Mid_High_Crossover_Freq);
    auto& gainOutParam = getParamHelper(Names::Gain_Out);

    inGainSlider = std::make_unique<RSWL>(&gainInParam,
        "dB",
        "INPUT TRIM");
    lowMidXoverSlider = std::make_unique<RSWL>(&lowMidParam,
        "Hz",
        "LOW-MID X-OVER");
    midHighXoverSlider = std::make_unique<RSWL>(&midHighParam,
        "Hz",
        "MID-HI X-OVER");
    outGainSlider = std::make_unique<RSWL>(&gainOutParam,
        "dB",
        "OUTPUT TRIM");


    auto makeAttachmentHelper = [&params, &apvts](auto& attachment,
        const auto& name,
        auto& slider)
    {
        makeAttachment(attachment, apvts, params, name, slider);
    };

    makeAttachmentHelper(inGainSliderAttachment,
        Names::Gain_In,
        *inGainSlider);

    makeAttachmentHelper(lowMidXoverSliderAttachment,
        Names::Low_Mid_Crossover_Freq,
        *lowMidXoverSlider);

    makeAttachmentHelper(midHighXoverSliderAttachment,
        Names::Mid_High_Crossover_Freq,
        *midHighXoverSlider);

    makeAttachmentHelper(outGainSliderAttachment,
        Names::Gain_Out,
        *outGainSlider);

    addLabelPairs(inGainSlider->labels,
        gainInParam,
        "dB");
    addLabelPairs(lowMidXoverSlider->labels,
        lowMidParam,
        "Hz");
    addLabelPairs(midHighXoverSlider->labels,
        midHighParam,
        "Hz");
    addLabelPairs(outGainSlider->labels,
        gainOutParam,
        "dB");

    addAndMakeVisible(*inGainSlider);
    addAndMakeVisible(*lowMidXoverSlider);
    addAndMakeVisible(*midHighXoverSlider);
    addAndMakeVisible(*outGainSlider);
}

void GlobalControls::paint(juce::Graphics& g)
{
    auto bounds = getLocalBounds();

    drawModuleBackground(g, bounds);
}

void GlobalControls::resized()
{
    auto bounds = getLocalBounds().reduced(5);
    using namespace juce;

    FlexBox flexBox;
    flexBox.flexDirection = FlexBox::Direction::row;
    flexBox.flexWrap = FlexBox::Wrap::noWrap;

    auto spacer = FlexItem().withWidth(4);
    auto endCap = FlexItem().withWidth(6);

    flexBox.items.add(endCap);
    flexBox.items.add(FlexItem(*inGainSlider).withFlex(1.f));
    flexBox.items.add(spacer);
    flexBox.items.add(FlexItem(*lowMidXoverSlider).withFlex(1.f));
    flexBox.items.add(spacer);
    flexBox.items.add(FlexItem(*midHighXoverSlider).withFlex(1.f));
    flexBox.items.add(spacer);
    flexBox.items.add(FlexItem(*outGainSlider).withFlex(1.f));
    flexBox.items.add(endCap);

    flexBox.performLayout(bounds);
}
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