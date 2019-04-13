/////////////////////////////////////////////////////////////////////////////
/*
This file is part of Topiary, Copyright Tom Tollenaere 2018-19.

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

void TOPIARYMODEL::setOverrideHostTransport(bool o)
{
	if (overrideHostTransport != o)
	{
		{   // block with a spinlock
			const GenericScopedLock<SpinLock> myScopedLock(lockModel);
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
		} // end spinlock

		broadcaster.sendActionMessage(MsgTransport);
		// careful here - if we just set the runstate to Stopped - it might have been stopped already and variables may be undefined
		// dirty hack below
		{
			const GenericScopedLock<SpinLock> myScopedLock(lockModel);
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
		const GenericScopedLock<SpinLock> myScopedLock(lockModel);
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


void TOPIARYMODEL::setVariation(int n, bool lockIt)
{
	jassert(n < 8);
	jassert(n >= 0);

	if (recordingMidi && (n!=variationSelected))
	{
		// do not allow switching of variations
		Log("Cannot switch variation during recording.", Topiary::LogType::Warning);
		return;
	}

	if ((n != variationSelected) || (runState == Topiary::Stopped))
		// the || runState || is needed because we may need to re-set a waiting variation to non-waiting; in that case we want the update to happen otherwise the buttons stays orange
	{
		if (lockIt)
			lockModel.enter();

		variationSelected = n;
		if (runState == Topiary::Stopped)  // otherwise the switch will be done when running depending on the variation switch Q
			variationRunning = n;
		//Log(String("Variation ") + String(n + 1) + String(" selected."), Topiary::LogType::Variations);
		broadcaster.sendActionMessage(MsgVariationSelected);  // if the editor is there it will pick up the change in variation
		if (lockIt)
			lockModel.exit();
	}

#ifdef PRESETZ
	if ((runState != Topiary::Running) && variation[variationSelected].enabled)
		// we ALWAYS do this - even if the variation does not change (because we may wanna hit the variation button to reset it (presetz)
		// so even if WFFN is on - as long as we are waiting wo do an immediate output of the variation settings!
		outputVariationEvents(); // output the variation preset values!
#endif
} // setVariation

///////////////////////////////////////////////////////////////////////


void TOPIARYMODEL::setEnded()
{
	variation[variationRunning].ended = true;
}

///////////////////////////////////////////////////////////////////////

void TOPIARYMODEL::getVariationEnables(bool enables[8])
{
	for (int i = 0; i < 8; i++)
		enables[i] = variation[i].enabled;

} // getVariationEnables

///////////////////////////////////////////////////////////////////////

int TOPIARYMODEL::getVariationLenInTicks(int v)
{
	return variation[v].pattern.patLenInTicks;

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
	
	//const GenericScopedLock<SpinLock> myScopedLock(lockModel); 
	
	//Logger::outputDebugString(String("Sel ")+String(variationSelected)+String(" Running ")+String(variationRunning));
	
	if (variationSelected == variationRunning) return false;
	else
	{
#ifdef PRESETZ
		if (threadRunnerState == Topiary::ThreadRunnerState::Generating)
		{
			//Logger::outputDebugString(" WAITING FOR GENERATION ---------------------------------------------------------------------------------------------");
			return false;  // for presetz only: we are still generating a transition; wait a bit
		}
#endif
	}

	if (blockCursor == 0)
	{
		runState = Topiary::Running;
		//Logger::outputDebugString("Starting - runstate is Armed but we need to go!");  // careful that the WFFN functonality keeps working!!!
		return true;
	}
	// in this case we want to start asap (unless we Q on the transport, but that is done in the call before this one!!!

//Logger::outputDebugString("Need to switch, now ???");
//Logger::outputDebugString("blockcursor "+String(blockCursor)+" NextRTcursor "+String(nextRTGenerationCursor));

// nothing is going to happen now, so don't bother calculating
//	if (nextRTGenerationCursor > (blockCursor + 2 * blockSize))
//	{
//		Logger::outputDebugString(" WAIT FOR NEXTRTGENERATIONCURSOR ---------------------------------------- >");
//		return false;
//	}
	// first decide whether we should switch in this block

	// we are at measure/beat/tick overall; time and tick apply to the pattern we are in, but not measure within the pattern so we calculate that by using patternCursorOn
	//int patternMeasure = patternCursorOn % (numerator * Topiary::TICKS_PER_QUARTER);

	//Logger::outputDebugString("Need to switch, now ???  Something is going to happen next");

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
		cursorToSwitch = (int64)(blockCursor + samplesPerTick * ((Topiary::TICKS_PER_QUARTER - tick - 1) + (numerator - beat - 1)* Topiary::TICKS_PER_QUARTER));
		//Logger::outputDebugString(String("MEASURE Sel ") + String(variationSelected) + String(" Running ") + String(variationRunning));
		break;
	}
	case (Topiary::Quarter):
	{
		// moment of next beat = blockCursor + time to next beat (ticksperquarter - tick) 
		cursorToSwitch = (int64)(blockCursor + samplesPerTick * ((Topiary::TICKS_PER_QUARTER - tick - 1)));
		//Logger::outputDebugString(String("QUARTER Sel ") + String(variationSelected) + String(" Running ") + String(variationRunning));
		break;
	}
	//case (Topiary::Half):
	//{
		// moment of next beat = blockCursor + time to next beat (ticksperquarter - tick) 
	//	cursorToSwitch = (int64)(blockCursor + samplesPerTick * ((Topiary::TICKS_PER_QUARTER - tick - 1) + Topiary::TICKS_PER_QUARTER));
	//	//Logger::outputDebugString(String("IMMEDIATE Half ") + String(variationSelected) + String(" Running ") + String(variationRunning));
	//	break;
	//}
	default: jassert(false);

	} // switch
	//Logger::outputDebugString(String("BlockCursor ") + String(blockCursor));
	//Logger::outputDebugString(String("CursorToSwitch ") + String(cursorToSwitch));

	//if ((nextRTGenerationCursor <= cursorToSwitch) && (cursorToSwitch < (blockCursor + blockSize))) 
	if (cursorToSwitch < (blockCursor + blockSize))
	{

		//Logger::outputDebugString("Need to switch, now ??? ----------------------------> yes we're sure");
		// we are sure we're switching variations now
		// so decide where in the variation we start - we need to set patternCursorOn (and we will set patternCursorOff to the same value)
		
		patternCursor = (int)floor(blockCursor / samplesPerTick) % getVariationLenInTicks(variationSelected);

		switch (switchVariation)
		{
		case (Topiary::SwitchFromStart): 
			patternCursorOffset = patternCursor;
			//Logger::outputDebugString("PatternCursorOffset in PROCESSVARIATIONSWITCH: " + String(patternCursorOffset));
			//Logger::outputDebugString("Blockcursor ; " + String(blockCursor));
			break;
		case (Topiary::SwitchWithinBeat): 
			patternCursorOffset = 0;
			patternCursor = patternCursor % Topiary::TICKS_PER_QUARTER;
			break;
		case (Topiary::SwitchWithinMeasure): 
			patternCursorOffset = 0;
			patternCursor = patternCursor % (Topiary::TICKS_PER_QUARTER*numerator);
			break;
		case (Topiary::SwitchWithinPattern):	
			patternCursorOffset = 0;
			patternCursor = patternCursor % (getVariationLenInTicks(variationSelected));
			break;
		}

		Log(String("Switch from variation ") + String(variationRunning) + String(" to ") + String(variationSelected), Topiary::LogType::Variations);
		variationRunning = variationSelected;
#ifdef PRESETZ
		threadRunnerState = Topiary::ThreadRunnerState::NothingToDo;
#endif
		broadcaster.sendActionMessage(MsgVariationSelected);

		return true;
	}
	else return false;

} // processVariationSwitch

///////////////////////////////////////////////////////////////////////

bool TOPIARYMODEL::switchingVariations()
{
	bool switching = (variationRunning != variationSelected);
	
#ifdef PRESETZ	
	// for Presetz, switching variations needs to trigger regeneration of the variations
	if (switching)
	{
		if (threadRunnerState == Topiary::ThreadRunnerState::NothingToDo)
		{
			// we just detected the variation switch for the first time
			//Logger::outputDebugString("Worker thread NOTIFIED --------------------------------------------------");
			transitioningFrom = variationRunning;
			transitioningTo = variationSelected;
			threadRunnerState = Topiary::ThreadRunnerState::Generating;
			topiaryThread.notify();  // we notify the thread that we need to generate a transition 
		}	
	}
#endif

	return switching;

} // switchingVariations

/*  FOR TESTING THIS IS NOW IN BEATSMODEL.cpp
///////////////////////////////////////////////////////////////////////

void TOPIARYMODEL::generateMidi(MidiBuffer* midiBuffer, MidiBuffer* recBuffer)
{ // main Generator

	const GenericScopedLock<SpinLock> myScopedLock(lockModel);

	/ *************************************************************************************************************************************************
	Uses a lot of model variables!  Summary of what is needed for what here

	variation[variationRunning].pattern;
	variation[variationRunning].currentPatternChild;
	variation[variationRunning].lenInTicks;
	variation[variationRunning].lenInMeasures;   DO WE NEED THIS, THINK NOT!

	int64 blockCursor;				// sampletime of start of current block
	int64 nextRTGenerationCursor;	// real time cursor of next event to generate
	int blockSize;					// size of block to generate
	int patternCursor;				// ticks where we are within the variation/pattern - if we do nothing it should still advance with blocksize/samplesPerTick

	************************************************************************************************************************************************** /

	int64 rtCursorFrom;				// sampletime to start generating from
	int64 rtCursorTo;				// we generate in principle till here
	int64 rtCursor;					// where we are, between rtCursorFrom and rtCursorTo
	//int64 patternCursorInSamples;   // needed to avoid rounding errors

	//int DEBUGpatCursor = 0;

	if (blockCursor == 0)			// blockCursor is updated at end of generation!
	{
		rtCursor = 0;
		patternCursor = 0;
		rtCursorFrom = 0;
		patternCursorOffset = 0;
		//patternCursorInSamples = 0;
		rtCursorTo = rtCursorFrom + blockSize;
		nextRTGenerationCursor = 0;
		measure = 0;
		beat = 0;
		tick = 0;
		noteOffBuffer.bufferSize = 0;
	}
	else
	{
		rtCursorFrom = blockCursor;
		rtCursorTo = rtCursorFrom + blockSize;
		rtCursor = rtCursorFrom;
		calcMeasureBeat();
		//DEBUGpatCursor = (int)floor(blockCursor / samplesPerTick);
		patternCursor = (int)(blockCursor / samplesPerTick) - patternCursorOffset; // not floor - that may lead to duplicate notes!!!
		//patternCursorInSamples = blockCursor - patternCursorOffset * samplesPerTick;

		//Logger::outputDebugString("Enter generateMidi with patternCursor " + String(patternCursor));
	}

	int parentLength; // in ticks
	bool ending; // indicates this variation will play only once
	bool ended; // indicates this variation was ending==true and has now ended
	MidiMessage msg;
	int parent = 0; //
	int noteChild = 0; //
	int noteChildIndex;
	jassert(false); // needs rework
	

#ifdef PRESETZ
	UNUSED(recBuffer)
#endif
#ifndef PRESETZ


	////////////////////////////////////////
	// Record logic here
	////////////////////////////////////////

#define RECBUFFERSIZE 100
	MidiMessage recordBuffer[RECBUFFERSIZE];
	int recordTiming[RECBUFFERSIZE];
	int recordBufferSize = 0; 
	int samplePos = 0;

	if (recordingMidi)
	{
		for (MidiBuffer::Iterator it(*recBuffer); it.getNextEvent(msg, samplePos);)
		{
			if (msg.isNoteOn() || msg.isNoteOff())
			{
				recordBuffer[recordBufferSize] = msg;
				recordTiming[recordBufferSize] = (int)(samplePos / samplesPerTick);  // timing in ticks
				recordBufferSize++;
				Logger::outputDebugString("RECORDING");
			}
		}

		if (recordBufferSize)
		{
			int rememberPatternCursor = patternCursor;

			// pick a channel	
			int channel = 10;

			jassert(channel != 0); // should not happen; editor should prevent that!

			// now insert the recorded events
			getVariationDetailForGenerateMidi(parent, noteChild, parentLength, ending, ended);

			// if pattern not empty
			if (parent->getNumChildElements())
			{
				patternCursor = (int)patternCursor % parentLength;
				for (int r = 0; r < recordBufferSize; r++)
				{
					noteChild = nullptr; // otherwise noteChildIndex won't be correct
					walkToTick(parent, &noteChild, patternCursor, noteChildIndex);
					XmlElement* newChild;
					newChild = new XmlElement("RECDATA");  // the RECDATA elements will get inserted in the pattern when recording done
					newChild->setAttribute("ID", 0); // dummy
					newChild->setAttribute("Note", recordBuffer[r].getNoteNumber());
					newChild->setAttribute("Velocity", recordBuffer[r].getVelocity());
					newChild->setAttribute("Channel", channel);
					if (recordBuffer[r].isNoteOn())
						newChild->setAttribute("midiType", Topiary::MidiType::NoteOn);
					else
						newChild->setAttribute("midiType", Topiary::MidiType::NoteOff);

					int64 cursorInTicks = (int64)floor(blockCursor / samplesPerTick) + recordTiming[r];
					cursorInTicks = cursorInTicks % variation[variationSelected].pattern.patLenInTicks;

					//int measure = (int)floor(cursorInTicks / (ticksPerBeat* denominator)) + 1;
					//int beat = (int)floor(cursorInTicks / ticksPerBeat);
					//int beat = (beat % denominator) + 1;

					newChild->setAttribute("Timestamp", (int)cursorInTicks);
					patternCursor = (int)cursorInTicks;
					parent->insertChildElement(newChild, noteChildIndex);
					///String myXmlDoc = parent->createDocument(String());
					///Logger::writeToLog(myXmlDoc);
					///Logger::outputDebugString("RECORDING in non-empty pattern --> inserted!");
				} // for
			}
			else
			{
				// pattern is empty; just insert everything
				
				for (int r = 0; r < recordBufferSize; r++)
				{
					XmlElement* newChild;
					newChild = new XmlElement("RECDATA");  // the RECDATA elements will get inserted in the pattern when recording done
					newChild->setAttribute("ID", 0); // dummy
					newChild->setAttribute("Note", recordBuffer[r].getNoteNumber());
					newChild->setAttribute("Velocity", recordBuffer[r].getVelocity());
					newChild->setAttribute("Channel", channel);
					if (recordBuffer[r].isNoteOn())
						newChild->setAttribute("midiType", Topiary::MidiType::NoteOn);
					else
						newChild->setAttribute("midiType", Topiary::MidiType::NoteOff);

					int64 cursorInTicks = (int64)floor(blockCursor / samplesPerTick) + recordTiming[r];
					cursorInTicks = cursorInTicks % variation[variationSelected].pattern.patLenInTicks;

					//int measure = (int)floor(cursorInTicks / (ticksPerBeat* denominator)) + 1;
					//int beat = (int)floor(cursorInTicks / ticksPerBeat);
					//int beat = (beat % denominator) + 1;

					newChild->setAttribute("Timestamp", (int)cursorInTicks);
					parent->insertChildElement(newChild, r);
					Logger::outputDebugString("RECORDING in empty pattern --> inserted!");
				} // for


			} // record in empty pattern

			// reset patternchild to nullptr so the rest of the logic can restart (because we may have messed with patternchild
			variation[variationRunning].currentPatternChild = 0;
			patternCursor = rememberPatternCursor;

		} // recordBufferSize != 0
	}

	////////////////////////////////////////
	// End of record logic 
	////////////////////////////////////////

#endif

	//Logger::outputDebugString("Generate midi; patcur" + String(patternCursor));
	//Logger::outputDebugString("next RTcursor " + String(nextRTGenerationCursor));

	jassert(beat >= 0);
	jassert(measure >= 0);

	getVariationDetailForGenerateMidi(&parent, &noteChild, parentLength, ending, ended);

	int nextPatternCursor;  // patternCursor is global and remembers the last one we generated

	int ticksTaken;
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
		walk = walkToTick(parent, &noteChild, patternCursor, noteChildIndex);
	}
	else
		walk = false; // meaning an ending variation and ended

	//Logger::outputDebugString(String("Walk: ") + String((int)walk));
	
	if (walk)
	{
		// set patternCursors where we are now, so the offsets in sample time are correct
		patternCursor = (int)patternCursor % parentLength;  //////// because rtCursors are multi loops !!!!
		//patternCursorInSamples = patternCursorInSamples % (parentLength * samplesPerTick);
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
				//Logger::outputDebugString("PatternCursor looped over end");

#ifdef BEATZ
				if (!recordingMidi) // because we do not want to loose the recorded notes!
				{
					if (threadRunnerState == Topiary::ThreadRunnerState::NothingToDo)
					{
						//threadRunnerState = Topiary::ThreadRunnerState::Generating;
						//topiaryThread.notify();  // trigger regeneration
						generateVariation(variationRunning);
						getVariationDetailForGenerateMidi(&parent, &noteChild, parentLength, ending, ended);
						walk = walkToTick(parent, &noteChild, patternCursor, noteChildIndex);
					}
				}
#endif

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
				Logger::outputDebugString("GENERATING A NOTE " + String(noteChild->getIntAttribute("midiType")) + "------------>" + String(noteChild->getIntAttribute("Note"))+" at timest "+ String(noteChild->getIntAttribute("Timestamp")));
				//Logger::outputDebugString(String("Next Patcursor ") + String(nextPatternCursor));
				 
				////// GENERATE MIDI EVENT
				int midiType = noteChild->getIntAttribute("midiType");
				if ((midiType == Topiary::MidiType::NoteOn) || (midiType == Topiary::MidiType::NoteOff))
				{ // Generate note on or off; ignore other midi events for now
					length = noteChild->getIntAttribute("Length");
					noteNumber = noteChild->getIntAttribute("Note");
					channel = noteChild->getIntAttribute("Channel");
					if (midiType == Topiary::MidiType::NoteOn)
					{
						msg = MidiMessage::noteOn(channel, noteNumber, (float)noteChild->getIntAttribute("Velocity") / 128);
						//if (noteNumber == debugLastNoteOn)
						//	jassert(false);
						//debugLastNoteOn = noteNumber;
					}
					else
					{
						msg = MidiMessage::noteOff(channel, noteNumber, (float)noteChild->getIntAttribute("Velocity") / 128);
						//debugLastNoteOn = 0;
					}

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

				XmlElement* rememberChild = noteChild;
				nextTick(parent, &noteChild);
				
				//Logger::outputDebugString(String("NOTE ON --------"));
				rtCursor = rtCursor + (int64)(ticksTaken * samplesPerTick);

				jassert((rtCursor - rtCursorFrom) >= 0);

				midiBuffer->addEvent(msg, (int)(rtCursor - rtCursorFrom));
				if (logMidiOut)
					logMidi(false, msg);

				if (noteChild == rememberChild)
				{
					// can only happen in recording in empty pattern; note OFF but no note ON yet
					// force end
					rtCursor = rtCursor + blockSize;
				}
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
			nextRTGenerationCursor = rtCursorTo;
			patternCursor = +(int)(blockSize / samplesPerTick);

		}

	}

	/* do any note off events schedules for this block

	while (noteO
	
	blockCursor = blockCursor + blockSize;

	calcMeasureBeat();


} // generateMidi
*/
//////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef PRESETZ
/*
void TOPIARYMODEL::cleanPattern(int p)
{
	// if there were edits done, recalculate stuff
	// check the length; if turns out to be longer than what the length should be; delete unneeded event
	// redo the Ids (might have added or deleted somthing
	// recalculate timestamps based on meabure, beat and tick
	// set the note number as the note label may have changed
	// regenerate any variations that depend on this pattern

	jassert(p < 8);
	
	jassert(false); // still to do
	XmlElement *child = patternData[p].noteData->getFirstChildElement();

	while (child != nullptr)
	{

		child = child->getNextElement();
	}
	
} // cleanPattern
*/
#endif

//////////////////////////////////////////////////////////////////////////////////////////////////

#endif