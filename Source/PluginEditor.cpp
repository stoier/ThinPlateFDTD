/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
PlateMiniProjectAudioProcessorEditor::PlateMiniProjectAudioProcessorEditor (PlateMiniProjectAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p),
 sig0SliderAttachment(audioProcessor.tree, "Frequency Independent Damping", sig0Slider), sig1SliderAttachment(audioProcessor.tree, "Frequency Dependent Damping", sig1Slider),
lengthXSliderAttachment(audioProcessor.tree, "Plate length X", lengthXSlider),
lengthYSliderAttachment(audioProcessor.tree, "Plate length Y", lengthYSlider),
excXSliderAttachment(audioProcessor.tree, "Excitation pos X", excXSlider),
excYSliderAttachment(audioProcessor.tree, "Excitation pos Y", excYSlider),
lisXSliderAttachment(audioProcessor.tree, "Listening pos X", lisXSlider),
lisYSliderAttachment(audioProcessor.tree, "Listening pos Y", lisYSlider),
thicknessSliderAttachment(audioProcessor.tree, "Plate thickness", thicknessSlider),
excFSliderAttachment(audioProcessor.tree, "Excitation force", excFSlider),
excTSliderAttachment(audioProcessor.tree, "Excitation time", excTSlider)
{

    
    setSliderAndLabelRotary(sig0Slider, sig0Label, "Freq Ind Damp");
    addAndMakeVisible(sig0Slider);
    addAndMakeVisible(sig0Label);
    
    setSliderAndLabelRotary(sig1Slider, sig1Label, "Freq Dep Damp");
    addAndMakeVisible(sig1Slider);
    addAndMakeVisible(sig1Label);
    
    
    setSliderAndLabelHorizontal(lengthXSlider, lengthXLabel, "Plate Length X");
    lengthXSlider.setTextValueSuffix(" m");
    addAndMakeVisible(lengthXSlider);
    addAndMakeVisible(lengthXLabel);
    
    setSliderAndLabelVertical(lengthYSlider, lengthYLabel, "Plate Length Y");
    lengthYSlider.setTextValueSuffix(" m");
    addAndMakeVisible(lengthYSlider);
    addAndMakeVisible(lengthYLabel);
    
    setSliderAndLabelHorizontal(excXSlider, excXLabel, "Excitation Pos X Ratio");
    addAndMakeVisible(excXSlider);
    addAndMakeVisible(excXLabel);
    
    setSliderAndLabelVertical(excYSlider, excYLabel, "Excitation Pos Y Ratio");
    addAndMakeVisible(excYSlider);
    addAndMakeVisible(excYLabel);
    
    setSliderAndLabelHorizontal(lisXSlider, lisXLabel, "Listening Pos X Ratio");
    addAndMakeVisible(lisXSlider);
    addAndMakeVisible(lisXLabel);
    
    setSliderAndLabelVertical(lisYSlider, lisYLabel, "Listening Pos Y Ratio");
    addAndMakeVisible(lisYSlider);
    addAndMakeVisible(lisYLabel);
    
    setSliderAndLabelRotary(thicknessSlider, thicknessLabel, "Thickness");
    thicknessSlider.setTextValueSuffix(" mm");
    addAndMakeVisible(thicknessSlider);
    addAndMakeVisible(thicknessLabel);
    
    setSliderAndLabelRotary(excFSlider, excFLabel, "Excitation Force");
    excFSlider.setTextValueSuffix(" N");
    addAndMakeVisible(excFSlider);
    addAndMakeVisible(excFLabel);
    
    setSliderAndLabelRotary(excTSlider, excTLabel, "Excitation Time");
    excTSlider.setTextValueSuffix(" ms");
    addAndMakeVisible(excTSlider);
    addAndMakeVisible(excTLabel);
    
    hitButton.setColour(juce::TextButton::ColourIds::buttonOnColourId, juce::Colours::purple);
    addAndMakeVisible(hitButton);
    hitButton.onClick = [this] {hitButtonClicked();};
    
    plateMaterialMenu.addItem("Brass", 1);
    plateMaterialMenu.addItem("Bronze", 2);
    plateMaterialMenu.addItem("Iron", 3);
    plateMaterialMenu.addItem("Aluminium", 4);
    plateMaterialMenu.addItem("Gold", 5);
    plateMaterialMenu.addItem("Silver", 6);
    plateMaterialMenu.addItem("Copper", 7);
    plateMaterialMenu.setSelectedId (audioProcessor.plateMaterialId);
    plateMaterialMenu.setSelectedId(1);
    plateMaterialMenu.onChange = [this]
    {
        switch (plateMaterialMenu.getSelectedId())
        {
            case 1: audioProcessor.plateMaterialId = 1; break;
            case 2: audioProcessor.plateMaterialId = 2; break;
            case 3: audioProcessor.plateMaterialId = 3; break;
            case 4: audioProcessor.plateMaterialId = 4; break;
            case 5: audioProcessor.plateMaterialId = 5; break;
            case 6: audioProcessor.plateMaterialId = 6; break;
            case 7: audioProcessor.plateMaterialId = 7; break;
            default: break;
        };
    };
    addAndMakeVisible(plateMaterialMenu);
    plateMaterialMenuLabel.setText("Plate Material:", juce::dontSendNotification);
    plateMaterialMenuLabel.setFont(18.0f);
    plateMaterialMenuLabel.setJustificationType(juce::Justification::left);
    plateMaterialMenuLabel.attachToComponent(&plateMaterialMenu , false);
    
    setResizable(true, true);
    setResizeLimits(300, 250, 1200, 1000);
    getConstrainer() -> setFixedAspectRatio(1.2);
    
    
    setSize (600, 500);
}

PlateMiniProjectAudioProcessorEditor::~PlateMiniProjectAudioProcessorEditor()
{
}

void PlateMiniProjectAudioProcessorEditor::hitButtonClicked()
{
    audioProcessor.hit = true;
}

void PlateMiniProjectAudioProcessorEditor::setSliderAndLabelRotary(juce::Slider& slider, juce::Label& label, const juce::String &Text)
{
    slider.setSliderStyle(juce::Slider::SliderStyle::RotaryVerticalDrag);
    slider.setColour(0x1001311, juce::Colours::purple);
    slider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 60, 20);
    label.setText(Text, juce::dontSendNotification);
    label.setFont(15.0f);
    label.setJustificationType(juce::Justification::centred);
    label.attachToComponent(&slider , false);
}


void PlateMiniProjectAudioProcessorEditor::setSliderAndLabelHorizontal(juce::Slider& slider, juce::Label& label, const juce::String &Text)
{
    slider.setSliderStyle(juce::Slider::SliderStyle::LinearHorizontal);
    slider.setColour(0x1001310, juce::Colours::purple);
    slider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 60, 20);
    label.setText(Text, juce::dontSendNotification);
    label.setFont(15.0f);
    label.setJustificationType(juce::Justification::centred);
    label.attachToComponent(&slider , false);
}


void PlateMiniProjectAudioProcessorEditor::setSliderAndLabelVertical(juce::Slider& slider, juce::Label& label, const juce::String &Text)
{
    slider.setSliderStyle(juce::Slider::SliderStyle::LinearVertical);
    slider.setColour(0x1001310, juce::Colours::purple);
    slider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 60, 20);
    label.setText(Text, juce::dontSendNotification);
    label.setFont(15.0f);
    label.setJustificationType(juce::Justification::centred);
    label.attachToComponent(&slider , false);
}


//==============================================================================
void PlateMiniProjectAudioProcessorEditor::paint (juce::Graphics& g)
{
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));

    g.fillAll (juce::Colours::black);
    g.setColour (juce::Colours::white);
    g.setFont(20.f);
}

void PlateMiniProjectAudioProcessorEditor::resized()
{
    auto excitationArea = getLocalBounds();
    auto plateArea = excitationArea.removeFromTop(excitationArea.getHeight()*0.4);
    plateArea.removeFromTop(plateArea.getHeight()*0.15);
    auto dampingArea0 = plateArea.removeFromLeft(plateArea.getWidth()*0.35);
    dampingArea0.removeFromBottom(dampingArea0.getHeight()*0.4);
    auto dampingArea1 = dampingArea0.removeFromLeft(dampingArea0.getWidth()*0.5);
    sig0Slider.setBounds(dampingArea0);
    sig1Slider.setBounds(dampingArea1);
    auto ThicknessArea =plateArea.removeFromLeft(plateArea.getWidth()*0.25);
    thicknessSlider.setBounds(ThicknessArea.removeFromTop(ThicknessArea.getHeight()*0.6));
    plateArea.removeFromBottom(plateArea.getHeight()*0.3);
    lengthXSlider.setBounds(plateArea.removeFromLeft(plateArea.getWidth()*0.5));
    lengthYSlider.setBounds(plateArea);
    auto excitationArea2 = excitationArea.removeFromTop(excitationArea.getHeight()*0.5);
    excitationArea.removeFromBottom(excitationArea.getHeight()*0.2);
    excXSlider.setBounds(excitationArea.removeFromLeft(excitationArea.getWidth()*0.25));
    excYSlider.setBounds(excitationArea.removeFromLeft(excitationArea.getWidth()*0.33));
    lisXSlider.setBounds(excitationArea.removeFromLeft(excitationArea.getWidth()*0.5));
    lisYSlider.setBounds(excitationArea);
    excitationArea2.removeFromBottom(excitationArea2.getHeight()*0.3);
    excFSlider.setBounds(excitationArea2.removeFromLeft(excitationArea2.getWidth()*0.25));
    excTSlider.setBounds(excitationArea2.removeFromLeft(excitationArea2.getWidth()*0.33));
    hitButton.setBounds(excitationArea2.removeFromLeft(excitationArea2.getWidth()*0.33));
    excitationArea2.removeFromTop(excitationArea2.getHeight()*0.2);
    excitationArea2.removeFromLeft(excitationArea2.getWidth()*0.2);
    excitationArea2.removeFromRight(excitationArea2.getWidth()*0.1);
    plateMaterialMenu.setBounds(excitationArea2);
    
}
