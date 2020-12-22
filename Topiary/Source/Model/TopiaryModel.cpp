/////////////////////////////////////////////////////////////////////////////
/*
This file is part of Topiary, Copyright Tom Tollenaere 2018-21.

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
*/
/////////////////////////////////////////////////////////////////////////////

#pragma once
#include "TopiaryModel.h"

void TopiaryModel::saveStateToMemoryBlock(MemoryBlock& destData)
{
	// virtual
	UNUSED(destData)

} // saveStateToMemoryBlock

/////////////////////////////////////////////////////////////////////////

void TopiaryModel::restoreStateFromMemoryBlock(const void* data, int sizeInBytes)
{
	// virtual
	UNUSED(data)
	UNUSED(sizeInBytes)
} // restoreStateFromMemoryBlock


void TopiaryModel::addParametersToModel() {} // virtual
void TopiaryModel::restoreParametersToModel() {} // virtual

/////////////////////////////////////////////////////////////////////////

void TopiaryModel::savePreset(String msg, String extension)
{
	File f;
	auto directory = File::getSpecialLocation(File::userHomeDirectory);

	if (filePath.compare("") == 0)
		directory = File::File(filePath);
	FileChooser myChooser(msg, directory, extension);

	if (myChooser.browseForFileToSave(true))
	{
		filePath = f.getParentDirectory().getFullPathName();
		f = myChooser.getResult();
		addParametersToModel();  // this adds all data as XML elements to model
		String myXmlDoc = model->toString();
		f.replaceWithText(myXmlDoc);
		//Logger::writeToLog(myXmlDoc);

	}
} // savePreset

/////////////////////////////////////////////////////////////////////////

void TopiaryModel::loadPreset(String msg, String extension)
{
	File f;
	auto directory = File::getSpecialLocation(File::userHomeDirectory);

	if (filePath.compare("") == 0)
		directory = File::File(filePath);

	FileChooser myChooser(msg, directory, extension);

	if (myChooser.browseForFileToOpen())
	{
		filePath = f.getParentDirectory().getFullPathName();
		f = myChooser.getResult();
		model = XmlDocument::parse(f);
		restoreParametersToModel();
	}

} //loadPreset

/////////////////////////////////////////////////////////////////////////

TopiaryModel::TopiaryModel()
{
	filePath = "";
	logString = "";
	
	overrideHostTransport = true;
	runState = Topiary::Stopped;
	BPM = 120;
	numerator = 4; denominator = 4;
	variationSelected = 0;
	variationRunning = 0;
	
} // TopiaryModel

//////////////////////////////////////////////////////////////////////////////////////////////////////

TopiaryModel::~TopiaryModel()
{
	runState = Topiary::Stopped;

} //~TopiaryModel

///////////////////////////////////////////////////////////////////////
// VARIATIONS
///////////////////////////////////////////////////////////////////////


int TopiaryModel::getVariationLenInTicks(int v)
{
	// virtual
	UNUSED(v)
	return(0);
}

void TopiaryModel::setVariation(int v)
{
	// virtual
	UNUSED(v)
}

bool TopiaryModel::getVariationEnabled(int v)
{
	// virtual
	UNUSED(v)
	return(false);
}

///////////////////////////////////////////////////////////////////////
// LOGGER
///////////////////////////////////////////////////////////////////////

String* TopiaryModel::getLog()
{
	return &logString;
} // getLog

///////////////////////////////////////////////////////////////////////

void TopiaryModel::Log(String s, int logType)
{
	if (logString.length() > 8000) 
		logString = logString.substring(4000);

	switch (logType)
	{
	case Topiary::LogType::MidiIn:
		if (!logMidiIn) return;
		break;
	case Topiary::LogType::MidiOut:
		if (!logMidiOut) return;
		break;
	case Topiary::LogType::Warning:
		if (!logWarning) return;
		break;
	case Topiary::LogType::Info:
		if (!logInfo) return;
		break;
	case Topiary::LogType::Transport:
		if (!logTransport) return;
		break;
	case Topiary::LogType::Variations:
		if (!logVariations) return;
		break;
	case Topiary::LogType::License:
		break;
	default: return;
		break;
	}

	Time t = Time::getCurrentTime();
	char pre[20];
	if (logType == Topiary::LogType::License)
		strcpy(pre, "");
	else
		sprintf(pre, "%02d:%02d:%02d:%03d : ", t.getHours(), t.getMinutes(), t.getSeconds(), t.getMilliseconds());

	logString.append(String(pre) + s + newLine, 250);

	broadcaster.sendActionMessage(MsgLog);
	if (logType == Topiary::LogType::Warning) {
		broadcaster.sendActionMessage(MsgWarning);  // warn the user in the header
		lastWarning = s;
	}
}  // Log

///////////////////////////////////////////////////////////////////////

String TopiaryModel::getLastWarning()
{
	return lastWarning;

} // getLastWarning

///////////////////////////////////////////////////////////////////////

void TopiaryModel::setLogSettings(bool warning, bool midiIn, bool midiOut, bool transport, bool variations, bool info)
{
	bool updateNeeded = false;

	if (logWarning != warning)
	{
		logWarning = warning;
		updateNeeded = true;
	}

	else if (logMidiIn != midiIn)
	{
		logMidiIn = midiIn;
		updateNeeded = true;
	}

	else if (logMidiOut != midiOut)
	{
		logMidiOut = midiOut;
		updateNeeded = true;
	}

	else if (logTransport != transport)
	{
		logTransport = transport;
		updateNeeded = true;
	}

	else if (logVariations != variations)
	{
		logVariations = variations;
		updateNeeded = true;
	}

	else if (logInfo != info)
	{
		logInfo = info;
		updateNeeded = true;
	}

	else if (updateNeeded)
		broadcaster.sendActionMessage(MsgLog);

} // setLogSettings

///////////////////////////////////////////////////////////////////////

void TopiaryModel::clearLog()
{
	logString = "";
	broadcaster.sendActionMessage(MsgLog);

} // clearLog

///////////////////////////////////////////////////////////////////////

void TopiaryModel::getLogSettings(bool& warning, bool& midiIn, bool& midiOut, bool &transport, bool &variations, bool &info)
{
	warning = logWarning;
	midiIn = logMidiIn;
	midiOut = logMidiOut;
	transport = logTransport;
	variations = logVariations;
	info = logInfo;
	
} // getLogSettings

///////////////////////////////////////////////////////////////////////

void TopiaryModel::getMidiLogSettings(bool& in, bool& out)
{
	// only used by processor to avoid unneeded calls to Log()
	in = logMidiIn;
	out = logMidiOut;

} // getMidiLogSettings

///////////////////////////////////////////////////////////////////////

void TopiaryModel::logMidi(bool in, MidiMessage &msg)
{
	// only used by processor to avoid unneeded calls to beatsLog()
	if (in)
	{
		if (!logMidiIn) return;
		if (msg.isNoteOff() || msg.isNoteOn())
		{
			int chan = msg.getChannel();
			int velo = msg.getVelocity();
			int  note = msg.getNoteNumber();

			if (msg.isNoteOff())
				Log(String("MIDI in ") + "Note off: "+ noteNumberToString(note) + " velocity: " + String(velo)+"  channel: " + String(chan) + ".", Topiary::LogType::MidiIn);
			else
				Log(String("MIDI in ") + "Note on: " + noteNumberToString(note) + " velocity: " + String(velo) + " channel: " + String(chan) + ".", Topiary::LogType::MidiIn);
		}
		else
		{
			if (msg.isController())
				Log("CC in Controller: " + String(msg.getControllerNumber()) + ": "+ String(msg.getControllerValue()) + " channel: " +  String(msg.getChannel()) + ".", Topiary::LogType::MidiIn);
		}
	}
	else
	{
		if (!logMidiOut) 
			return;
		if (msg.isNoteOff() || msg.isNoteOn())
		{
			int chan = msg.getChannel();
			int velo = msg.getVelocity();
			int  note = msg.getNoteNumber();

			if (msg.isNoteOff())
				Log(String("MIDI out ") + "Note off: " + noteNumberToString(note)+ " velocity: " + String(velo) + " channel: " + String(chan)+".", Topiary::LogType::MidiOut);
			else
				Log(String("MIDI out ") + "Note on: " + noteNumberToString(note) + " velocity: " + String(velo) + " channel: " + String(chan) + ".", Topiary::LogType::MidiOut);
		}
		else
		{
			if (msg.isController())
				Log("CC out Controller: " + String(msg.getControllerNumber()) + ": " + String(msg.getControllerValue()) + " channel: " + String(msg.getChannel()) + ".", Topiary::LogType::MidiOut);
		}
	}

} // logMidi

///////////////////////////////////////////////////////////////////////
// TimeSignatures & Transport
///////////////////////////////////////////////////////////////////////

void TopiaryModel::setOverrideHostTransport(bool o)
{
	// virtual
	UNUSED(o)
}


///////////////////////////////////////////////////////////////////////

void TopiaryModel::getTransportState(int& b, int& n, int& d, int& bs, bool& override, bool &waitFFN)
// all transport variables, inc signature, BPM etc
{
	b = BPM;
	n = numerator;
	d = denominator;
	bs = runState;
	waitFFN = WFFN;
	override = overrideHostTransport;
	
} // getTransportState

///////////////////////////////////////////////////////////////////////

void TopiaryModel::setNumeratorDenominator(int nu, int de)
{
	UNUSED(nu);
	UNUSED(de);
	jassert(false); // virtual

} // setNumeratorDenominator

///////////////////////////////////////////////////////////////////////

void TopiaryModel::setBPM(int n)
{
	
	if (n == 0) n = 120;  // do not allow 0 BPM
	if (BPM != n)
	{
		const GenericScopedLock<CriticalSection> myScopedLock(lockModel);
		BPM = n;
		recalcRealTime(); // housekeeping!
		Log(String("BPM set to ") + String(n), Topiary::LogType::Transport);
		
		broadcaster.sendActionMessage(MsgTransport);
	}
} // setBPM

///////////////////////////////////////////////////////////////////////

void TopiaryModel::setRunState(int n)
{
	jassert(false);
	// only call with false when called from generateMidi - because there we already have the lock!

	int remember;
	remember = runState;  // needed because in 1 case setting to Armed should fail!!!
	bool varEnabled = false;
	int enabledVariation = -1;
	bool changeSetVariation = false;

	if (runState != n)
	{
		const GenericScopedLock<CriticalSection> myScopedLock(lockModel);
		
		switch (n)
		{
		case  Topiary::Running:
			Log("Start running.", Topiary::LogType::Transport);
			//Logger::outputDebugString("RTcur to 0");
			// initialize the patterncursor
			patternCursor = 0;
			blockCursor = 0;
			cursorToStop = (int64)-1;
			runState = Topiary::Running;
			broadcaster.sendActionMessage(MsgTransport);
			break;

		case Topiary::Stopped:

			// reset stuff
			cursorToStop = (int64)-1;
			
			Log("Stopped.", Topiary::LogType::Transport);
			broadcaster.sendActionMessage(MsgTransport);

			// if there is a variation waiting - do that outside this scoped lock otherwise we'll lock -- see below
			runState = Topiary::Stopped;
			break;

		case Topiary::Ending:
			Log("Ending, cleaning up.", Topiary::LogType::Transport);
			runState = Topiary::Ending;
			broadcaster.sendActionMessage(MsgTransport);
			break;
		case Topiary::Armed:
			
				// make sure there are variations enbabled
				// and that we selected an enabled variation
				blockCursor = 0;
				patternCursor = 0;

				for (int i = 0; i < 8; i++)
				{
					if (getVariationEnabled(i))
					{
						varEnabled = true;
						if (enabledVariation == -1)
						{
							enabledVariation = i;
						}
					}
				}
				if (varEnabled)
				{
					if (!getVariationEnabled(variationSelected))
					{
						changeSetVariation = true;
						// we need to call setVariation but that one also needs the lock - do that when lock has been released
					}

					if (runState != Topiary::Ending)
					{
						runState = Topiary::Armed;
						Log("Armed, waiting for first note.", Topiary::LogType::Transport);
						broadcaster.sendActionMessage(MsgTransport);
					}
				}
				else
				{
					Log("Cannot run because there is no variation enabled.", Topiary::LogType::Warning);
					runState = Topiary::Stopped;
				}
			
			break;
		default:
			break;

		}
		
		broadcaster.sendActionMessage(MsgTransport);
		
	}

	if (changeSetVariation)
	{
		// if the currently selected variation is disabled switch to one that has been selected!
		// we do that here because setVariation needs the model lock!
		setVariation(enabledVariation);
	}
	else
	{
		setVariation(variationSelected);	// so that if the button was orange, it becomes blue again
	}

	// now the first waiting variation might stil be orange; fix that below
	if (remember == Topiary::Armed)
		setVariation(variationSelected);

} // setRunState

///////////////////////////////////////////////////////////////////////

int TopiaryModel::getRunState()
{
	return runState;
} // getRunState

///////////////////////////////////////////////////////////////////////

void TopiaryModel::processTransportControls(int buttonEnabled)  // buttonEnabled; 0=Stop; 1=start; 2=rec
{
	// careful - transport has 3 buttons but runState has more states!!! need logic to convert here!
	
	// can only be called when overiden
	jassert(overrideHostTransport);

	if (buttonEnabled == 1)
	{
		//  start button pressed
		if ((runState != Topiary::Running) && (runState != Topiary::Armed))
		{
			setRunState(Topiary::Armed);  // if override then either processblock will switch it to Running asap, or processblock will set it to Running at first note 
			// else do nothing otherwise it would restart!	
			broadcaster.sendActionMessage(MsgTransport);
		}
	}
	else
	{   // stop button pressed
		if (buttonEnabled == 0)
		{
			if (runState == Topiary::Armed)
			{
				setRunState(Topiary::Stopped);  // because then it never got started in the first place
				broadcaster.sendActionMessage(MsgTransport);
				return;
			}
			else
			{
				if (runState == Topiary::Running)
				{
					setRunState(Topiary::Ending);  // it will go to Stopped in processblock, when the time has come (depending on runStopQ)
					broadcaster.sendActionMessage(MsgTransport);
				}
				else
				{
					setRunState(Topiary::Stopped);
					broadcaster.sendActionMessage(MsgTransport);
				}
				return;
			}

		}
		else
		{
			jassert(false); // REC not written yet
		}
	}

} // processTransportControls

///////////////////////////////////////////////////////////////////////



///////////////////////////////////////////////////////////////////////
// GENERATOR STUFF
///////////////////////////////////////////////////////////////////////

void TopiaryModel::setSampleRate(double sr)
{
	if (sr != sampleRate)  // not really needed because also checked in processor !!!
	{
		sampleRate = sr;
		recalcRealTime(); // housekeeping
		Log(String("Samplerate set to ") + String(sampleRate) + String("."), Topiary::LogType::Info);
	}

} // setSampleRate

///////////////////////////////////////////////////////////////////////

void TopiaryModel::setStartTimes()
{ // time in samples when running really starts + other housekeeping
	//rtCursor = 0;
	blockCursor = 0;
	variationRunning = variationSelected;
	//for (int i = 0; i < 4; i++)
	//	for (int j = 0; j < 8; j++)
	//	{
	//		// initialize variation regeneration
	//		variation[j].poolIdCursor[i] = 1;
	//		variation[j].poolTickCursor[i] = 0;
	//	}
	broadcaster.sendActionMessage(MsgVariationSelected);  // so orange waiting button turn blue;

} // setStartTimes

///////////////////////////////////////////////////////////////////////

void TopiaryModel::setBlockSize(int blocksz)
{
	if (blocksz != blockSize)
	{
		blockSize = blocksz;
		Log(String("Blocksize ") + String(blockSize), Topiary::LogType::Info);
	}
} // setBlockSize

///////////////////////////////////////////////////////////////////////

void TopiaryModel::generateMidi(MidiBuffer *buffer, MidiBuffer* recBuffer)
{
	UNUSED(buffer)
	UNUSED(recBuffer)
} // generateMidi

///////////////////////////////////////////////////////////////////////

bool TopiaryModel::processVariationSwitch() // called just before generateMidi - to see if a variation is changed, and if so whether to switch now (and make the switch)
{
	// virtual, but standard code that is in TopiaryIncludes.h
	jassert(false);
	return true;
} // processVariationSwitch

///////////////////////////////////////////////////////////////////////

void TopiaryModel::outputModelEvents(MidiBuffer& buffer)
{	
	// outputs what is in modelEventBuffer
	MidiMessage msg;
	
	const GenericScopedLock<CriticalSection> myScopedLock(lockModel);

	for (const MidiMessageMetadata metadata : modelEventBuffer)
	{
		msg = metadata.getMessage();
		buffer.addEvent(msg, 0);
	}

	modelEventBuffer.clear();

}  // outputModelEvents

///////////////////////////////////////////////////////////////////////

void TopiaryModel::outputVariationEvents()
{
	// virtual - potentially generate events when variation button is pressed (outside of running) - certainly needed for presetz

} // outputVariationEvents

///////////////////////////////////////////////////////////////////////

bool TopiaryModel::switchingVariations()
{
	// virtual, but standard code that is in TopiaryIncludes.h
	return true;
} // switchingVariations

///////////////////////////////////////////////////////////////////////

void TopiaryModel::initializeVariationsForRunning()
{
	// virtual
}
///////////////////////////////////////////////////////////////////////

void TopiaryModel::endNotesOn(MidiBuffer* midiBuffer)
{
	
	for (int i=1;i<=16;i++)
	{
		MidiMessage msg = MidiMessage::allNotesOff(i);
		midiBuffer->addEvent(msg, 1);
	}

} // endNotesOn

///////////////////////////////////////////////////////////////////////

bool TopiaryModel::processEnding() // called just before generateMidi - to see if we need to stop, and if so whether to stop now (and stop if needed)
{
	// see if we are past the last note
	// we are going to set global variable cursorToStop, based on current block cursor; and only if it is not equal to -1 

	if (cursorToStop == -1)
	{
		switch (runStopQ) {
		case (Topiary::Immediate):
		{
			cursorToStop = blockCursor;
			break;
		}
		case (Topiary::WholePattern):
		{
			cursorToStop = (int64)((blockCursor - patternCursor*samplesPerTick) /* start of this pattern */ + (getVariationLenInTicks(variationRunning)*samplesPerTick));
			//Logger::outputDebugString(String("blockCur ") + String(blockCursor) + String(" stopCursor ") + String(cursorToStop));

			break;
		}
		case (Topiary::Measure):
		{
			cursorToStop = (int64)(blockCursor + samplesPerTick * ((Topiary::TicksPerQuarter - tick - 1) + (numerator - beat - 1)* Topiary::TicksPerQuarter));
			break;
		}
		case (Topiary::Half):
		{
			cursorToStop = (int64)(blockCursor + samplesPerTick * ((Topiary::TicksPerQuarter - tick - 1) + Topiary::TicksPerQuarter));
			break;
		}
		case (Topiary::Quarter):
		{
			cursorToStop = (int64)(blockCursor + samplesPerTick * ((Topiary::TicksPerQuarter - tick - 1)));
			break;
		}

		} // switch
	}

	// now see if we're close enough to cursorToStop
	if (cursorToStop < (blockCursor + blockSize))
	{
		cursorToStop = -1;
		setRunState(Topiary::Stopped);
		return true;
	}
	else return false;

} // processEnding


///////////////////////////////////////////////////////////////////////
// Settings stuff
///////////////////////////////////////////////////////////////////////

void TopiaryModel::setWFFN(bool wffn)
{
	WFFN = wffn;
}

///////////////////////////////////////////////////////////////////////

bool TopiaryModel::getWFFN()
{
	return WFFN;
}

///////////////////////////////////////////////////////////////////////

void TopiaryModel::setNotePassThrough(bool echo)
{
	notePassThrough = echo;
} // setNotePassThrough

///////////////////////////////////////////////////////////////////////

bool TopiaryModel::getNotePassThrough()
{
	return notePassThrough;
} // getNotePassThrough

///////////////////////////////////////////////////////////////////////

void TopiaryModel::setVariationStartQ(int q)
{
	variationStartQ = q;
} // setVariationStartQ

///////////////////////////////////////////////////////////////////////

int TopiaryModel::getVariationStartQ()
{
	return variationStartQ;
} // getVariationStartQ

///////////////////////////////////////////////////////////////////////

void TopiaryModel::setRunStopQ(int q)
{
	runStopQ = q;
} // setRunStopQ

///////////////////////////////////////////////////////////////////////

int TopiaryModel::getRunStopQ()
{
	return runStopQ;
} // setRunStopQ

///////////////////////////////////////////////////////////////////////

void TopiaryModel::getTime(int& m, int& b)
{
	const GenericScopedLock<CriticalSection> myScopedLock(lockModel); // wait if it's generating
	m = measure;
	b = beat;

} // setRunStopQ

///////////////////////////////////////////////////////////////////////

String TopiaryModel::getName()
{
	return name;

} // getName

///////////////////////////////////////////////////////////////////////

void TopiaryModel::setName(String n)
{
	name = n;

} // setName

///////////////////////////////////////////////////////////////////////

void TopiaryModel::setSwitchVariation(int a)
{
	switchVariation = a;

} // setSwitchVariation

///////////////////////////////////////////////////////////////////////

int TopiaryModel::getSwitchVariation()
{
	return switchVariation;

} // getSwitchVariation

///////////////////////////////////////////////////////////////////////
// Broadcaster & listeners
///////////////////////////////////////////////////////////////////////

void TopiaryModel::setListener(ActionListener *listener)
{
	broadcaster.addActionListener(listener);

} // setListener


void TopiaryModel::removeListener(ActionListener *listener)
{
	broadcaster.removeActionListener(listener);

} // setListener

void TopiaryModel::sendActionMessage(String s)
{
	broadcaster.sendActionMessage(s);

} // sendActionMessage




///////////////////////////////////////////////////////////////////////
// Automation Parameters
///////////////////////////////////////////////////////////////////////

void TopiaryModel::setVariationControl(bool ccSwitching, int channel, int switches[8])
{
	ccVariationSwitching = ccSwitching;
	midiChannelListening = channel;

	for (int i = 0; i < 8; i++)
	{
		variationSwitch[i] = switches[i];
	}

} // setVariationControl

///////////////////////////////////////////////////////////////////////

void TopiaryModel::getVariationControl(bool& ccSwitching, int& channel, int switches[8])
{
	ccSwitching = ccVariationSwitching;
	channel = midiChannelListening;

	for (int i = 0; i < 8; i++)
	{
		switches[i] = variationSwitch[i];
	}

} // getVariationControl

///////////////////////////////////////////////////////////////////////

void TopiaryModel::processCC(MidiMessage& msg, MidiBuffer* buffer)
{
	// virtual
	UNUSED(msg);
	UNUSED(buffer);

} // processCC

void TopiaryModel::processCC(MidiMessage& msg)
{
	// virtual
	UNUSED(msg);

} // processCC

///////////////////////////////////////////////////////////////////////

void TopiaryModel::processAutomation(MidiMessage& msg)
{
	
	// assert that called only calls this consistently: msg is CC if ccVariationSwithing; msg is NoteOn it not
	if (ccVariationSwitching && !msg.isController())
		return;

	if (!ccVariationSwitching && !msg.isNoteOn())
	return;

	int channel = msg.getChannel();
	int note;
	int controller;

	if (ccVariationSwitching)
	{
		// we use midi CC messages for variation switching
		for (int i = 0; i < 8; i++)
		{
			controller = msg.getControllerNumber();
			if (controller == variationSwitch[i])
			{
				if ((midiChannelListening == 0) || (midiChannelListening == channel))
				{
					setVariation(i); 
					break;
				}
			}
		}
	}
	else
	{
		// we use note on events for variation switching
		for (int i = 0; i < 8; i++)
		{
			note = msg.getNoteNumber();
			if (note == variationSwitch[i])
			{
				if ((midiChannelListening == 0) || (midiChannelListening == channel))
				{
					setVariation(i); 
					break;
				}
			}
		}
	}
} // processAutomation

///////////////////////////////////////////////////////////////////////


ActionBroadcaster* TopiaryModel::getBroadcaster()
{
	return &broadcaster;

}

///////////////////////////////////////////////////////////////////////

void TopiaryModel::copyVariation(int from, int to)
{
	// virtual
	UNUSED(from);
	UNUSED(to);

} // copyVariation

///////////////////////////////////////////////////////////////////////

void TopiaryModel::swapVariation(int from, int to)
{
	// virtual
	UNUSED(from);
	UNUSED(to);

} // swapVariation

///////////////////////////////////////////////////////////////////////

void TopiaryModel::swapPreset(int from, int to)
{
	// virtual
	UNUSED(from);
	UNUSED(to);

} // swapPreset

///////////////////////////////////////////////////////////////////////

void TopiaryModel::copyPreset(int from, int to)
{
	// virtual
	UNUSED(from);
	UNUSED(to);

} // copyPreset

///////////////////////////////////////////////////////////////////////

void TopiaryModel::swapPattern(int from, int to)
{
	// virtual
	UNUSED(from);
	UNUSED(to);

} // swapPattern

///////////////////////////////////////////////////////////////////////

void TopiaryModel::copyPattern(int from, int to)
{
	// virtual
	UNUSED(from);
	UNUSED(to);

} // copyPattern

///////////////////////////////////////////////////////////////////////

void TopiaryModel::learnMidi(int ID)
{
	const GenericScopedLock<CriticalSection> myScopedLock(lockModel);
	learningMidi = true;  // signals processor of incoming midi that we want to learn
	midiLearnID = ID;
	Log("Learning MIDI.", Topiary::LogType::Warning);
}

/////////////////////////////////////////////////////////////////////////////

void TopiaryModel::stopLearningMidi()
{
	const GenericScopedLock<CriticalSection> myScopedLock(lockModel);
	learningMidi = false;
} // stopLearningMidi

/////////////////////////////////////////////////////////////////////////////

void TopiaryModel::record(bool b)
{
	// careful - overridden in Riffz & friends
	const GenericScopedLock<CriticalSection> myScopedLock(lockModel);
	
	recordingMidi = b;
	// inform transport
	broadcaster.sendActionMessage(MsgTransport);
}

/////////////////////////////////////////////////////////////////////////////

bool TopiaryModel::getRecording()
{
	return recordingMidi;

} // getRecording

/////////////////////////////////////////////////////////////////////////////

void TopiaryModel::setPatternSelectedInPatternEditor(int p)
{
	// needed to that when setting "record" we can check whether the pattern being edited is actually going to run 
	patternSelectedInPatternEditor = p;
}

/////////////////////////////////////////////////////////////////////////////

void TopiaryModel::processMidiRecording()
{
	// virtual
}

/////////////////////////////////////////////////////////////////////////////

void TopiaryModel::timestampToMBT(int t, int& m, int& b, int& tck)
{
	m = (int)floor(t / (denominator*Topiary::TicksPerQuarter));
	b = t - (m * denominator*Topiary::TicksPerQuarter);
	b = (int)floor(b / Topiary::TicksPerQuarter);
	tck = t % Topiary::TicksPerQuarter;
}

/////////////////////////////////////////////////////////////////////////////

void TopiaryModel::MBTToTick(int& t, int m, int b, int tck)
{
	t = tck +
		b * Topiary::TicksPerQuarter +
		m * Topiary::TicksPerQuarter*denominator;
}

/////////////////////////////////////////////////////////////////////////////

void TopiaryModel::addToModelEventBuffer(MidiMessage* msg)
{
	// adds the msg to the modelEventBuffer

	const GenericScopedLock<CriticalSection> myScopedLock(lockModel);
	modelEventBuffer.addEvent(*msg, 0);

}  // outputModelEvents


//////////////////////////////////////////////////////////////////////////////////////////////////
// load & save stuff stuff	
//////////////////////////////////////////////////////////////////////////////////////////////////

void TopiaryModel::addToModel(XmlElement *p, int i, char* iname)
{
		auto parameter = new XmlElement("Parameter");
		parameter->setAttribute("Name", iname);
		parameter->setAttribute("Value", i);
		p->addChildElement(parameter);

} //addIntToModel

//////////////////////////////////////////////////////////////////////////////////////////////////

void TopiaryModel::addToModel(XmlElement *p, int i, char* iname, int index)
{
		auto parameter = new XmlElement("Parameter");
		parameter->setAttribute("Name", iname);
		parameter->setAttribute("Value", i);
		parameter->setAttribute("Index", index);
		p->addChildElement(parameter);

} // addIntToModel

void TopiaryModel::addToModel(XmlElement* p, int i, char* iname, int index1, int index2)
{
	auto parameter = new XmlElement("Parameter");
	parameter->setAttribute("Name", iname);
	parameter->setAttribute("Value", i);
	parameter->setAttribute("Index1", index1);
	parameter->setAttribute("Index2", index2);
	p->addChildElement(parameter);
}
//////////////////////////////////////////////////////////////////////////////////////////////////

void TopiaryModel::addToModel(XmlElement *p, bool b, char* bname)
{
		auto parameter = new XmlElement("Parameter");
		parameter->setAttribute("Name", bname);
		parameter->setAttribute("Value", (int)b);
		p->addChildElement(parameter);

} // addBoolToModel

//////////////////////////////////////////////////////////////////////////////////////////////////

void TopiaryModel::addToModel(XmlElement *p, bool b, char* bname, int index)
{
		auto parameter = new XmlElement("Parameter");
		parameter->setAttribute("Name", bname);
		parameter->setAttribute("Value", (int)b);
		parameter->setAttribute("Index", index);
		p->addChildElement(parameter);

} // addBoolToModel

//////////////////////////////////////////////////////////////////////////////////////////////////

void TopiaryModel::addToModel(XmlElement *p, String value, char* sname)
{
		auto parameter = new XmlElement("Parameter");
		parameter->setAttribute("Name", sname);
		parameter->setAttribute("Value", value);
		p->addChildElement(parameter);

} // addStringToModel

//////////////////////////////////////////////////////////////////////////////////////////////////

void TopiaryModel::addToModel(XmlElement *p, String value, char* sname, int index)
{
		auto parameter = new XmlElement("Parameter");
		parameter->setAttribute("Name", sname);
		parameter->setAttribute("Value", value);
		parameter->setAttribute("Index", index);
		p->addChildElement(parameter);

} // addStringToModel

//////////////////////////////////////////////////////////////////////////////////////////////////

