/////////////////////////////////////////////////////////////////////////////
/*
This file is part of Topiary, Copyright Tom Tollenaere 2019.

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
	return variation[v].lenInTicks;

} // getVariationLenInTicks

///////////////////////////////////////////////////////////////////////

bool TOPIARYMODEL::getVariationEnabled(int v)
{
	return variation[v].enabled;

} // getVariationEnabled

///////////////////////////////////////////////////////////////////////

void TOPIARYMODEL::getVariationDetailForGenerateMidi(XmlElement** parent, XmlElement** noteChild, int& parentLength, bool& ending, bool& ended)
{
	parentLength = variation[variationRunning].lenInTicks;
	ending = variation[variationRunning].ending;
	ended = variation[variationRunning].ended;
	*parent = variation[variationRunning].pattern;
	*noteChild = variation[variationRunning].currentPatternChild;

} // getVariationDetailForGenerateMidi

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
		// next for Presetz
		threadRunnerState = Topiary::ThreadRunnerState::NothingToDo;
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

#endif