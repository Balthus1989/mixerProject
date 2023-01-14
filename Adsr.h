/*
  ==============================================================================

    Adsr.cpp
    Created: 3 Feb 2022 10:48:22am
    Author:  renda

  ==============================================================================
*/
#include <JuceHeader.h>

//==============================================================================
class Adsr : public juce::Component,
    public juce::Slider::Listener
{
public:
    Adsr() {
        setSize(200, 200);
        attackSlider.setSliderStyle(juce::Slider::SliderStyle::LinearBarVertical);
        attackSlider.setRange(0.1f, 1.0f);
        attackSlider.setValue(0.1f);
        attackSlider.setTextBoxStyle(juce::Slider::NoTextBox, true, 0, 0);
        attackSlider.setSkewFactorFromMidPoint(0.2f);
        addAndMakeVisible(&attackSlider);

        decaySlider.setSliderStyle(juce::Slider::SliderStyle::LinearBarVertical);
        decaySlider.setRange(0.1f, 1.0f);
        decaySlider.setValue(0.1f);
        decaySlider.setTextBoxStyle(juce::Slider::NoTextBox, true, 0, 0);
        decaySlider.setSkewFactorFromMidPoint(0.2f);
        addAndMakeVisible(&decaySlider);

        sustainSlider.setSliderStyle(juce::Slider::SliderStyle::LinearBarVertical);
        sustainSlider.setRange(0.0f, 1.0f);
        sustainSlider.setValue(1.0f);
        sustainSlider.setTextBoxStyle(juce::Slider::NoTextBox, true, 0, 0);
        sustainSlider.setSkewFactorFromMidPoint(0.2f);
        addAndMakeVisible(&sustainSlider);

        releaseSlider.setSliderStyle(juce::Slider::SliderStyle::LinearBarVertical);
        releaseSlider.setRange(0.1f, 1.0f);
        releaseSlider.setValue(0.1f);
        releaseSlider.setTextBoxStyle(juce::Slider::NoTextBox, true, 0, 0);
        releaseSlider.setSkewFactorFromMidPoint(0.2f);
        addAndMakeVisible(&releaseSlider);
    }

    ~Adsr() override
    {
    }

    void sliderValueChanged(juce::Slider* slider) override
    {

    }

    void paint(juce::Graphics& g) override
    {
        juce::Rectangle<int> titleArea(0, 10, getWidth(), 20);
        g.fillAll(juce::Colours::black);
        g.setColour(juce::Colours::white);
        g.drawText("Envelope", titleArea, juce::Justification::centred);

        g.drawText("A", 53, 150, 20, 20, juce::Justification::centredTop);
        g.drawText("D", 77, 150, 20, 20, juce::Justification::centredTop);
        g.drawText("S", 103, 150, 20, 20, juce::Justification::centredTop);
        g.drawText("R", 123, 150, 20, 20, juce::Justification::centredTop);

        juce::Rectangle<float> area(25, 25, 150, 150);

        g.setColour(juce::Colours::yellow);
        g.drawRoundedRectangle(area, 20.0f, 2.0f);
    }

    void resized() override
    {
        juce::Rectangle<int> area = getLocalBounds().reduced(50);

        int sliderWidth = 25;
        int sliderHeight = 175;

        attackSlider.setBounds(area.removeFromLeft(sliderWidth).removeFromTop(sliderHeight).withTrimmedTop(10));
        decaySlider.setBounds(area.removeFromLeft(sliderWidth).removeFromTop(sliderHeight).withTrimmedTop(10));
        sustainSlider.setBounds(area.removeFromLeft(sliderWidth).removeFromTop(sliderHeight).withTrimmedTop(10));
        releaseSlider.setBounds(area.removeFromLeft(sliderWidth).removeFromTop(sliderHeight).withTrimmedTop(10));

    }

    void getPointer(MidiChannel* midiChan)
    {
        chan = midiChan;
    }

//private:
    juce::Slider attackSlider;
    juce::Slider decaySlider;
    juce::Slider sustainSlider;
    juce::Slider releaseSlider;
    MidiChannel* chan;
    //JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Adsr);

};


