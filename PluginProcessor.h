/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "AudioChannel.h"
#include "MidiChannel.h"
#include "FilterProcessor.h"
#include "GainProcessor.h"

//==============================================================================
/**
*/
class MixerPluginProjectFinaleAudioProcessor  : public juce::AudioProcessor
{
public:
    //===Defining some variables of the namespace for the sake of simplicity===//
    using AudioGraphIOProcessor = juce::AudioProcessorGraph::AudioGraphIOProcessor;
    using Node = juce::AudioProcessorGraph::Node;
    //==============================================================================
    MixerPluginProjectFinaleAudioProcessor();
    ~MixerPluginProjectFinaleAudioProcessor() override;

    //==============================================================================
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

   #ifndef JucePlugin_PreferredChannelConfigurations
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
   #endif

    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    //==============================================================================
    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    //==============================================================================
    const juce::String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    //==============================================================================
    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram (int index) override;
    const juce::String getProgramName (int index) override;
    void changeProgramName (int index, const juce::String& newName) override;

    //==============================================================================
    void getStateInformation (juce::MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;

    //===Initialising and handling the AudioProcessorGraph===//
    void initialiseGraph();
    void connectMidiNodes();
    void connectAudioNodes();
    void connectGeneralVolume();
    //juce::AudioProcessorGraph::Node::Ptr addAudioToGraph(int idx, juce::File folder);

    std::tuple<juce::AudioProcessorGraph::Node::Ptr, AudioChannel*>
        MixerPluginProjectFinaleAudioProcessor::addAudioToGraph(int idx, juce::File folder);

    std::tuple<juce::AudioProcessorGraph::Node::Ptr, MidiChannel*, FilterProcessor*, juce::AudioProcessorGraph::Node::Ptr> addMidiToGraph(std::vector<int> notes);

    void removeAudioNode(juce::AudioProcessorGraph::NodeID nodeID, 
        juce::AudioProcessorGraph::Node::Ptr nodePointer);

    void removeMidiNode(juce::AudioProcessorGraph::NodeID nodeID,
        juce::AudioProcessorGraph::Node::Ptr nodePointer,
        juce::AudioProcessorGraph::NodeID filterId,
        juce::AudioProcessorGraph::Node::Ptr filterPtr);

    juce::ReferenceCountedArray<Node> nodesForMixer;
    std::unique_ptr<juce::AudioProcessorGraph> mainProcessor;
    Node::Ptr generalVolumeNode;

private:

    //==============================================================================
    //===AudioProcessorGraph and input/output nodes===//
    
    
    Node::Ptr audioInputNode;
    Node::Ptr audioOutputNode;
    Node::Ptr midiInputNode;
    Node::Ptr midiOutputNode;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MixerPluginProjectFinaleAudioProcessor)
};
