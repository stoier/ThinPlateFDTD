/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

struct ChainSettings
{
    int  excF { 0 };
    float sig0 { 0 }, sig1 { 0 }, lengthX { 0 }, lengthY { 0 }, excX { 0 }, excY { 0 }, lisX { 0 }, lisY { 0 }, thickness { 0 }, excT { 0 };
};

ChainSettings getChainSettings(juce::AudioProcessorValueTreeState& apvts);

//==============================================================================
PlateMiniProjectAudioProcessor::PlateMiniProjectAudioProcessor()
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

PlateMiniProjectAudioProcessor::~PlateMiniProjectAudioProcessor()
{
}

//==============================================================================
const juce::String PlateMiniProjectAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool PlateMiniProjectAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool PlateMiniProjectAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool PlateMiniProjectAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double PlateMiniProjectAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int PlateMiniProjectAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int PlateMiniProjectAudioProcessor::getCurrentProgram()
{
    return 0;
}

void PlateMiniProjectAudioProcessor::setCurrentProgram (int index)
{
}

const juce::String PlateMiniProjectAudioProcessor::getProgramName (int index)
{
    return {};
}

void PlateMiniProjectAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}

//==============================================================================
void PlateMiniProjectAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    hit = false;
    firstHit = false;
    // Retrieve sample rate
    fs = sampleRate;
    thinPlate = std::make_unique<ThinPlate> (1.0 / fs);
    thinPlate-> getSampleRate(fs);
    thinPlate-> initParameters();
}

void PlateMiniProjectAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool PlateMiniProjectAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
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

void PlateMiniProjectAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    
    //Allow midi notes to activate a plate hit
    juce::MidiBuffer::Iterator mIt(midiMessages);
    juce::MidiMessage curMes;
    int samplePosition;
    while (mIt.getNextEvent(curMes, samplePosition))
    {
        if (curMes.isNoteOn())
        {
            hit = true;
        }
    }
    
    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear (i, 0, buffer.getNumSamples());

    auto* channelDataL = buffer.getWritePointer (0);
    auto* channelDataR = buffer.getWritePointer (1);
    
    auto chainSettings = getChainSettings(tree);
    
    thinPlate -> updateParameters(chainSettings.sig0, chainSettings.sig1, chainSettings.lengthX, chainSettings.lengthY, chainSettings.excX, chainSettings.excY, chainSettings.lisX, chainSettings.lisY, chainSettings.thickness, chainSettings.excF, chainSettings.excT);
    thinPlate -> updatePlateMaterial(plateMaterialId);
    
    if (hit == true)
    {
        firstHit = true;
        thinPlate-> initParameters();
        thinPlate -> plateHit();
        hit = false;
    }
    
    for (int i = 0; i < buffer.getNumSamples(); ++i)
    {
        if (firstHit == true)
        {
            thinPlate->calculateScheme();
            output = thinPlate->getOutput();
            channelDataL[i] = limit(output, -1, 1);
            channelDataR[i] = channelDataL[i];

        }
    }
}


//==============================================================================
bool PlateMiniProjectAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* PlateMiniProjectAudioProcessor::createEditor()
{
    return new PlateMiniProjectAudioProcessorEditor (*this);
}

//==============================================================================
void PlateMiniProjectAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    juce::MemoryOutputStream mos(destData,true);
    tree.state.writeToStream(mos);
}

void PlateMiniProjectAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    auto valueTree = juce::ValueTree::readFromData(data, sizeInBytes);
    if( valueTree.isValid() )
    {
        tree.replaceState(valueTree);
    }
}

ChainSettings getChainSettings(juce::AudioProcessorValueTreeState& tree)
{
    ChainSettings settings;
    
    settings.sig0 = tree.getRawParameterValue("Frequency Independent Damping")->load();
    settings.sig1 = tree.getRawParameterValue("Frequency Dependent Damping")->load();
    settings.lengthX = tree.getRawParameterValue("Plate length X")->load();
    settings.lengthY = tree.getRawParameterValue("Plate length Y")->load();
    settings.excX = tree.getRawParameterValue("Excitation pos X")->load();
    settings.excY = tree.getRawParameterValue("Excitation pos Y")->load();
    settings.lisX = tree.getRawParameterValue("Listening pos X")->load();
    settings.lisY = tree.getRawParameterValue("Listening pos Y")->load();
    settings.thickness = tree.getRawParameterValue("Plate thickness")->load();
    settings.excF = tree.getRawParameterValue("Excitation force")->load();
    settings.excT = tree.getRawParameterValue("Excitation time")->load();
    return settings;
}

juce::AudioProcessorValueTreeState::ParameterLayout PlateMiniProjectAudioProcessor::createParameterLayout()
{
    juce::AudioProcessorValueTreeState::ParameterLayout layout;
    
    layout.add(std::make_unique<juce::AudioParameterFloat>("Frequency Independent Damping", "Frequency Independent Damping", 0.01f, 10.f, 1.f));
    layout.add(std::make_unique<juce::AudioParameterFloat>("Frequency Dependent Damping", "Frequency Dependent Damping", juce::NormalisableRange<float>(0.0001f, 0.1f, 0.00001f, 0.35f), 0.0005f));
    layout.add(std::make_unique<juce::AudioParameterFloat>("Plate length X", "Plate length X", 0.2f, 1.f, 0.5f));
    layout.add(std::make_unique<juce::AudioParameterFloat>("Plate length Y", "Plate length Y", 0.2f, 1.f, 0.5f));
    layout.add(std::make_unique<juce::AudioParameterFloat>("Excitation pos X", "Excitation pos X", 0.1f, 0.9f, 0.5f));
    layout.add(std::make_unique<juce::AudioParameterFloat>("Excitation pos Y", "Excitation pos Y", 0.1f, 0.9f, 0.5f));
    layout.add(std::make_unique<juce::AudioParameterFloat>("Listening pos X", "Listening pos X", 0.1f, 0.9f, 0.5f));
    layout.add(std::make_unique<juce::AudioParameterFloat>("Listening pos Y", "Listening pos Y", 0.1f, 0.9f, 0.5f));
    layout.add(std::make_unique<juce::AudioParameterFloat>("Plate thickness", "Plate thickness", 4.f, 20.f, 8.f));
    layout.add(std::make_unique<juce::AudioParameterInt>("Excitation force", "Excitation force", 1, 500, 10));
    layout.add(std::make_unique<juce::AudioParameterFloat>("Excitation time", "Excitation time", 0.1f, 5.f, 1.f));
    return layout;
}

float PlateMiniProjectAudioProcessor::limit (float val, float min, float max)
{
    if (val < min)
        return min;
    else if (val > max)
        return max;
    else
        return val;
}


//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new PlateMiniProjectAudioProcessor();
}
