/*
  ==============================================================================

    Params.h
    Created: 9 Oct 2022 11:55:33am
    Author:  Quessada

  ==============================================================================
*/

#pragma once

#include<JuceHeader.h>

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

        Mute_Low_Band,
        Mute_Low_Mid_Band,
        Mute_Mid_High_Band,
        Mute_High_Band,

        Solo_Low_Band,
        Solo_Low_Mid_Band,
        Solo_Mid_High_Band,
        Solo_High_Band,

        Gain_In,
        Gain_Out


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
        {Bypassed_High_Band, "Bypassed High Band"},
        {Mute_Low_Band, "Mute Low Band" },
        {Mute_Low_Mid_Band, "Mute Low Mid Band"},
        {Mute_Mid_High_Band, "Mute Mid High Band"},
        {Mute_High_Band, "Mute High Band"},
        {Solo_Low_Band, "Solo Low Band" },
        {Solo_Low_Mid_Band, "Solo Low Mid Band"},
        {Solo_Mid_High_Band, "Solo Mid High Band"},
        {Solo_High_Band, "Solo High Band"},
        {Gain_In,"Gain In"},
        {Gain_Out, "Gain Out"}
        };

        return params;
    }
}