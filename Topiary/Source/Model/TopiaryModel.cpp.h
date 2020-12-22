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

/////////////////////////////////////////////////////////////////////////////
/*

This code has generic stuff that can be included in every Topiary model code.
It includes:
- Variation logic (that depends on variation variables defined in the actual 
  models (not in the generic TopiaryModel))

CAREFUL: needs a symbol TOPIARYMODEL to actually build!

*/
/////////////////////////////////////////////////////////////////////////////

#ifdef TOPIARYMODEL 

void TOPIARYMODEL::setRunState(int n)
{
	
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

#ifdef RIFFZ
			keytracker.init();
			parentPattern = nullptr;
#endif
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
#ifdef RIFFZ
			
			if (recordingMidi)
			{
				if (patternSelectedInPatternEditor == -1)
				{
					Log("No pattern selected in pattern editor.", Topiary::LogType::Warning);
					Log("Recording turned off.", Topiary::LogType::Warning);
					recordingMidi = false;
					broadcaster.sendActionMessage(MsgTransport);
				}
				else
				{
					// If we can arm and recording is on, we need to make a copy of the pattern selected in patternEditor, into variation[8]
					int numItems = patternData[patternSelectedInPatternEditor].numItems;
					TopiaryVariation* v = &(variation[8].pattern[0]);
					TopiaryPattern* p = &(patternData[patternSelectedInPatternEditor]);

					v->patLenInTicks = p->patLenInTicks;
					v->numItems = numItems;
					for (int i = 0; i < numItems; i++)
					{
						//v->dataList[i].CC = p->dataList[i].CC;
						v->dataList[i].note = p->dataList[i].note;
						v->dataList[i].length = p->dataList[i].length;
						v->dataList[i].velocity = p->dataList[i].velocity;
						v->dataList[i].timestamp = p->dataList[i].timestamp;
						v->dataList[i].midiType = p->dataList[i].midiType;
						v->dataList[i].channel = p->dataList[i].channel;
						v->dataList[i].value = p->dataList[i].value;
					}
				}
			}

#endif

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

/////////////////////////////////////////////////////////////////////////////

void TOPIARYMODEL::setOverrideHostTransport(bool o)
{
	if (overrideHostTransport != o)
	{
		{   // block with a lock
			const GenericScopedLock<CriticalSection> myScopedLock(lockModel);
			if (o)
				overrideHostTransport = o;
			else
			{
				// make sure we are ready to accept host commands; there must be at least one variation enabled!
				bool ok = false;
				for (int v = 0; v < 8; v++)
					if (variation[v].enabled)
						ok = true;

				if (ok)
					overrideHostTransport = false;
				else
					Log("Host can only be in control if there is at least one enabled variation.", Topiary::LogType::Warning);
			}
		} // end lock

		broadcaster.sendActionMessage(MsgTransport);
		// careful here - if we just set the runstate to Stopped - it might have been stopped already and variables may be undefined
		// dirty hack below
		{
			const GenericScopedLock<CriticalSection> myScopedLock(lockModel);
			runState = -1000000; // to force a runstate stopped below!!!
		}
		setRunState(Topiary::Stopped);
		if (overrideHostTransport)
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

/////////////////////////////////////////////////////////////////////////////

void TOPIARYMODEL::setNumeratorDenominator(int nu, int de)
{
	
	if ((numerator != nu) || (denominator != de))
	{
#ifndef PRESETZ
		// check if there is data - if so do not allow meter changes!

		if (getNumPatterns() >0)
		{
			Log("Set meter first!", Topiary::LogType::Warning);
			Log("Meter change not allowed when patterndata available.", Topiary::LogType::Warning);
			return;
		}
#endif	
		const GenericScopedLock<CriticalSection> myScopedLock(lockModel);
		numerator = nu;
		denominator = de;
		recalcRealTime();
		Log(String("Signature set to ") + String(numerator) + String("/") + String(denominator), Topiary::LogType::Transport);
		broadcaster.sendActionMessage(MsgTransport);	
	}

} // setNumeratorDenominator

/////////////////////////////////////////////////////////////////////////////

void TOPIARYMODEL::getVariation(int& running, int& selected)
{
	running = variationRunning;
	selected = variationSelected;
	return;

} // getVariation

///////////////////////////////////////////////////////////////////////


void TOPIARYMODEL::setVariation(int n)
{
	jassert(n < 8);
	jassert(n >= 0);

	if (recordingMidi && (n!=variationSelected))
	{
		// do not allow switching of variations
		Log("Cannot switch variation during recording.", Topiary::LogType::Warning);
		return;
	}

 	if (variation[n].enabled == false)
	{
		// do not allow switching of disabled variations
		Log("Cannot switch to disabled variation.", Topiary::LogType::Warning);
		return;
	}
	if ((n != variationSelected) || (runState == Topiary::Stopped))
		// the || runState || is needed because we may need to re-set a waiting variation to non-waiting; in that case we want the update to happen otherwise the buttons stays orange
	{
		const GenericScopedLock<CriticalSection> myScopedLock(lockModel);

		variationSelected = n;
		if (runState == Topiary::Stopped)  // otherwise the switch will be done when running depending on the variation switch Q
			variationRunning = n;
		//Log(String("Variation ") + String(n + 1) + String(" selected."), Topiary::LogType::Variations);
		broadcaster.sendActionMessage(MsgVariationSelected);  // if the editor is there it will pick up the change in variation	
	}

#ifdef PRESETZ
	if ((runState != Topiary::Running) && variation[variationSelected].enabled)
		// we ALWAYS do this - even if the variation does not change (because we may wanna hit the variation button to reset it (presetz)
		// so even if WFFN is on - as long as we are waiting wo do an immediate output of the variation settings!
		outputVariationEvents(); // output the variation preset values!
#endif
} // setVariation

///////////////////////////////////////////////////////////////////////

void TOPIARYMODEL::getVariationEnables(bool enables[8])
{
	for (int i = 0; i < 8; i++)
		enables[i] = variation[i].enabled;

} // getVariationEnables

///////////////////////////////////////////////////////////////////////

int TOPIARYMODEL::getVariationLenInTicks(int v)
{

#if defined(BEATZ) ||  defined(PRESETZ)
		return variation[v].pattern.patLenInTicks; 
#endif

#if defined(RIFFZ)
	// we are assuming that all patterns in one variation have same length, so just pick one and return that.
	return variation[v].pattern[0].patLenInTicks;
#endif

} // getVariationLenInTicks

///////////////////////////////////////////////////////////////////////

bool TOPIARYMODEL::getVariationEnabled(int v)
{
	return variation[v].enabled;

} // getVariationEnabled

///////////////////////////////////////////////////////////////////////

bool TOPIARYMODEL::processVariationSwitch() // called just before generateMidi - to see if a variation is changed, and if so whether to switch now (and make the switch)
{
	// next block goes from blockCursor to blockCursor+blockSize;
	// generation is at patternCursorOn within the pattern - which is variationRunning, not (yet) variationSelected
	// and both cursors go from 0 to the tickLength of that pattern
	// we also know that we are currently at rtCursor, and given rtCursor and patternCursorOn we can calculate when the current pattern started in realtime 
	
	//Logger::outputDebugString(String("Sel ")+String(variationSelected)+String(" Running ")+String(variationRunning));

	auto processStartQ = [&]()
	{
		patternCursor = (int)round(blockCursor / samplesPerTick) % getVariationLenInTicks(variationSelected);

		switch (switchVariation)
		{
		case (Topiary::SwitchFromStart):
			patternCursorOffset = patternCursor;
			//Logger::outputDebugString("PatternCursorOffset in PROCESSVARIATIONSWITCH: " + String(patternCursorOffset));
			//Logger::outputDebugString("Blockcursor ; " + String(blockCursor));
			break;
		case (Topiary::SwitchWithinBeat):
			patternCursorOffset = 0;
			patternCursor = patternCursor % Topiary::TicksPerQuarter;
			break;
		case (Topiary::SwitchWithinMeasure):
			patternCursorOffset = 0;
			patternCursor = patternCursor % (Topiary::TicksPerQuarter * numerator);
			break;
		case (Topiary::SwitchWithinPattern):
			patternCursorOffset = 0;
			patternCursor = patternCursor % (getVariationLenInTicks(variationSelected));
			break;
		}
	};

#ifdef BEATZ
	if (variationSelected == variationRunning) return false;
#endif
#ifdef PRESETZ
	if (variationSelected == variationRunning) return false;
#endif

#ifdef RIFFZ
	//Logger::outputDebugString("ENTERING PROCESSVARIATIONSWITH Blockcursor ; " + String(blockCursor));
	if ((variationSelected == variationRunning) && (keytracker.notePlaying == keytracker.nextNotePlaying) && latch1) return false;  // why latch ???
#endif

	if (blockCursor == 0)
	{
		runState = Topiary::Running;
		//Logger::outputDebugString("Starting - runstate is Armed but we need to go!");  // careful that the WFFN functonality keeps working!!!
#ifdef RIFFZ
		if (!latch1 && !latch2)
		{
			if (keytracker.bufferSize == 0) 
				parentPattern = nullptr;
			else
			{
				// we need to start running
				keytracker.notePlaying = keytracker.nextNotePlaying;
				maintainParentPattern();
			}
		}
		else if (keytracker.notePlaying != keytracker.nextNotePlaying)
		{
			if (keytracker.nextNotePlaying != -1)
			{
				keytracker.notePlaying = keytracker.nextNotePlaying;
				keytracker.lastNotePlaying = keytracker.notePlaying;
				maintainParentPattern();
			}
			/*  Can't happen at blokCursor = 0
			else if (keytracker.lastNotePlaying != -1)
			{
				keytracker.notePlaying = keytracker.lastNotePlaying;
				maintainParentPattern();
				if (latch2)
				{
					setRunState(Topiary::Ending);
					latch2Restart = true;
				}
			}*/
		}
#endif
		return true;
	}  /// end if blockcursor == 0 (meaning we just got started - options are fewer :)


	// in this case we want to start asap (unless we Q on the transport, but that is done in the call before this one!!!

	//Logger::outputDebugString("Need to switch, now ???");
	//Logger::outputDebugString("blockcursor "+String(blockCursor)+" NextRTcursor "+String(nextRTGenerationCursor));

	// first decide whether we should switch in this block
	// we are at measure/beat/tick overall; time and tick apply to the pattern we are in, but not measure within the pattern so we calculate that by using patternCursor

	//Logger::outputDebugString("Need to switch, now ???  Something is going to happen next");
	calcMeasureBeat(); 
	int64 cursorToSwitch = 0;  // time the switch should happen; if  cursorToSwitch < blockCursor+blockSize then we know we want to switch;
	//Logger::outputDebugString("VariationStartQ " + String(variationStartQ));

	switch (variationStartQ)
	{
	case (Topiary::Immediate):
	{
		cursorToSwitch = blockCursor; // i.e. now
		//Logger::outputDebugString(String("IMMEDIATE Sel ") + String(variationSelected) + String(" Running ") + String(variationRunning));
		break;
	}
	case (Topiary::Measure):
	{
		// moment of next measure = blockCursor + time to next beat (ticksperquarter - tick) + #beats to go till end of measure			
		cursorToSwitch = (int64)(blockCursor + samplesPerTick * ((Topiary::TicksPerQuarter - tick - 1) + (numerator - beat /*-1*/ )* Topiary::TicksPerQuarter));
		//Logger::outputDebugString(String("MEASURE Sel ") + String(variationSelected) + String(" Running ") + String(variationRunning));
		break;
	}
	case (Topiary::Quarter):
	{
		// moment of next beat = blockCursor + time to next beat (ticksperquarter - tick) 
		cursorToSwitch = (int64)(blockCursor + samplesPerTick * ((Topiary::TicksPerQuarter - tick - 1)));
		//Logger::outputDebugString(String("QUARTER Sel ") + String(variationSelected) + String(" Running ") + String(variationRunning));
		break;
	}
	//case (Topiary::Half):
	//{
		// moment of next beat = blockCursor + time to next beat (ticksperquarter - tick) 
	//	cursorToSwitch = (int64)(blockCursor + samplesPerTick * ((Topiary::TicksPerQuarter - tick - 1) + Topiary::TicksPerQuarter));
	//	//Logger::outputDebugString(String("IMMEDIATE Half ") + String(variationSelected) + String(" Running ") + String(variationRunning));
	//	break;.
	//}
	default: jassert(false);

	} // switch
	//Logger::outputDebugString(String("BlockCursor ") + String(blockCursor));
	//Logger::outputDebugString(String("CursorToSwitch ") + String(cursorToSwitch));

	//if ((nextRTGenerationCursor <= cursorToSwitch) && (cursorToSwitch < (blockCursor + blockSize))) 
	if (cursorToSwitch < (blockCursor + blockSize))
	{
		// Logger::outputDebugString("Need to switch, now ??? ----------------------------> yes we're sure");
		// we are sure we're switching variations now
		// so decide where in the variation we start - we need to set patternCursor

		// if we are in Beatz or Presets below will always be true because of check earlier;
		// in Riffz that may not be the case - we can have a switch because of notes instead of because of variation change.


		if (variationRunning != variationSelected)  
		{
			//Logger::outputDebugString("Need to switch, now ??? ----------------------------> yes we're sure");
			// we are sure we're switching variations now
			// so decide where in the variation we start - we need to set patternCursor

			processStartQ();
			
			Log(String("Switch from variation ") + String(variationRunning) + String(" to ") + String(variationSelected), Topiary::LogType::Variations);
			variationRunning = variationSelected;

			if (variation[variationRunning].type == Topiary::VariationTypeSteady)
				previousSteadyVariation = variationRunning;

			broadcaster.sendActionMessage(MsgVariationSelected);
#ifdef RIFFZ
			cursorToStop = -1;  // so that in case of a pattern latching to end, the endpoint calculation gets reset
			keytracker.lastNotePlaying = -1;
			if ((keytracker.notePlaying != keytracker.nextNotePlaying) && (!latch1))
				keytracker.notePlaying = keytracker.nextNotePlaying;
			maintainParentPattern();
			
#endif
			
		} // if variation switch

#ifdef RIFFZ
		
		// check if we have a noteswitch and hence pattern switch

		if (latch2 && (keytracker.notePlaying != keytracker.nextNotePlaying) && (cursorToStop != -1) && (keytracker.nextNotePlaying != -1))
			// this means we have a) a new note and b) we are latch2 and c) a previous note is still doing its latch2 --> we stop doing the latch2 end (by setting cursorToStop to -1
			cursorToStop = -1;

		if (!latch1 && !latch2 && (keytracker.bufferSize == 0))
		{
			parentPattern = nullptr;
			keytracker.init(); // reset the tracker because it may otherwise remember the last note played
		}
		else if (keytracker.notePlaying != keytracker.nextNotePlaying) 
		{
			//if ((keytracker.bufferSize==0) && latch2)
			//	keytracker.notePlaying = keytracker.nextNotePlaying; // lastnoteplaying will be remembered

			if (keytracker.nextNotePlaying != -1)
			{
				keytracker.notePlaying = keytracker.nextNotePlaying;
				if (latch1 || latch2)
					keytracker.lastNotePlaying = keytracker.notePlaying; // since we are latching we'll remember that this note plays
				
				maintainParentPattern(); // if we are latching maintainParentPattern will use lastNotePlaying
				if (latch2)
				{
					// make sure we start correctly, possibly off-beat
					processStartQ();
					cursorToStop = -1; // because we may 'restart' a pattern that was still 'ending' so ignore previous value of cursortostop
				}
			}
			
				
			else if (keytracker.lastNotePlaying != -1)
			{
				if (latch2)
				{
					if (keytracker.bufferSize==0) // schedule the ending
					{
						//keytracker.notePlaying = keytracker.lastNotePlaying;
						maintainParentPattern();  // since we are latching maintainParentPattern will use lastNotePlaying
						// we keep running until it is time to stop - same logic as ending logic
						if (cursorToStop == -1)
							switch (runStopQ) {
							case (Topiary::Immediate):
							{
								cursorToStop = blockCursor;
								break;
							}
							case (Topiary::WholePattern):
							{
								cursorToStop = (int64)((blockCursor - patternCursor * samplesPerTick) /* start of this pattern */ + (getVariationLenInTicks(variationRunning) * samplesPerTick));
								//Logger::outputDebugString(String("blockCur ") + String(blockCursor) + String(" stopCursor ") + String(cursorToStop));
								break;
							}
							case (Topiary::Measure):
							{
								cursorToStop = (int64)(blockCursor + samplesPerTick * ((Topiary::TicksPerQuarter - tick - 1) + (numerator - beat - 1) * Topiary::TicksPerQuarter));
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


						// now see if we're close enough to cursorToStop
						if (cursorToStop < (blockCursor + blockSize))
						{
							parentPattern = nullptr;
							keytracker.init(); // reset the tracker because it may otherwise remember the last note played
							cursorToStop = -1;
						}
					}
				} // if (latch2)
				else
				{	
					// if latching (latch1) and no note is pressed we keep playing the last note
					keytracker.notePlaying = keytracker.lastNotePlaying;
					maintainParentPattern();
				}
			}
		}
#endif // ifdef RIFFZ
		return true;
	} // decide to switch NOW
	else return false;

} // processVariationSwitch 

///////////////////////////////////////////////////////////////////////

bool TOPIARYMODEL::switchingVariations()
{
	bool switching = (variationRunning != variationSelected);
	if (recordingMidi) return false;

#ifdef PRESETZ	
	// for Presetz, switching variations needs to trigger regeneration of the variations
	if (switching)
	{
		generateTransition(); // call regeneration of pattern 
	}
#endif

#ifdef RIFFZ
	switching = switching || (keytracker.nextNotePlaying != keytracker.notePlaying);
	switching = switching || (((latch1) || (latch2)) && (keytracker.bufferSize == 0) && (keytracker.lastNotePlaying != -1));
	//switching = switching || ((!latch1) && (!latch2) && (keytracker.bufferSize != 0));  
	//switching = switching || (latch2 || keytracker.bufferSize == 0);
#endif

	return switching;

} // switchingVariations

///////////////////////////////////////////////////////////////////////

void TOPIARYMODEL::generateMidi(MidiBuffer* midiBuffer, MidiBuffer* recBuffer)
{ // main Generator

	const GenericScopedLock<CriticalSection> myScopedLock(lockModel);

	/*************************************************************************************************************************************************
	Uses a lot of model variables!  Summary of what is needed for what here

	variation[variationRunning].pattern;
	variation[variationRunning].lenInTicks;
	variation[variationRunning].lenInMeasures;   DO WE NEED THIS, THINK NOT!

	int64 blockCursor;				// sampletime of start of current block
	int64 nextRTGenerationCursor;	// real time cursor of next event to generate
	int blockSize;					// size of block to generate
	int patternCursor;				// ticks where we are within the variation/pattern - if we do nothing it should still advance with blocksize/samplesPerTick

	**************************************************************************************************************************************************/

	int64 rtCursorFrom = 0;				// sampletime to start generating from
	int64 rtCursorTo = 0;				// we generate in principle till here
	int64 rtCursor = 0;					// where we are, between rtCursorFrom and rtCursorTo


	if ((runState == Topiary::Running) || (runState == Topiary::Ending) 
#ifndef PRESETZ
		|| (noteOffBuffer.bufferSize > 0)
#endif
		)

		if (blockCursor == 0)			// blockCursor is updated at end of generation!
		{
			patternCursorOffset = 0;
			rtCursorTo = rtCursorFrom + blockSize;
			nextRTGenerationCursor = 0;
			measure = 0;
			beat = 0;
			tick = 0;
			//previousMeasure = -1;
#ifndef PRESETZ
			noteOffBuffer.bufferSize = 0;
#endif
		}
		else
		{
			rtCursorFrom = blockCursor;
			rtCursorTo = rtCursorFrom + blockSize;
			rtCursor = rtCursorFrom;
			calcMeasureBeat();
			patternCursor = (int)round((blockCursor / samplesPerTick) - patternCursorOffset); // not floor - that may lead to duplicate notes!!!

			//Logger::outputDebugString("Enter generateMidi with patternCursor " + String(patternCursor));
		}

	int parentLength; // in ticks
	MidiMessage msg;
	int noteChild = 0;
	int ticksTaken;
	int noteNumber;
	int length;
	int channel;
#ifdef RIFFZ
	channel = outputChannel;
#endif
	int CC = 0;
	int value = 0;
	int64 timestamp;

#ifdef PRESETZ
	UNUSED(recBuffer)
#endif

#if defined(BEATZ) || defined(RIFFZ)


		////////////////////////////////////////
		// Record logic here
		////////////////////////////////////////

#define RECBUFFERSIZE 100
		MidiMessage recordBuffer[RECBUFFERSIZE];
	int samplePos = 0;

	if (recordingMidi && ((runState == Topiary::Running) || (runState == Topiary::Ending)))
	{
		TopiaryVariation* varpat = nullptr;
		int rememberPatternCursor = patternCursor;

		//for (MidiBuffer::Iterator it(*recBuffer); it.getNextEvent(msg, samplePos);)
		for (const MidiMessageMetadata metadata : *recBuffer)
		{
			msg = metadata.getMessage();
			int midiType = -1;
			if (msg.isNoteOn())
				midiType = Topiary::NoteOn;
			else if (msg.isNoteOff())
				midiType = Topiary::NoteOff;
			else if (msg.isController())
				midiType = Topiary::CC;
			else if (msg.isPitchWheel())
				midiType = Topiary::Pitch;
			else if (msg.isAftertouch())
				midiType = Topiary::AfterTouch;

			if ((midiType==Topiary::NoteOn) || (midiType == Topiary::NoteOn) 
#ifdef RIFFZ
				|| (midiType == Topiary::Pitch) || (midiType == Topiary::CC) || (midiType == Topiary::AfterTouch)
#endif
				)
			{
				if (varpat == nullptr)
				{
#ifdef BEATZ
					varpat = &(variation[variationRunning].pattern);
#endif
#ifdef RIFFZ
					varpat = &(variation[8].pattern[0]);
#endif
				}
				varpat->add();
				int nIndex = varpat->numItems - 1;
				varpat->dataList[nIndex].ID = -1; // dummy
				if ((midiType == Topiary::NoteOn) || (midiType == Topiary::NoteOff))
				{
					varpat->dataList[nIndex].note = msg.getNoteNumber();
					varpat->dataList[nIndex].velocity = msg.getVelocity();
					varpat->dataList[nIndex].channel = msg.getChannel();
				}
				else if (midiType == Topiary::CC)
				{
					varpat->dataList[nIndex].CC = msg.getControllerNumber();
					varpat->dataList[nIndex].velocity = msg.getControllerValue();
				}
				else if (midiType == Topiary::Pitch)
				{
					varpat->dataList[nIndex].velocity = msg.getPitchWheelValue();
				}
				else if (midiType == Topiary::AfterTouch)
				{
					varpat->dataList[nIndex].velocity = msg.getAfterTouchValue();
				}

				varpat->dataList[nIndex].length = -1; // so we know not to generate a noteOff event (because the noteOff will follow)

				varpat->dataList[nIndex].midiType = midiType;

				int64 cursorInTicks = (int64)floor(blockCursor / samplesPerTick) + (int)(samplePos / samplesPerTick);
#ifdef BEATZ
				cursorInTicks = cursorInTicks % variation[variationSelected].pattern.patLenInTicks;
#endif
#ifdef RIFFZ
				cursorInTicks = cursorInTicks % variation[8].pattern[0].patLenInTicks;
#endif

				varpat->dataList[nIndex].timestamp = (int)cursorInTicks;
				//Logger::outputDebugString("RECORDING");
			} // 
		} // iterator over recordBuffer

		// reset patternchild to nullptr so the rest of the logic can restart (because we may have messed with patternchild
		//variation[variationRunning].currentPatternChild = 0;
		patternCursor = rememberPatternCursor;

		// sort by timstamp
		if (varpat != nullptr) // we did record something
			varpat->sortByTimestamp(false); // keep the -1 IDs se we know later which events got inserted

	}

	////////////////////////////////////////
	// End of record logic
	////////////////////////////////////////

#endif

	//Logger::outputDebugString("Generate midi; patcur" + String(patternCursor));
	//Logger::outputDebugString("next RTcursor " + String(nextRTGenerationCursor));

	if (((runState == Topiary::Running) || (runState == Topiary::Ending))
#if defined(RIFFZ)
		&& (parentPattern != nullptr)
#endif
		)
	{
		jassert(beat >= 0);
		jassert(measure >= 0);

#if defined(BEATZ) || defined(PRESETZ)
		TopiaryVariation* parentPattern = &(variation[variationRunning].pattern);
		parentLength = parentPattern->patLenInTicks;
#endif
#if defined(RIFFZ)
		// in RIFFZ parentPattern is global in the model
		// and it is maintained by processvariationSwitch
		// except when recording
		if (recordingMidi)
			parentPattern = &(variation[8].pattern[0]);

		parentLength = parentPattern->patLenInTicks;
#endif

#if defined(BEATZ) || defined(RIFFZ)
		/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		// GENERATE AHEAD LOGIC
		// given we now know where we are in patternCursor, and given how many ticks in the pattern we do per block (blocksize), 
		// we can now estimate whether we just ran over 1/8 (and whether we are about to run over the end of the pattern)
		/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	
		patternCursor = (int)patternCursor % parentLength;
		int patternSpan = (int)(blockSize / samplesPerTick);
		int measureToGenerate = -1;
		int eightToGenerate = -1;
		if ((parentLength - patternCursor) <= patternSpan)
		{
			// we are going to run over end of pattern 
			measureToGenerate = 0;
			eightToGenerate = 0;
		}
		else
		{
			// calculate which measure and eight we are on
			// int gMeasure = (int)patternCursor / (numerator * Topiary::TicksPerQuarter);
			int gEight = (int) patternCursor / (Topiary::TicksPerQuarter / 2);
			// see if we are going to run over an eight
			int pEight = (gEight+1) * (Topiary::TicksPerQuarter / 2); // start of the current eight in ticks
			if ((patternCursor + (2*patternSpan)) >= pEight)
				eightToGenerate = gEight + 2;

			if ((eightToGenerate * (Topiary::TicksPerQuarter / 2)) >= parentLength)
				eightToGenerate = eightToGenerate - parentLength/ (Topiary::TicksPerQuarter / 2);

		}
		
		
		if ((eightToGenerate != -1) && !recordingMidi)
		{
			Logger::outputDebugString("generate eight " + String(eightToGenerate));
			generateVariation(variationRunning, eightToGenerate);
		}
#endif
		noteChild = 0; // variation[variationRunning].currentPatternChild;

		int nextPatternCursor;  // patternCursor is global and remembers the last one we generated

		//Logger::outputDebugString("Next note on to generate after current tick " + String(patternCursor));

		bool walk;

		if (!(variation[variationRunning].type == Topiary::VariationTypeEnd) || ((variation[variationRunning].type == Topiary::VariationTypeEnd) && !variation[variationRunning].ended))
		{
			walk = walkToTick(parentPattern, noteChild, patternCursor);
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


			while (rtCursor < rtCursorTo)
			{
				auto rememberChild = noteChild;

				nextPatternCursor = parentPattern->dataList[noteChild].timestamp;

				ticksTaken = nextPatternCursor - patternCursor;  // ticks taken in this timeframe
				if (ticksTaken < 0)
				{
					//Logger::outputDebugString("PatternCursor looped over end");
					//previousMeasure = -1; // so that  we regenerate first measure
					if (variation[variationRunning].type == Topiary::VariationTypeEnd)
					{
						// this is an ending variation and it has now ended - do nothing (but keep running)
						//Log("Ended ---------------------------------------", Topiary::LogType::Variations);
						variation[variationRunning].ended = true;
						rtCursor = rtCursorTo; // prevent next if to pass so nothing further is generated
					}
					else if (runState == Topiary::Ending)
					{
						setRunState(Topiary::TransportRunState::Stopped); 

						//Log("Ended ---------------------------------------", Topiary::LogType::Variations);
						rtCursor = rtCursorTo; // prevent next if to pass so nothing further is generated
					}

					ticksTaken += parentLength;
					jassert(ticksTaken >= 0);
				}

				if ((rtCursor + (int64)(ticksTaken*samplesPerTick)) < rtCursorTo)
				{
					//Logger::outputDebugString("GENERATING A NOTE " + String(noteChild->getIntAttribute("midiType")) + "------------>" + String(noteChild->getIntAttribute("Note"))+" at timest "+ String(noteChild->getIntAttribute("Timestamp")));
					//Logger::outputDebugString(String("Next Patcursor ") + String(nextPatternCursor));

					////// GENERATE MIDI EVENT
					int midiType = parentPattern->dataList[noteChild].midiType;		

					if ((variation[variationRunning].type == Topiary::VariationTypeFill) && (variationRunning == variationSelected))
					{
						// if variationRunning != variationSelected that takes precedence !!!!
						// if we are just now entering the last beat of the last measure, set variationSelected to the previously running steady variation; if none, pick the first one
						// Careful -  user should not set Q var switch to immediate because in that case it will switch early!
						// (if folks use that they will probably have Q Var Switch at measure anyway)
						// alternatively, check whether this is the last run over the variation before we might switch (based on 2*blocksize or something)
						if (((patternCursor + (int)(2 * blockSize / samplesPerTick)) >= parentPattern->patLenInTicks) || (noteChild == (parentPattern->numItems - 1)))
						{
							variationSelected = previousSteadyVariation;  // initialized at run start (tellModelToRun) and maintained in processVariationSwitch
						}
					}
					else if (variation[variationRunning].type == Topiary::VariationTypeIntro)
					{
						// if variationRunning != variationSelected that takes precedence !!!!
						// if we are entering the last beat of the last measure of the variation, set variationSelected to the first steady variation; if none, stay where  we are
						// Careful - user should not set Q var switch to immediate because in that case it will switch early!
						// (if folks use that they will probably have Q Var Switch at measure anyway)
						// alternatively, check whether this is the last run over the variation before we might switch (based on 2*blocksize or something)

						if (((patternCursor + (int)(2 * blockSize / samplesPerTick)) >= parentPattern->patLenInTicks) || (noteChild == (parentPattern->numItems - 1)))
						{
							int newVariation = variationRunning;
							for (int v = 0; v < 8; v++)
							{
								// limit search to enabled variations
								if ((variation[v].type == Topiary::VariationTypeSteady) || variation[v].enabled)
								{
									newVariation = v;
									v = 8;
								}
								variationSelected = newVariation;
							}
						}

					}

#ifndef RIFFZ
					channel = parentPattern->dataList[noteChild].channel; // RIFFZ always uses outputChannel
#endif
#ifdef RIFFZ
					channel = outputChannel;
#endif	
					if ((midiType == Topiary::MidiType::NoteOn) || (midiType == Topiary::MidiType::NoteOff))
					{ // Generate note on/off; ignore other midi events for now
						length = parentPattern->dataList[noteChild].length;
						noteNumber = parentPattern->dataList[noteChild].note;
#ifdef RIFFZ
						// do the offset in RIFFZ
						int offSet;
						if ((latch1||latch2) && (keytracker.lastNotePlaying !=-1))
							offSet = variation[variationRunning].noteAssignmentList.findOffset(keytracker.lastNotePlaying);
						else
							offSet = variation[variationRunning].noteAssignmentList.findOffset(keytracker.notePlaying);

						if (offSet == Topiary::NoNoteAssigned)
							midiType = Topiary::NOP;  // suppress the note
						else
							noteNumber = noteNumber + offSet;
#endif

						timestamp = (int)(rtCursor - rtCursorFrom);

						if (midiType == Topiary::MidiType::NoteOn)
						{
							msg = MidiMessage::noteOn(channel, noteNumber, (juce::uint8)parentPattern->dataList[noteChild].velocity);
							//jassert(msg.getVelocity() > 126);
						}
						else if (midiType == Topiary::MidiType::NoteOff)
							msg = MidiMessage::noteOff(channel, noteNumber, (juce::uint8)0);
						

						// DEBUG LOGIC !!!!!!!!!
						// outputting the pattern tick values + the tick value in the generated pattern
						// int64 cursorInTicks = (int64)floor( (rtCursor + (int64)(ticksTaken*samplesPerTick) ) / samplesPerTick    );
						// now do that modulo the patternlenght in ticks
						// cursorInTicks = cursorInTicks % parentLength;
						// Logger::outputDebugString("Generated note at realtime pat tick " + String(cursorInTicks) + " / tick in pattern " + String(noteChildOn->getStringAttribute("Timestamp")));
						/////////////////////////

						/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
						// schedule noteOff event if this is noteOn and len != -1; otherwise it's a recording event and the noteOff will follow
						/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#if defined (BEATZ) || defined (RIFFZ)

						if ((midiType == Topiary::MidiType::NoteOn) && (parentPattern->dataList[noteChild].length != -1))
						{
							noteOffBuffer.push(channel, noteNumber, rtCursor + (int64)((ticksTaken + parentPattern->dataList[noteChild].length) * samplesPerTick));
							jassert(channel > 0);
						}
#endif


						///////////////////////////////////
						// OUTPUT THE NOTE EVENT
						///////////////////////////////////

						midiBuffer->addEvent(msg, (int)timestamp);
						if (logMidiOut)
							logMidi(false, msg);
						Logger::outputDebugString(String("Outputting Note")+String(noteNumber));
					}
					else if (midiType == Topiary::MidiType::CC)
					{
						CC = parentPattern->dataList[noteChild].CC;  
						value = parentPattern->dataList[noteChild].value;
						msg = MidiMessage::controllerEvent(channel, CC, value);
						timestamp = (int)(rtCursor - rtCursorFrom);
						processCC(msg); // make sure model is updated with new cc values

						///////////////////////////////////
						// OUTPUT THE CC EVENT
						///////////////////////////////////

						midiBuffer->addEvent(msg, (int)timestamp);
						if (logMidiOut)
							logMidi(false, msg);
					}
					else if (midiType == Topiary::MidiType::AfterTouch)
					{
						value = parentPattern->dataList[noteChild].value;
						msg = MidiMessage::channelPressureChange(channel, value);
						timestamp = (int)(rtCursor - rtCursorFrom);
						midiBuffer->addEvent(msg, (int)timestamp);
						if (logMidiOut)
							logMidi(false, msg);
					}
					else if (midiType == Topiary::MidiType::Pitch)
					{
						value = parentPattern->dataList[noteChild].value;
						msg = MidiMessage::pitchWheel(channel, value);
						timestamp = (int)(rtCursor - rtCursorFrom);
						midiBuffer->addEvent(msg, (int)timestamp);
						if (logMidiOut)
							logMidi(false, msg);
					}
					else if (midiType == Topiary::MidiType::NOP)
					{
						//jassert(false); // this is really fine
					}

					patternCursor = nextPatternCursor;  // that is the tick of the event we just generated
					if (patternCursor >= parentLength) patternCursor = patternCursor - parentLength;

					nextTick(parentPattern, noteChild);

					//Logger::outputDebugString(String("NOTE ON --------"));
					rtCursor = rtCursor + (int64)(ticksTaken * samplesPerTick);

					jassert((rtCursor - rtCursorFrom) >= 0);
					jassert((rtCursor - rtCursorFrom) < blockSize);

					if (noteChild == rememberChild)
					{
						// can only happen in recording in empty pattern; note OFF but no note ON yet
						// force end
						rtCursor = rtCursor + blockSize;
					}
					//Logger::outputDebugString(String("nxtcursor ") + String(nextRTGenerationCursor));

				}  // generated a note 
				else
#ifdef RIFFZ
				if (parentPattern!=nullptr)  // because we may just have done a stop to re-arm (in case of latch2)
#endif
				{
					// done for now; next event is over rtCursorTo
					// let's place ourselves ready for the next round
					// so either walkOn and we have the next On note ready
					// or walkOff and next Off note ready as well
					// main goal is to set nextPatternCursor!

					//Logger::outputDebugString(String(" ++++++++++++++ done +++++++++++++++++++++"));

					nextPatternCursor = parentPattern->dataList[noteChild].timestamp;
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
			} // while until cursorTo
		}
		else
		{
			// walk did not find a note to generate
			// so pattern is empty or no fitting note events found
			if ((noteChild == 0))
			{
				// make sure our cursors keep running
				// 		  nextRTGenerationCursor = rtCursorTo + 1;
				nextRTGenerationCursor = rtCursorTo;
				patternCursor = +(int)(blockSize / samplesPerTick);

			}
		} // pattern is empty
	} // if Running or Ending

#ifndef PRESETZ
	// output any noteOff events scheduled in this block - this can be called even when ended!!!

	if (noteOffBuffer.bufferSize > 0)

		while (noteOffBuffer.firstTimeStamp() < (blockCursor + blockSize))
		{
			noteOffBuffer.pop(channel, noteNumber, timestamp);
			msg = MidiMessage::noteOff(channel, noteNumber, (float)0.0);
			midiBuffer->addEvent(msg, (int)(timestamp - blockCursor));
			jassert((timestamp - blockCursor) >= 0);
			jassert((timestamp - blockCursor) < blockSize);

			if (logMidiOut)
				logMidi(false, msg);
#if defined(RIFFZ)
			if ((noteOffBuffer.bufferSize == 0) && ((runState == Topiary::Stopped)|| (runState == Topiary::Stopped)) ) // may have been rearmed for latch2
			{
				// output a damper off just to make sure 
				msg = MidiMessage::controllerEvent(channel, 64, 0);
				midiBuffer->addEvent(msg, (int)(timestamp - blockCursor));
				if (logMidiOut)
					logMidi(false, msg);
			}
#endif
		}
#endif


	if ((runState == Topiary::Running) || (runState == Topiary::Ending) 
#ifndef PRESETZ
		|| (noteOffBuffer.bufferSize > 0) 
#endif
		)
	{
		blockCursor = blockCursor + blockSize;
		calcMeasureBeat();
	}

	
} // generateMidi

///////////////////////////////////////////////////////////////////////

bool TOPIARYMODEL::walkToTick(TopiaryVariation* parent, int& childIndex, int toTick)
{
	// Find the first child on or after this tick, starting from current child; 
	// Caller has to make sure that child is child of parent, or pass nullptr to initialize
	// Return false if pattern is empty or nothing to do within the blockSize; if empty then child == nullptr; 
	// This loops around the pattern! 
	// returns childIndex when called from recording (before record we set nextPatternChild to nullptr to make this correct)
	// returns correct prevChild if there is one

	int childTick;

	if (childIndex == 0)
	{
		if (parent->numItems == 0)
		{
			return false; // empty pattern!
		}
	}
	childTick = parent->dataList[childIndex].timestamp;

	while (childTick < toTick)
	{   // as long as our child is behind time we're looking for

		childIndex++;
		if (childIndex == parent->numItems) break; // there are no events afther the given time
		childTick = parent->dataList[childIndex].timestamp;
	}

	if (childIndex == parent->numItems) // wrap around the pattern
	{
		childIndex = 0;
		return true;
	}
	return true;

} // walkToTick

///////////////////////////////////////////////////////////////////////

void TOPIARYMODEL::nextTick(TopiaryVariation* parent, int& childIndex)
{	// assert that parent has at least 1 child of each!!! (do a walk to a tick first!)
	// this one loops around the pattern

	childIndex++;
	if (childIndex == parent->numItems)
	{
		childIndex = 0;
		//Logger::outputDebugString(String("-------- LOOPING OVER END OFF PATTERN ==================== "));
	}

}  // nextTick

///////////////////////////////////////////////////////////////////////

#ifndef PRESETZ

void TOPIARYMODEL::validateTableEdit(int p, XmlElement* child, String attribute)
{
	// called by TopiaryTable
	// careful - can be called when editing patterns but also when editing note pool entries, and patternList etc.  DOES NOT get called for NoteAssignment entries.
	// do processing of user edits to notes/CC/Velocity/Pitch and make sure all is consistent
	

	int index = child->getIntAttribute("ID")-1;  // ID 1 is in position [0]
	

	if (attribute.compare("Label") == 0) 
	{
		if (patternData[p].dataList[index].midiType == Topiary::MidiType::NoteOn)
		{
				// set the note value correctly based on the label
		
				child->setAttribute("Note", validNoteNumber(child->getStringAttribute("Label")));
				child->setAttribute("Label", validateNote(child->getStringAttribute("Label")));
				if (p != -1)
				{
					patternData[p].dataList[index].note = child->getIntAttribute("Note");
					patternData[p].dataList[index].label = child->getStringAttribute("Label");
				}
#ifdef BEATZ				
				else
				{
					poolList.dataList[index].note = child->getIntAttribute("Note");
					poolList.dataList[index].label = child->getStringAttribute("Label");
					poolList.sortByNote();
				}
#endif
		} 
		else
		{
			// label should not be edited; set it back to what is was!
			child->setAttribute("Label",patternData[p].dataList[index].label);
		}

	}  // Label
	else if ((attribute.compare("Measure") == 0) || (attribute.compare("Beat") == 0) || (attribute.compare("Tick") == 0))
	{
		// recalculate timestamp

		if (child->getIntAttribute("Beat") >= denominator)
		{
			child->setAttribute("Beat", 0);
			Log("Beat should be between 0 and " + String(denominator) + ".", Topiary::LogType::Warning);
		}


		int timestamp = child->getIntAttribute("Tick") +
			child->getIntAttribute("Beat")* Topiary::TicksPerQuarter +
			child->getIntAttribute("Measure") * Topiary::TicksPerQuarter * denominator;

		child->setAttribute("Timestamp", String(timestamp));

		patternData[p].dataList[index].timestamp = timestamp;
		patternData[p].dataList[index].measure = child->getIntAttribute("Measure");
		patternData[p].dataList[index].beat = child->getIntAttribute("Beat");
		patternData[p].dataList[index].tick = child->getIntAttribute("Tick");
		patternData[p].sortByTimestamp();
	} // measure, beat and tick

	else if ((attribute.compare("MidiType") == Topiary::MidiType::AfterTouch) || (attribute.compare("MidiType") == Topiary::MidiType::Pitch))
	{
		// in the case we are editing an AT, Pitch, length need not be edited and we always set it to 0
		child->setAttribute("Length", String(0));

	}
	else if (attribute.compare("Val-Vel") == 0)
	{
		if ((patternData[p].dataList[index].midiType == Topiary::MidiType::CC) || (patternData[p].dataList[index].midiType == Topiary::MidiType::AfterTouch) || (patternData[p].dataList[index].midiType == Topiary::MidiType::NoteOn))
		{
			int val = child->getIntAttribute("Val-Vel");
			if ((val >= 0) && (val < 128))
			{
				patternData[p].dataList[index].value = val;
				child->setAttribute("Val-Vel", patternData[p].dataList[index].value = val);
			}
			else
			{
				patternData[p].dataList[index].value = 0;
				patternData[p].dataList[index].velocity = 0;
				child->setAttribute("Val-Vel", 0);
				Log("Value must be between 0-127.", Topiary::LogType::Warning);
			}
		}
		else if (patternData[p].dataList[index].midiType == Topiary::MidiType::Pitch)
		{
			int val = child->getIntAttribute("Val-Vel");
			if ((val >= 0) && (val < 16383))
			{
				patternData[p].dataList[index].value = val;
				child->setAttribute("Val-Vel", patternData[p].dataList[index].value = val);
			}
			else
			{
					patternData[p].dataList[index].value = 0;
					child->setAttribute("Val-Vel", 0);
					Log("Pitch value must be between 0-16384.", Topiary::LogType::Warning);
			}
		}
	}


	// what follows detects what we are editing (pattern, notepool or patternlist)

	if (child->hasAttribute("Timestamp"))
	{

		// if miditype == NoteOn
		// check note does not run over pattern end
		// make sure note length never runs over total patternlength
		if (patternData[p].dataList[index].midiType == Topiary::MidiType::NoteOn)
		{
			if ((child->getIntAttribute("Timestamp") + child->getIntAttribute("Length")) >= patternData[p].patLenInTicks)
			{
				child->setAttribute("Length", String(patternData[p].patLenInTicks - child->getIntAttribute("Timestamp") - 1));
				patternData[p].dataList[index].length = child->getIntAttribute("Length");
				Log("Note shortened so it does not run over pattern end.", Topiary::LogType::Warning);
			}
		}
		else if (patternData[p].dataList[index].midiType == Topiary::MidiType::CC)
		{
				int CC = child->getIntAttribute("Length");
				if ((CC<0)||(CC>128))
				{
					Log("CC must be between 0 and 128", Topiary::LogType::Warning);
					CC = 0;
				}
				patternData[p].dataList[index].length = CC;
				patternData[p].dataList[index].note = CC;
				child->setAttribute("Length", CC);
				child->setAttribute("Note", CC);
		}

		patternData[p].sortByTimestamp();

		// regenerate variations that depend on this one
		
		for (int v = 0; v < 8; v++) 
		{
#ifdef RIFFZ
			bool needToGenerate = false;
			// loop over all note assignments
			for (int na = 0; na < variation[v].noteAssignmentList.numItems; na++)
			{
				// if the assignment uses this pattern; delete it
				if (variation[v].noteAssignmentList.dataList[na].patternId==p)
					needToGenerate = true;
				
			} // loop over all note assignments
			if (needToGenerate)
				generateVariation(v, -1);
#endif
#ifdef BEATZ
			if (variation[v].patternToUse == p)
				generateVariation(v, -1);
#endif		
		} // loop over all variations


#ifdef BEATZ
		rebuildPool(false);
#endif
		broadcaster.sendActionMessage(MsgPattern);
	}
	else if (child->hasAttribute("Note"))
	{
		// edited the note pool
		broadcaster.sendActionMessage(MsgNotePool);
	}
	else if (child->hasAttribute("Measures"))
	{
		// edited the patternlist
		broadcaster.sendActionMessage(MsgPatternList);
#ifdef RIFFZ
		// in case we change pattern name, rename in the note assignments
		if (p>-1)
			for (int pt = 0; pt < patternList.numItems; pt++)
			{
				for (int v = 0; v < 8; v++)
					variation[v].noteAssignmentList.redoPatternNames(pt, patternList.dataList[p].name);
			}
#endif
	}
	
} // validateTableEdit

#endif // for validateTableEdits

///////////////////////////////////////////////////////////////////////
// State logic
///////////////////////////////////////////////////////////////////////

#if defined RIFFZ || defined BEATZ
bool TOPIARYMODEL::getLockState()
{
	return lockState;
}

void TOPIARYMODEL::setLockState(bool state)
{
	lockState = state;
	broadcaster.sendActionMessage(MsgLockState);
}

void TOPIARYMODEL::saveState()
{
	addParametersToModel();
}

void TOPIARYMODEL::restoreState()
{
	restoreParametersToModel();
}

#endif // ifdef RIFFZ

//////////////////////////////////////////////////////////////////////////////////////////////////
// Plugin Parameters Logic
//////////////////////////////////////////////////////////////////////////////////////////////////


#if defined RIFFZ || defined BEATZ
void TOPIARYMODEL::processPluginParameters()
{
	// check whether we are running or not as that defines which variation it applies to!!!

	int var;
	if (runState == Topiary::Running)
		var = variationRunning;
	else
		var = variationSelected;

#if defined(RIFFZ)
	///////////////////////////////
	// RANDOM NOTE LENGTH
	///////////////////////////////

	auto currentRndNoteLen = rndNoteLength->get();  // between 0 and 100

	if (prevRndNoteLength != -1)
	{
		if (abs(prevRndNoteLength - currentRndNoteLen) > 0.99)
		{
			if (variation[var].lengthValue != (int)currentRndNoteLen)
			{
				variation[var].lengthValue = (int)currentRndNoteLen;
				prevRndNoteLength = currentRndNoteLen;
				//Log("UPDATING VARIATION " + String(var), Topiary::LogType::Info);
				broadcaster.sendActionMessage(MsgVariationDefinition);
			}
		}
	}
	else
	{
		prevRndNoteLength = currentRndNoteLen; // init
		//Log("INIT value not processed: " + String(currentRndNoteLen), Topiary::LogType::Info);
		broadcaster.sendActionMessage(MsgVariationDefinition);

	}

	int currentBoolNoteLen = (int)boolNoteLength->get();  // between 0 and 100

	if (prevBoolNoteLength != -1)
	{
		if (prevBoolNoteLength != currentBoolNoteLen)
		{

			if (variation[var].randomizeLength != (bool)currentBoolNoteLen)
			{
				Log("Currentbool " + String(currentBoolNoteLen), Topiary::LogType::Info);
				variation[var].randomizeLength = (bool)currentBoolNoteLen;
				prevBoolNoteLength = currentBoolNoteLen;
				//Log("UPDATING VARIATION " + String(var), Topiary::LogType::Info);
				broadcaster.sendActionMessage(MsgVariationDefinition);
			}

		}

	}
	else
	{
		prevBoolNoteLength = currentBoolNoteLen; // init
		//Log("INIT value not processed: " + String(currentBoolNoteLen), Topiary::LogType::Info);
		broadcaster.sendActionMessage(MsgVariationDefinition);

	}
#endif // ifdef RIFFZ - random note length

	///////////////////////////////
	// RANDOM NOTE OCCURRENCE
	///////////////////////////////

	auto currentRndNoteOccurrence = rndNoteOccurrence->get();  // between 0 and 100

	if (prevRndNoteOccurrence != -1)
	{
		if (abs(prevRndNoteOccurrence - currentRndNoteOccurrence) > 0.99)
		{
			if (variation[var].randomizeNotesValue != (int)currentRndNoteOccurrence)
			{
				variation[var].randomizeNotesValue = (int)currentRndNoteOccurrence;
				prevRndNoteOccurrence = currentRndNoteOccurrence;
				//Log("UPDATING VARIATION " + String(var), Topiary::LogType::Info);
				broadcaster.sendActionMessage(MsgVariationDefinition);
			}
		}
	}
	else
	{
		prevRndNoteOccurrence = currentRndNoteOccurrence; // init
		//Log("INIT value not processed: " + String(currentRndNoteLen), Topiary::LogType::Info);
		broadcaster.sendActionMessage(MsgVariationDefinition);

	}

	int currentBoolNoteOccurrence = (int)boolNoteOccurrence->get();  // between 0 and 127

	if (prevBoolNoteOccurrence != -1)
	{
		if (prevBoolNoteOccurrence != currentBoolNoteOccurrence)
		{

			if (variation[var].randomizeNotes != (bool)currentBoolNoteOccurrence)
			{
				//Log("Currentbool " + String(currentBoolNoteLen), Topiary::LogType::Info);
				variation[var].randomizeNotes = (bool)currentBoolNoteOccurrence;
				prevBoolNoteOccurrence = currentBoolNoteOccurrence;
				//Log("UPDATING VARIATION " + String(var), Topiary::LogType::Info);
				broadcaster.sendActionMessage(MsgVariationDefinition);
			}
		}

	}
	else
	{
		prevBoolNoteOccurrence = currentBoolNoteOccurrence; // init
		//Log("INIT value not processed: " + String(currentBoolNoteLen), Topiary::LogType::Info);
		broadcaster.sendActionMessage(MsgVariationDefinition);

	}

	///////////////////////////////
	// SWING
	///////////////////////////////

	auto currentSwingAmount = swingAmount->get();  // between 0 and 100

	if (prevSwingAmount != -1)
	{
		if (abs(prevSwingAmount - currentSwingAmount) > 0.99)
		{
			if (variation[var].swingValue != (int)currentSwingAmount)
			{
				variation[var].swingValue = (int)currentSwingAmount;
				prevSwingAmount = currentSwingAmount;
				//Log("UPDATING VARIATION " + String(var), Topiary::LogType::Info);
				broadcaster.sendActionMessage(MsgVariationDefinition);
			}
		}
	}
	else
	{
		prevSwingAmount = currentSwingAmount; // init
		//Log("INIT value not processed: " + String(currentRndNoteLen), Topiary::LogType::Info);
		broadcaster.sendActionMessage(MsgVariationDefinition);

	}

	int currentBoolSwing = (int)boolSwing->get();  // between 0 and 127

	if (prevBoolSwing != -1)
	{
		if (prevBoolSwing != currentBoolSwing)
		{

			if (variation[var].swing != (bool)currentBoolSwing)
			{
				//Log("Currentbool " + String(currentBoolNoteLen), Topiary::LogType::Info);
				variation[var].swing = (bool)currentBoolSwing;
				prevBoolSwing = currentBoolSwing;
				//Log("UPDATING VARIATION " + String(var), Topiary::LogType::Info);
				broadcaster.sendActionMessage(MsgVariationDefinition);
			}
		}

	}
	else
	{
		prevBoolSwing = currentBoolSwing; // init
		//Log("INIT value not processed: " + String(currentBoolNoteLen), Topiary::LogType::Info);
		broadcaster.sendActionMessage(MsgVariationDefinition);

	}

	///////////////////////////////
	// TIMING
	///////////////////////////////

	auto currentRndTiming = rndTiming->get();  // between 0 and 100

	if (prevRndTiming != -1)
	{
		if (abs(prevRndTiming - currentRndTiming) > 0.99)
		{
			if (variation[var].timingValue != (int)currentRndTiming)
			{
				variation[var].timingValue = (int)currentRndTiming;
				prevRndTiming = currentRndTiming;
				//Log("UPDATING VARIATION " + String(var), Topiary::LogType::Info);
				broadcaster.sendActionMessage(MsgVariationDefinition);
			}
		}
	}
	else
	{
		prevRndTiming = currentRndTiming; // init
		//Log("INIT value not processed: " + String(currentRndNoteLen), Topiary::LogType::Info);
		broadcaster.sendActionMessage(MsgVariationDefinition);

	}

	int currentBoolTiming = (int)boolTiming->get();  // between 0 and 127

	if (prevBoolTiming != -1)
	{
		if (prevBoolTiming != currentBoolTiming)
		{

			if (variation[var].randomizeTiming != (bool)currentBoolTiming)
			{
				//Log("Currentbool " + String(currentBoolNoteLen), Topiary::LogType::Info);
				variation[var].randomizeTiming = (bool)currentBoolTiming;
				prevBoolTiming = currentBoolTiming;
				//Log("UPDATING VARIATION " + String(var), Topiary::LogType::Info);
				broadcaster.sendActionMessage(MsgVariationDefinition);
			}
		}

	}
	else
	{
		prevBoolTiming = currentBoolTiming; // init
		//Log("INIT value not processed: " + String(currentBoolNoteLen), Topiary::LogType::Info);
		broadcaster.sendActionMessage(MsgVariationDefinition);

	}


	///////////////////////////////
	// VELOCITY
	///////////////////////////////

	auto currentRndVelocity = rndVelocity->get();  // between 0 and 100

	if (prevRndVelocity != -1)
	{
		if (abs(prevRndVelocity - currentRndVelocity) > 0.99)
		{
			if (variation[var].velocityValue != (int)currentRndVelocity)
			{
				variation[var].velocityValue = (int)currentRndVelocity;
				prevRndVelocity = currentRndVelocity;
				//Log("UPDATING VARIATION " + String(var), Topiary::LogType::Info);
				broadcaster.sendActionMessage(MsgVariationDefinition);
			}
		}
	}
	else
	{
		prevRndVelocity = currentRndVelocity; // init
		//Log("INIT value not processed: " + String(currentRndNoteLen), Topiary::LogType::Info);
		broadcaster.sendActionMessage(MsgVariationDefinition);

	}

	int currentBoolVelocity = (int)boolVelocity->get();  // between 0 and 127

	if (prevBoolVelocity != -1)
	{
		if (prevBoolVelocity != currentBoolVelocity)
		{

			if (variation[var].randomizeVelocity != (bool)currentBoolVelocity)
			{
				//Log("Currentbool " + String(currentBoolNoteLen), Topiary::LogType::Info);
				variation[var].randomizeVelocity = (bool)currentBoolVelocity;
				prevBoolVelocity = currentBoolVelocity;
				//Log("UPDATING VARIATION " + String(var), Topiary::LogType::Info);
				broadcaster.sendActionMessage(MsgVariationDefinition);
			}
		}

	}
	else
	{
		prevBoolVelocity = currentBoolVelocity; // init
		//Log("INIT value not processed: " + String(currentBoolNoteLen), Topiary::LogType::Info);
		broadcaster.sendActionMessage(MsgVariationDefinition);

	}
} // processPluginParameters

///////////////////////////////////////////////////////////////////////////////////////

void TOPIARYMODEL::processMidiRecording()
{
	// process recorded events and add to pattern

	int patPos; // where to insert the new events

#ifdef BEATZ
	int loopLen = variation[variationSelected].pattern.numItems;
	TopiaryPattern* pat = &(patternData[variation[variationSelected].patternToUse]);
	TopiaryVariation *var = &(variation[variationSelected].pattern);
#endif
#ifdef RIFFZ
	int loopLen = variation[8].pattern[0].numItems;
	TopiaryVariation* var = &(variation[8].pattern[0]);
	TopiaryPattern* pat = &(patternData[patternSelectedInPatternEditor]);
#endif
	// loop over the variation and look for IDs == -1
	for (int v=0; v < loopLen; v++)
	{
		if (var->dataList[v].length == -1) // newly recorded elements get lenght -1 because we do not know their length yet
		{
			int midiType = var->dataList[v].midiType;
			if ((midiType == Topiary::MidiType::NoteOn) || (midiType == Topiary::MidiType::CC) || (midiType == Topiary::MidiType::Pitch) || (midiType == Topiary::MidiType::AfterTouch))
			{
				patPos = pat->numItems;
				pat->add();

				pat->dataList[patPos].ID = -1; // will be renumbered later
				pat->dataList[patPos].note = 0;

				if (midiType == Topiary::NoteOn)
				{
					pat->dataList[patPos].note = var->dataList[v].note;
					pat->dataList[patPos].label = noteNumberToString(var->dataList[v].note);
					pat->dataList[patPos].length = 0; // wil be calculated next
				}
				else if (midiType == Topiary::Pitch)
					pat->dataList[patPos].label = "Pitch";
				else if (midiType == Topiary::AfterTouch)
					pat->dataList[patPos].label = "AT";
				else if (midiType == Topiary::CC)
				{
					pat->dataList[patPos].label = "CC";
					pat->dataList[patPos].length = var->dataList[v].CC;
				}

				pat->dataList[patPos].velocity = var->dataList[v].velocity; // also for CC, AT and Pitch values			

				pat->dataList[patPos].timestamp = var->dataList[v].timestamp;
				int measur = (int)floor(var->dataList[v].timestamp / (numerator * Topiary::TicksPerQuarter));
				pat->dataList[patPos].measure = measur;
				int timestamp = var->dataList[v].timestamp - measur * (numerator * Topiary::TicksPerQuarter);
				int bea = (int)floor(timestamp / Topiary::TicksPerQuarter);
				pat->dataList[patPos].beat = bea;
				int tic = timestamp - bea * Topiary::TicksPerQuarter;
				pat->dataList[patPos].tick = tic;
				pat->dataList[patPos].midiType = midiType;
			}
			else if (var->dataList[v].midiType == Topiary::MidiType::NoteOff)
			{
				// find the note and set the length
				int timestamp = var->dataList[v].timestamp; // timestamp of end of note
				int note = var->dataList[v].note;
				bool cont = true;
				int pIndex = 0;
				int numPats = pat->numItems;

				while (cont && (pIndex<numPats))
				{
					if ((pat->dataList[pIndex].ID == -1) && (pat->dataList[pIndex].note  == note) && (pat->dataList[pIndex].midiType == Topiary::MidiType::NoteOn))
					{
						// we found the note to end
						pat->dataList[pIndex].length = timestamp - pat->dataList[pIndex].timestamp;
						pat->dataList[pIndex].ID = -2; // so we don't cover it again
						cont = false;
					}
					pIndex++;
				}
			}
			else jassert(false); // should not happen; should be note on or note off event for now
		}

	}
	// resort
	pat->sortByTimestamp();

	Logger::outputDebugString("------------------------");
	for (int j = 0; j < pat->numItems; j++)
	{
		Logger::outputDebugString("<" + String(j) + "> <ID" + String(pat->dataList[j].ID) + "> Note: " + String(pat->dataList[j].note) + " timestamp " + String(pat->dataList[j].timestamp));
	}
	Logger::outputDebugString("------------------------");

#ifdef BEATZ
	// rebuild the pool list
	rebuildPool(false);
#endif

	generateAllVariations(-1);
	// inform pattern tab
	broadcaster.sendActionMessage(MsgPattern);

}  // processMidiRecording

///////////////////////////////////////////////////////////////////////////////////////

bool TOPIARYMODEL::midiLearn(MidiMessage m)
{
	// called by processor; if midiLearn then learn based on what came in
	const GenericScopedLock<CriticalSection> myScopedLock(lockModel);
	bool remember = learningMidi;
	if (learningMidi)
	{
		bool note = m.isNoteOn();
		bool cc = m.isController();

		if (note || cc)
		{
			// check the Id to learn; tells us what to set
			if ((midiLearnID >= Topiary::LearnMidiId::variationSwitch) && (midiLearnID < (Topiary::LearnMidiId::variationSwitch + 8)))
			{
				// learning variation switch
				if (note)
				{
					ccVariationSwitching = false;
					variationSwitch[midiLearnID] = m.getNoteNumber();
				}
				else
				{
					ccVariationSwitching = true;
					variationSwitch[midiLearnID] = m.getControllerNumber();
				}
				learningMidi = false;
				Log("Midi learned", Topiary::LogType::Warning);
				broadcaster.sendActionMessage(MsgVariationAutomation);	// update utility tab
			}
#ifdef RIFFZ
			else if (note && (midiLearnID == Topiary::LearnMidiId::noteAssignmentNote))
			{
				// set in a global variable noteAssignmentNote ; then inform the variation tab to pick it up
				noteAssignmentNote = m.getNoteNumber();
				learningMidi = false;
				Log("Midi learned", Topiary::LogType::Warning);
				broadcaster.sendActionMessage(MsgNoteAssignmentNote);	// update note editor in Variation Tab
			}
			else if (note && ((midiLearnID == Topiary::LearnMidiId::keyrangeFrom) || (midiLearnID == Topiary::LearnMidiId::keyrangeTo)))
			{
				// set in a global variable noteAssignmentNote ; then inform the variation tab to pick it up
				if ((midiLearnID == Topiary::LearnMidiId::keyrangeFrom))
				{
					keyRangeFrom = m.getNoteNumber();
					if (keyRangeFrom > keyRangeTo)
					{
						keyRangeTo = keyRangeFrom;
						Log("Inconsistent key range - 'to' reset as well.", Topiary::LogType::Warning);
					}
				}
				else
				{
					keyRangeTo = m.getNoteNumber();
					if (keyRangeFrom > keyRangeTo)
					{
						keyRangeFrom = keyRangeTo;
						Log("Inconsistent key range - 'from' reset as well.", Topiary::LogType::Warning);
					}

				}
				learningMidi = false;
				Log("Midi learned", Topiary::LogType::Warning);
				broadcaster.sendActionMessage(MsgKeyRangeAssignment);	// update not editors in master Tab
			}
#endif

		}
	}

	return remember;

} // midiLearn

///////////////////////////////////////////////////////////////////////////////////////


#endif // RIFFZ & BEATZ
#endif   // ifdef TOPIARYMODEL

