/*
  ==============================================================================

    MidiEnvelope.h
    Created: 15 Mar 2022 11:05:33am
    Author:  renda

  ==============================================================================
*/

#include <JuceHeader.h>
#include "Oscillator.h"
#include "Filter.h"
#include "Adsr.h"

//==============================================================================
class MidiEnvelope : public juce::Component
{
public:
    MidiEnvelope(MidiChannel* midiChan)
    {
        setSize(600, 200);

        addAndMakeVisible(&oscGui);
        addAndMakeVisible(adsrGui);
        addAndMakeVisible(&filterGui);

        //oscGui.oscMenu.onChange = [this] {prova(); };
        
    }

    ~MidiEnvelope()
    {

    }

    void paint(juce::Graphics& g) override
    {
    }

    void resized() override
    {
        juce::Rectangle<int> area = getLocalBounds();

        const int componentWidth = 200;
        const int componentHeight = 200;

        oscGui.setBounds(area.removeFromLeft(componentWidth).removeFromTop(componentHeight));
        filterGui.setBounds(area.removeFromLeft(componentWidth).removeFromTop(componentHeight));
        adsrGui.setBounds(area.removeFromLeft(componentWidth).removeFromTop(componentHeight));
    }


//private:
    Oscillator oscGui;
    Filter filterGui;
    Adsr adsrGui;
};




