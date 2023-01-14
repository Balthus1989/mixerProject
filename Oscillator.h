/*
  ==============================================================================

    Oscillator.cpp
    Created: 3 Feb 2022 10:15:02am
    Author:  renda

  ==============================================================================
*/

#include <JuceHeader.h>

//==============================================================================
class Oscillator : public juce::Component
{
public:
    Oscillator()
    {
        setSize(200, 200);

        oscMenu.addItem("Sine", 1);
        oscMenu.addItem("Saw", 2);
        oscMenu.addItem("Square", 3);
        oscMenu.setSelectedId(1, true);
        oscMenu.setJustificationType(juce::Justification::centred);
        addAndMakeVisible(&oscMenu);
        oscMenu.onChange = [this] {changeOscillator(); };
    }

    void changeOscillator()
    {

    }

    void paint(juce::Graphics& g) override
    {
        juce::Rectangle<int> titleArea(0, 10, getWidth(), 20);

        g.fillAll(juce::Colours::black);
        g.setColour(juce::Colours::white);

        juce::Rectangle<float> area(25, 25, 150, 150);

        g.setColour(juce::Colours::yellow);
        g.drawRoundedRectangle(area, 20.0f, 2.0f);
    }

    void resized() override
    {
        juce::Rectangle<int> area = getLocalBounds().reduced(40);
        oscMenu.setBounds(area.removeFromTop(20));

    }

    void getPointer(MidiChannel* midiChan)
    {
        chan = midiChan;
    }

//private:
    juce::ComboBox oscMenu;
    MidiChannel* chan;
};



