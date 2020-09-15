/////////////////////////////////////////////////////////////////////////////
/*
This file is part of Topiary, Copyright Tom Tollenaere 2018-2020.

Topiary Riffz is free software: you can redistribute it and/or modify
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

bool loadMidiPattern(const File& fileToRead, int patternIndex, int& measures, int& lenInTicks)
	{
		// fileToRead must have been initialized!!
		// if the input file contains more than 1 real track, the result will
		// be undefined and possibly mix note data from different channels
		// note data need not be on Channel 10 (any channel will do as long as it's one track and one channel

		int index; // index of the new even in the TopiaryPattern

		if (!fileToRead.existsAsFile())
		{
			Log("File " + fileToRead.getFileName() + " does not exist.)", Topiary::LogType::Warning);  // file doesn't exist
			return false;
		}
		FileInputStream inputStream(fileToRead);

		if (!inputStream.openedOk())
		{
			Log("Cannot open file " + fileToRead.getFileName() + ".", Topiary::LogType::Warning);
			return false;  // failed to open
		}

		double bpm = 120.0; // default 
		int num = numerator; // default 
		int den = denominator; // default 

		int ticksPerFrame = 0;
		int framesPerSecond = 0;
		int ticksPerQuarter = 0;
		int timeStampMeasure = 0;
		measures = -1; // logic for multi-track stuff to find longest track

		MidiFile midifile;
		if (!midifile.readFrom(inputStream))
		{
			Log("Invalid MIDI format in file " + fileToRead.getFileName() + ".", Topiary::LogType::Warning);
			return false;
		}

		int numTracks = midifile.getNumTracks();
		/// check header value like timing info
		short timeFormat = midifile.getTimeFormat();
		
		int noteCounter = 0; // index (ID) of the Notes

		if (timeFormat > 0)
		{
			Logger::writeToLog("Ticks per quarter note :" + String(timeFormat));
			ticksPerQuarter = timeFormat; // what is the tick then -> need to know BPM for that!
		}
		else
		{

			ticksPerFrame = timeFormat & 0xFF;
			framesPerSecond = -(timeFormat >> 8);
			//Logger::writeToLog("SMTP format; frames per second:" + String(framesPerSecond) + " ; ticks/frame: " + String(ticksPerFrame));
		}

		//Logger::writeToLog(String("Tracks:" + String(numTracks)));
		// careful: if the file contains more than 1 midi track the result will be undefined!!!

		if (numTracks >1)
			Log("Multiple tracks ("+String(numTracks)+") in input file; result may be unexpected!" , Topiary::LogType::Warning);

		for (int t = 0; t < numTracks; t++) {
			auto sequence = midifile.getTrack(t);
			Logger::writeToLog(String("Track " + String(t)));
			timeStampMeasure = 0; // in case of empty tracks!!!

			for (int i = 0; i < sequence->getNumEvents(); i++) {
				auto event = sequence->getEventPointer(i);
				auto message = event->message;
				if (message.isNoteOn())
				{
					//Logger::writeToLog(message.getDescription());
					//Logger::writeToLog(String(message.getTimeStamp()));

					// create a note
					
					patternData[patternIndex].add();

					index = patternData[patternIndex].numItems-1;
					patternData[patternIndex].dataList[index].ID = noteCounter + 1;
					patternData[patternIndex].dataList[index].note = message.getNoteNumber();
					patternData[patternIndex].dataList[index].velocity = message.getVelocity();
					patternData[patternIndex].dataList[index].label = MidiMessage::getMidiNoteName(message.getNoteNumber(), true, true, 5);
					// calculate start markers
					// duration will follow @noteoff event

					double timeStamp = message.getTimeStamp();
					// recalculate timeStamp to our reference range
					timeStamp = timeStamp * Topiary::TicksPerQuarter / ticksPerQuarter;
					lenInTicks = (int)timeStamp; // passed on to caller
					patternData[patternIndex].dataList[index].timestamp = 	(int)floor(timeStamp);
					timeStampMeasure = (int)floor(timeStamp / (num*Topiary::TicksPerQuarter));
					timeStamp = timeStamp - (timeStampMeasure * num*Topiary::TicksPerQuarter);
					int timeStampBeat = (int)floor(timeStamp / Topiary::TicksPerQuarter);
					timeStamp = timeStamp - (timeStampBeat * Topiary::TicksPerQuarter);
					int timeStampTicks = (int)timeStamp;
					patternData[patternIndex].dataList[index].measure = timeStampMeasure;	
					patternData[patternIndex].dataList[index].beat = timeStampBeat;
					patternData[patternIndex].dataList[index].tick =  timeStampTicks;
					noteCounter++;
					
				}
				if (message.isNoteOff())
				{
					// find the note that is off and set the length
					int oldNote = message.getNoteNumber();
					
					// now find the last occurrence of this note in the children of noteList
					index = patternData[patternIndex].numItems - 1;
					while ((index >=0) && patternData[patternIndex].dataList[index].note != oldNote)
					{
						index--;
					}

					double timeStamp = message.getTimeStamp();
					
					// recalculate timeStamp to our reference range
					timeStamp = timeStamp * Topiary::TicksPerQuarter / ticksPerQuarter;
					lenInTicks = (int)timeStamp; // passed on to caller
					if (index>=0)
						patternData[patternIndex].dataList[index].length = (int)floor(timeStamp) - patternData[patternIndex].dataList[index].timestamp;

				}
				if (message.isMetaEvent())
				{
					if (message.isTempoMetaEvent())
					{
						//getTempoMetaEventTickLength(short timeFormat)
						//Logger::writeToLog(String("TEMPO META EVENT tempo seconds per quarter note: " + String(message.getTempoSecondsPerQuarterNote())));
						bpm = (60 / message.getTempoSecondsPerQuarterNote());
						//Logger::writeToLog(String("=====> BPM " + String(bpm)));
					}
					if (message.isTimeSignatureMetaEvent())
					{
						message.getTimeSignatureInfo(num, den);
						//Logger::writeToLog(String("SIGNATURE META EVENT: " + String(num) + String("/") + String(den)));
					}
				}
			} // loop over events

			// careful - we may have empty tracks11
			if (timeStampMeasure > (measures))
			{
				measures = timeStampMeasure; // because that gets passed on to  caller !! and we do +1 at the end!
				measures++;
			}
			int finalLength = measures * num * Topiary::TicksPerQuarter;
			lenInTicks = finalLength;
			patternData[patternIndex].sortByTimestamp(); // this one renumbers!
				
		} // loop over tracks

		Log("File " + fileToRead.getFileName() + " imported.", Topiary::LogType::Info);

		if ((num != numerator) || (den != denominator))
			Log("Time signature in file is different from plugin timesignature; results may be unexpected!", Topiary::LogType::Warning);

		return true;
	}; // loadMidiDrumPattern