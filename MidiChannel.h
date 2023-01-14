/*
  ==============================================================================

    MidiChannel.h
    Created: 15 Mar 2022 11:06:37am
    Author:  renda

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>
#include "ProcessorBase.h"
#include "MidiVoice.h"
#include "MidiSound.h"
#include "SawSound.h"
#include "Constellations.h"

class MidiChannel : public ProcessorBase
{
public:
    MidiChannel(std::vector<int> notes)
    {
        synth.clearVoices();
        for (auto i = 0; i < 5; i++)
            synth.addVoice(new MidiVoice());

        synth.clearSounds();
        synth.addSound(new MidiSound());


        for (int i = 0; i < notes.size(); i++)
        {
            notesToPlay.push_back(juce::MidiMessage::noteOn(1, notes[i], 1.0f));
        }

        for (int i = 0; i < notes.size(); i++)
        {
            notesToPlay.push_back(juce::MidiMessage::noteOff(1, notes[i], 1.0f));
        }

        for (int i = 0; i < notesToPlay.size(); i++)
        {
            DBG(notesToPlay[i].getDescription());
        }

        addParameter(waveSel = new juce::AudioParameterChoice("wave", "Wave", waves, 0));
        addParameter(paused = new juce::AudioParameterBool("paused", "Paused", false));
        addParameter(stopped = new juce::AudioParameterBool("stopped", "Stopped", false));

    }



    void prepareToPlay(double sampleRate, int samplesPerBlockExpected) override
    {

        synth.setCurrentPlaybackSampleRate(sampleRate);

        for (int i = 0; i < notesToPlay.size(); i++)
        {
            timeToPlay.push_back((i + 4) * getSampleRate()/2);
        }

        for (int j = 0; j < timeToPlay.size(); j++)
        {
            DBG("Time to play: " << timeToPlay[j]);
        }

        jassert(timeToPlay.size() == notesToPlay.size());

        nextPlayTime = timeToPlay[0];

    }

    //=== Process Block ===//
    void processBlock(juce::AudioSampleBuffer& buffer, juce::MidiBuffer& midiBuffer) override
    {
        juce::AudioSourceChannelInfo bufferToFill(buffer);
        bufferToFill.clearActiveBufferRegion();

        if (paused->get() == false && stopped->get() == false)
        {
            auto numSamples = buffer.getNumSamples();
            auto endOfThisBuffer = sampleCounter + numSamples;

            /*
            * we check for every processBlock if we have any notes to play in this time spot
            * [sampleCounter .. sampleCounter + numSamples]
            * N.B we us while() here instead of if() while there could (most probably) be more
            * than one midi event at the same position
            * (you could check this logic by modifying timeToPlay and set the notes to start at the same time)
            * */
            while (nextPlayTime <= endOfThisBuffer)
            {
                /*this term is really important. It controls where in time the midi event will be placed
                * in the time duration of the processblock, 0 means at the start, numSamples -1 at the very end
                * if ouside this interval it will most probably be disregarded and never hear of
                * */


                auto positionInBuffer = int(nextPlayTime - sampleCounter);

                midiBuffer.addEvent(notesToPlay[currentNote], positionInBuffer);


                if (++currentNote < timeToPlay.size())
                    nextPlayTime = timeToPlay[currentNote];
                else
                {
                    for (int i = 0; i < notesToPlay.size(); i++)
                    {
                        timeToPlay[i] = timeToPlay[i] + (int)sampleCounter / divider;
                    }
                    nextPlayTime = timeToPlay[0];
                    currentNote = 0;

                    divider++;
                }

            }
            /*this is the key to be able to play a sample anywhere on the timeline i.e at any time after start
            * we add the played number of samples of every processBlock being played
            * after some amount of time (processblock calls) we reach the first playPos (nextPlayTime above)
            * */
            sampleCounter += numSamples;


            synth.renderNextBlock(*bufferToFill.buffer, midiBuffer,
                bufferToFill.startSample, bufferToFill.numSamples);




        }
    }

    void applyChanges()
    {
        for (int i = 0; i < synth.getNumVoices(); i++)
        {
            if ((voice = dynamic_cast<MidiVoice*>(synth.getVoice(i))))
            {

                voice->soundChoice = waveSel->getCurrentValueAsText();
                voice->adsrParams.attack = attack;
                voice->adsrParams.decay = decay;
                voice->adsrParams.sustain = sustain;
                voice->adsrParams.release = release;

            }
        }
    }

    void sendPause()
    {
        synth.allNotesOff(0, true);
    }

    void sendStop()
    {
        synth.allNotesOff(0, true);
        currentNote = 0;
        nextPlayTime = timeToPlay[0];
        sampleCounter = 0;
    }

    //private:
        //=== The synthesiser ===//
    juce::Synthesiser synth;
    MidiVoice* voice;
    double lastSampleRate;
    float frequency = 1.0f;
    float resonance = 1.0f;

    //=== Handling MIDI notes to be played ===//
    std::vector<juce::MidiMessage> notesToPlay;
    std::vector<juce::int64>timeToPlay;
    juce::int64 nextPlayTime;

    juce::int64 sampleCounter = 0;
    int numSamples = 0;
    int currentNote = 0;
    int divider = 1;

    juce::StringArray waves{ "SineWave", "SawWave", "TriangleWave" };
    juce::AudioParameterChoice* waveSel;
    juce::AudioParameterBool* paused;
    juce::AudioParameterBool* stopped;

    float attack = 0.1f;
    float decay = 0.1;
    float sustain = 1.0f;
    float release = 0.1f;

};