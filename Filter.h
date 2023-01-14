/*
  ==============================================================================

    Filter.cpp
    Created: 3 Feb 2022 10:16:59am
    Author:  renda

  ==============================================================================
*/
#include <JuceHeader.h>

//==============================================================================
class Filter : public juce::Component,
    public juce::Slider::Listener
{
public:
    Filter()
    {
        setSize(200, 200);

        filterMenu.addItem("Low Pass", 1);
        filterMenu.addItem("High Pass", 2);
        filterMenu.addItem("Band Pass", 3);
        filterMenu.setJustificationType(juce::Justification::centred);
        filterMenu.onChange = [this] { changeFilter(); };
        addAndMakeVisible(&filterMenu);

        filterCutoff.setSliderStyle(juce::Slider::SliderStyle::RotaryHorizontalVerticalDrag);
        filterCutoff.setRange(20.0, 10000.0);
        filterCutoff.setValue(400.0);
        filterCutoff.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
        addAndMakeVisible(&filterCutoff);
        filterCutoff.setSkewFactorFromMidPoint(1000.0);

        filterRes.setSliderStyle(juce::Slider::SliderStyle::RotaryHorizontalVerticalDrag);
        filterRes.setRange(1, 5);
        filterRes.setValue(1);
        filterRes.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
        addAndMakeVisible(&filterRes);
    }

    void paint(juce::Graphics& g) override
    {
        juce::Rectangle<int> titleArea(0, 10, getWidth(), 20);

        g.fillAll(juce::Colours::black);
        g.setColour(juce::Colours::white);
        g.drawText("Filter", titleArea, juce::Justification::centred);

        juce::Rectangle<float> area(25, 25, 150, 150);
        g.setColour(juce::Colours::yellow);
        g.drawRoundedRectangle(area, 20.0f, 2.0f);
    }

    void changeFilter()
    {

    }

    void sliderValueChanged(juce::Slider* slider) override
    {

    }

    void resized() override
    {
        juce::Rectangle<int> area = getLocalBounds().reduced(40);

        filterMenu.setBounds(area.removeFromTop(20));
        filterCutoff.setBounds(30, 100, 70, 70);
        filterRes.setBounds(100, 100, 70, 70);

    }

    void getPointer(MidiChannel* midiChan)
    {
        chan = midiChan;
    }

//private:
    juce::Slider filterCutoff;
    juce::Slider filterRes;

    juce::ComboBox filterMenu;
    MidiChannel* chan;
};


