/*
  ==============================================================================

    Constellations.h
    Created: 25 Mar 2022 12:57:20pm
    Author:  renda

  ==============================================================================
*/

#pragma once
#include <Windows.h>
#include <iostream>
#include <JuceHeader.h>
#include <Python.h>
#include <cstdlib>
#include <ctime>

class Constellations
{
public:
    std::vector<std::string> visibleConstellations();
    std::vector<int> sonification(juce::String star);


};
