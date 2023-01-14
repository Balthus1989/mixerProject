/*
  ==============================================================================

    FilterProcessor.h
    Created: 22 Mar 2022 3:13:46pm
    Author:  renda

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>
#include "ProcessorBase.h"

class FilterProcessor : public ProcessorBase
{
public:
    FilterProcessor()
    {
        addParameter(volume = new juce::AudioParameterFloat("volume", "Volume", juce::NormalisableRange<float>(0.0f, 1.0f), 0.5f));
        addParameter(filterSel = new juce::AudioParameterChoice("filter", "Filter", filters, 0));
        addParameter(frequency = new juce::AudioParameterFloat("frequency", "Frequency",
            juce::NormalisableRange<float>(20.0f, 10000.0f), 400.0f));
        addParameter(resonance = new juce::AudioParameterFloat("resonance", "Resonance",
            juce::NormalisableRange<float>(1.0f, 5.0f), 2.0f));
    }

    void prepareToPlay(double sampleRate, int samplesPerBlock) override
    {
        juce::dsp::ProcessSpec spec{ sampleRate, static_cast<juce::uint32> (samplesPerBlock), 2 };
        filter.prepare(spec);
        setFilter();
    }

    void processBlock(juce::AudioSampleBuffer& buffer, juce::MidiBuffer&) override
    {

        buffer.applyGain(volume->get());

        // Applying DSP
        juce::dsp::AudioBlock<float> block(buffer);
        juce::dsp::ProcessContextReplacing<float> context(block);
        filter.process(context);
    }

    void setFilter()
    {
        auto freq = frequency->get();
        auto res = resonance->get();

        if (filterSel->getCurrentChoiceName() == "LowPass")
        {
            filter.state->type = juce::dsp::StateVariableFilter::Parameters<float>::Type::lowPass;

            filter.state->setCutOffFrequency(getSampleRate(), freq, res);
        }

        if (filterSel->getCurrentChoiceName() == "HighPass")
        {
            filter.state->type = juce::dsp::StateVariableFilter::Parameters<float>::Type::highPass;

            filter.state->setCutOffFrequency(getSampleRate(), freq, res);
        }

        if (filterSel->getCurrentChoiceName() == "BandPass")
        {
            filter.state->type = juce::dsp::StateVariableFilter::Parameters<float>::Type::bandPass;

            filter.state->setCutOffFrequency(getSampleRate(), freq, res);
        }
    }

    void reset() override
    {
        filter.reset();
    }

    const juce::String getName() const override { return "Filter"; }

    juce::dsp::ProcessorDuplicator<juce::dsp::StateVariableFilter::Filter<float>,
        juce::dsp::StateVariableFilter::Parameters<float>> filter;
    juce::StringArray filters{ "LowPass", "HighPass", "BandPass"};
    juce::AudioParameterChoice* filterSel;
    juce::AudioParameterFloat* frequency;
    juce::AudioParameterFloat* resonance;
    juce::AudioParameterFloat* volume;
};