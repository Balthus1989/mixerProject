/*
  ==============================================================================

    GainProcessor.h
    Created: 27 Apr 2022 9:39:36am
    Author:  renda

  ==============================================================================
*/

#include <JuceHeader.h>
#include "ProcessorBase.h"

class GainProcessor : public ProcessorBase
{
public:
    GainProcessor()
    {
        addParameter(volume = new juce::AudioParameterFloat("volume", "Volume", juce::NormalisableRange<float>(0.0f, 1.0f), 1.0f));
        gain.setGainLinear(*volume);
        
    }

    void prepareToPlay(double sampleRate, int samplesPerBlock) override
    {
        juce::dsp::ProcessSpec spec{ sampleRate, static_cast<juce::uint32> (samplesPerBlock), 2 };
        gain.prepare(spec);
    }

    void processBlock(juce::AudioSampleBuffer& buffer, juce::MidiBuffer&) override
    {
        juce::dsp::AudioBlock<float> block(buffer);
        juce::dsp::ProcessContextReplacing<float> context(block);
        gain.process(context);
        gain.setGainLinear(*volume);
    }

    void reset() override
    {
        gain.reset();
    }


    const juce::String getName() const override { return "Gain"; }

private:
    juce::dsp::Gain<float> gain;
    juce::AudioParameterFloat* volume;
};
