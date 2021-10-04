/*
  ==============================================================================

    SynthVoice.cpp
    Created: 24 Jul 2021 12:43:29pm
    Author:  tuiji da

  ==============================================================================
*/
#include <cmath>
#include "SynthVoice.h"
#include<string>
#include<fstream>
#include<iostream>

bool SynthVoice::canPlaySound (juce::SynthesiserSound* sound)
{
    return dynamic_cast<SynthSound*>(sound) != nullptr;
}

void SynthVoice::startNote (int midiNoteNumber, float velocity, juce::SynthesiserSound* sound, int currentPitchWheelPosition)
{
    noteMidiNumber = midiNoteNumber;
    frequency = juce::MidiMessage::getMidiNoteInHertz(midiNoteNumber);
    currentAngle = 0.f;
    angleIncrement = frequency / getSampleRate(); //* juce::MathConstants<float>::twoPi; // 2pi * f / fs
    tailOff = 0.0;
    value = 0;
    isPlaying = true;
}

void SynthVoice::stopNote (float velocity, bool allowTailOff)
{
    if (allowTailOff)
    {
        if (tailOff == 0.0)
            tailOff = 1.0;
    }
    else
    {
        clearCurrentNote();
        level = 0;
        currentAngle = 0.f;
    }
}

void SynthVoice::pitchWheelMoved (int newPitchWheelValue)
{
    // handle pitch wheel moved midi event
}

void SynthVoice::controllerMoved (int controllerNumber, int newControllerValue)
{
    // handle midi control change
}

void SynthVoice::renderNextBlock (juce::AudioBuffer <float> &outputBuffer, int startSample, int numSamples)
{
    if (isPlaying) //去除雜音
   {
       
       //Sawtooth wave
       /*
       if (tailOff > 0.0)
           {
               for (int i = startSample; i < (startSample + numSamples); i++)
               {
                   //float value = std::sin(currentAngle) * level * tailOff;
                   if (currentAngle<0.5) value = currentAngle * level;
                   else value = (currentAngle-0.5) * level;
                   std::cout<<value<<'\n';
                   logger(std::to_string(value));
                   if (currentAngle>=1) currentAngle-=1;
                   
                   outputBuffer.addSample(0, i, value);
                   outputBuffer.addSample(1, i, value);
                   
                   currentAngle += angleIncrement;
                   tailOff *= 0.99;
                   
                   if (tailOff <= 0.05)
                   {
                       clearCurrentNote();
                       angleIncrement = 0.0;
                       level = 0.0;
                       isPlaying = false;
                       break;
                   }
               }
                    
           }
           else
           {
               for (int i = startSample; i < (startSample + numSamples); i++)
               {
                   if (currentAngle<0.5) value = currentAngle * level;
                   else value = (currentAngle-0.5) * level;
                   std::cout<<value<<'\n';
                   logger(std::to_string(value));
                   if (currentAngle>=1) currentAngle-=1;
                   if (currentAngle>=1) currentAngle-=1;
                   outputBuffer.addSample(0, i, value);
                   outputBuffer.addSample(1, i, value);
                   
                   currentAngle += angleIncrement;
                   
           }
           }
       */
       
    //triangle
       
     if (tailOff > 0.0)
         {
             for (int i = startSample; i < (startSample + numSamples); i++)
             {
                 //float value = std::sin(currentAngle) * level * tailOff;
                 if (currentAngle<0.25)value=currentAngle * level;
                 else if(currentAngle>=0.25&&currentAngle<0.5)value=(0.5-currentAngle)*level;
                 else if(currentAngle>=0.5&&currentAngle<=0.75)value=-1*(currentAngle-0.5) * level;
                 else value=-1*(1-currentAngle) * level;
                 std::cout<<value<<'\n';
                 logger(std::to_string(value));
                 if (currentAngle>=1) currentAngle-=1;
                 
                 outputBuffer.addSample(0, i, value);
                 outputBuffer.addSample(1, i, value);
                 
                 currentAngle += angleIncrement;
                 tailOff *= 0.99;
                 
                 if (tailOff <= 0.05)
                 {
                     clearCurrentNote();
                     angleIncrement = 0.0;
                     level = 0.0;
                     isPlaying = false;
                     break;
                 }
             }
                  
         }
         else
         {
             for (int i = startSample; i < (startSample + numSamples); i++)
             {
                 if (currentAngle<0.25)value=currentAngle * level;
                 else if(currentAngle>=0.25&&currentAngle<0.5)value=(0.5-currentAngle)*level;
                 else if(currentAngle>=0.5&&currentAngle<=0.75)value=-1*(currentAngle-0.5) * level;
                 else value=-1*(1-currentAngle) * level;
                 std::cout<<value<<'\n';
                 logger(std::to_string(value));
                 if (currentAngle>=1) currentAngle-=1;
                 outputBuffer.addSample(0, i, value);
                 outputBuffer.addSample(1, i, value);
                 
                 currentAngle += angleIncrement;
                 
         }
         }
     
     
     
    
    // rectengular
       /*
     if (isPlaying)
    {
        if (tailOff > 0.0)
        {
            for (int i = startSample; i < (startSample + numSamples); i++)
            {
    //            float value = std::sin(currentAngle) * level * tailOff;
                
    //            float pi = juce::MathConstants<float>::pi;
                
                if(currentAngle < 0.5)
                {
                    value = 1 * level;
                    std::cout<<value<<'\n';
                    logger(std::to_string(value));
                }
                else {
                    value = -1 * level;
                    std::cout<<value<<'\n';
                    logger(std::to_string(value));
                }
                                
                outputBuffer.addSample(0, i, value);
                outputBuffer.addSample(1, i, value);
                
                currentAngle += angleIncrement;
                
                if (currentAngle >= 1)
                    currentAngle -= 1;

                
                tailOff *= 0.99; //控制延音
//                std::cout << tailOff << std::endl;
                
                if (tailOff <= 0.05f) //處理前面value沒有收到值的雜訊
                {
                    clearCurrentNote();
                    angleIncrement = 0.0;
                    level = 0.0;
                    isPlaying = false;
                }
            }

        }
        else
        {
            for (int i = startSample; i < (startSample + numSamples); i++)
            {
                //float value = std::sin(currentAngle) * level;
                
                if(currentAngle < 0.5)
                {
                    value = 1 * level;
                    std::cout<<value<<'\n';
                    logger(std::to_string(value));
                }
                else {
                    value = -1 * level;
                    std::cout<<value<<'\n';
                    logger(std::to_string(value));
                }
            
                outputBuffer.addSample(0, i, value);
                outputBuffer.addSample(1, i, value);
                
                currentAngle += angleIncrement;
                
                if (currentAngle >= 1)
                    currentAngle -= 1;
            }
        }
       
       //logger((value));
    } */
}
        
}

void SynthVoice::setLevel(float newLevel)
{
    level = newLevel;
}

inline void SynthVoice::logger(std::string msg)
{
    std::string filePath = "/Users/abc/Desktop/log.txt";
    std::ofstream ofs(filePath.c_str(),std::ios_base::out |std::ios_base::app);
    ofs<<msg<<'\n';
    ofs.close();
}
