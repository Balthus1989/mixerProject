/*
  ==============================================================================

    Constellations.cpp
    Created: 25 Mar 2022 12:57:20pm
    Author:  renda

  ==============================================================================
*/

#include "Constellations.h"

std::vector<std::string> Constellations::visibleConstellations()
{
    Py_Initialize();

    PyRun_SimpleString("import pandas as pd");
    PyRun_SimpleString("from datetime import date");
    PyRun_SimpleString("import time");
    PyRun_SimpleString("import requests");
    PyRun_SimpleString("import json");
    PyRun_SimpleString("from astroquery.mast import Observations");
    PyRun_SimpleString("from astropy.coordinates import EarthLocation, AltAz, SkyCoord");
    PyRun_SimpleString("import astropy.units as u");
    PyRun_SimpleString("from astropy.io import fits");
    PyRun_SimpleString("import visible_stars_data");
    PyRun_SimpleString("import numpy as np");

    PyObject* pyModule = PyImport_ImportModule("get_visible_stars");

    if (pyModule != NULL)
    {
        PyObject* pValue = PyObject_CallMethod(pyModule, "get_stars", NULL);


        if (pValue == NULL)
        {
            //DBG("ERROR");
            //exit(ERROR);
            std::vector<std::string> vect;
            vect = { "Dra", "Crv", "Leo", "Lib",
                "Lup", "Lyn", "Lac", "Lyr", "Com", "Cep",
                "Boo", "UMa", "CVn", "Sge", "Cen", "Vul", "Sex",
                "Oph", "Her", "Ser", "UMi", "Cnc", "CrB", "Cam",
                "Crt", "Cas", "Cyg", "Vir", "LMi" };
            return vect;
        }
        else
        {
            auto result = _PyUnicode_AsString(pValue);
            auto result2 = std::string(result);
            //DBG(result2);

            std::vector<std::string> vect;

            std::stringstream ss(result2);

            while (ss.good())
            {
                std::string substr;
                std::getline(ss, substr, ',');
                vect.push_back(substr);
            }
            DBG("SUCCESS");
            return vect;
        }
        
    }
    else
    {
        std::vector<std::string> vect;
        vect = { "Dra", "Crv", "Leo", "Lib", 
            "Lup", "Lyn", "Lac", "Lyr", "Com", "Cep",
            "Boo", "UMa", "CVn", "Sge", "Cen", "Vul", "Sex",
            "Oph", "Her", "Ser", "UMi", "Cnc", "CrB", "Cam",
            "Crt", "Cas", "Cyg", "Vir", "LMi"};
        return vect;
    }

    Py_Finalize();
}


std::vector<int> Constellations::sonification(juce::String star)
{

    Py_Initialize();

    //PyRun_SimpleString("import sys");
    //PyRun_SimpleString("sys.path.append(\"C:\\Users\\renda\\Documents\\provaJuce\\mixerPluginProjectFinale\\Builds\\VisualStudio2022\")");

    PyRun_SimpleString("import pandas as pd");

    PyRun_SimpleString("import requests");

    PyRun_SimpleString("from astroquery.mast import Observations");
    PyRun_SimpleString("from astropy.coordinates import SkyCoord");
    PyRun_SimpleString("import astropy.units as u");
    PyRun_SimpleString("from astropy.io import fits");
    PyRun_SimpleString("import visible_stars_data");
    PyRun_SimpleString("from astropy import coordinates");
    PyRun_SimpleString("import numpy as np");
    PyRun_SimpleString("from astropy.io import fits");
    PyRun_SimpleString("from sklearn.preprocessing import MinMaxScaler");

    //juce::String filePath = juce::File::getCurrentWorkingDirectory().getFullPathName();
    //DBG(filePath);

    PyObject* pyModule = PyImport_ImportModule("get_visible_stars");
    


    //jassert(pyModule != NULL);

    if (pyModule != NULL)
    {
        auto starString = star.toStdString();
        auto starChar = starString.c_str();

        
        PyObject* pValue = PyObject_CallMethod(pyModule, "get_sonification", "s", starChar);

        //jassert(pValue != NULL);

        if (pValue == NULL)
        {
            //DBG("ERROR");
            //exit(ERROR);
            srand(time(0));

            int _min = 48;//min
            int _max = 120;//max

            const int total_numbers = 100;//total numbers you want to generate
            int _array[total_numbers] = { 0 };

            std::vector<int> intNumbers;

            int random_number = -1;

            for (int i = 0; i < total_numbers; ++i)
            {


                random_number = _min + rand() % ((_max - _min) + 1);

                _array[i] = random_number;
                intNumbers.push_back(_array[i]);

            }

            return intNumbers;
        }
        else
        {
            
            
            auto result = _PyUnicode_AsString(pValue);

            auto result2 = std::string(result);

            std::vector<std::string> vect;

            std::stringstream ss(result2);

            while (ss.good())
            {
                std::string substr;
                std::getline(ss, substr, ',');
                vect.push_back(substr);
            }

            std::vector<int> intNumbers;


            for (int i = 0; i < vect.size(); i++)
            {
                std::stringstream str(vect[i]);
                int x;
                str >> x;
                if (!str)
                {
                    DBG("CONVERSION ERROR");
                }
                intNumbers.push_back(x);
                
            }

            DBG("SUCCESS");
    
            return intNumbers;
        }

    }
    else
    {
        srand(time(0));

        int _min = 48;//min
        int _max = 120;//max

        const int total_numbers = 100;//total numbers you want to generate
        int _array[total_numbers] = { 0 };

        std::vector<int> intNumbers;

        int random_number = -1;

        for (int i = 0; i < total_numbers; ++i)
        {

            random_number = _min + rand() % ((_max - _min) + 1);

            _array[i] = random_number;
            intNumbers.push_back(_array[i]);
            
        }

        return intNumbers;

    }

    

    Py_Finalize();
}