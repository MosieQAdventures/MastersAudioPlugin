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

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MastersPluginVer2023AudioProcessorEditor)
};
