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

#include "PluginProcessor.h"
#include "PluginEditor.h"

#ifdef TOPIARYMODEL

/////////////////////////////////////////////////////////////////////////

TopiaryAudioProcessor::TopiaryAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
	: AudioProcessor(BusesProperties()
#if ! JucePlugin_IsMidiEffect
#if ! JucePlugin_IsSynthvv
		.withInput("Input", AudioChannelSet::stereo(), true)
#endif
		.withOutput("Output", AudioChannelSet::stereo(), true)
#endif  
	)
#endif
{
	processedMidi.ensureSize(50000);
}

/////////////////////////////////////////////////////////////////////////

TopiaryAudioProcessor::~TopiaryAudioProcessor()
{
} // audioProcessor

//==============================================================================
const String TopiaryAudioProcessor::getName() const
{
	return JucePlugin_Name;
} // TopiaryAudioProcessor

/////////////////////////////////////////////////////////////////////////

bool TopiaryAudioProcessor::acceptsMidi() const
{
#if JucePlugin_WantsMidiInput
	return true;
#else
	return false;
#endif
}

/////////////////////////////////////////////////////////////////////////

bool TopiaryAudioProcessor::producesMidi() const
{
#if JucePlugin_ProducesMidiOutput
	return true;
#else
	return false;
#endif
}

/////////////////////////////////////////////////////////////////////////

bool TopiaryAudioProcessor::isMidiEffect() const
{
#if JucePlugin_IsMidiEffect
	return false;
#else
	return false;
#endif
}

/////////////////////////////////////////////////////////////////////////

double TopiaryAudioProcessor::getTailLengthSeconds() const
{
	return 0.0;
}

/////////////////////////////////////////////////////////////////////////

int TopiaryAudioProcessor::getNumPrograms()
{
	return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
				// so this should be at least 1, even if you're not really implementing programs.
}

/////////////////////////////////////////////////////////////////////////

int TopiaryAudioProcessor::getCurrentProgram()
{
	return 0;
} // getCurrentProgram

/////////////////////////////////////////////////////////////////////////

void TopiaryAudioProcessor::setCurrentProgram(int index)
{
	UNUSED(index)
} // setCurrentProgram

/////////////////////////////////////////////////////////////////////////

const String TopiaryAudioProcessor::getProgramName(int index)
{
	UNUSED(index)
		return {};
} // getProgramName

/////////////////////////////////////////////////////////////////////////

void TopiaryAudioProcessor::changeProgramName(int index, const String& newName)
{
	UNUSED(newName)
		UNUSED(index)
} // changeProgramName

/////////////////////////////////////////////////////////////////////////

void TopiaryAudioProcessor::prepareToPlay(double sampleRate, int samplesPerBlock)
{ 
	ignoreUnused(samplesPerBlock);
	model.setSampleRate(sampleRate);
	model.setBlockSize(samplesPerBlock);
	setLatencySamples((int) sampleRate / 1000); //debug for testing - works equally fine
}  // prepareToPlay

/////////////////////////////////////////////////////////////////////////

void TopiaryAudioProcessor::releaseResources()
{
	// When playback stops, you can use this as an opportunity to free up any
	// spare memory, etc.
} // releaseResources

/////////////////////////////////////////////////////////////////////////

#ifndef JucePlugin_PreferredChannelConfigurations
bool TopiaryAudioProcessor::isBusesLayoutSupported(const BusesLayout& layouts) const
{
#if JucePlugin_IsMidiEffect
	ignoreUnused(layouts);
	return true;
#else
	// This is the place where you check if the layout is supported.
	// In this template code we only support mono or stereo.
	if (layouts.getMainOutputChannelSet() != AudioChannelSet::mono()
		&& layouts.getMainOutputChannelSet() != AudioChannelSet::stereo())
		return false;

	// This checks if the input layout matches the output layout
#if ! JucePlugin_IsSynth
	if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
		return false;
#endif

	return true;
#endif
} // isBusesLayoutSupported
#endif

/////////////////////////////////////////////////////////////////////////

void TopiaryAudioProcessor::processBlock(AudioBuffer<float>& buffer, MidiBuffer& midiMessages)
{
	// the audio buffer in a midi effect will have zero channels!
	jassert(buffer.getNumChannels() == 0);

	// processedMidi is declared as a member - because we preallocate plenty of space to avoid a mallo should it run out of space!!!
	// midibuffer object to hold what we generate
	// at the end of the processorblock we will swap &midiMessages (that came in) by processedMidi (what goes out)
	// but we need to clear it because it may still have data in it from previous run

	processedMidi.data.clearQuick();

	model.setBlockSize(buffer.getNumSamples());
	buffer.clear();

	int BPM, numerator, denominator, runState;
	bool override, waitFFN;
	model.getTransportState(BPM, numerator, denominator, runState, override, waitFFN);

	AudioPlayHead::CurrentPositionInfo lastPosInfo;
	auto playHed = getPlayHead();
	if (playHed != nullptr)
	{
		playHed->getCurrentPosition(lastPosInfo);

		if (lastPosInfo.isPlaying && (runState != Topiary::Running)) // Armed as well to take into account??? Don't think so
		{
			if (!override)
			{
				// just arm it - if we get a note it will switch to playing if WFFN; else if will auto-switch before generating MIDI
				model.setRunState(Topiary::Armed);
				runState = model.getRunState();  // because the plugin may want to refuse to arm!!!
			}
		} // detect we are playing depending on override
		else if (!override)
		{ // all other stuff we pick up from host transport
			if (!lastPosInfo.isPlaying && ((runState == Topiary::Running) || (runState == Topiary::Armed)))
			{
				// we'll set model to Stopping; it will then decide to really stop when needed
				model.setRunState(Topiary::Ending);
				runState = Topiary::Ending;
			}
			if (lastPosInfo.bpm != BPM)
			{
				model.setBPM((int)lastPosInfo.bpm);
			}
			if ((lastPosInfo.timeSigNumerator != numerator) || (lastPosInfo.timeSigDenominator != denominator))
			{
				model.setNumeratorDenominator(lastPosInfo.timeSigNumerator, lastPosInfo.timeSigDenominator);
			}
		}
	}
	else model.Log("ERROR>>>> No playhead provided!", Topiary::LogType::Warning);

	if (!waitFFN && (runState == Topiary::Armed))
	{
		
		tellModelToRun();
		runState = Topiary::Running;
	}

	
	MidiMessage msg;
	bool logMidiIn, logMidiOut;	
	model.getMidiLogSettings(logMidiIn, logMidiOut);
	bool notePassThrough = model.getNotePassThrough();


	// first see if there are (CC) messages to be output by the model
	model.outputModelEvents(processedMidi);


	int ignore;  // for the samplePosition in getnextEvent - we ignore that c'se we process immeditately
	for (MidiBuffer::Iterator it(midiMessages); it.getNextEvent(msg, ignore);)
	{
		if (msg.isNoteOn())
		{
			if (!model.midiLearn(msg)) {
				// if we are ready to play and waiting for first note in, start playing
				if (waitFFN && (runState == Topiary::Armed))
				{
					tellModelToRun();
					runState = Topiary::Running;
				}

				model.processAutomation(msg); // because we may have switching by notes
			}
		}
		else
		{
			if (msg.isController())
			{
				model.processAutomation(msg);  // automation by cc messages
				if (!model.midiLearn(msg))
					model.processCC(msg, &processedMidi);
			}
			else
			{
				if (msg.isMidiMachineControlMessage() && !override)
				{   // only respond to this if not overridden
					auto command = msg.getMidiMachineControlCommand();
					switch (command)
					{
					case juce::MidiMessage::mmc_stop:
						model.setRunState(Topiary::Ending);
						runState = Topiary::Ending;
						break;
					case juce::MidiMessage::mmc_play:
						if (waitFFN)
						{
							model.setRunState(Topiary::Armed);
							runState = Topiary::Armed;
						}
						else
						{
							model.setRunState(Topiary::Running);
							runState = Topiary::Running;
						}
						break;
					case juce::MidiMessage::mmc_deferredplay:
						break;
					case juce::MidiMessage::mmc_fastforward:
						break;
					case juce::MidiMessage::mmc_rewind:
						break;
					case juce::MidiMessage::mmc_recordStart:
						model.Log("Host sent Record - only works in editor mode.", Topiary::LogType::Warning);
						break;
					case juce::MidiMessage::mmc_recordStop:
						break;
					case juce::MidiMessage::mmc_pause:
						break;
					default:
						break;
					} // switch MMC command				
				} // incoming MCC commands
			}
			
		}

		if (notePassThrough)
		{
			// pass through anything that came in
			processedMidi.addEvent(msg, (int)msg.getTimeStamp());
			if (logMidiOut) 
				model.logMidi(false, msg);
			
		}

		if (logMidiIn)
			model.logMidi(true, msg);

	} // for loop over incoming midiBuffer 

	////////////////////////////////////
	// start generating new Midi data
	////////////////////////////////////

	//Logger::outputDebugString(String(">>>>>>>>>>>>>>>>>>>Processor ready to check switchVariations"));

	if (runState == Topiary::Running) 
	{
		//bool wasRunning = (runState == Topiary::Running);
		//Logger::outputDebugString(String(">>>>>>>>>>>>>>>>>>>Processor ready to check switchVariations Running ")+String((int)wasRunning));
		if (model.switchingVariations())
		{
			// new variation selected but old one still running; only do this when really running!
			model.processVariationSwitch();
			if (!model.switchingVariations())  // for now this one does the notification
			{
			}
		}
	}

	if (runState == Topiary::Ending)
	{
		if (model.processEnding())
		{
			runState = Topiary::Stopped; // already set to Stopped in the model by processEnding if true; so no need to do that again
		}
	}

	//Logger::outputDebugString(String(">>>>>>>>>>>>>>>>>>>Processor ready to run Running ") + String((int)runState));

	// always call generateMidi - even though we are note Running and not Ending, there may be noteOff events to process

	model.generateMidi(&processedMidi, &midiMessages); // midiMessages might contain data to record; 
	

	midiMessages.swapWith(processedMidi);

} // processBlock

/////////////////////////////////////////////////////////////////////////

bool TopiaryAudioProcessor::hasEditor() const
{
	return true;
} // hasEditor

AudioProcessorEditor* TopiaryAudioProcessor::createEditor()
{
	return new TopiaryAudioProcessorEditor(*this);
}  // createEditor

/////////////////////////////////////////////////////////////////////////

void TopiaryAudioProcessor::getStateInformation(MemoryBlock& destData)
{
	// You should use this method to store your parameters in the memory block.
	// You could do that either as raw data, or use the XML or ValueTree classes
	// as intermediaries to make it easy to save and load complex data.

	model.saveStateToMemoryBlock(destData);
} // getStateInformation

void TopiaryAudioProcessor::setStateInformation(const void* data, int sizeInBytes)
{
	// You should use this method to restore your parameters from this memory block,
	// whose contents will have been created by the getStateInformation() call.
	model.restoreStateFromMemoryBlock(data, sizeInBytes);
} // setStateInformation

/////////////////////////////////////////////////////////////////////////

TOPIARYMODEL* TopiaryAudioProcessor::getModel()
{
	return (&model);
} // getModel

/////////////////////////////////////////////////////////////////////////
// This creates new instances of the plugin.

AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
	return new TopiaryAudioProcessor();
}  // createPluginFilter

#endif