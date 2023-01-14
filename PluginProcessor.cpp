/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
MixerPluginProjectFinaleAudioProcessor::MixerPluginProjectFinaleAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
    : AudioProcessor(BusesProperties()
#if ! JucePlugin_IsMidiEffect
#if ! JucePlugin_IsSynth
        .withInput("Input", juce::AudioChannelSet::stereo(), true)
#endif
        .withOutput("Output", juce::AudioChannelSet::stereo(), true)
#endif
    ),
    mainProcessor(new juce::AudioProcessorGraph())
#endif
{
}

MixerPluginProjectFinaleAudioProcessor::~MixerPluginProjectFinaleAudioProcessor()
{
}

//==============================================================================
const juce::String MixerPluginProjectFinaleAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool MixerPluginProjectFinaleAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool MixerPluginProjectFinaleAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool MixerPluginProjectFinaleAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double MixerPluginProjectFinaleAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int MixerPluginProjectFinaleAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int MixerPluginProjectFinaleAudioProcessor::getCurrentProgram()
{
    return 0;
}

void MixerPluginProjectFinaleAudioProcessor::setCurrentProgram (int index)
{
}

const juce::String MixerPluginProjectFinaleAudioProcessor::getProgramName (int index)
{
    return {};
}

void MixerPluginProjectFinaleAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}

//==============================================================================
void MixerPluginProjectFinaleAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    juce::ignoreUnused(samplesPerBlock);
    mainProcessor->setPlayConfigDetails(getMainBusNumInputChannels(),
        getMainBusNumOutputChannels(),
        sampleRate, samplesPerBlock);

    mainProcessor->prepareToPlay(sampleRate, samplesPerBlock);

    initialiseGraph();
    DBG("AudioProcessorGraph initialisation...");
}

void MixerPluginProjectFinaleAudioProcessor::initialiseGraph()
{
    mainProcessor->clear();

    audioInputNode = mainProcessor->addNode(std::make_unique<AudioGraphIOProcessor>(AudioGraphIOProcessor::audioInputNode));
    audioOutputNode = mainProcessor->addNode(std::make_unique<AudioGraphIOProcessor>(AudioGraphIOProcessor::audioOutputNode));
    midiInputNode = mainProcessor->addNode(std::make_unique<AudioGraphIOProcessor>(AudioGraphIOProcessor::midiInputNode));
    midiOutputNode = mainProcessor->addNode(std::make_unique<AudioGraphIOProcessor>(AudioGraphIOProcessor::midiOutputNode));

    connectAudioNodes();
    connectMidiNodes();
    connectGeneralVolume();
}

void MixerPluginProjectFinaleAudioProcessor::connectAudioNodes()
{
    for (int channel = 0; channel < 2; ++channel)
        mainProcessor->addConnection({ { audioInputNode->nodeID,  channel },
                                        { audioOutputNode->nodeID, channel } });
}

void MixerPluginProjectFinaleAudioProcessor::connectMidiNodes()
{
    mainProcessor->addConnection({ { midiInputNode->nodeID,  juce::AudioProcessorGraph::midiChannelIndex },
                                { midiOutputNode->nodeID, juce::AudioProcessorGraph::midiChannelIndex } });

}

void MixerPluginProjectFinaleAudioProcessor::connectGeneralVolume()
{
    DBG("BEFORE instantiating generalVolumeNode");

    generalVolumeNode = mainProcessor->addNode(std::make_unique<GainProcessor>());

    DBG("AFTER instantiating generalVolumeNode");

    for (int channel = 0; channel < 2; channel++)
    {
        mainProcessor->addConnection({ { generalVolumeNode->nodeID,         channel},
                                       { audioOutputNode->nodeID,   channel } });
    }
    DBG("audioProcessorGraph initialisation completed!");
}

void MixerPluginProjectFinaleAudioProcessor::releaseResources()
{
    mainProcessor->releaseResources();
    nodesForMixer.clear();
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool MixerPluginProjectFinaleAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
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

void MixerPluginProjectFinaleAudioProcessor::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    for (int i = getTotalNumInputChannels(); i < getTotalNumOutputChannels(); ++i)
    {
        buffer.clear(i, 0, buffer.getNumSamples());
    }

    mainProcessor->processBlock(buffer, midiMessages);
}

std::tuple<juce::AudioProcessorGraph::Node::Ptr, AudioChannel*> MixerPluginProjectFinaleAudioProcessor::addAudioToGraph(int idx, juce::File folder)
{
    auto mixerPtr = std::make_unique<AudioChannel>(folder);
    auto mixerChannel = mixerPtr.get();

    DBG("Adding audio node to the processor...");

    nodesForMixer.insert(idx, mainProcessor->addNode(std::move(mixerPtr)));
    mixerChannel->transportPtr.get()->start();

    DBG("Number of nodes in the graph: " << mainProcessor->getNumNodes());

    for (int channel = 0; channel < 2; channel++)
    {
        mainProcessor->addConnection({ { nodesForMixer[idx]->nodeID,         channel},
                                       { generalVolumeNode->nodeID,   channel } });
    }

    

    return { nodesForMixer[idx], mixerChannel };
}

std::tuple<juce::AudioProcessorGraph::Node::Ptr, MidiChannel*, FilterProcessor*, juce::AudioProcessorGraph::Node::Ptr> MixerPluginProjectFinaleAudioProcessor::addMidiToGraph(std::vector<int> notes)
{
    auto idx = nodesForMixer.size();

    auto midiPtr = std::make_unique<MidiChannel>(notes);
    auto midiChannel = midiPtr.get();

    auto filterPtr = std::make_unique<FilterProcessor>();
    auto filterChannel = filterPtr.get();


    nodesForMixer.insert(idx, mainProcessor->addNode(std::move(midiPtr)));
    nodesForMixer.insert(idx + 1, mainProcessor->addNode(std::move(filterPtr)));


    for (int channel = 0; channel < 2; channel++)
    {
        mainProcessor->addConnection({ { nodesForMixer[idx]->nodeID,         channel},
                                       { nodesForMixer[idx + 1]->nodeID,   channel}});

 

        mainProcessor->addConnection({ { nodesForMixer[idx + 1]->nodeID,         channel},
                                       { generalVolumeNode->nodeID,   channel } });



    }
    return { nodesForMixer[idx], midiChannel, filterChannel, nodesForMixer[idx+1]};
    
}

void MixerPluginProjectFinaleAudioProcessor::removeAudioNode(juce::AudioProcessorGraph::NodeID nodeID, 
    juce::AudioProcessorGraph::Node::Ptr nodePointer)
{
    mainProcessor->removeNode(nodeID);
    
    auto index = nodesForMixer.indexOf(nodePointer);
    jassert(index >= 0);

    nodesForMixer.remove(index);
    DBG("Node ID removed: " << index);
}

void MixerPluginProjectFinaleAudioProcessor::removeMidiNode(juce::AudioProcessorGraph::NodeID nodeID,
    juce::AudioProcessorGraph::Node::Ptr nodePointer,
    juce::AudioProcessorGraph::NodeID filterId,
    juce::AudioProcessorGraph::Node::Ptr filterPtr)
{
    mainProcessor->removeNode(filterId);
    mainProcessor->removeNode(nodeID);

    auto indexFilter = nodesForMixer.indexOf(filterPtr);
    jassert(indexFilter >= 0);
    nodesForMixer.remove(indexFilter);
    DBG("Node ID removed: " << indexFilter);

    auto index = nodesForMixer.indexOf(nodePointer);
    jassert(index >= 0);
    nodesForMixer.remove(index);
    DBG("Node ID removed: " << index);
}

//==============================================================================
bool MixerPluginProjectFinaleAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* MixerPluginProjectFinaleAudioProcessor::createEditor()
{
    return new MixerPluginProjectFinaleAudioProcessorEditor (*this);
}

//==============================================================================
void MixerPluginProjectFinaleAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
}

void MixerPluginProjectFinaleAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new MixerPluginProjectFinaleAudioProcessor();
}
