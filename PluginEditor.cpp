/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include <iostream>
#include <tuple>
#include "PluginProcessor.h"
#include "PluginEditor.h"

//#include <Python.h>


//==============================================================================
MixerPluginProjectFinaleAudioProcessorEditor::MixerPluginProjectFinaleAudioProcessorEditor (MixerPluginProjectFinaleAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p)
{
    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
    setSize (800, 600);
    setResizable(true, true);
    
    addAndMakeVisible(&startButton);
    startButton.setButtonText("CLICCAMI");
    startButton.onClick = [this] { getVisibleConstellations(); };
}

//void MixerPluginProjectFinaleAudioProcessorEditor::testSonification()
//{
//    auto risultato = starsInstance.sonification();
//    DBG("NESSUN ERRORE 1 " << risultato);
//
//}

void MixerPluginProjectFinaleAudioProcessorEditor::getVisibleConstellations()
{
    //auto listOfConstellations = visibleConstellations();
    //auto listOfConstellations = constInstance.visibleConstellations();
    std::vector<std::string> listOfConstellations;
    if (testing == false)
    {
        listOfConstellations = starsInstance.visibleConstellations();
    }
    else if (testing == true)
    {
        listOfConstellations.push_back("UMa");
        listOfConstellations.push_back("UMi");
        listOfConstellations.push_back("Sex");
        listOfConstellations.push_back("Cam");
    }
    //}
    //catch (...)
    //{
    //    DBG("Errore nello script Python!");
    //    listOfConstellations.push_back("UMi");
    //    listOfConstellations.push_back("UMa");
    //    listOfConstellations.push_back("Sex");
    //    listOfConstellations.push_back("Dra");
    //}
    //auto listOfConstellations = starsInstance.visibleConstellations();

    startButton.setVisible(false);

    // Does the method loaded some constellations?
    jassert(listOfConstellations.size() > 0);

    for (int i = 0; i < listOfConstellations.size(); i++)
    {
        comboConstellations.addItem(listOfConstellations[i], (i + 1));
        midiConstellations.addItem(listOfConstellations[i], (i + 1));
    }

    generalVolumeSlider.setName("Master");
    generalVolumeSlider.setSliderStyle(juce::Slider::SliderStyle::LinearVertical);
    generalVolumeSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, true, 0, 0);
    generalVolumeSlider.setRange(0, 1);
    generalVolumeSlider.setValue(1);
    generalVolumeSlider.setSkewFactorFromMidPoint(0.2);
    generalVolumeSlider.onValueChange = [this] { generalVolumeChanged(); };

    generalVolumeText.reset(new juce::DrawableText());
    generalVolumeText->setText("Master");
    generalVolumeText->setColour(juce::Colours::white);
    addAndMakeVisible(generalVolumeText.get());
    generalVolumeText->setBounds(getBounds().getRight() - 80, 30 + 350, 40, 40);


    addAndMakeVisible(comboConstellations);
    addAndMakeVisible(midiConstellations);
    addAndMakeVisible(alreadyAddedConstellations);
    addAndMakeVisible(musicPath);
    addAndMakeVisible(getPathButton);
    addAndMakeVisible(generalVolumeSlider);

    getPathButton.setButtonText("Set Path");
    comboConstellations.onChange = [this] {managingConstellations("Audio"); };
    midiConstellations.onChange = [this] {managingConstellations("Midi");; };
    //alreadyAddedConstellations.onChange = [this] {deletingConstellations(); };
    getPathButton.onClick = [this] {updateMusicPath(); };
    

}

void MixerPluginProjectFinaleAudioProcessorEditor::updateMusicPath()
{
    //auto pathText = musicPath.getText();
    juce::File thePath(musicPath.getText());

    if (thePath.exists())
    {
        listOfFiles = thePath.findChildFiles(2, false);

        if (listOfFiles.size() > 0)
        {
            DBG("Files founded!");
            musicFolder = thePath;
            musicPath.setText("Files founded!");
        }
        else
        {
            DBG("Files not founded... try with another path");
            musicPath.setText("Files not founded!");
        }
    }
    else
    {
        DBG("The path inserted is incorrect!");
        musicPath.setText("The path inserted is incorrect!");
    }

}

void MixerPluginProjectFinaleAudioProcessorEditor::managingConstellations(juce::String type)
{
    if (musicFolder.exists())
    {
        jassert(type == "Audio" || type == "Midi");
        // Get ID
        int itemID;
        if (type == "Audio")
        {
            itemID = comboConstellations.getSelectedId() - 1;
        }
        else if (type == "Midi")
        {
            itemID = midiConstellations.getSelectedId() - 1;
        }
        // Is the ID equal to 0 or major than 0?
        jassert(itemID >= 0);

        // Get constellation name
        juce::String itemText;
        if (type == "Audio")
        {
            itemText = comboConstellations.getItemText(itemID);
        }
        else if (type == "Midi")
        {
            itemText = midiConstellations.getItemText(itemID);
        }
        jassert(itemText != "");

        auto idx = audioProcessor.nodesForMixer.size();

        alreadyAddedConstellations.addItem(itemText, alreadyAddedConstellations.getNumItems() + 1);

        juce::AudioProcessorGraph::Node::Ptr resultingNode;
        AudioChannel* resultingAudioPointer;
        MidiChannel* resultingMidiPointer;
        FilterProcessor* resultingFilter;
        juce::AudioProcessorGraph::Node::Ptr resultingFilterNode;
        juce::AudioProcessorGraph::Node::Ptr resultingCtrlNode;


        if (type == "Audio")
        {
            auto result = audioProcessor.addAudioToGraph(idx, musicFolder);
            
            resultingNode = std::get<0>(result);
            resultingAudioPointer = std::get<1>(result);

            jassert(resultingNode.get() != nullptr);
            jassert(resultingAudioPointer != nullptr);
            
        }
        else if (type == "Midi")
        {
            auto notes = starsInstance.sonification(itemText);
            //std::vector<int> notes = { 31, 30, 45, 46 };
            auto result = audioProcessor.addMidiToGraph(notes);
            resultingNode = std::get<0>(result);
            resultingMidiPointer = std::get<1>(result);
            resultingFilter = std::get<2>(result);
            resultingFilterNode = std::get<3>(result);

            jassert(resultingNode.get() != nullptr);
            jassert(resultingMidiPointer != nullptr);
            jassert(resultingFilter != nullptr);
            jassert(resultingFilterNode.get() != nullptr);
        }
        else
        {
            auto result = audioProcessor.addAudioToGraph(idx, musicFolder);
            resultingNode = std::get<0>(result);
            resultingAudioPointer = std::get<1>(result);

            jassert(resultingNode.get() != nullptr);
            jassert(resultingAudioPointer != nullptr);
        }

        
        settingTheUI(type, itemText, resultingNode, resultingAudioPointer, resultingMidiPointer,
            resultingFilter, resultingFilterNode);
    }
    else
    {
        musicPath.setText("Please set a path for audio sampling!");
    }
}



void MixerPluginProjectFinaleAudioProcessorEditor::settingTheUI(juce::String type, juce::String itemText,
    juce::AudioProcessorGraph::Node::Ptr newNode, AudioChannel* channel, MidiChannel* midiChan, FilterProcessor* filter,
    juce::AudioProcessorGraph::Node::Ptr filterNode)
{
    auto nodeId = newNode->nodeID;
    
    juce::AudioProcessorGraph::NodeID filterId;
    juce::TextButton* copyDial;
    if (filterNode.get() != nullptr)
        filterId = filterNode->nodeID;
    

    DBG("Type " << type);
    std::unique_ptr<juce::DrawableText> text;
    text.reset(new juce::DrawableText());
    text->setText(itemText);
    text->setColour(juce::Colours::white);
    addAndMakeVisible(text.get());
    text->setBounds((elementsOffset * texts.size()) + 45, 380, 40, 40);
    auto copyText = text.get();
    texts.add(std::move(text));

    std::unique_ptr<juce::Slider> slider;
    slider.reset(new juce::Slider());
    slider->setName(juce::String(itemText));
    slider->setSliderStyle(juce::Slider::SliderStyle::LinearVertical);
    slider->setTextBoxStyle(juce::Slider::TextBoxBelow, true, 0, 0);
    slider->setRange(0, 1);
    slider->setValue(0.5);
    slider->setSkewFactorFromMidPoint(0.2);
    auto copySlider = slider.get();

    if (type == "Audio")
    {
        slider->onValueChange = [this, newNode, copySlider] {volumeChangeSlider(newNode, copySlider); };
    }
    else if (type == "Midi")
    {
        slider->onValueChange = [this, filterNode, copySlider] {volumeChangeSlider(filterNode, copySlider); };
    }
    slider->setBounds(elementsOffset * sliders.size(), 180, 100, 200);
    addAndMakeVisible(slider.get());
    sliders.add(std::move(slider));

    std::unique_ptr<juce::TextButton> play;
    play.reset(new juce::TextButton());
    play->setBounds((elementsOffset * playButtons.size()) + 30, 410, 55, 40);
    addAndMakeVisible(play.get());
    auto copyPlay = play.get();

    std::unique_ptr<juce::TextButton> stop;
    stop.reset(new juce::TextButton());
    stop->setButtonText("Stop");
    stop->setBounds((elementsOffset * stopButtons.size()) + 30, 460, 55, 40);
    stop->setEnabled(true);
    addAndMakeVisible(stop.get());
    auto copyStop = stop.get();

    if (type == "Audio")
    {
        play->setButtonText("Play");
        play->onClick = [this, copyPlay, copyStop, channel] {buttonClicked(copyPlay, copyStop, channel, "Play/Pause"); };
        stop->onClick = [this, copyPlay, copyStop, channel] {buttonClicked(copyPlay, copyStop, channel, "Stop"); };
    }
    else if (type == "Midi")
    {
        play->setButtonText("Pause");
        play->onClick = [this, copyPlay, copyStop, midiChan]{ buttonClickedMidi(copyPlay, copyStop, midiChan, "Play/Pause"); };
        stop->onClick = [this, copyPlay, copyStop, midiChan] { buttonClickedMidi(copyPlay, copyStop, midiChan, "Stop"); };
    }

    playButtons.add(std::move(play));
    stopButtons.add(std::move(stop));

    if (type == "Midi")
    {
        std::unique_ptr<juce::TextButton> openDialog;
        openDialog.reset(new juce::TextButton());
        openDialog->setButtonText("Open");
        openDialog->setBounds((elementsOffset * playButtons.size() - 1) - 25, 560, 55, 40);
        addAndMakeVisible(openDialog.get());
        openDialog->setEnabled(true);
        copyDial = openDialog.get();

        auto env = envs.add(new MidiEnvelope(midiChan));
        env->oscGui.oscMenu.onChange = [this, midiChan, env] {changeOsc(midiChan, env); };
        env->filterGui.filterMenu.onChange = [this, env, filter] {changeFilter(env, filter); };
        env->filterGui.filterCutoff.onValueChange = [this, env, filter] {changeFrequency(env, filter); };
        env->filterGui.filterRes.onValueChange = [this, env, filter] {changeResonance(env, filter); };

        env->adsrGui.attackSlider.onValueChange = [this, midiChan, env] {changeAttack(midiChan, env); };
        env->adsrGui.releaseSlider.onValueChange = [this, midiChan, env] {changeRelease(midiChan, env); };
        env->adsrGui.decaySlider.onValueChange = [this, midiChan, env] {changeDecay(midiChan, env);  };
        env->adsrGui.sustainSlider.onValueChange = [this, midiChan, env] {changeSustain(midiChan, env); };

        openDialog->onClick = [this, midiChan, env] { windowOpener(midiChan, env); };
        dialButtons.add(std::move(openDialog));

    }

    std::unique_ptr<juce::TextButton> del;
    del.reset(new juce::TextButton());
    del->setButtonText("Del");
    del->setBounds((elementsOffset * removeButtons.size()) + 30, 510, 55, 40);
    addAndMakeVisible(del.get());
    auto copyDel = del.get();

    if (type == "Audio")
    {
        del->onClick = [this, nodeId, newNode, copySlider, copyPlay, copyStop, copyDel, copyText]
        {removeNodeAndUI(nodeId, newNode, copySlider, copyPlay, copyStop,
            copyDel, copyText); };
    }
    else if (type == "Midi")
    {
        del->onClick = [this, nodeId, filterId, newNode, filterNode, copySlider, copyPlay, copyStop, copyDel, copyDial, copyText]
        {removeNodeAndUIMidi(nodeId, filterId, newNode, filterNode, copySlider, copyPlay, copyStop,
            copyDel, copyDial, copyText); };
    }

    removeButtons.add(std::move(del));

    if (type == "Audio")
    {
        channel->reproductionState->operator=(1);
        channel->reproductionStateChanged();
    }


}

void MixerPluginProjectFinaleAudioProcessorEditor::buttonClickedMidi(juce::TextButton* playButton, juce::TextButton* stopButton,
    MidiChannel* midiChan, juce::String op)
{

    if (op == "Play/Pause")
    {
        if (playButton->getButtonText() == "Pause")
        {
            midiChan->sendPause();
            midiChan->paused->operator=(true);
            playButton->setButtonText("Play");
        }
        else if (playButton->getButtonText() == "Play")
        {
            midiChan->paused->operator=(false);
            midiChan->stopped->operator=(false);
            playButton->setButtonText("Pause");

            if (stopButton->isEnabled() == false)
                stopButton->setEnabled(true);
            

        }
    }

    if (op == "Stop")
    {
        midiChan->sendStop();
        midiChan->stopped->operator=(true);

        stopButton->setEnabled(false);

        if (playButton->getButtonText() == "Pause")
            playButton->setButtonText("Play");
    }
}

void MixerPluginProjectFinaleAudioProcessorEditor::removeNodeAndUIMidi(juce::AudioProcessorGraph::NodeID nodeID,
    juce::AudioProcessorGraph::NodeID filterID,
    juce::AudioProcessorGraph::Node::Ptr nodePtr,
    juce::AudioProcessorGraph::Node::Ptr filterPtr,
    juce::Slider* slider,
    juce::TextButton* play, juce::TextButton* stop, juce::TextButton* del, juce::TextButton* dial,
    juce::DrawableText* text)
{
    auto index = sliders.indexOf(slider);

    DBG("Deleting node " << index);
    jassert(index >= 0);

    audioProcessor.removeMidiNode(nodeID, nodePtr, filterID, filterPtr);

    auto idxSlider = sliders.indexOf(slider);
    sliders.remove(idxSlider, true);
    auto idxText = texts.indexOf(text);
    texts.remove(idxText, true);
    auto idxPlay = playButtons.indexOf(play);
    playButtons.remove(idxPlay, true);
    auto idxStop = stopButtons.indexOf(stop);
    stopButtons.remove(idxStop, true);
    auto idxDel = removeButtons.indexOf(del);
    removeButtons.remove(idxDel, true);
    auto idxOpen = dialButtons.indexOf(dial);
    dialButtons.remove(idxOpen, true);

    //DBG("Index of remove " << index);
    settingUIAfterDelete(index);
}

void MixerPluginProjectFinaleAudioProcessorEditor::windowOpener(MidiChannel* midiChan, MidiEnvelope* env)
{
    juce::Component* component = new juce::Component();

    component->addAndMakeVisible(env);
    component->setSize(env->getWidth(), env->getHeight());
    //env->oscGui.oscMenu.onChange = [this, midiChan, env] {changeOsc(midiChan, env); };
    //env->filterGui.filterMenu.onChange = [this, env, filter] {changeFilter(env, filter); };
    //env->filterGui.filterCutoff.onValueChange = [this, env, filter] {changeFrequency(env, filter); };
    //env->filterGui.filterRes.onValueChange = [this, env, filter] {changeResonance(env, filter); };

    dialog.content.setOwned(component);
    dialog.launchAsync();
}

void MixerPluginProjectFinaleAudioProcessorEditor::changeAttack(MidiChannel* midiChan, MidiEnvelope* env)
{
    midiChan->attack = env->adsrGui.attackSlider.getValue();
    midiChan->applyChanges();
}
void MixerPluginProjectFinaleAudioProcessorEditor::changeDecay(MidiChannel* midiChan, MidiEnvelope* env)
{
    midiChan->decay = env->adsrGui.decaySlider.getValue();
    midiChan->applyChanges();
}
void MixerPluginProjectFinaleAudioProcessorEditor::changeSustain(MidiChannel* midiChan, MidiEnvelope* env)
{
    midiChan->sustain = env->adsrGui.sustainSlider.getValue();
    midiChan->applyChanges();
}
void MixerPluginProjectFinaleAudioProcessorEditor::changeRelease(MidiChannel* midiChan, MidiEnvelope* env)
{
    midiChan->release = env->adsrGui.releaseSlider.getValue();
    midiChan->applyChanges();
}


void MixerPluginProjectFinaleAudioProcessorEditor::changeFrequency(MidiEnvelope* env, FilterProcessor* filter)
{
    filter->frequency->operator=(env->filterGui.filterCutoff.getValue());
    filter->setFilter();

}

void MixerPluginProjectFinaleAudioProcessorEditor::changeResonance(MidiEnvelope* env, FilterProcessor* filter)
{
    filter->resonance->operator=(env->filterGui.filterRes.getValue());
    filter->setFilter();

}

void MixerPluginProjectFinaleAudioProcessorEditor::changeFilter(MidiEnvelope* env, FilterProcessor* filter)
{
    filter->filterSel->operator=(env->filterGui.filterMenu.getSelectedId() - 1);
    filter->setFilter();
}

void MixerPluginProjectFinaleAudioProcessorEditor::changeOsc(MidiChannel* midiChan, MidiEnvelope* env)
{
    midiChan->waveSel->operator=(env->oscGui.oscMenu.getSelectedId() - 1);
    midiChan->applyChanges();
}

//======================================================================================================================//

void MixerPluginProjectFinaleAudioProcessorEditor::generalVolumeChanged()
{
    //auto currentValue = generalVolumeSlider.getValue();

    //audioProcessor.volumeChannel->setGain();
    auto params = audioProcessor.generalVolumeNode->getProcessor()->getParameters();

    auto currentValue = generalVolumeSlider.getValue();

    params[0]->setValueNotifyingHost(currentValue);
}

void MixerPluginProjectFinaleAudioProcessorEditor::volumeChangeSlider(juce::AudioProcessorGraph::Node::Ptr node, 
    juce::Slider* slider)
{
    auto params = node->getProcessor()->getParameters();

    auto currentValue = slider->getValue();

    params[0]->setValueNotifyingHost(currentValue);
}

void MixerPluginProjectFinaleAudioProcessorEditor::settingUIAfterDelete(int index)
{
    for (int i = index; i < sliders.size(); i++)
    {
        sliders[i]->setBounds(sliders[i]->getX() - elementsOffset, sliders[i]->getY(), sliders[i]->getWidth(), sliders[i]->getHeight());
    }

    for (int i = index; i < texts.size(); i++)
    {
        texts[i]->setBounds(texts[i]->getX() - elementsOffset, texts[i]->getY(), texts[i]->getWidth(), texts[i]->getHeight());
    }

    for (int i = index; i < playButtons.size(); i++)
    {
        playButtons[i]->setBounds(playButtons[i]->getX() - elementsOffset, playButtons[i]->getY(),
            playButtons[i]->getWidth(), playButtons[i]->getHeight());
    }

    for (int i = index; i < stopButtons.size(); i++)
    {
        stopButtons[i]->setBounds(stopButtons[i]->getX() - elementsOffset, stopButtons[i]->getY(),
            stopButtons[i]->getWidth(), stopButtons[i]->getHeight());
    }

    for (int i = index; i < removeButtons.size(); i++)
    {
        removeButtons[i]->setBounds(removeButtons[i]->getX() - elementsOffset, removeButtons[i]->getY(),
            removeButtons[i]->getWidth(), removeButtons[i]->getHeight());
    }

    for (int i = index; i < dialButtons.size(); i++)
    {
        dialButtons[i]->setBounds(dialButtons[i]->getX() - elementsOffset, dialButtons[i]->getY(),
            dialButtons[i]->getWidth(), dialButtons[i]->getHeight());
    }
}

void MixerPluginProjectFinaleAudioProcessorEditor::removeNodeAndUI(juce::AudioProcessorGraph::NodeID nodeID, 
    juce::AudioProcessorGraph::Node::Ptr nodePtr, juce::Slider* slider,
    juce::TextButton* play, juce::TextButton* stop, juce::TextButton* del,
    juce::DrawableText* text)
{
    auto index = audioProcessor.nodesForMixer.indexOf(nodePtr);
    DBG("Deleting node " << index);
    jassert(index >= 0); 
    audioProcessor.removeAudioNode(nodeID, nodePtr);

    auto idxSlider = sliders.indexOf(slider);
    sliders.remove(idxSlider, true);
    auto idxText = texts.indexOf(text);
    texts.remove(idxText, true);
    auto idxPlay = playButtons.indexOf(play);
    playButtons.remove(idxPlay, true);
    auto idxStop = stopButtons.indexOf(stop);
    stopButtons.remove(idxStop, true);
    auto idxDel = removeButtons.indexOf(del);
    removeButtons.remove(idxDel, true);

    //DBG("Index of remove " << index);
    settingUIAfterDelete(index);
}


//void MixerPluginProjectFinaleAudioProcessorEditor::playButtonClicked(juce::AudioProcessorGraph::Node::Ptr node,
//    juce::TextButton* button, AudioChannel* channel)
//{
//    auto params = node->getProcessor()->getParameters();
//    
//    
//    DBG("Name " << params[1]->getName(8));
//    DBG("Value " << params[1]->getCurrentValueAsText());
//
//    auto values = params[1]->getAllValueStrings();
//    for (int i = 0; i < values.size(); i++)
//    {
//        DBG("Value in the array " << values[i]);
//    }
//
//    channel->reproductionState->operator=(3);
//    channel->reproductionStateChanged();
//    
//    //params[1]->beginChangeGesture();
//    //auto params = node->getProcessor()->getParameters();
//    //auto actualState = params[1]->getCurrentValueAsText();
//    //
//    //
//    ////  IF the node is playing something ... //
//    //if (actualState == "Playing")
//    //{
//    //    // ... put the node on pause //
//    //    DBG("Node is playing, putting it on pause...");
//    //    params[1]->beginChangeGesture();
//    //    params[1]->setValueNotifyingHost(1.0f);
//    //    params[1]->endChangeGesture();
//    //}
//    //
//    //DBG("Actual State: " << params[1]->getCurrentValueAsText());
//
//}

void MixerPluginProjectFinaleAudioProcessorEditor::buttonClicked(juce::TextButton* playButton, 
    juce::TextButton* stopButton, AudioChannel* channel, juce::String op)
{
    if (op=="Play/Pause")
    {
        if (playButton->getButtonText() == "Play")
        {
            channel->reproductionState->operator=(4);
            channel->reproductionStateChanged();
            playButton->setButtonText("Pause");
            if (stopButton->isEnabled() == false)
            {
                stopButton->setEnabled(true);
            }
        }
        else if (playButton->getButtonText() == "Pause")
        {
            channel->reproductionState->operator=(3);
            channel->reproductionStateChanged();
            playButton->setButtonText("Play");
        }
    }

    if (op == "Stop")
    {
        channel->reproductionState->operator=(2);
        channel->reproductionStateChanged();
        playButton->setButtonText("Play");
        stopButton->setEnabled(false);
    }


}

MixerPluginProjectFinaleAudioProcessorEditor::~MixerPluginProjectFinaleAudioProcessorEditor()
{
    generalVolumeText = nullptr;
}

//==============================================================================
void MixerPluginProjectFinaleAudioProcessorEditor::paint (juce::Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));

    g.setColour (juce::Colours::white);
    g.setFont (15.0f);
}


void MixerPluginProjectFinaleAudioProcessorEditor::resized()
{
    juce::Rectangle<int> area = getLocalBounds().reduced(60);

    const int componentWidth = 100;
    const int componentHeight = 30;

    startButton.setBounds(area.removeFromRight(160).removeFromTop(componentHeight));
    comboConstellations.setBounds(area.removeFromLeft(120).removeFromTop(componentHeight));

    generalVolumeSlider.setBounds(getLocalBounds().getRight() - 100, componentHeight + 150, componentWidth, componentHeight + 170);

    midiConstellations.setBounds(area.removeFromRight(120).removeFromTop(componentHeight));
    musicPath.setBounds(componentWidth-50, componentWidth, componentWidth + 150, componentHeight);
    getPathButton.setBounds(componentWidth + 200, componentWidth, componentWidth +50, componentHeight);
    
}
