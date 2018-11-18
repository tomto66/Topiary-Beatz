/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

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
	
	void tellModelToRun()
	{ // do all that is needed to tell the model to really start running at (sample) time t
		beatsModel.setRunState(Topiary::Running);
		beatsModel.setSampleRate(getSampleRate());	// see if prepareToPlay is called, if so no need to keep checking this
		beatsModel.setStartTimes();	// and do some housekeeping like set the parents to the correct variation
		
	} // tellModelToRun
	

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (TopiaryBeatsAudioProcessor)
};