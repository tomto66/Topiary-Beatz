//////////////////////////////////////////////////////////////////////////////
/*
This file is part of Topiary Beats, Copyright Tom Tollenaere 2018.

Topiary Beats is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

Topiary Beats is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Topiary Beats. If not, see <https://www.gnu.org/licenses/>.
*/
/////////////////////////////////////////////////////////////////////////////

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include "TopiaryBeatsModel.h" 

//==============================================================================
/**
*/
class TopiaryBeatsAudioProcessor  : public AudioProcessor
{
public:
    //==============================================================================
    TopiaryBeatsAudioProcessor();
    ~TopiaryBeatsAudioProcessor();

    //==============================================================================
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

   #ifndef JucePlugin_PreferredChannelConfigurations
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
   #endif

    void processBlock (AudioBuffer<float>&, MidiBuffer&) override;

    //==============================================================================
    AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    //==============================================================================
    const String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    //==============================================================================
    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram (int index) override;
    const String getProgramName (int index) override;
    void changeProgramName (int index, const String& newName) override;

    //==============================================================================
    void getStateInformation (MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;

	TopiaryBeatsModel* getModel();

private:
	TopiaryBeatsModel beatsModel;	// do this first!!!
	
	int tellModelToRun()
	{ // do all that is needed to tell the model to really start running at (sample) time t
		beatsModel.setRunState(Topiary::Running);
		beatsModel.setSampleRate(getSampleRate());	// see if prepareToPlay is called, if so no need to keep checking this
		beatsModel.setStartTimes();	// and do some housekeeping like set the parents to the correct variation
		return beatsModel.getRunState();  // because if there are no variations selected, it shouldn't run!!!
	} // tellModelToRun
	

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (TopiaryBeatsAudioProcessor)
};