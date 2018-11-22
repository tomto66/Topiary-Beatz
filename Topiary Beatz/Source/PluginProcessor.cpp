/////////////////////////////////////////////////////////////////////////////
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

#include "PluginProcessor.h"
#include "PluginEditor.h"

/////////////////////////////////////////////////////////////////////////

TopiaryBeatsAudioProcessor::TopiaryBeatsAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynthvv
                       .withInput  ("Input",  AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", AudioChannelSet::stereo(), true)
                     #endif  
                       )
#endif
{
	
}

/////////////////////////////////////////////////////////////////////////

TopiaryBeatsAudioProcessor::~TopiaryBeatsAudioProcessor()
{
} // audioProcessor

//==============================================================================
const String TopiaryBeatsAudioProcessor::getName() const
{
    return JucePlugin_Name;
} // TopiaryBeatsAudioProcessor

/////////////////////////////////////////////////////////////////////////

bool TopiaryBeatsAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

/////////////////////////////////////////////////////////////////////////

bool TopiaryBeatsAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

/////////////////////////////////////////////////////////////////////////

bool TopiaryBeatsAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return false;
   #else
    return false;
   #endif
}

/////////////////////////////////////////////////////////////////////////

double TopiaryBeatsAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

/////////////////////////////////////////////////////////////////////////

int TopiaryBeatsAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

/////////////////////////////////////////////////////////////////////////

int TopiaryBeatsAudioProcessor::getCurrentProgram()
{
    return 0;
} // getCurrentProgram

/////////////////////////////////////////////////////////////////////////

void TopiaryBeatsAudioProcessor::setCurrentProgram (int index)
{
	 UNUSED(index)
} // setCurrentProgram

/////////////////////////////////////////////////////////////////////////

const String TopiaryBeatsAudioProcessor::getProgramName (int index)
{
	UNUSED(index)
    return {};
} // getProgramName

/////////////////////////////////////////////////////////////////////////

void TopiaryBeatsAudioProcessor::changeProgramName (int index, const String& newName)
{
	UNUSED(newName)
	UNUSED(index)
} // changeProgramName

/////////////////////////////////////////////////////////////////////////

void TopiaryBeatsAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{ // if there is no audio (which there isn't here, this will never get called!!
  ignoreUnused(samplesPerBlock);
  beatsModel.setSampleRate(sampleRate);  
  beatsModel.setBlockSize(samplesPerBlock);
  
}  // prepareToPlay

/////////////////////////////////////////////////////////////////////////

void TopiaryBeatsAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
} // releaseResources

/////////////////////////////////////////////////////////////////////////

#ifndef JucePlugin_PreferredChannelConfigurations
bool TopiaryBeatsAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    ignoreUnused (layouts);
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

void TopiaryBeatsAudioProcessor::processBlock (AudioBuffer<float>& buffer, MidiBuffer& midiMessages)
{
	// the audio buffer in a midi effect will have zero channels!
	jassert(buffer.getNumChannels() == 0);

	int BPM, numerator, denominator, runState;
	bool override, waitFFN;		
	beatsModel.getTransportState(BPM, numerator, denominator, runState, override, waitFFN);
	
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
				beatsModel.setRunState(Topiary::Armed);
				runState = Topiary::Armed;
			} 
		} // detect we are playing depending on override
		else if (!override) 
		{ // all other stuff we pick up from host transport
			if (!lastPosInfo.isPlaying && (runState == Topiary::Running))
			{   
				// we'll set model to Stopping; it will then decide to really stop when needed
				beatsModel.setRunState(Topiary::Ending);
				runState = Topiary::Ending;
			}
			if (lastPosInfo.bpm != BPM)
			{
				beatsModel.setBPM((int)lastPosInfo.bpm);
			}
			if ((lastPosInfo.timeSigNumerator != numerator) || (lastPosInfo.timeSigDenominator != denominator))
			{
				beatsModel.setNumeratorDenominator(lastPosInfo.timeSigNumerator, lastPosInfo.timeSigDenominator);
			}
		}
	}
	else beatsModel.beatsLog("ERROR>>>> No playhead provided!");
	
	if (!waitFFN && (runState == Topiary::Armed))
	{
		//beatsModel.setRunState(TopiaryBeatsModel::Running); is in tellModel
		tellModelToRun();
		runState = Topiary::Running;
	}
	
	MidiBuffer processedMidi;   // midibuffer object to hold what we generate
								// at the end of the processorblock we will swap &midiMessages (that came in) by processedMidi (what goes out)
	MidiMessage msg;
	
	int ignore;  // for the samplePosition in getnextEvent - we ignore that c'se we process immeditately
	for (MidiBuffer::Iterator it(midiMessages); it.getNextEvent(msg, ignore);)                          
	{
		if (msg.isNoteOn()) 
		{
			Logger::outputDebugString("Note on");
			Logger::outputDebugString(String("WaitFFN ")+String((int)waitFFN));
			Logger::outputDebugString(String("RunState ")+String(runState));

			// if we are ready to play and waiting for first note in, start playing
			if (waitFFN  && (runState == Topiary::Armed))
			{
				tellModelToRun();
				runState = Topiary::Running;
			}
		}
		else 
		{
			if (msg.isController())
			{
				auto controller = msg.getControllerNumber();
				// auto value = msg.getControllerValue();
				// Logger::outputDebugString(String("Controller ") + String(controller) + String(" / ") + String(value) );
				// see if this is a known controller; if so react to it, e.g. change variation
				if ((controller > 21) && (controller < 30))
				{   // hardcoded for now; listen to CC 22-29 for variation change
					int variation = controller - 21;
					beatsModel.setVariation(variation);
				}
			}
			else 
			{
				if (msg.isMidiMachineControlMessage() && override)
				{   // only respond to this if overridden
						auto command = msg.getMidiMachineControlCommand();
						switch (command) 
						{
						case juce::MidiMessage::mmc_stop:
							beatsModel.setRunState(Topiary::Ending);	
							runState = Topiary::Ending;
							break;
						case juce::MidiMessage::mmc_play:
							if (waitFFN)
							{
								beatsModel.setRunState(Topiary::Armed);
								runState = Topiary::Armed;
							}
							else
							{
								beatsModel.setRunState(Topiary::Running);
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
							beatsModel.beatsLog("Host sent RECORD NOTE DONE YET");
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
			if (beatsModel.getNotePassThrough())
			{   
				// pass through anything that came in
				processedMidi.addEvent(msg, 1);
			}
		}	
	} // for loop over incoming midiBuffer 
	
	////////////////////////////////////
	// start generating new Midi data
	////////////////////////////////////
	
	//Logger::outputDebugString(String(">>>>>>>>>>>>>>>>>>>Processor ready to check switchVariations"));

	if ((runState == Topiary::Running) || (runState == Topiary::Armed))
	{
		// in case Armed and not running yet we need to at least set the variation current!
		bool wasRunning = (runState == Topiary::Running);
		//Logger::outputDebugString(String(">>>>>>>>>>>>>>>>>>>Processor ready to check switchVariations Running ")+String((int)wasRunning));
		if (beatsModel.switchingVariations())
		{
			// new variation selected but old one still running; only do this when really running!
			beatsModel.processVariationSwitch();
			if (!beatsModel.switchingVariations())
			{
				// meaning we just did the switch
				// do remaining notes off
				// but only if we are really running (not in case of still Armed)
				if (wasRunning)
					beatsModel.endNotesOn(&processedMidi);
			}
		}
	}

	if (runState == Topiary::Ending)
	{
		if (beatsModel.processEnding())
		{
			beatsModel.endNotesOn(&processedMidi);
			runState = Topiary::Stopped; // already set to Stopped in the model so no need to do that again
		}
	}

	//Logger::outputDebugString(String(">>>>>>>>>>>>>>>>>>>Processor ready to run Running ") + String((int)runState));

	if ((runState == Topiary::Running ) || (runState == Topiary::Ending))
	{
		beatsModel.generateMidi(&processedMidi);
	}
	
	midiMessages.swapWith(processedMidi);
	
} // processBlock

/////////////////////////////////////////////////////////////////////////

bool TopiaryBeatsAudioProcessor::hasEditor() const
{
    return true; 
} // hasEditor

AudioProcessorEditor* TopiaryBeatsAudioProcessor::createEditor()
{
    return new TopiaryBeatsAudioProcessorEditor (*this);
}  // createEditor

/////////////////////////////////////////////////////////////////////////

void TopiaryBeatsAudioProcessor::getStateInformation (MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
	
	beatsModel.saveStateToMemoryBlock(destData);
} // getStateInformation

void TopiaryBeatsAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
	beatsModel.restoreStateFromMemoryBlock(data, sizeInBytes);
} // setStateInformation

/////////////////////////////////////////////////////////////////////////

TopiaryBeatsModel* TopiaryBeatsAudioProcessor::getModel()
{
	return (&beatsModel);
} // getModel

/////////////////////////////////////////////////////////////////////////
// This creates new instances of the plugin.

AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new TopiaryBeatsAudioProcessor();
}  // createPluginFilter