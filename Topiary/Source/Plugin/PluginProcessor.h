/////////////////////////////////////////////////////////////////////////////
/*
This file is part of Topiary, Copyright Tom Tollenaere 2018-2019.

Topiary is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

Topiary is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Topiary. If not, see <https://www.gnu.org/licenses/>.

/////////////////////////////////////////////////////////////////////////////

This code has a generic processor component that can be included in every Topiary plugin.

CAREFUL: needs symbols:
- TOPIARYMODEL e.g. TOPIARYMODEL
- TOPIARYEDITORCOMPONENT - e.g. TopiaryPresetsComponent

*/
/////////////////////////////////////////////////////////////////////////////

#pragma once

#ifdef TOPIARYMODEL

//==============================================================================

class TopiaryAudioProcessor  : public AudioProcessor
{
public:
    //==============================================================================
    TopiaryAudioProcessor();
    ~TopiaryAudioProcessor();

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

	TOPIARYMODEL* getModel();

private:
	TOPIARYMODEL model;	// do this first!!!
	MidiBuffer processedMidi;

	int tellModelToRun()
	{ // do all that is needed to tell the model to really start running 
		model.setRunState(Topiary::Running);
#ifndef PRESETZ
#ifndef PADZ
		model.initializePreviousSteadyVariation();
#endif
#ifdef PADZ
		jassert(false);
#endif
#endif
		model.setSampleRate(getSampleRate());	// see if prepareToPlay is called, if so no need to keep checking this
		model.setStartTimes();	// and do some housekeeping like set the parents to the correct variation
		model.initializeVariationsForRunning();
		return model.getRunState();  // because if there are no variations selected, it shouldn't run!!!
	} // tellModelToRun
	

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (TopiaryAudioProcessor)
};

#endif

