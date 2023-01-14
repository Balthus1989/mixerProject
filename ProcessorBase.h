/*
  ==============================================================================

    ProcessorBase.h
    Created: 10 Feb 2022 11:43:16am
    Author:  renda

  ==============================================================================
*/
#pragma once
#include <JuceHeader.h>
//#include "Sonification.h"

class ProcessorBase : public juce::AudioProcessor
{
public:
    //==============================================================================
    ProcessorBase()
        : AudioProcessor(BusesProperties().withInput("Input", juce::AudioChannelSet::stereo())
            .withOutput("Output", juce::AudioChannelSet::stereo()))
        //,
        //apvts(*this, nullptr, "Parameters", createParameters())
    {

    }

    //==============================================================================
    void prepareToPlay(double, int) override {}
    void releaseResources() override {}
    void processBlock(juce::AudioSampleBuffer&, juce::MidiBuffer&) override {}

    //==============================================================================
    juce::AudioProcessorEditor* createEditor() override { return nullptr; }
    bool hasEditor() const override { return false; }

    //==============================================================================
    const juce::String getName() const override { return {}; }
    bool acceptsMidi() const override { return false; }
    bool producesMidi() const override { return false; }
    double getTailLengthSeconds() const override { return 0; }

    //==============================================================================
    int getNumPrograms() override { return 0; }
    int getCurrentProgram() override { return 0; }
    void setCurrentProgram(int) override {}
    const juce::String getProgramName(int) override { return {}; }
    void changeProgramName(int, const juce::String&) override {}

    //==============================================================================
    void getStateInformation(juce:: MemoryBlock&) override {}
    void setStateInformation(const void*, int) override {}

    //juce::AudioProcessorValueTreeState apvts;

private:
    //juce::AudioProcessorValueTreeState::ParameterLayout ProcessorBase::createParameters()
    //{
    //    std::vector<std::unique_ptr<juce::RangedAudioParameter>> params;
    //    params.push_back(std::make_unique<juce::AudioParameterFloat>("GAIN", "Gain", 0.0f, 1.0f, 0.5f));

    //    return { params.begin(), params.end() };
    //}

    //ProcessorBase& nodeAudioProcessor;
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ProcessorBase)
};
