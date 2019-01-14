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
*/
/////////////////////////////////////////////////////////////////////////////

#include "../JuceLibraryCode/JuceHeader.h"
#include "Topiary.h"
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

void TopiaryModel::savePreset(File f)
{
	addParametersToModel();  // this adds and XML element "Parameters" to the model
	String myXmlDoc = model->createDocument(String());
	f.replaceWithText(myXmlDoc);
	//Logger::writeToLog(myXmlDoc);

	// now delete the no-longer-needed "Parameters" child
	model->deleteAllChildElementsWithTagName("Parameters");

} // savePreset

/////////////////////////////////////////////////////////////////////////

void TopiaryModel::loadPreset(File f)
{
	model.reset(XmlDocument::parse(f));
	restoreParametersToModel();

} //loadPreset

/////////////////////////////////////////////////////////////////////////

#define xstr(s) str(s)
#define str(s) #s

TopiaryModel::TopiaryModel()
{
	filePath = "";
	logString = "";
	
	overrideHostTransport = false;
	runState = Topiary::Stopped;
	BPM = 120;
	numerator = 4; denominator = 4;
	topiaryThread.setModel(this);
	topiaryThread.startThread(10); // thread should start with a wait - and inherited model should call notify when init is done

} // TopiaryModel

//////////////////////////////////////////////////////////////////////////////////////////////////////

TopiaryModel::~TopiaryModel()
{
	topiaryThread.stopThread(-1);
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
	case Topiary::LogType::Debug:
		if (!logDebug) return;
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

void TopiaryModel::setLogSettings(bool warning, bool midiIn, bool midiOut, bool debug, bool transport, bool variations, bool info)
{
	bool updateNeeded = false;

	if (logWarning != warning)
	{
		logWarning = warning;
		updateNeeded = true;
	}

	if (logMidiIn != midiIn)
	{
		logMidiIn = midiIn;
		updateNeeded = true;
	}

	if (logMidiOut != midiOut)
	{
		logMidiOut = midiOut;
		updateNeeded = true;
	}

	if (logDebug != debug)
	{
		logDebug = debug;
		updateNeeded = true;
	}

	if (logTransport != transport)
	{
		logTransport = transport;
		updateNeeded = true;
	}

	if (logVariations != variations)
	{
		logVariations = variations;
		updateNeeded = true;
	}

	if (logInfo != info)
	{
		logInfo = info;
		updateNeeded = true;
	}

	if (updateNeeded)
		broadcaster.sendActionMessage(MsgLog);

} // setLogSettings

///////////////////////////////////////////////////////////////////////

void TopiaryModel::clearLog()
{
	logString = "";
	broadcaster.sendActionMessage(MsgLog);

} // clearLog

///////////////////////////////////////////////////////////////////////

void TopiaryModel::getLogSettings(bool& warning, bool& midiIn, bool& midiOut, bool& debug, bool &transport, bool &variations, bool &info)
{
	warning = logWarning;
	midiIn = logMidiIn;
	midiOut = logMidiOut;
	debug = logDebug;
	transport = logTransport;
	variations = logVariations;
	info = logInfo;
	

} // getLogSettings

///////////////////////////////////////////////////////////////////////

void TopiaryModel::getMidiLogSettings(bool& in, bool& out)
{
	// only used by processor to avoid unneeded calls to beatsLog()
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
				Log(String("MIDI in: ") + "Note off "+ MidiMessage::getMidiNoteName(note, true, true, 5) +" Velocity "+String(velo)+" Channel"+String(chan) + ".", Topiary::LogType::MidiIn);
			else
				Log(String("MIDI in: ") + "Note on " + MidiMessage::getMidiNoteName(note, true, true, 5) + " Velocity " + String(velo) + " Channel" + String(chan) + ".", Topiary::LogType::MidiIn);
		}
		else
		{
			if (msg.isController())
				Log("CC in: controller " + String(msg.getControllerNumber()) + ": "+ String(msg.getControllerValue()) + " on channel " +  String(msg.getChannel()) + ".", Topiary::LogType::MidiIn);
		}
	}
	else
	{
		if (!logMidiOut) return;
		if (msg.isNoteOff() || msg.isNoteOn())
		{
			int chan = msg.getChannel();
			int velo = msg.getVelocity();
			int  note = msg.getNoteNumber();

			if (msg.isNoteOff())
				Log(String("MIDI out: ") + "Note off " + MidiMessage::getMidiNoteName(note, true, true, 5) + " Velocity " + String(velo) + " on Channel" + String(chan)+".", Topiary::LogType::MidiIn);
			else
				Log(String("MIDI out: ") + "Note on " + MidiMessage::getMidiNoteName(note, true, true, 5) + " Velocity " + String(velo) + " on Channel" + String(chan) + ".", Topiary::LogType::MidiIn);
		}
		else
		{
			if (msg.isController())
				Log("CC out: controller " + String(msg.getControllerNumber()) + ": " + String(msg.getControllerValue()) + " on channel " + String(msg.getChannel()) + ".", Topiary::LogType::MidiOut);
		}
	}

} // getLogMidi

///////////////////////////////////////////////////////////////////////
// TimeSignatures & Transport
///////////////////////////////////////////////////////////////////////

void TopiaryModel::setOverrideHostTransport(bool o)
{
	//const GenericScopedLock<SpinLock> myScopedLock(lockModel); 

	if (overrideHostTransport != o)
	{
		overrideHostTransport = o;
		
		broadcaster.sendActionMessage(MsgTransport);
		// careful here - if we just set the runstate to Stopped - it might have been stopped already and variables may be undefined
		// dirty hack below
		runState = -1000000; // to force a runstate stopped below!!!
		setRunState(Topiary::Stopped);
		if (o)
		{
			Log(String("Host transport overriden."), Topiary::LogType::Transport);
			recalcRealTime();
		}
		else
		{
			Log(String("Host transport in control."), Topiary::LogType::Transport);
			recalcRealTime();
		}

	}
} // setOverrideHostTransport

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
	const GenericScopedLock<SpinLock> myScopedLock(lockModel);
	if ((numerator != nu) || (denominator != de))
	{
		numerator = nu;
		denominator = de;
		recalcRealTime();
		Log(String("Signature set to ") + String(numerator) + String("/") + String(denominator), Topiary::LogType::Transport);
		broadcaster.sendActionMessage(MsgTransport);
		//if (numPatterns > 0)
		//	beatsLog("Careful: if you have data loaded in the patterns and you change time signature, results may be unexpected", Topiary::LogType::Warning);
	}

} // setNumeratorDenominator

///////////////////////////////////////////////////////////////////////

void TopiaryModel::setBPM(int n)
{
	const GenericScopedLock<SpinLock> myScopedLock(lockModel);
	if (n == 0) n = 120;  // do not allow 0 BPM
	if (BPM != n)
	{
		BPM = n;
		recalcRealTime(); // housekeeping!
		Log(String("BPM set to ") + String(n), Topiary::LogType::Transport);
		
		broadcaster.sendActionMessage(MsgTransport);
	}
} // setBPM

///////////////////////////////////////////////////////////////////////

void TopiaryModel::setRunState(int n)
{
	
	int remember;
	remember = runState;  // needed because in 1 case setting to Armed should fail!!!
	bool varEnabled = false;
	int enabledVariation = -1;
	bool changeSetVariation = false;

	if (runState != n)
	{
		const GenericScopedLock<SpinLock> myScopedLock(lockModel);  // because we may need to call setVariation and that also relies on the lock!
		runState = n;
		switch (n)
		{
		case  Topiary::Running:
			Log("Start running.", Topiary::LogType::Transport);
			//Logger::outputDebugString("RTcur to 0");
			// initialize the patterncursor
			patternCursor = 0;
			blockCursor = 0;
			cursorToStop = (int64)-1;
			threadRunnerState = Topiary::ThreadRunnerState::NothingToDo;
			broadcaster.sendActionMessage(MsgTransport);
			break;

		case Topiary::Stopped:

			// reset stuff
			patternCursor = 0;
			blockCursor = 0;
			cursorToStop = (int64)-1;
			Log("Stopped.", Topiary::LogType::Transport);
			broadcaster.sendActionMessage(MsgTransport);

			// if there is a variation waiting, then we need to make sure it becomes boue again - do that outside this scoped lock otherwise we'll lock -- see below

			break;
		case Topiary::Ending:
			Log("Ending, cleaning up.", Topiary::LogType::Transport);
			broadcaster.sendActionMessage(MsgTransport);
			break;
		case Topiary::Ended:
			// Note we use Ended, not Stopped - this only for the case of an ending pattern (not implemented yet (8/11/2018).  Because the transport 
			// may still be running, and if state is stopped and transport starts running it 
			// would restart or re-arm!!!
			// DELETE THIS STATE
			//Log("Ended, done with last notes.", Topiary::LogType::Transport);
			// reset stuff
			//patternCursorOn = 0;
			//patternCursorOff = 0;
			//blockCursor = 0;
			//cursorToStop = (int64)-1;
			// and do in all patterns
			//for (int i = 0; i < 8; ++i)
			//{
			//	variation[i].currentPatternChild = nullptr;
			//	variation[i].currentPatternChildOff = nullptr;
			//}
			//break;
		case Topiary::Armed:
			if (numPatterns == 0)
			{
				Log("Cannot run because there is no pattern data loaded.", Topiary::LogType::Warning);
				runState = remember;
			}
			else
			{
				// make sure there are variations enbabled
				// and that we selected an enabled variation
				
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

					Log("Armed, waiting for first note.", Topiary::LogType::Transport);
					broadcaster.sendActionMessage(MsgTransport);
				}
				else
				{
					Log("Cannot run because there is no variation enabled.", Topiary::LogType::Warning);
					runState = remember;
				}
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

int TopiaryModel::getNumPatterns()
{
	return numPatterns;

} // getNumPatterns


///////////////////////////////////////////////////////////////////////
// GENERATOR STUFF
///////////////////////////////////////////////////////////////////////

void TopiaryModel::threadRunner()
{

} // threadRunner

void TopiaryModel::setSampleRate(double sr)
{
	if (sr != sampleRate)  // not really needed because also checked in processor !!!
	{
		sampleRate = sr;
		recalcRealTime(); // housekeeping
		Log(String("Samplerate set to ") + String(sampleRate) + String("."), Topiary::LogType::Debug);
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
		Log(String("Blocksize ") + String(blockSize), Topiary::LogType::Debug);
	}
} // setBlockSize

///////////////////////////////////////////////////////////////////////

void TopiaryModel::getVariationDetailForGenerateMidi(XmlElement** parent, XmlElement** noteChild, int& parentLength, bool& ending, bool& ended)
{                  
	// virtual
	UNUSED(parent);
	UNUSED(noteChild);
	UNUSED(parentLength);
	UNUSED(ending);
	UNUSED(ended);
} //getVariationDetailForGenerateMidi

///////////////////////////////////////////////////////////////////////

void TopiaryModel::setEnded()
{
	// virtual
} // setEnded

///////////////////////////////////////////////////////////////////////

void TopiaryModel::generateMidi(MidiBuffer* midiBuffer)
{ // main Generator

	const GenericScopedLock<SpinLock> myScopedLock(lockModel);

	/*************************************************************************************************************************************************
	Uses a lot of model variables!  Summary of what is needed for what here

	variation[variationRunning].pattern;
	variation[variationRunning].currentPatternChild;
	variation[variationRunning].lenInTicks;
	variation[variationRunning].lenInMeasures;   DO WE NEED THIS, THINK NOT!

	int64 blockCursor;				// sampletime of start of current block
	int64 nextRTGenerationCursor;	// real time cursor of next event to generate
	int blockSize;					// size of block to generate
	int patternCursor;				// ticks where we are within the variation/pattern - if we do nothing it should still advance with blocksize/samplesPerTick

	**************************************************************************************************************************************************/

	int64 rtCursorFrom;				// sampletime to start generating from
	int64 rtCursorTo;				// we generate in principle till here
	int64 rtCursor;					// where we are, between rtCursorFrom and rtCursorTo

	int DEBUGpatCursor = 0;

	if (blockCursor == 0)			// blockCursor is updated at end of generation!
	{
		rtCursor = 0;
		patternCursor = 0;
		rtCursorFrom = 0;
		patternCursorOffset = 0;
		rtCursorTo = rtCursorFrom + blockSize;
		nextRTGenerationCursor = 0;
		measure = 0;
		beat = 0;
		tick = 0;
	}
	else
	{
		rtCursorFrom = blockCursor;
		rtCursorTo = rtCursorFrom + blockSize;
		rtCursor = rtCursorFrom;
		calcMeasureBeat();
		DEBUGpatCursor = (int)floor(blockCursor / samplesPerTick);
		patternCursor = (int)floor(blockCursor / samplesPerTick) - patternCursorOffset;
	}


	//Logger::outputDebugString("Generate midi; patcur" + String(patternCursor));
	//Logger::outputDebugString("next RTcursor " + String(nextRTGenerationCursor));

	jassert(beat >= 0);
	jassert(measure >= 0);

	//if (nextRTGenerationCursor > rtCursorTo)
	//{
	//	blockCursor = blockCursor + blockSize;
	//	//Logger::outputDebugString("Nothing to generate");
	//	return;  // nothing to generate now
	//}

	// be careful with below; when changing patterns need to see if below is really correct!!!!
	XmlElement* parent = nullptr;
	XmlElement* noteChild = nullptr;

	int parentLength; // in ticks
	bool ending; // indicates this variation will play only once
	bool ended; // indicates this variation was ending==true and has now ended

	getVariationDetailForGenerateMidi(&parent, &noteChild, parentLength, ending, ended);

	int nextPatternCursor;  // patternCursor is global and remembers the last one we generated

	int ticksTaken;
	MidiMessage msg;
	int noteNumber;
	int length;
	int channel;
	int CC = 0;
	int value = 0;

	patternCursor = (int)patternCursor % parentLength;  
	//DEBUGpatCursor = (int)DEBUGpatCursor % parentLength;

	//Logger::outputDebugString("Next note on to generate afer current tick " + String(patternCursor));

	bool walk;
	
	if (!ending || (ending && !ended))
	{
		walk = walkToTick(parent, &noteChild, patternCursor);
	}
	else
		walk = false; // meaning an ending variation and ended
	
	//Logger::outputDebugString(String("Walk: ") + String((int)walk));

	if (walk)
	{
		// set patternCursors where we are now, so the offsets in sample time are correct
		patternCursor = (int)patternCursor % parentLength;  //////// because rtCursors are multi loops !!!!
		//Logger::outputDebugString("PatternCursor = " + String(patternCursor));
		//Logger::outputDebugString("DEBUGPatternCursor = " + String(DEBUGpatCursor));
		//Logger::outputDebugString("PatternCursorOffset = " + String(patternCursorOffset));
		//Logger::outputDebugString("Blockcursor ; " + String(blockCursor));
		//calcMeasureBeat();

		while (rtCursor < rtCursorTo)
		{
			nextPatternCursor = noteChild->getIntAttribute("Timestamp");

			ticksTaken = nextPatternCursor - patternCursor;  // ticks taken in this timeframe
			if (ticksTaken < 0)
			{
				//Logger::outputDebugString("PatternOnCursor looped over end");

				if (ending)
				{
					// this is an ending variation and it has now ended
					ended = true;
					setEnded();
					//Log("Ended ---------------------------------------", Topiary::LogType::Variations);
					rtCursor = rtCursorTo; // prevent next if to pass so nothing further is generated
				}

				ticksTaken += parentLength;
				jassert(ticksTaken >= 0);
			}

			if ((rtCursor + (int64)(ticksTaken*samplesPerTick)) < rtCursorTo)
			{
				//Logger::outputDebugString("GENERATING A NOTE ------------>");
				//Logger::outputDebugString(String("Next Patcursor ") + String(nextPatternCursor));

				////// GENERATE MIDI EVENT
				int midiType = noteChild->getIntAttribute("midiType");
				if ((midiType == Topiary::MidiType::NoteOn) || (midiType == Topiary::MidiType::NoteOff))
				{ // Generate note on or off; ignore other midi events for now
					length = noteChild->getIntAttribute("Length");
					noteNumber = noteChild->getIntAttribute("Note");
					channel = noteChild->getIntAttribute("Channel");
					if (midiType == Topiary::MidiType::NoteOn)
						msg = MidiMessage::noteOn(channel, noteNumber, (float)noteChild->getIntAttribute("Velocity") / 128);
					else
						msg = MidiMessage::noteOff(channel, noteNumber, (float)noteChild->getIntAttribute("Velocity") / 128);

					// DEBUG LOGIC !!!!!!!!!
					// outputting the pattern tick values + the tick value in the generated pattern
					// int64 cursorInTicks = (int64)floor( (rtCursor + (int64)(ticksTaken*samplesPerTick) ) / samplesPerTick    );  
					// now do that modulo the patternlenght in ticks
					// cursorInTicks = cursorInTicks % parentLength;
					// Logger::outputDebugString("Generated note at realtime pat tick " + String(cursorInTicks) + " / tick in pattern " + String(noteChildOn->getStringAttribute("Timestamp")));
					/////////////////////////

				}
				else
				{
					if (midiType == Topiary::MidiType::CC)
					{
						channel = noteChild->getIntAttribute("Channel");
						CC = noteChild->getIntAttribute("CC");
						value = noteChild->getIntAttribute("Value");
						msg = MidiMessage::controllerEvent(channel, CC, value);
						processCC(msg); // make sure model is updated with new cc values
					}
				}

				patternCursor = nextPatternCursor;  // that is the tick of the event we just generated
				if (patternCursor >= parentLength) patternCursor = patternCursor - parentLength;
				nextTick(parent, &noteChild);
				//Logger::outputDebugString(String("NOTE ON --------"));
				rtCursor = rtCursor + (int64)(ticksTaken * samplesPerTick);

				jassert((rtCursor - rtCursorFrom) >= 0);

				midiBuffer->addEvent(msg, (int)(rtCursor - rtCursorFrom));
				if (logMidiOut)
					logMidi(false, msg);

				//Logger::outputDebugString(String("nxtcursor ") + String(nextRTGenerationCursor));

			}  // generated a note (on or off)
			else
			{
				// done for now; next event is over rtCursorTo 
				// let's place ourselves ready for the next round
				// so either walkOn and we have the next On note ready
				// or walkOff and next Off note ready as well
				// main goal is to set nextPatternCursor!

				//Logger::outputDebugString(String(" ++++++++++++++ done +++++++++++++++++++++"));

				nextPatternCursor = noteChild->getIntAttribute("Timestamp");
				ticksTaken = nextPatternCursor - patternCursor;
				if (ticksTaken < 0)
				{
					ticksTaken += parentLength;
					//Logger::outputDebugString(String("Running over end of pattern!!!"));
				}

				patternCursor = nextPatternCursor;
				if (patternCursor >= parentLength) patternCursor = patternCursor - parentLength;

				// we set rtCursor at the time of the next event; will possibly break out of the loop if not within this block

				rtCursor = rtCursorTo; // force break out of the loop
				nextRTGenerationCursor = rtCursorFrom + (int64)(ticksTaken * samplesPerTick);
				//int nextTick = (int) (nextRTGenerationCursor / samplesPerTick) % parentLength;
				//Logger::outputDebugString("Next tick to generate (off nextRTcursor): " + String(nextTick));
			}  // end loop over from --> to
		}
	}
	else
	{
		// walk did not find a note to generate	
		// so pattern is empty or no fitting note events found
		if ((noteChild == nullptr))
		{
			// make sure our cursors keep running
			// 		  nextRTGenerationCursor = rtCursorTo + 1;
			//nextRTGenerationCursor = rtCursorTo;
			patternCursor = +(int)(blockSize / samplesPerTick);

		}

	}

	blockCursor = blockCursor + blockSize;

	calcMeasureBeat();

} // generateMidi

///////////////////////////////////////////////////////////////////////

bool TopiaryModel::processVariationSwitch() // called just before generateMidi - to see if a variation is changed, and if so whether to switch now (and make the switch)
{
	// virtual, but standard code that is in TopiaryIncludes.h
	return true;
} // processVariationSwitch

///////////////////////////////////////////////////////////////////////

void TopiaryModel::outputModelEvents(MidiBuffer& buffer)
{	
	// outputs what is in modelWorkBuffer
	MidiMessage msg;
	int position;
	const GenericScopedLock<SpinLock> myScopedLock(lockModel);

	auto iterator = MidiBuffer::Iterator(modelEventBuffer);

	while (iterator.getNextEvent(msg, position))
	{
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
			cursorToStop = (int64)(blockCursor + samplesPerTick * ((Topiary::TICKS_PER_QUARTER - tick - 1) + (numerator - beat - 1)* Topiary::TICKS_PER_QUARTER));
			break;
		}
		case (Topiary::Half):
		{
			cursorToStop = (int64)(blockCursor + samplesPerTick * ((Topiary::TICKS_PER_QUARTER - tick - 1) + Topiary::TICKS_PER_QUARTER));
			break;
		}
		case (Topiary::Quarter):
		{
			cursorToStop = (int64)(blockCursor + samplesPerTick * ((Topiary::TICKS_PER_QUARTER - tick - 1)));
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
	const GenericScopedLock<SpinLock> myScopedLock(lockModel); // wait if it's generating
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
	variationSwitchChannel = channel;

	for (int i = 0; i < 8; i++)
	{
		variationSwitch[i] = switches[i];
	}

} // setVariationControl

///////////////////////////////////////////////////////////////////////

void TopiaryModel::getVariationControl(bool& ccSwitching, int& channel, int switches[8])
{
	ccSwitching = ccVariationSwitching;
	channel = variationSwitchChannel;

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
				if ((variationSwitchChannel == 0) || (variationSwitchChannel == channel))
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
				if ((variationSwitchChannel == 0) || (variationSwitchChannel == channel))
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
