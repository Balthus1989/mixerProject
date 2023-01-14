
/*
  ==============================================================================

    anotherVoice.h
    Created: 6 Feb 2022 5:02:51pm
    Author:  renda

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>

//==============================================================================
struct SineWaveSound : public juce::SynthesiserSound
{
    SineWaveSound() {}

    bool appliesToNote(int) override { return true; }
    bool appliesToChannel(int) override { return true; }
};

//==============================================================================
struct SineWaveVoice : public juce::SynthesiserVoice
{
    SineWaveVoice() {
        adsrParams.attack = 0.1f;
        adsrParams.decay = 0.1;
        adsrParams.sustain = 1.0f;
        adsrParams.release = 0.1f;
    }

    bool canPlaySound(juce::SynthesiserSound* sound) override
    {
        return dynamic_cast<SineWaveSound*> (sound) != nullptr;
    }

    void startNote(int midiNoteNumber, float velocity,
        juce::SynthesiserSound*, int /*currentPitchWheelPosition*/) override
    {
        adsr.noteOn();
        currentAngle = 0.0;
        level = velocity * 0.15;
        tailOff = 0.0;

        auto cyclesPerSecond = juce::MidiMessage::getMidiNoteInHertz(midiNoteNumber);
        auto cyclesPerSample = cyclesPerSecond / getSampleRate();

        angleDelta = cyclesPerSample * 2.0 * juce::MathConstants<double>::pi;
    }

    void stopNote(float velocity, bool allowTailOff) override
    {
        adsr.noteOff();
        if (allowTailOff)
        {
            if (tailOff == 0.0)
                tailOff = 1.0;
        }
        else
        {
            clearCurrentNote();
            angleDelta = 0.0;
        }
    }

    void pitchWheelMoved(int) override {}
    void controllerMoved(int, int) override {}

    void renderNextBlock(juce::AudioSampleBuffer& outputBuffer, int startSample, int numSamples) override
    {
        adsr.setParameters(adsrParams);

        if (angleDelta != 0.0)
        {
            if (tailOff > 0.0) // [7]
            {
                while (--numSamples >= 0)
                {
                    auto currentSample = (float)(std::sin(currentAngle) * level * tailOff);

                    for (auto i = outputBuffer.getNumChannels(); --i >= 0;)
                        outputBuffer.addSample(i, startSample, adsr.getNextSample() * currentSample);

                    currentAngle += angleDelta;
                    ++startSample;

                    tailOff *= 0.99; // [8]

                    if (tailOff <= 0.005)
                    {
                        clearCurrentNote(); // [9]

                        angleDelta = 0.0;
                        break;
                    }
                }
            }
            else
            {
                while (--numSamples >= 0) // [6]
                {
                    auto currentSample = (float)(std::sin(currentAngle) * level);

                    for (auto i = outputBuffer.getNumChannels(); --i >= 0;)
                        outputBuffer.addSample(i, startSample, adsr.getNextSample() * currentSample);

                    currentAngle += angleDelta;
                    ++startSample;

                }
            }
        }
    }


private:
    double currentAngle = 0.0, angleDelta = 0.0, level = 0.0, tailOff = 0.0;
    juce::ADSR adsr; juce::ADSR::Parameters adsrParams;
};
