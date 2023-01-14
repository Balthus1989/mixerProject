/*
  ==============================================================================

    MidiVoice.h
    Created: 13 May 2022 3:37:18pm
    Author:  renda

  ==============================================================================
*/


#pragma once
#include <JuceHeader.h>

//==============================================================================
struct MidiSound : public juce::SynthesiserSound
{
    MidiSound() {}

    bool appliesToNote(int) override { return true; }
    bool appliesToChannel(int) override { return true; }
};

//==============================================================================
struct MidiVoice : public juce::SynthesiserVoice
{
    MidiVoice() {
        adsrParams.attack = 0.1f;
        adsrParams.decay = 0.1;
        adsrParams.sustain = 1.0f;
        adsrParams.release = 0.1f;
        
        
        adsr.setSampleRate(getSampleRate());

        soundChoice = "SineWave";
    }

    bool canPlaySound(juce::SynthesiserSound* sound) override
    {
        return dynamic_cast<MidiSound*> (sound) != nullptr;
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
                    auto currentSample = getCurrentSample(tailOff);
                    //auto currentSample = (float)(std::sin(currentAngle) * level * tailOff);

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
                    auto currentSample = getCurrentSample(tailOff);

                    for (auto i = outputBuffer.getNumChannels(); --i >= 0;)
                        outputBuffer.addSample(i, startSample, adsr.getNextSample() * currentSample);

                    currentAngle += angleDelta;
                    ++startSample;

                }
            }
        }

        
    }

    float getCurrentSample(double tail)
    {
        float result = 0.0;

        if (soundChoice == "SineWave")
        {
            if (tail > 0.0)
            {
                result = (float)(std::sin(currentAngle) * level * tailOff);
            }
            else
            {
                result = (float)(std::sin(currentAngle) * level);
            }
        }
        else if (soundChoice == "SawWave")
        {
            if (tail > 0.0)
            {
                auto resultingAngle = fmod(currentAngle + juce::MathConstants<double>::pi, 2 * juce::MathConstants<double>::pi);
                result = resultingAngle / juce::MathConstants<double>::pi - double(1);
                result = result * level * tailOff;
            }
            else
            {
                auto resultingAngle = fmod(currentAngle + juce::MathConstants<double>::pi, 2 * juce::MathConstants<double>::pi);
                result = resultingAngle / juce::MathConstants<double>::pi - double(1);
                result = result * level;
            }
        }
        else if (soundChoice == "TriangleWave")
        {
            auto angle = fmod(currentAngle + juce::MathConstants<double>::pi / 2, double(2 * juce::MathConstants<double>::pi));
            double frac = angle / (2 * juce::MathConstants<double>::pi);

            if (frac < 0.5)
            {
                result = 2 * frac;
            }
            else
            {
                result = 1 - 2 * (frac - 0.5);
            }

            if (tail > 0.0)
            {
                result = result * level * tailOff;
            }
            else
            {
                result = result * level;
            }
        }

        return result;
    }


//private:
    double currentAngle = 0.0, angleDelta = 0.0, level = 0.0, tailOff = 0.0;
    juce::String soundChoice = "SineWave";
    juce::ADSR adsr; juce::ADSR::Parameters adsrParams;
};

