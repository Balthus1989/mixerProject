/*
  ==============================================================================

    mixelChanner.h
    Created: 8 Feb 2022 1:38:47pm
    Author:  renda

  ==============================================================================
*/

#include <JuceHeader.h>
#include "ProcessorBase.h"

class AudioChannel : public ProcessorBase
{
public:
    AudioChannel(juce::File audioPath)
    {
        
        addParameter(volume = new juce::AudioParameterFloat("volume", "Volume", juce::NormalisableRange<float> (0.0f, 1.0f), 0.5f));
        
        addParameter(reproductionState = new juce::AudioParameterChoice("state", "State", states, 0));

        
        formatReaderPtr.reset(new juce::AudioFormatManager());
        formatReaderPtr->registerBasicFormats();

        juce::Array<juce::File> listOfFiles = audioPath.findChildFiles(2, false);
        int randomInt = juce::Random::getSystemRandom().nextInt(listOfFiles.size() - 1);

        readerPtr.reset(formatReaderPtr->createReaderFor(listOfFiles[randomInt]));

        jassert(readerPtr != nullptr);

        if (readerPtr)
        {
            DBG("Audio file readed correctly!");

            sourcePtr.reset(new juce::AudioFormatReaderSource(readerPtr.release(), true));
            jassert(sourcePtr != nullptr);

            if (sourcePtr == nullptr)
            {
                DBG("Failed to assign the Audio to the transport source!");
            }
            sourcePtr->setLooping(true);
            transportPtr.reset(new juce::AudioTransportSource());
            transportPtr->setSource(sourcePtr.get());

        }
    }

    void prepareToPlay(double sampleRate, int samplesPerBlock) override
    {
        transportPtr->prepareToPlay(samplesPerBlock, sampleRate);
    }


    void processBlock(juce::AudioSampleBuffer& buffer, juce::MidiBuffer&) override
    {

        juce::AudioSourceChannelInfo bufferToFill(buffer);
        bufferToFill.clearActiveBufferRegion();
 
        transportPtr->getNextAudioBlock(bufferToFill);


        transportPtr->setGain(volume->get());

        

    }

    void reproductionStateChanged()
    {
        auto newState = reproductionState->getCurrentValueAsText();
        
        if (newState != actualState)
        {
            DBG("Stato cambiato (prima) " << actualState);
            actualState = newState;
            DBG("Stato cambiato (dopo) " << actualState);

            if (actualState == "Ready" || actualState == "Stopped")
            {
                transportPtr->stop();
                transportPtr->setPosition(0.0);
            }

            if (actualState == "Play")
            {
                transportPtr->start();
            }

            if (actualState == "Paused")
            {
                transportPtr->stop();
            }
        }
        
    }

    void setStateInformation(const void* data, int sizeInBytes) override
    {

    }

    void releaseResources() override
    {

    }

    ~AudioChannel() override
    {
        transportPtr->setSource(nullptr);
        formatReaderPtr = nullptr;
        readerPtr = nullptr;
    }


    //juce::AudioProcessorValueTreeState::ParameterLayout ProcessorBase::createParameters()
    //{
    //    std::vector<std::unique_ptr<juce::RangedAudioParameter>> params;
    //    params.push_back(std::make_unique<juce::AudioParameterFloat>("GAIN", "Gain", 0.0f, 1.0f, 0.5f));
    //    return { params.begin(), params.end() };
    //}

    const juce::String getName() const override { return "Audio Channel"; }

    juce::StringArray states{ "Awake", "Ready", "Stopped", "Paused", "Play" };
    juce::AudioParameterChoice* reproductionState;
    juce::String actualState = "Awake";
    juce::AudioParameterFloat* volume;
    juce::AudioParameterInt* integerState;

    std::unique_ptr<juce::AudioTransportSource> transportPtr;
    std::unique_ptr<juce::AudioFormatReaderSource> sourcePtr;
    std::unique_ptr<juce::AudioFormatManager> formatReaderPtr;
    std::unique_ptr<juce::AudioFormatReader> readerPtr;

};
