/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"

//==============================================================================
/**
*/
class PlateMiniProjectAudioProcessorEditor  : public juce::AudioProcessorEditor
{
public:
    PlateMiniProjectAudioProcessorEditor (PlateMiniProjectAudioProcessor&);
    ~PlateMiniProjectAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;
    
    void setSliderAndLabelRotary(juce::Slider& slider, juce::Label& label, const juce::String &Text);

    void setSliderAndLabelHorizontal(juce::Slider& slider, juce::Label& label, const juce::String &Text);

    void setSliderAndLabelVertical(juce::Slider& slider, juce::Label& label, const juce::String &Text);

    void hitButtonClicked();
private:

    PlateMiniProjectAudioProcessor& audioProcessor;
    
    using APVTS = juce::AudioProcessorValueTreeState;
    using sliderAttachment = APVTS::SliderAttachment;
    
    juce::Slider sig0Slider, sig1Slider, lengthXSlider, lengthYSlider, excXSlider, excYSlider, lisXSlider, lisYSlider, excFSlider, excTSlider, thicknessSlider;
    
    juce::Label sig0Label, sig1Label, lengthXLabel, lengthYLabel, excXLabel, excYLabel, lisXLabel, lisYLabel, excFLabel, excTLabel, thicknessLabel, plateMaterialMenuLabel;
    
    juce::ComboBox plateMaterialMenu;
    
    sliderAttachment sig0SliderAttachment, sig1SliderAttachment, lengthXSliderAttachment, lengthYSliderAttachment, excXSliderAttachment, excYSliderAttachment, lisXSliderAttachment, lisYSliderAttachment, thicknessSliderAttachment, excFSliderAttachment, excTSliderAttachment;
    
    juce::TextButton hitButton{"Hit plate"};


    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PlateMiniProjectAudioProcessorEditor)
};
