/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

// Client Server

#include <boost/asio.hpp>
// include boost -> Narzedzia -> Menager Pakietow NuGet
// Update-Package -reinstall

#include "json.hpp"

#define DEFAULT_PORT_NR 54000

void networkClient();
std::string json_from_server_data =
"{ \"1\" : 0.1, \"2\" : 0.2, \"3\" : 0.3, \"4\" : 0.0, \"5\" : 0.0, \"6\" : 0.0, \"7\" : 0.0, \"8\" : 0.0 }"; //inicjalizacja zmiennej

// test


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
    std::thread t1{ networkClient };
    t1.detach();

    
    //networkClient();
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

    juce::dsp::ProcessSpec spec;
    spec.maximumBlockSize = samplesPerBlock;
    spec.numChannels = 1;
    spec.sampleRate = sampleRate;

    setMyParameters(apvts); //test

    leftChain.prepare(spec);
    rightChain.prepare(spec);

    auto chainSettings = getChainSettings(apvts);

    updateMacros(chainSettings);
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

    auto chainSettings = getChainSettings(apvts);

    updateMacros(chainSettings);

    juce::dsp::AudioBlock<float> block(buffer);

    auto leftBlock = block.getSingleChannelBlock(0);
    auto rightBlock = block.getSingleChannelBlock(1);

    juce::dsp::ProcessContextReplacing<float> leftContext(leftBlock);
    juce::dsp::ProcessContextReplacing<float> rightContext(rightBlock);

    leftChain.process(leftContext);
    rightChain.process(rightContext);

    // - 

    setMyParameters(apvts);
}

//==============================================================================
bool MastersPluginVer2023AudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* MastersPluginVer2023AudioProcessor::createEditor()
{
    return new MastersPluginVer2023AudioProcessorEditor (*this);
    //return new juce::GenericAudioProcessorEditor (*this);
}

//==============================================================================
void MastersPluginVer2023AudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.

    juce::MemoryOutputStream mos(destData, true);
    apvts.state.writeToStream(mos);
}

void MastersPluginVer2023AudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.

    auto tree = juce::ValueTree::readFromData(data, sizeInBytes);

    if (tree.isValid()) {
        setMyParameters(apvts);
        apvts.replaceState(tree);

        auto chainSettings = getChainSettings(apvts);
        updateMacros(chainSettings);
    }
}










//============= CUSTOM =========================================================


ChainSettings getChainSettings(juce::AudioProcessorValueTreeState& apvts) {
    ChainSettings settings;

    settings.macro1 = apvts.getRawParameterValue("Macro 1")->load();
    settings.macro2 = apvts.getRawParameterValue("Macro 2")->load();
    settings.macro3 = apvts.getRawParameterValue("Macro 3")->load();
    settings.macro4 = apvts.getRawParameterValue("Macro 4")->load();
    settings.macro5 = apvts.getRawParameterValue("Macro 5")->load();
    settings.macro6 = apvts.getRawParameterValue("Macro 6")->load();
    settings.macro7 = apvts.getRawParameterValue("Macro 7")->load();
    settings.macro8 = apvts.getRawParameterValue("Macro 8")->load();

    return settings;
}

Coefficients makeMacro(const ChainSettings& chainSettings, double sampleRate)
{
    return juce::dsp::IIR::Coefficients<float>::makePeakFilter(sampleRate,
        100.f, 1.f, 1.f);
}
void updateCoefficients(Coefficients& old, const Coefficients& replacements)
{
    *old = *replacements;
}

void MastersPluginVer2023AudioProcessor::updateMacros(const ChainSettings& chainSettings) {
    auto peak1Coefficients = makeMacro(chainSettings, getSampleRate()); // name for change TODO

    updateCoefficients(leftChain.get<ChainPositions::Macro>().coefficients, peak1Coefficients);
    updateCoefficients(rightChain.get<ChainPositions::Macro>().coefficients, peak1Coefficients);
}

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

void setMyParameters(juce::AudioProcessorValueTreeState& apvts) {
    nlohmann::json j = nlohmann::json::parse(json_from_server_data);

    float new_macro1_Value = j["1"];
    float new_macro2_Value = j["2"];
    float new_macro3_Value = j["3"];
    float new_macro4_Value = j["4"];
    float new_macro5_Value = j["5"];
    float new_macro6_Value = j["6"];
    float new_macro7_Value = j["7"];
    float new_macro8_Value = j["8"];

    auto settings = getChainSettings(apvts);

    changeParameterValue(apvts, "Macro 1", (new_macro1_Value * 100));
    changeParameterValue(apvts, "Macro 2", (new_macro2_Value * 100));
    changeParameterValue(apvts, "Macro 3", (new_macro3_Value * 100));
    changeParameterValue(apvts, "Macro 4", (new_macro4_Value * 100));
    changeParameterValue(apvts, "Macro 5", (new_macro5_Value * 100));
    changeParameterValue(apvts, "Macro 6", (new_macro6_Value * 100));
    changeParameterValue(apvts, "Macro 7", (new_macro7_Value * 100));
    changeParameterValue(apvts, "Macro 8", (new_macro8_Value * 100));
}

void changeParameterValue(juce::AudioProcessorValueTreeState& apvts, std::string param_name, float new_value)
{
    auto* param = apvts.getParameter(param_name);
    auto oldValue = param->convertTo0to1((float)new_value);
    param->beginChangeGesture();
    param->setValueNotifyingHost(param->convertTo0to1((float)new_value));
    param->endChangeGesture();
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new MastersPluginVer2023AudioProcessor();
}










// CLIENT

void networkClient()
{
    boost::asio::io_service io_service;
    // socket creation
    boost::asio::ip::tcp::socket client_socket(io_service);

    try {
        client_socket.connect(boost::asio::ip::tcp::endpoint
        (boost::asio::ip::address::from_string
        ("127.0.0.1"), /*54000*/ DEFAULT_PORT_NR));
    }
    catch (...) {
        return; //if cant connect - return from networking
    }

    // Getting username from user
    std::string u_name, reply, response;
    u_name = "";

    // Sending username to another end
    // to initiate the conversation
    //sendData(client_socket, u_name);

    // Infinite loop for chit-chat
    while (true) {
        //ODBIOR DANYCH
        
        // Fetching response
        //response = getData(client_socket);
        
        boost::asio::streambuf buf; //wyciagniecie fcji
        read_until(client_socket, buf, "\n");
        std::string response = boost::asio::buffer_cast<const char*>(buf.data());

        // Popping last character "\n"
        response.pop_back();

        // Validating if the connection has to be closed
        if (response == "exit") {
            //std::cout << "Connection terminated" << std::endl;
            break;
        }
        
        //system("CLS");
        //std::cout << response << std::endl;

        //ustawienie zmiennej? ------------------------> tu cos wywala juceThread
        
        json_from_server_data.assign(response);

        //---------------------------------

        //WYSYLKA DANYCH

        // Reading new message from input stream
        //cout << u_name << "";
        //getline(cin, reply); //tutaj czeka na klawisz wiec lipa
        //reply = "ok";

        //sendData(client_socket, reply /*json_parameter_data*/); //poki co nic nie wysylam
        
        //boost::asio::write(client_socket, boost::asio::buffer(reply)); //wyciagniecie fcji

        //boost::asio::write(client_socket, boost::asio::buffer("json:"+json_from_server_data)); //wyciagniecie fcji

        //response = "";

        //cout << str_i << endl;
        //i = i++;
        
        if (reply == "exit")
            break;

    }
}