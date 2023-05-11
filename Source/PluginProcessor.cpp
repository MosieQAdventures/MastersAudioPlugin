/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
MastersPluginVer2023AudioProcessor::MastersPluginVer2023AudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
                     #endif
                       )
#endif
{
}

MastersPluginVer2023AudioProcessor::~MastersPluginVer2023AudioProcessor()
{
}

//==============================================================================
const juce::String MastersPluginVer2023AudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool MastersPluginVer2023AudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool MastersPluginVer2023AudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool MastersPluginVer2023AudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double MastersPluginVer2023AudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int MastersPluginVer2023AudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int MastersPluginVer2023AudioProcessor::getCurrentProgram()
{
    return 0;
}

void MastersPluginVer2023AudioProcessor::setCurrentProgram (int index)
{
}

const juce::String MastersPluginVer2023AudioProcessor::getProgramName (int index)
{
    return {};
}

void MastersPluginVer2023AudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}

//==============================================================================
void MastersPluginVer2023AudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    // Use this method as the place to do any pre-playback
    // initialisation that you need..
}

void MastersPluginVer2023AudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool MastersPluginVer2023AudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    juce::ignoreUnused (layouts);
    return true;
  #else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    // Some plugin hosts, such as certain GarageBand versions, will only
    // load plugins that support stereo bus layouts.
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

    // This checks if the input layout matches the output layout
   #if ! JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
   #endif

    return true;
  #endif
}
#endif

void MastersPluginVer2023AudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    // In case we have more outputs than inputs, this code clears any output
    // channels that didn't contain input data, (because these aren't
    // guaranteed to be empty - they may contain garbage).
    // This is here to avoid people getting screaming feedback
    // when they first compile a plugin, but obviously you don't need to keep
    // this code if your algorithm always overwrites all the output channels.
    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear (i, 0, buffer.getNumSamples());

    // This is the place where you'd normally do the guts of your plugin's
    // audio processing...
    // Make sure to reset the state if your inner loop is processing
    // the samples and the outer loop is handling the channels.
    // Alternatively, you can process the samples with the channels
    // interleaved by keeping the same state.
    for (int channel = 0; channel < totalNumInputChannels; ++channel)
    {
        auto* channelData = buffer.getWritePointer (channel);

        // ..do something to the data...
    }
}

//==============================================================================
bool MastersPluginVer2023AudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* MastersPluginVer2023AudioProcessor::createEditor()
{
    //return new MastersPluginVer2023AudioProcessorEditor (*this);
    return new juce::GenericAudioProcessorEditor(*this);
}

//==============================================================================
void MastersPluginVer2023AudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
}

void MastersPluginVer2023AudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
}

//============= CUSTOM =========================================================

juce::AudioProcessorValueTreeState::ParameterLayout MastersPluginVer2023AudioProcessor::createParameterLayout() {

    juce::AudioProcessorValueTreeState::ParameterLayout layout;

    float startRange = 0.f;
    float endRange = 100.f;
    float defaultValue = 0.f;

    layout.add(std::make_unique<juce::AudioParameterFloat>(
        "Macro 1", 
        "Macro 1", 
        juce::NormalisableRange<float>(startRange, endRange, 1.f, 1.f),
        defaultValue));

    layout.add(std::make_unique<juce::AudioParameterFloat>(
        "Macro 2",
        "Macro 2",
        juce::NormalisableRange<float>(startRange, endRange, 1.f, 1.f),
        defaultValue));

    layout.add(std::make_unique<juce::AudioParameterFloat>(
        "Macro 3",
        "Macro 3",
        juce::NormalisableRange<float>(startRange, endRange, 1.f, 1.f),
        defaultValue));

    layout.add(std::make_unique<juce::AudioParameterFloat>(
        "Macro 4",
        "Macro 4",
        juce::NormalisableRange<float>(startRange, endRange, 1.f, 1.f),
        defaultValue));

    layout.add(std::make_unique<juce::AudioParameterFloat>(
        "Macro 5",
        "Macro 5",
        juce::NormalisableRange<float>(startRange, endRange, 1.f, 1.f),
        defaultValue));

    layout.add(std::make_unique<juce::AudioParameterFloat>(
        "Macro 6",
        "Macro 6",
        juce::NormalisableRange<float>(startRange, endRange, 1.f, 1.f),
        defaultValue));

    layout.add(std::make_unique<juce::AudioParameterFloat>(
        "Macro 7",
        "Macro 7",
        juce::NormalisableRange<float>(startRange, endRange, 1.f, 1.f),
        defaultValue));

    layout.add(std::make_unique<juce::AudioParameterFloat>(
        "Macro 8",
        "Macro 8",
        juce::NormalisableRange<float>(startRange, endRange, 1.f, 1.f),
        defaultValue));

    return layout;
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new MastersPluginVer2023AudioProcessor();
}
