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

struct LookAndFeel : juce::LookAndFeel_V4
{
    void drawRotarySlider(juce::Graphics&,
        int x, int y, int width, int height,
        float sliderPosProportional,
        float rotaryStartAngle,
        float rotaryEndAngle,
        juce::Slider&) override;
};


struct RotarySliderWithLabels : juce::Slider {
    RotarySliderWithLabels(juce::RangedAudioParameter& rap, const juce::String& unitSuffix) : juce::Slider(juce::Slider::SliderStyle::RotaryHorizontalVerticalDrag, juce::Slider::TextEntryBoxPosition::NoTextBox),
        param(&rap),
        suffix(unitSuffix)
    {
        setLookAndFeel(&lnf);
    }

    ~RotarySliderWithLabels()
    {
        setLookAndFeel(nullptr);
    }

    struct LabelPos
    {
        float pos;
        juce::String label;
    };

    juce::Array<LabelPos> labels;

    void paint(juce::Graphics& g) override;;
    juce::Rectangle<int> getSliderBounds() const;
    int getTextHeight() const { return 14; }
    juce::String getDisplayString() const;

private:
    LookAndFeel lnf;


    juce::RangedAudioParameter* param;
    juce::String suffix;
};

class MastersPluginVer2023AudioProcessorEditor  : public juce::AudioProcessorEditor
{
public:
    MastersPluginVer2023AudioProcessorEditor (MastersPluginVer2023AudioProcessor&);
    ~MastersPluginVer2023AudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;

private:
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    MastersPluginVer2023AudioProcessor& audioProcessor;

    RotarySliderWithLabels macro1Slider,
        macro2Slider,
        macro3Slider,
        macro4Slider,
        macro5Slider,
        macro6Slider,
        macro7Slider,
        macro8Slider;

    using APVTS = juce::AudioProcessorValueTreeState;
    using Attachment = APVTS::SliderAttachment;

    Attachment macro1SliderAttachment,
        macro2SliderAttachment,
        macro3SliderAttachment,
        macro4SliderAttachment,
        macro5SliderAttachment,
        macro6SliderAttachment,
        macro7SliderAttachment,
        macro8SliderAttachment;

    std::vector<juce::Component*> getComps();

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MastersPluginVer2023AudioProcessorEditor)
};
