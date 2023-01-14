/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"
#include "Constellations.h"
#include "MidiEnvelope.h"


//==============================================================================
/**
*/
class MixerPluginProjectFinaleAudioProcessorEditor  : public juce::AudioProcessorEditor
{
public:
    MixerPluginProjectFinaleAudioProcessorEditor (MixerPluginProjectFinaleAudioProcessor&);
    ~MixerPluginProjectFinaleAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;
    void getVisibleConstellations();
    void updateMusicPath();
    void managingConstellations(juce::String type);
    void settingTheUI(juce::String type, juce::String itemText, juce::AudioProcessorGraph::Node::Ptr newNode,
        AudioChannel* channel, MidiChannel* midi, FilterProcessor* filter, juce::AudioProcessorGraph::Node::Ptr filterNode);
    void removeNodeAndUI(juce::AudioProcessorGraph::NodeID nodeToRemove, juce::AudioProcessorGraph::Node::Ptr nodePtr,
        juce::Slider* slider, juce::TextButton* play, juce::TextButton* stop, juce::TextButton* del,
        juce::DrawableText* text);
    void removeNodeAndUIMidi(juce::AudioProcessorGraph::NodeID nodeID,
        juce::AudioProcessorGraph::NodeID filterID,
        juce::AudioProcessorGraph::Node::Ptr nodePtr,
        juce::AudioProcessorGraph::Node::Ptr filterPtr,
        juce::Slider* slider,
        juce::TextButton* play, juce::TextButton* stop, juce::TextButton* del, juce::TextButton* open,
        juce::DrawableText* text);
    void volumeChangeSlider(juce::AudioProcessorGraph::Node::Ptr node, juce::Slider* slider);
    void generalVolumeChanged();
    //void playButtonClicked(juce::AudioProcessorGraph::Node::Ptr node,
    //    juce::TextButton* button, AudioChannel* channel);
    void buttonClicked(juce::TextButton* playButton, juce::TextButton* stopButton, AudioChannel* channel, juce::String op);
    void buttonClickedMidi(juce::TextButton* playButton, juce::TextButton* stopButton, MidiChannel* midiChan, juce::String op);
    //void stopButtonClicked(juce::AudioProcessorGraph::Node::Ptr node,
    //    juce::TextButton* button, AudioChannel* channel);
    void windowOpener(MidiChannel* midiChan, MidiEnvelope* env);
    void settingUIAfterDelete(int index);
    void changeOsc(MidiChannel* midiChan, MidiEnvelope* env);
    void changeFilter(MidiEnvelope* env, FilterProcessor* filter);
    void changeFrequency(MidiEnvelope* env, FilterProcessor* filter);
    void changeResonance(MidiEnvelope* env, FilterProcessor* filter);
    
    void changeAttack(MidiChannel* midiChan, MidiEnvelope* env);
    void changeDecay(MidiChannel* midiChan, MidiEnvelope* env);
    void changeRelease(MidiChannel* midiChan, MidiEnvelope* env);
    void changeSustain(MidiChannel* midiChan, MidiEnvelope* env);

private:
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    MixerPluginProjectFinaleAudioProcessor& audioProcessor;

    Constellations starsInstance;
    //Sonification soniInstance;
    juce::TextButton startButton;
    juce::ComboBox comboConstellations;
    juce::ComboBox midiConstellations;
    juce::Slider generalVolumeSlider;
    juce::ComboBox alreadyAddedConstellations;
    juce::TextEditor musicPath;
    juce::TextButton getPathButton;


    // User Interface //
    juce::OwnedArray<juce::Slider> sliders;
    juce::OwnedArray<juce::DrawableText> texts;
    juce::OwnedArray<juce::TextButton> playButtons;
    juce::OwnedArray<juce::TextButton> stopButtons;
    juce::OwnedArray<juce::TextButton> removeButtons;
    juce::OwnedArray<juce::TextButton> dialButtons;
    juce::Array<juce::File> listOfFiles;
    juce::File musicFolder;
    int elementsOffset = 55;
    bool testing = false;
    std::unique_ptr<juce::DrawableText> generalVolumeText;

    //MidiEnvelope midiEnv;
    juce::DialogWindow::LaunchOptions dialog;
    juce::OwnedArray<MidiEnvelope> envs;

    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MixerPluginProjectFinaleAudioProcessorEditor)
};
