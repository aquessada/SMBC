/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once



//==============================================================================


struct Placeholder : juce::Component
{


    Placeholder();


    void paint(juce::Graphics& g) override
    {
        g.fillAll(customColor);
    }

    juce::Colour customColor;
};




struct RotarySlider : juce::Slider
{
    RotarySlider() :
        juce::Slider(juce::Slider::SliderStyle::RotaryHorizontalVerticalDrag,
            juce::Slider::TextEntryBoxPosition::NoTextBox)
    { }
};



template<
    typename Attachment,
    typename APVTS,
    typename Params,
    typename ParamName,
    typename SliderType
>
void makeAttachment(std::unique_ptr<Attachment>& attachment,
    APVTS& apvts,
    const Params& params,
    const ParamName& name,
    SliderType& slider)
{
    attachment = std::make_unique<Attachment>(apvts,
        params.at(name),
        slider);
}

template<
    typename APVTS,
    typename Params,
    typename Name
>

juce::RangedAudioParameter& getParam(APVTS& apvts, const Params& params, const Name& name)
{
    auto param = apvts.getParameter(params.at(name));
    jassert(param != nullptr);

    return *param;
}




juce::String getValString(const juce::RangedAudioParameter& param,
    bool getLow,
    juce::String suffix);



template<
    typename Labels,
    typename ParamType,
    typename SuffixType
>

void addLabelPairs(Labels& labels, const ParamType& param, const SuffixType& suffix)
{
    labels.clear();
    labels.add({ 0.f, getValString(param, true, suffix) });
    labels.add({ 1.f, getValString(param, false, suffix) });
}

struct CompressorBandControls : juce::Component, juce::Button::Listener
{
    CompressorBandControls(juce::AudioProcessorValueTreeState& apvts);
    ~CompressorBandControls() override;
    void resized() override;
    void paint(juce::Graphics& g) override;

    void buttonClicked(juce::Button* button) override;
private:
    juce::AudioProcessorValueTreeState& apvts;

    RotarySliderWithLabels attackSlider, releaseSlider, thresholdSlider/*, ratioSlider*/;
    RatioSlider ratioSlider;

    using Attachment = juce::AudioProcessorValueTreeState::SliderAttachment;
    std::unique_ptr<Attachment> attackSliderAttachment,
        releaseSliderAttachment,
        thresholdSliderAttachment,
        ratioSliderAttachment;

    juce::ToggleButton bypassButton, soloButton, muteButton, lowBand, lowMidBand, midHighBand, highBand;

        using BtnAttachment = juce::AudioProcessorValueTreeState::ButtonAttachment;
    std::unique_ptr<BtnAttachment> bypassButtonAttachment,
        soloButtonAttachment,
         muteButtonAttachment;

    juce::Component::SafePointer<CompressorBandControls> safePtr{ this };

    void updateAttachments();
    void updateSliderEnablements();
    void updateSoloMuteBypassToggleStates(juce::Button& clickedButton);
};

struct GlobalControls : juce::Component
{




    GlobalControls(juce::AudioProcessorValueTreeState& apvts);

    void paint(juce::Graphics& g) override;


    void resized() override;


private:

    using RSWL = RotarySliderWithLabels;
    std::unique_ptr<RSWL> inGainSlider, lowMidXoverSlider, midHighXoverSlider, outGainSlider;


    using Attachment = juce::AudioProcessorValueTreeState::SliderAttachment;
    std::unique_ptr<Attachment> lowMidXoverSliderAttachment,
        midHighXoverSliderAttachment,
        inGainSliderAttachment,
        outGainSliderAttachment;
};

/**
*/
class SimpleMBCompAudioProcessorEditor : public juce::AudioProcessorEditor
{
public:
    SimpleMBCompAudioProcessorEditor(SimpleMBCompAudioProcessor&);
    ~SimpleMBCompAudioProcessorEditor() override;

    //==============================================================================
    void paint(juce::Graphics&) override;
    void resized() override;

private:
    LookAndFeel lnf;
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    SimpleMBCompAudioProcessor& audioProcessor;

    Placeholder controlBar, analyzer /*globalControls,*/ /*bandControls*/;
    GlobalControls globalControls{ audioProcessor.apvts };
    CompressorBandControls bandControls{ audioProcessor.apvts };








    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SimpleMBCompAudioProcessorEditor)
};
