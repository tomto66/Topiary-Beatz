/////////////////////////////////////////////////////////////////////////////
/*
This file is part of Topiary Beatz, Copyright Tom Tollenaere 2018-19.

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


#pragma once
#include "../../Topiary/Source/TopiaryModel.h"
#define TOPIARYMODEL TopiaryBeatsModel
#define BEATZ
#include "../../Topiary/Source/TopiaryTable.h"

class TopiaryBeatsModel : public TopiaryModel
{
public:
	TopiaryBeatsModel();
	~TopiaryBeatsModel();

	String name = "New Beatz Pattern"; // name of this preset

	void getMasterModel(XmlElement** h, XmlElement** d, XmlElement** ph, XmlElement** pd);
	void getPatternModel(int patternIndex, XmlElement** h, XmlElement** d);
	void getPatterns(String pats[8]);
	int getPatternLengthInMeasures(int i);  // needed in variationComponent for validating pool length values;
	void setPatternLengthInMeasures(int i, int l);
	void deletePattern(int i);
	void addPattern();
	void setPatternTableHeaders(int p); // set measure & beat limits in patterntable headers

	void cleanPattern(int p) override;
	void setPatternLength(int p, int l, bool keepTail);
	void deleteNote(int p, int n);				// deletes the note with ID n from pattern p
	void getNote(int p, int ID, int& note, int &velocity, int &timestamp, int &length);  // get note with id ID from pattern p
	void addNote(int p, int n, int v, int l, int t);	// adds note n in pattern p, with velocity v at time t
	void regenerateVariationsForPattern(int p);

	void addPoolNote();
	void deletePoolNote(int i);
	void rebuildPool();
	void setGMDrumMapLabels();
	bool insertPatternFromFile(int patternIndex);
	XmlElement* addToModel(char* type);
	XmlElement* addToModel(char *type, int index);
	void removeFromModel(char *type, XmlElement *child);
	
	void validateNoteEdit(int p, XmlElement* child, String attribute); // see if user edits to this attribute make sense and do housekeeping

	void saveStateToMemoryBlock(MemoryBlock& destData) override;
	void restoreStateFromMemoryBlock(const void* data, int sizeInBytes) override;
	bool processVariationSwitch() override;
	bool switchingVariations() override;
	void getVariationDetailForGenerateMidi(XmlElement** parent, XmlElement** noteChild, int& parentLength, bool& ending, bool& ended) override;
	void initializeVariationsForRunning() override;
	void setEnded() override;
	void generateMidi(MidiBuffer* midiBuffer) override;
	
	////// Variations

	void setVariation(int i) override;
	void getVariation(int& running, int& selected);
	void getVariationEnables(bool enables[8]);
	bool getVariationEnabled(int v);
	int getVariationLenInTicks(int v);

	void getVariationDefinition(int i, bool& enabled, String& vname, int& patternId, bool enables[4], int poolLength[4][3][2], int poolChannel[4]);   // pass variation Definition on to VariationComponent in editor
	void setVariationDefinition(int i, bool enabled, String vname, int patternId, bool enables[4], int poolLength[4][3][2], int poolChannel[4]);      // set variation Definition parameters from editor; return false if we try to disable the last enabled variation
	bool validateVariationDefinition(int i, bool enable, String vname, int patternId, int poolLength[4][3][2]);
	void setRandomizeNotes(int v, bool enable, bool enablePool[4], int value);
	void getRandomizeNotes(int v, bool &enable, bool enablePool[4], int &value);
	void setSwing(int v, bool enable, bool enablePool[4], int value);
	void getSwing(int v, bool &enable, bool enablePool[4], int &value);
	void setRandomizeVelocity(int v, bool enable, bool enablePool[4], int value, bool plus, bool min);
	void getRandomizeVelocity(int v, bool &enable, bool enablePool[4], int &value, bool &plus, bool &min);
	void setRandomizeTiming(int v, bool enable, bool enablePool[4], int value, bool plus, bool min);
	void getRandomizeTiming(int v, bool &enable, bool enablePool[4], int &value, bool &plus, bool &min);

	void generateVariation(int i); // Generates the variation; at the end , swaps it in (using spinlock)
	void generateAllVariations();
	void threadRunner() override;

	void setOverrideHostTransport(bool o) override;
	void setNumeratorDenominator(int nu, int de) override;

	struct Variation {

		int patternToUse;					// index in patterndata
		int lenInTicks;
		int lenInMeasures;
		XmlElement* pattern;				// pattern  events
		XmlElement* currentPatternChild;    // where we are in generation 
		
		bool ending;					// indicates that once pattern played, we no longer generate notes! (but we keep running (status Ended)
		bool ended;

		bool enabled = false;
		String name = "";
		bool enablePool[4];
		
		/// pool length definitions
		int beatFrom[4]; int beatTo[4];
		int measureFrom[4]; int measureTo[4];
		int tickFrom[4];
		int tickTo[4];
		int fullTickTo[4];		// pool measure, beat, tick translated tot total!
		int fullTickFrom[4];	// pool measure, beat, tick translated tot total!
		int poolChannel[4];		// output channel for this pool in this variation
		int poolTickCursor[4];	// offset in ticks of next note to generate in the pool (offset in the to-generate pattern, not offset in the pool :)
		int poolIdCursor[4];	// Id of next note to generate in the pool (ID of note in the parent pattern; both cursors to initialize every time run starts

		// note occurrence randomization
		bool randomizeNotes;
		int randomizeNotesValue;
		bool randomizeNotePool[4];

		// swing
		bool swing;
		int swingValue;
		bool swingPool[4];

		// velocity
		bool randomizeVelocity;
		int velocityValue;
		bool velocityPool[4];
		bool velocityPlus;
		bool velocityMin;

		// timing
		bool randomizeTiming;
		int timingValue;
		bool timingPool[4];
		bool timingPlus;
		bool timingMin;

	};

	void swapVariation(int from, int to) override;
	void copyVariation(int from, int to) override;
	//void swapPattern(int from, int to) override;
	//void copyPattern(int from, int to) override;

	
#define NUMBEROFQUANTIZERS 10

private:

	XmlElement *patternListHeader = new XmlElement("PatternListHeader");
	XmlElement *patternListData = new XmlElement("PatternListData");

	XmlElement *poolListHeader = new XmlElement("PoolListHeader");
	XmlElement *poolListData = new XmlElement("PoolListData");

	XmlElement *patternHeader = new XmlElement("PatternHeader");
	struct Pattern
	{
		XmlElement *noteData;
		
		int numNotes = 0;
		int notesRealID = 1;
		int patLenInTicks = 0;
	} patternData[8];

	int patternsRealID = 1;
	int numPoolNotes = 0;
	int poolNotesRealID = 1;
	
	int debugLastNoteOn = 0; // delete - for debugging duplicate notes

	Variation variation[8];  // struct to hold variation detail
	
	///////////////////////////////////////////////////////////////////////

	void renumberPool(XmlElement *list)
	{  // resort by timestamp and set the Ids in that order
		DataSorter sorter("Note", true);
		list->sortChildElements(sorter);
		XmlElement* child = list->getFirstChildElement();
		int id = 1;
		while (child != nullptr)
		{
			child->setAttribute("ID", id);
			id++;
			child = child->getNextElement();
		}
	} //renumberPool

	///////////////////////////////////////////////////////////////////////

	bool loadMidiDrumPattern(const File& fileToRead, XmlElement *noteList, int patternIndex, int& measures, int& lenInTicks)
	{
		// fileToRead and noteList must have been initialized!!
		// noteList will receive children that have tag NOTEDATA
		// if the input file contains more than 1 real track, the result will
		// be undefined and possibly mix note data from different channels
		// note data need not be on Channel10 (any channel will do as long as it's one track and one channel

		//jassert(noteList->getTagName().compare("PATTERNDATA"));

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

		MidiFile midifile;
		if (!midifile.readFrom(inputStream))
		{
			Log("Invalid MIDI format in file " + fileToRead.getFileName() + ".", Topiary::LogType::Warning);
			return false;
		}

		int numTracks = midifile.getNumTracks();
		/// check header value like timing info
		short timeFormat = midifile.getTimeFormat();
		XmlElement *newNote;

		int noteCounter = 0; // index (ID) of the Notes

		if (timeFormat > 0)
		{
			Logger::writeToLog("Ticks per quarter note :" + String(timeFormat));
			ticksPerQuarter = timeFormat; // what is the tick then??? - need to know BPM for that!
		}
		else
		{

			ticksPerFrame = timeFormat & 0xFF;
			framesPerSecond = -(timeFormat >> 8);
			Logger::writeToLog("SMTP format; frames per second:" + String(framesPerSecond) + " ; ticks/frame: " + String(ticksPerFrame));
		}

		Logger::writeToLog(String("Tracks:" + String(numTracks)));
		// careful: if the file contains more than 1 midi track the result will be undefined!!!
		for (int t = 0; t < numTracks; t++) {
			auto sequence = midifile.getTrack(t);
			Logger::writeToLog(String("Track " + String(t)));
			for (int i = 0; i < sequence->getNumEvents(); i++) {
				auto event = sequence->getEventPointer(i);
				auto message = event->message;
				if (message.isNoteOn())
				{
					Logger::writeToLog(message.getDescription());
					Logger::writeToLog(String(message.getTimeStamp()));
					// create a note
					newNote = new XmlElement("DATA");
					newNote->setAttribute("ID", noteCounter + 1);
					newNote->setAttribute("REALID", noteCounter + 1); // because noteCounter starts at 0 (it will be renumbered at the end) but REALID has to start at 1
					newNote->setAttribute("Note", message.getNoteNumber());
					newNote->setAttribute("Velocity", message.getVelocity());
					newNote->setAttribute("Label", MidiMessage::getMidiNoteName(message.getNoteNumber(), true, true, 5));
					// calculate start markers
					// duration will follow @noteoff event

					double timeStamp = message.getTimeStamp();
					// recalculate timeStamp to our reference range
					timeStamp = timeStamp * Topiary::TICKS_PER_QUARTER / ticksPerQuarter;
					lenInTicks = (int)timeStamp; // passed on to caller
					newNote->setAttribute("Timestamp", (int)floor(timeStamp));
					timeStampMeasure = (int)floor(timeStamp / (num*Topiary::TICKS_PER_QUARTER));
					timeStamp = timeStamp - (timeStampMeasure * num*Topiary::TICKS_PER_QUARTER);
					int timeStampBeat = (int)floor(timeStamp / Topiary::TICKS_PER_QUARTER);
					timeStamp = timeStamp - (timeStampBeat * Topiary::TICKS_PER_QUARTER);
					int timeStampTicks = (int)timeStamp;
					newNote->setAttribute("Measure", timeStampMeasure);
					newNote->setAttribute("Beat", timeStampBeat);
					newNote->setAttribute("Tick", timeStampTicks);
					noteCounter++;
					noteList->prependChildElement(newNote); // we do this backwards to it's easier to find the note on event!!!
				}
				if (message.isNoteOff())
				{
					// find the note that is off and set the length
					int oldNote = message.getNoteNumber();
					// now find the last occurrence of this note in the children of noteList
					XmlElement *child = noteList->getFirstChildElement();
					while ((child->getStringAttribute("Note")).equalsIgnoreCase(String(oldNote)) == false)
					{
						String note = (child->getStringAttribute("Note"));
						child = child->getNextElement();
					}
					double timeStamp = message.getTimeStamp();
					// recalculate timeStamp to our reference range
					timeStamp = timeStamp * Topiary::TICKS_PER_QUARTER / ticksPerQuarter;
					lenInTicks = (int)timeStamp; // passed on to caller
					child->setAttribute("Length", (int)floor(timeStamp - child->getStringAttribute("Timestamp").getIntValue()));

					// so child is our note ON event; fill the note OFF event
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
			}

			patternData[patternIndex].numNotes = noteList->getNumChildElements();
			patternData[patternIndex].notesRealID = patternData[patternIndex].numNotes;

			measures = timeStampMeasure; // because that gets passed on to  caller !! and we do +1 at the end!
			measures++;
			int finalLength = measures * num * Topiary::TICKS_PER_QUARTER;
			lenInTicks = finalLength;
			renumberByTimestamp(noteList);
			
			// summary:
			//Logger::writeToLog(String("************ BPM: ") + String(bpm) + String(" signature ") + String(num) + String("/") + String(den));
			//Logger::writeToLog(String("************ From header timeframe:  tickperQ ") + String(timeFormat) + String("ticksperFrame ") +
			//	String(ticksPerFrame) + String("framesPerSecond ") + String(framesPerSecond));
			//String myXmlDoc2 = noteList->createDocument(String());
			//Logger::writeToLog(myXmlDoc2);

			//String myXmlDoc = noteOffList->createDocument(String());
			//Logger::writeToLog(myXmlDoc);


		}
		Log("File " + fileToRead.getFileName() + " imported.", Topiary::LogType::Info);

		if ((num != numerator) || (den != denominator))
			Log("Time signature in file is different from plugin timesignature; results may be unexpected!", Topiary::LogType::Warning);

		return true;
	}; // loadMidiDrumPattern

	//////////////////////////////////////////////////////////////////////////////////////////////////
	// load & save stuff stuff	
	//////////////////////////////////////////////////////////////////////////////////////////////////

	void addIntToModel(XmlElement *p, int i, char* iname)
	{
		auto parameter = new XmlElement("Parameter");
		parameter->setAttribute("Name", iname);
		parameter->setAttribute("Value", i);
		p->addChildElement(parameter);

	} //addIntToModel

	//////////////////////////////////////////////////////////////////////////////////////////////////

	void addIntToModel(XmlElement *p, int i, char* iname, int index)
	{
		auto parameter = new XmlElement("Parameter");
		parameter->setAttribute("Name", iname);
		parameter->setAttribute("Value", i);
		parameter->setAttribute("Index", index);
		p->addChildElement(parameter);

	} // addIntToModel

	//////////////////////////////////////////////////////////////////////////////////////////////////

	void addBoolToModel(XmlElement *p, bool b, char* bname)
	{
		auto parameter = new XmlElement("Parameter");
		parameter->setAttribute("Name", bname);
		parameter->setAttribute("Value", (int)b);
		p->addChildElement(parameter);

	} // addBoolToModel

	//////////////////////////////////////////////////////////////////////////////////////////////////

	void addBoolToModel(XmlElement *p, bool b, char* bname, int index)
	{
		auto parameter = new XmlElement("Parameter");
		parameter->setAttribute("Name", bname);
		parameter->setAttribute("Value", (int)b);
		parameter->setAttribute("Index", index);
		p->addChildElement(parameter);

	} // addBoolToModel

	//////////////////////////////////////////////////////////////////////////////////////////////////

	void addStringToModel(XmlElement *p, String value, char* sname)
	{
		auto parameter = new XmlElement("Parameter");
		parameter->setAttribute("Name", sname);
		parameter->setAttribute("Value", value);
		p->addChildElement(parameter);

	} // addStringToModel

	//////////////////////////////////////////////////////////////////////////////////////////////////

	void addStringToModel(XmlElement *p, String value, char* sname, int index)
	{
		auto parameter = new XmlElement("Parameter");
		parameter->setAttribute("Name", sname);
		parameter->setAttribute("Value", value);
		parameter->setAttribute("Index", index);
		p->addChildElement(parameter);

	} // addStringToModel

	//////////////////////////////////////////////////////////////////////////////////////////////////

	void addParametersToModel()
	{
		auto parameters = new XmlElement("Parameters");
		model->addChildElement(parameters);

		addStringToModel(parameters, name, "name");
		addIntToModel(parameters, BPM, "BPM");
		addIntToModel(parameters, numerator, "numerator");
		addIntToModel(parameters, denominator, "denominator");
		addIntToModel(parameters, variationSelected, "variationSelected");
		addIntToModel(parameters, switchVariation, "switchVariation");
		addIntToModel(parameters, runStopQ, "runStopQ");
		addIntToModel(parameters, variationStartQ, "variationStartQ");
		addBoolToModel(parameters, WFFN, "WFFN");
		addStringToModel(parameters, name, "name");
		addBoolToModel(parameters, overrideHostTransport, "overrideHostTransport");
		addBoolToModel(parameters, notePassThrough, "notePassThrough");

		addBoolToModel(parameters, logMidiIn, "logMidiIn");
		addBoolToModel(parameters, logMidiOut, "logMidiOut");
		addBoolToModel(parameters, logDebug, "logDebug");
		addBoolToModel(parameters, logTransport, "logTransport");
		addBoolToModel(parameters, logVariations, "logVariations");
		addBoolToModel(parameters, logInfo, "logInfo");
		addStringToModel(parameters, filePath, "filePath");

		//addIntToModel(parameters, numPatterns, "numPatterns"); NOT SAVED BUT COUNTED
		addIntToModel(parameters, patternsRealID, "patternsRealID");
		addIntToModel(parameters, numPoolNotes, "numPoolNotes");
		addIntToModel(parameters, poolNotesRealID, "poolNotesRealId");

		addIntToModel(parameters, variationSwitchChannel, "variationSwitchChannel");
		addBoolToModel(parameters, ccVariationSwitching, "ccVariationSwitching");

		for (int i = 0; i < 8; i++) {
			// pattern variables
			addIntToModel(parameters, patternData[i].numNotes, "numNotes", i);
			addIntToModel(parameters, patternData[i].notesRealID, "notesRealID", i);
			addIntToModel(parameters, patternData[i].patLenInTicks, "patLenInTicks", i);

			// Variations
			addIntToModel(parameters, variation[i].lenInTicks, "lenInTicks", i);
			addIntToModel(parameters, variation[i].lenInMeasures, "lenInMeasures", i);
			addIntToModel(parameters, variation[i].patternToUse, "patternToUse", i);
			addStringToModel(parameters, variation[i].name, "variationName", i);
			addBoolToModel(parameters, variation[i].enabled, "variationEnabled", i);

			addBoolToModel(parameters, variation[i].enablePool[0], "enablePool0", i);
			addBoolToModel(parameters, variation[i].enablePool[1], "enablePool1", i);
			addBoolToModel(parameters, variation[i].enablePool[2], "enablePool2", i);
			addBoolToModel(parameters, variation[i].enablePool[3], "enablePool3", i);

			addIntToModel(parameters, variation[i].measureFrom[0], "measureFrom0", i);
			addIntToModel(parameters, variation[i].measureFrom[1], "measureFrom1", i);
			addIntToModel(parameters, variation[i].measureFrom[2], "measureFrom2", i);
			addIntToModel(parameters, variation[i].measureFrom[3], "measureFrom3", i);

			addIntToModel(parameters, variation[i].measureTo[0], "measureTo0", i);
			addIntToModel(parameters, variation[i].measureTo[1], "measureTo1", i);
			addIntToModel(parameters, variation[i].measureTo[2], "measureTo2", i);
			addIntToModel(parameters, variation[i].measureTo[3], "measureTo3", i);

			addIntToModel(parameters, variation[i].beatFrom[0], "beatFrom0", i);
			addIntToModel(parameters, variation[i].beatFrom[1], "beatFrom1", i);
			addIntToModel(parameters, variation[i].beatFrom[2], "beatFrom2", i);
			addIntToModel(parameters, variation[i].beatFrom[3], "beatFrom3", i);

			addIntToModel(parameters, variation[i].beatTo[0], "beatTo0", i);
			addIntToModel(parameters, variation[i].beatTo[1], "beatTo1", i);
			addIntToModel(parameters, variation[i].beatTo[2], "beatTo2", i);
			addIntToModel(parameters, variation[i].beatTo[3], "beatTo3", i);

			addIntToModel(parameters, variation[i].tickFrom[0], "tickFrom0", i);
			addIntToModel(parameters, variation[i].tickFrom[1], "tickFrom1", i);
			addIntToModel(parameters, variation[i].tickFrom[2], "tickFrom2", i);
			addIntToModel(parameters, variation[i].tickFrom[3], "tickFrom3", i);

			addIntToModel(parameters, variation[i].tickTo[0], "tickTo0", i);
			addIntToModel(parameters, variation[i].tickTo[1], "tickTo1", i);
			addIntToModel(parameters, variation[i].tickTo[2], "tickTo2", i);
			addIntToModel(parameters, variation[i].tickTo[3], "tickTo3", i);

			addIntToModel(parameters, variation[i].poolChannel[0], "poolChannel0", i);
			addIntToModel(parameters, variation[i].poolChannel[1], "poolChannel1", i);
			addIntToModel(parameters, variation[i].poolChannel[2], "poolChannel2", i);
			addIntToModel(parameters, variation[i].poolChannel[3], "poolChannel3", i);

			addBoolToModel(parameters, variation[i].randomizeNotes, "randomizeNotes", i);
			addIntToModel(parameters, variation[i].randomizeNotesValue, "randomizeNotesValue", i);
			addBoolToModel(parameters, variation[i].randomizeNotePool[0], "randomizeNotePool0", i);
			addBoolToModel(parameters, variation[i].randomizeNotePool[1], "randomizeNotePool1", i);
			addBoolToModel(parameters, variation[i].randomizeNotePool[2], "randomizeNotePool2", i);
			addBoolToModel(parameters, variation[i].randomizeNotePool[3], "randomizeNotePool3", i);

			addBoolToModel(parameters, variation[i].swing, "swing", i);
			addIntToModel(parameters, variation[i].swingValue, "swingValue", i);
			addBoolToModel(parameters, variation[i].swingPool[0], "swingPool0", i);
			addBoolToModel(parameters, variation[i].swingPool[1], "swingPool1", i);
			addBoolToModel(parameters, variation[i].swingPool[2], "swingPool2", i);
			addBoolToModel(parameters, variation[i].swingPool[3], "swingPool3", i);

			addBoolToModel(parameters, variation[i].randomizeVelocity, "randomizeVelocity", i);
			addIntToModel(parameters, variation[i].velocityValue, "velocityValue", i);
			addBoolToModel(parameters, variation[i].velocityPool[0], "velocityPool0", i);
			addBoolToModel(parameters, variation[i].velocityPool[1], "velocityPool1", i);
			addBoolToModel(parameters, variation[i].velocityPool[2], "velocityPool2", i);
			addBoolToModel(parameters, variation[i].velocityPool[3], "velocityPool3", i);
			addBoolToModel(parameters, variation[i].velocityPlus, "velocityPlus", i);
			addBoolToModel(parameters, variation[i].velocityMin, "velocityMin", i);

			addBoolToModel(parameters, variation[i].randomizeTiming, "randomizeTiming", i);
			addIntToModel(parameters, variation[i].timingValue, "timingValue", i);
			addBoolToModel(parameters, variation[i].timingPool[0], "timingPool0", i);
			addBoolToModel(parameters, variation[i].timingPool[1], "timingPool1", i);
			addBoolToModel(parameters, variation[i].timingPool[2], "timingPool2", i);
			addBoolToModel(parameters, variation[i].timingPool[3], "timingPool3", i);
			addBoolToModel(parameters, variation[i].timingPlus, "timingPlus", i);
			addBoolToModel(parameters, variation[i].timingMin, "timingMin", i);

			// automation
			addIntToModel(parameters, variationSwitch[i], "variationSwitch", i);
		}

	} // addParametersToModel

	//////////////////////////////////////////////////////////////////////////////////////////////////

	void restoreParametersToModel()
	{ 
		// model now has a child "Parameters"; set all non-XML parameters to their new values
		overrideHostTransport = true; // otherwise we might get very weird effects if the host were running
		setRunState(Topiary::Stopped);

		// preliminary housekeeping
		// restore some of the pointers
		auto child = model->getFirstChildElement();
		patternHeader = child;
		child = child->getNextElement();
		patternListHeader = child;
		child = child->getNextElement();
		patternListData = child;
		child = child->getNextElement();
		poolListData = child;
		child = child->getNextElement();
		poolListHeader = child;
		child = child->getNextElement();
		for (int i = 0; i < 8; i++)
		{
			patternData[i].noteData = child;
			child = child->getNextElement();
		}

		bool rememberOverride = true; // we do not want to set that right away!

		{
			while (child != nullptr)
			{
				String tagName = child->getTagName();

				if (tagName.compare("Parameters") == 0)
				{
					auto parameter = child->getFirstChildElement();
					while (parameter != nullptr)
					{
						String parameterName = parameter->getStringAttribute("Name");
						if (parameterName.compare("name") == 0)	name = parameter->getStringAttribute("Value");
						if (parameterName.compare("BPM") == 0) BPM = parameter->getIntAttribute("Value");
						if (parameterName.compare("numerator") == 0) numerator = parameter->getIntAttribute("Value");
						if (parameterName.compare("overrideHostTransport") == 0) rememberOverride = (bool)parameter->getIntAttribute("Value");
						if (parameterName.compare("denominator") == 0) denominator = parameter->getIntAttribute("Value");
						if (parameterName.compare("variationSelected") == 0)
						{
							variationSelected = parameter->getIntAttribute("Value");
							variationRunning = variationSelected;
						}

						if (parameterName.compare("switchVariation") == 0) switchVariation = parameter->getIntAttribute("Value");
						if (parameterName.compare("runStopQ") == 0) runStopQ = parameter->getIntAttribute("Value");
						if (parameterName.compare("variationStartQ") == 0) variationStartQ = parameter->getIntAttribute("Value");
						if (parameterName.compare("name") == 0) name = parameter->getStringAttribute("Value");

						if (parameterName.compare("WFFN") == 0)	WFFN = parameter->getBoolAttribute("Value");
						if (parameterName.compare("notePassThrough") == 0) 	notePassThrough = (bool)parameter->getIntAttribute("Value");

						if (parameterName.compare("logMidiIn") == 0) logMidiIn = parameter->getBoolAttribute("Value");
						if (parameterName.compare("logMidiOut") == 0)	logMidiOut = parameter->getBoolAttribute("Value");
						if (parameterName.compare("logDebug") == 0)	logDebug = parameter->getBoolAttribute("Value");
						if (parameterName.compare("logTransport") == 0)	logTransport = parameter->getBoolAttribute("Value");
						if (parameterName.compare("logVariations") == 0) logVariations = parameter->getBoolAttribute("Value");
						if (parameterName.compare("logInfo") == 0)	logInfo = parameter->getBoolAttribute("Value");

						if (parameterName.compare("filePath") == 0)	filePath = parameter->getStringAttribute("Value");

						//if (parameterName.compare("numPatterns") == 0) numPatterns = parameter->getIntAttribute("Value");
						if (parameterName.compare("patternsRealID") == 0) patternsRealID = parameter->getIntAttribute("Value");
						if (parameterName.compare("numPoolNotes") == 0) numPoolNotes = parameter->getIntAttribute("Value");
						if (parameterName.compare("poolNotesRealID") == 0) poolNotesRealID = parameter->getIntAttribute("Value");

						if (parameterName.compare("numNotes") == 0)  patternData[parameter->getIntAttribute("Index")].numNotes = parameter->getIntAttribute("Value");
						if (parameterName.compare("notesRealID") == 0)  patternData[parameter->getIntAttribute("Index")].notesRealID = parameter->getIntAttribute("Value");

						if (parameterName.compare("lenInTicks") == 0) variation[parameter->getIntAttribute("Index")].lenInTicks = parameter->getIntAttribute("Value");
						if (parameterName.compare("lenInMeasures") == 0) variation[parameter->getIntAttribute("Index")].lenInMeasures = parameter->getIntAttribute("Value");
						if (parameterName.compare("patternToUse") == 0) variation[parameter->getIntAttribute("Index")].patternToUse = parameter->getIntAttribute("Value");
						if (parameterName.compare("variationEnabled") == 0) variation[parameter->getIntAttribute("Index")].enabled = parameter->getBoolAttribute("Value");

						if (parameterName.compare("enablePool0") == 0) variation[parameter->getIntAttribute("Index")].enablePool[0] = parameter->getBoolAttribute("Value");
						if (parameterName.compare("enablePool1") == 0) variation[parameter->getIntAttribute("Index")].enablePool[1] = parameter->getBoolAttribute("Value");
						if (parameterName.compare("enablePool2") == 0) variation[parameter->getIntAttribute("Index")].enablePool[2] = parameter->getBoolAttribute("Value");
						if (parameterName.compare("enablePool3") == 0) variation[parameter->getIntAttribute("Index")].enablePool[3] = parameter->getBoolAttribute("Value");

						if (parameterName.compare("poolChannel0") == 0) variation[parameter->getIntAttribute("Index")].poolChannel[0] = parameter->getIntAttribute("Value");
						if (parameterName.compare("poolChannel1") == 0) variation[parameter->getIntAttribute("Index")].poolChannel[1] = parameter->getIntAttribute("Value");
						if (parameterName.compare("poolChannel2") == 0) variation[parameter->getIntAttribute("Index")].poolChannel[2] = parameter->getIntAttribute("Value");
						if (parameterName.compare("poolChannel3") == 0) variation[parameter->getIntAttribute("Index")].poolChannel[3] = parameter->getIntAttribute("Value");

						if (parameterName.compare("variationName") == 0) variation[parameter->getIntAttribute("Index")].name = parameter->getStringAttribute("Value");

						if (parameterName.compare("measureFrom0") == 0) variation[parameter->getIntAttribute("Index")].measureFrom[0] = parameter->getIntAttribute("Value");
						if (parameterName.compare("measureFrom1") == 0) variation[parameter->getIntAttribute("Index")].measureFrom[1] = parameter->getIntAttribute("Value");
						if (parameterName.compare("measureFrom2") == 0) variation[parameter->getIntAttribute("Index")].measureFrom[2] = parameter->getIntAttribute("Value");
						if (parameterName.compare("measureFrom3") == 0) variation[parameter->getIntAttribute("Index")].measureFrom[3] = parameter->getIntAttribute("Value");

						if (parameterName.compare("measureTo0") == 0) variation[parameter->getIntAttribute("Index")].measureTo[0] = parameter->getIntAttribute("Value");
						if (parameterName.compare("measureTo1") == 0) variation[parameter->getIntAttribute("Index")].measureTo[1] = parameter->getIntAttribute("Value");
						if (parameterName.compare("measureTo2") == 0) variation[parameter->getIntAttribute("Index")].measureTo[2] = parameter->getIntAttribute("Value");
						if (parameterName.compare("measureTo3") == 0) variation[parameter->getIntAttribute("Index")].measureTo[3] = parameter->getIntAttribute("Value");

						if (parameterName.compare("beatFrom0") == 0) variation[parameter->getIntAttribute("Index")].beatFrom[0] = parameter->getIntAttribute("Value");
						if (parameterName.compare("beatFrom1") == 0) variation[parameter->getIntAttribute("Index")].beatFrom[1] = parameter->getIntAttribute("Value");
						if (parameterName.compare("beatFrom2") == 0) variation[parameter->getIntAttribute("Index")].beatFrom[2] = parameter->getIntAttribute("Value");
						if (parameterName.compare("beatFrom3") == 0) variation[parameter->getIntAttribute("Index")].beatFrom[3] = parameter->getIntAttribute("Value");

						if (parameterName.compare("beatTo0") == 0) variation[parameter->getIntAttribute("Index")].beatTo[0] = parameter->getIntAttribute("Value");
						if (parameterName.compare("beatTo1") == 0) variation[parameter->getIntAttribute("Index")].beatTo[1] = parameter->getIntAttribute("Value");
						if (parameterName.compare("beatTo2") == 0) variation[parameter->getIntAttribute("Index")].beatTo[2] = parameter->getIntAttribute("Value");
						if (parameterName.compare("beatTo3") == 0) variation[parameter->getIntAttribute("Index")].beatTo[3] = parameter->getIntAttribute("Value");

						if (parameterName.compare("tickFrom0") == 0) variation[parameter->getIntAttribute("Index")].tickFrom[0] = parameter->getIntAttribute("Value");
						if (parameterName.compare("tickFrom1") == 0) variation[parameter->getIntAttribute("Index")].tickFrom[1] = parameter->getIntAttribute("Value");
						if (parameterName.compare("tickFrom2") == 0) variation[parameter->getIntAttribute("Index")].tickFrom[2] = parameter->getIntAttribute("Value");
						if (parameterName.compare("tickFrom3") == 0) variation[parameter->getIntAttribute("Index")].tickFrom[3] = parameter->getIntAttribute("Value");

						if (parameterName.compare("tickTo0") == 0) variation[parameter->getIntAttribute("Index")].tickTo[0] = parameter->getIntAttribute("Value");
						if (parameterName.compare("tickTo1") == 0) variation[parameter->getIntAttribute("Index")].tickTo[1] = parameter->getIntAttribute("Value");
						if (parameterName.compare("tickTo2") == 0) variation[parameter->getIntAttribute("Index")].tickTo[2] = parameter->getIntAttribute("Value");
						if (parameterName.compare("tickTo3") == 0) variation[parameter->getIntAttribute("Index")].tickTo[3] = parameter->getIntAttribute("Value");

						if (parameterName.compare("randomizeNotes") == 0) variation[parameter->getIntAttribute("Index")].randomizeNotes = parameter->getBoolAttribute("Value");
						if (parameterName.compare("randomizeNotesValue") == 0) variation[parameter->getIntAttribute("Index")].randomizeNotesValue = parameter->getIntAttribute("Value");
						if (parameterName.compare("randomizeNotePool0") == 0) variation[parameter->getIntAttribute("Index")].randomizeNotePool[0] = parameter->getBoolAttribute("Value");
						if (parameterName.compare("randomizeNotePool1") == 0) variation[parameter->getIntAttribute("Index")].randomizeNotePool[1] = parameter->getBoolAttribute("Value");
						if (parameterName.compare("randomizeNotePool2") == 0) variation[parameter->getIntAttribute("Index")].randomizeNotePool[2] = parameter->getBoolAttribute("Value");
						if (parameterName.compare("randomizeNotePool3") == 0) variation[parameter->getIntAttribute("Index")].randomizeNotePool[3] = parameter->getBoolAttribute("Value");

						if (parameterName.compare("swing") == 0) variation[parameter->getIntAttribute("Index")].swing = parameter->getBoolAttribute("Value");
						if (parameterName.compare("swingValue") == 0) variation[parameter->getIntAttribute("Index")].swingValue = parameter->getIntAttribute("Value");
						if (parameterName.compare("swingPool0") == 0) variation[parameter->getIntAttribute("Index")].swingPool[0] = parameter->getBoolAttribute("Value");
						if (parameterName.compare("swingPool1") == 0) variation[parameter->getIntAttribute("Index")].swingPool[1] = parameter->getBoolAttribute("Value");
						if (parameterName.compare("swingPool2") == 0) variation[parameter->getIntAttribute("Index")].swingPool[2] = parameter->getBoolAttribute("Value");
						if (parameterName.compare("swingPool3") == 0) variation[parameter->getIntAttribute("Index")].swingPool[3] = parameter->getBoolAttribute("Value");

						if (parameterName.compare("randomizeVelocity") == 0) variation[parameter->getIntAttribute("Index")].randomizeVelocity = parameter->getBoolAttribute("Value");
						if (parameterName.compare("velocityValue") == 0) variation[parameter->getIntAttribute("Index")].velocityValue = parameter->getIntAttribute("Value");
						if (parameterName.compare("velocityPool0") == 0) variation[parameter->getIntAttribute("Index")].velocityPool[0] = parameter->getBoolAttribute("Value");
						if (parameterName.compare("velocityPool1") == 0) variation[parameter->getIntAttribute("Index")].velocityPool[1] = parameter->getBoolAttribute("Value");
						if (parameterName.compare("velocityPool2") == 0) variation[parameter->getIntAttribute("Index")].velocityPool[2] = parameter->getBoolAttribute("Value");
						if (parameterName.compare("velocityPool3") == 0) variation[parameter->getIntAttribute("Index")].velocityPool[3] = parameter->getBoolAttribute("Value");
						if (parameterName.compare("velocityPlus") == 0) variation[parameter->getIntAttribute("Index")].velocityPlus = parameter->getBoolAttribute("Value");
						if (parameterName.compare("velocityMin") == 0) variation[parameter->getIntAttribute("Index")].velocityMin = parameter->getBoolAttribute("Value");

						if (parameterName.compare("randomizeTiming") == 0) variation[parameter->getIntAttribute("Index")].randomizeTiming = parameter->getBoolAttribute("Value");
						if (parameterName.compare("timingValue") == 0) variation[parameter->getIntAttribute("Index")].timingValue = parameter->getIntAttribute("Value");
						if (parameterName.compare("timingPool0") == 0) variation[parameter->getIntAttribute("Index")].timingPool[0] = parameter->getBoolAttribute("Value");
						if (parameterName.compare("timingPool1") == 0) variation[parameter->getIntAttribute("Index")].timingPool[1] = parameter->getBoolAttribute("Value");
						if (parameterName.compare("timingPool2") == 0) variation[parameter->getIntAttribute("Index")].timingPool[2] = parameter->getBoolAttribute("Value");
						if (parameterName.compare("timingPool3") == 0) variation[parameter->getIntAttribute("Index")].timingPool[3] = parameter->getBoolAttribute("Value");
						if (parameterName.compare("timingPlus") == 0) variation[parameter->getIntAttribute("Index")].timingPlus = parameter->getBoolAttribute("Value");
						if (parameterName.compare("timingMin") == 0) variation[parameter->getIntAttribute("Index")].timingMin = parameter->getBoolAttribute("Value");

						// patterndata array variables
						if (parameterName.compare("patLenInTicks") == 0)  patternData[parameter->getIntAttribute("Index")].patLenInTicks = parameter->getIntAttribute("Value");
						if (parameterName.compare("numNotes") == 0)  patternData[parameter->getIntAttribute("Index")].numNotes = parameter->getIntAttribute("Value");
						if (parameterName.compare("notesRealID") == 0)  patternData[parameter->getIntAttribute("Index")].notesRealID = parameter->getIntAttribute("Value");

						// automation
						if (parameterName.compare("variationSwitch") == 0)  variationSwitch[parameter->getIntAttribute("Index")] = parameter->getIntAttribute("Value");
						if (parameterName.compare("ccVariationSwitching") == 0)  ccVariationSwitching = (bool)parameter->getIntAttribute("Value");
						if (parameterName.compare("variationSwitchChannel") == 0)  variationSwitchChannel = parameter->getIntAttribute("Value");

						parameter = parameter->getNextElement();
					}
					break;
				}
				if (tagName.compare("PatternHeader") == 0)
					patternHeader = child;
				if (tagName.compare("PatternListHeader") == 0)
					patternListHeader = child;
				if (tagName.compare("PatternListData") == 0)
					patternListData = child;
				if (tagName.compare("PoolListData") == 0)
					poolListData = child;
				if (tagName.compare("PoolListHeader") == 0)
					poolListHeader = child;

				if (tagName.compare("Pattern") == 0) patternData[child->getIntAttribute("Index")].noteData = child;

				//if (tagName.compare("ShadowPatternOn") == 0) variation[child->getIntAttribute("Index")].shadowPatternOn = child;  LEAK
				//if (tagName.compare("ShadowPatternOff") == 0) variation[child->getIntAttribute("Index")].shadowPatternOff = child;

				child = child->getNextElement();

			} // foreach parameters

			// do some housekeeping to set variables always contained in the model object

			// if there are no patterns; all variations need to be disabled!!!

			numPatterns = patternListData->getNumChildElements(); // these are not saved but counted; ALSO DO THIS WHERE THIS GETS CALLED (not sure which part of XML processed when!!!

			if (numPatterns == 0)
			{
				for (int i = 0; i < 8; i++)
					variation[i].enabled = false;
			}

			for (int i = 0; i < 8; i++)
			{
				// housekeeping auxciliary values for pools
				variation[i].lenInTicks = patternData[variation[i].patternToUse].patLenInTicks;

				for (int j = 0; j < 4; j++)
				{
					variation[i].fullTickFrom[j] = variation[i].tickFrom[j] + variation[i].beatFrom[j] * Topiary::TICKS_PER_QUARTER + numerator * variation[i].measureFrom[j] * Topiary::TICKS_PER_QUARTER;
					variation[i].fullTickTo[j] = variation[i].tickTo[j] + variation[i].beatTo[j] * Topiary::TICKS_PER_QUARTER + numerator * variation[i].measureTo[j] * Topiary::TICKS_PER_QUARTER;
				}

			}

			// now delete the no-longer-needed "Parameters" child
			model->deleteAllChildElementsWithTagName("Parameters");

		} // end block protected by lock

		setRunState(Topiary::Stopped);
		setOverrideHostTransport(rememberOverride);

		// generate the variations
		for (int v = 0; v < 8; v++)
			generateAllVariations();
		
		// inform editor
		broadcaster.sendActionMessage(MsgLoad); // tell everyone we've just loaded something (table headers need to be re-set
		broadcaster.sendActionMessage(MsgTransport);
		broadcaster.sendActionMessage(MsgLog);
		broadcaster.sendActionMessage(MsgMaster);
		broadcaster.sendActionMessage(MsgVariationEnables);		// so that if needed variationbuttons are disabled/enabled
		broadcaster.sendActionMessage(MsgVariationDefinition);	// inform editor of variation settings;
		broadcaster.sendActionMessage(MsgVariationAutomation);	// inform editor of variation automation settings;	

	} // restoreParametersToModel

	//////////////////////////////////////////////////////////////////////////////////////////////////
	// Generator stuff	
	//////////////////////////////////////////////////////////////////////////////////////////////////

	void recalcRealTime()
	{ // housekeeping in case sampleRate and/or BPM changes
		// BPM/60 = beatPerSecond
		// lenght of 1 beat depends on denumerator (if 4 then beat == quarter)
		if (numerator == 0) return; // not initialized!
		if (denominator == 0) return;
		ticksPerBeat = Topiary::TICKS_PER_QUARTER * 4 / denominator;
		samplesPerTick = (double)sampleRate / ((double)ticksPerBeat * BPM / 60.0);
		Log("Samples per tick" + String(samplesPerTick), Topiary::LogType::Debug);

	} // recalcRealTime

	///////////////////////////////////////////////////////////////////////

	void calcMeasureBeat()
	{
		int64 cursorInTicks = (int64)floor(blockCursor / samplesPerTick);  // use BlockCursor instead of rtCursor as rtCursor might jump back & forth
		int newMeasure = (int)floor(cursorInTicks / (ticksPerBeat* denominator)) + 1;

		int newBeat = (int)floor(cursorInTicks / ticksPerBeat);
		newBeat = (newBeat % denominator) + 1;

		jassert((newMeasure > 0));
		jassert((newMeasure < 10000));

		if (beat != newBeat)
		{
			beat = newBeat;
			measure = newMeasure;
			broadcaster.sendActionMessage(MsgTiming);
		}
	} // calcMeasureBeat

	///////////////////////////////////////////////////////////////////////

	bool walkToTick(XmlElement* parent, XmlElement** child, int toTick)
	{ // find the first child on or after this tick, starting from current child; 
	  // caller has to make sure that child is child of parent, or pass nullptr to initialize
	  // return false if pattern is empty or nothing to do within the blockSize; if empty then child == nullptr; 
	  // This loops around the pattern! To avoid infinite loop, take blockSize into account; only loop over pattern
	  // if the amount of ticks walked is less than blocksize

		// DOES NOT TAKE LIMITED PATTERN LENGTH INTO ACCOUNT (yet) - we will do that in the variations later -- NO THE VARIATION WILL BE AS LONG AS THE BASE PATTERN

		int childTick;

		if (*child == nullptr)
		{
			*child = parent->getFirstChildElement(); // initialize
			if (*child == nullptr)
			{
				return false; // empty pattern!
			}
		}
		childTick = (*child)->getIntAttribute("Timestamp");

		while (childTick < toTick)
		{   // as long as our child is behind time we're looking for
			*child = (*child)->getNextElement();
			if (*child == nullptr) break; // there are no events afther the given time
			childTick = (*child)->getIntAttribute("Timestamp");
		}

		if (*child == nullptr)
		{
			*child = parent->getFirstChildElement();
			return true;
			/* we only return true if first child is < blockSize away from where we are now; actually conservative, can be blockSize/samplesPerTick !!!
			childTick = (*child)->getIntAttribute("Timestamp");
			if (childTick <= (blockSize / samplesPerTick)*2)
				return true;
			else
				return false; */

		}
		return true;
	} // walkToTick

	///////////////////////////////////////////////////////////////////////

	void nextTick(XmlElement* parent, XmlElement** child)
	{	// assert that parent has at least 1 child of each!!! (do a walk to a tick first!)
		// this one loops around the pattern

		*child = (*child)->getNextElement();
		if (*child == nullptr)
		{
			*child = parent->getFirstChildElement();
			//Logger::outputDebugString(String("-------- LOOPING OVER END OFF PATTERN ==================== "));
		}

	}  // nextTick

	///////////////////////////////////////////////////////////////////////

	void initializeVariation(int i)
	{
		// full initialize to nothing!!!
		variation[i].patternToUse = -1;				// index in patterndata
		
		// be sure to keep the actual pattern here!!! if ever changed it will get overwritten by generateVariation

		for (int j = 0; j < 4; j++)
		{
			variation[i].enablePool[j] = true;
			variation[i].measureTo[j] = -1;
			variation[i].beatFrom[j] = -1;
			variation[i].beatTo[j] = -1;
			variation[i].tickFrom[j] = -1;
			variation[i].tickTo[j] = -1;
			variation[i].poolIdCursor[j] = 1;
			variation[i].poolTickCursor[j] = 0;
		}
		variation[i].lenInMeasures = 0;
		variation[i].lenInTicks = 0;
		variation[i].ending = false;				// indicates that once pattern played, we no longer generate notes! (but we keep running (status Ended) till done

		variation[i].name = "Variation " + String(i + 1);
		variation[i].enabled = false;
		for (int j = 0; j < 4; j++)
		{
			if (j)  // always initialize pool1 as enabled
				variation[i].enablePool[j] = false;
			else
				variation[i].enablePool[j] = true;
			variation[i].poolChannel[j] = 10;
			variation[i].measureFrom[j] = 0;
			variation[i].measureTo[j] = -1;
			variation[i].beatFrom[j] = 0;
			variation[i].beatTo[j] = -1;
			variation[i].tickFrom[j] = 0;
			variation[i].tickTo[j] = -1;
		}
	} // initializeVariation

	///////////////////////////////////////////////////////////////////////

	void initializeVariations()
	{
		// called as soon as first pattern is added; initializes all variations to use that pattern
		for (int i = 0; i < 8; ++i)
		{
			initializeVariation(i);
		}
	} // initializeVariations

	///////////////////////////////////////////////////////////////////////

	void initializePatternToVariation(int p, int v)
	{
		// to call when setting a pattern, 
		// uses indexes in the data structures; not variation or pattern numbers!
		// if pattern number == -1 disable the variation

		if (p == -1) variation[v].enabled = false;

		variation[v].patternToUse = p;
		for (int i = 0; i < 4; i++)
		{
			variation[v].measureFrom[i] = 0;
			variation[v].beatFrom[i] = 0;
			variation[v].tickFrom[i] = 0;
			variation[v].measureTo[i] = -1;
			variation[v].beatTo[i] = 0;
			variation[v].tickTo[i] = 0;
		}

		// don't forget to set the length!!!
		
		if (p != -1)
		{
			variation[v].lenInMeasures = getPatternLengthInMeasures(p + 1);
			variation[v].lenInTicks = patternData[variation[v].patternToUse].patLenInTicks;
		}

	} // initializePatternToVariations

	///////////////////////////////////////////////////////////////////////

	void generatePool(int v, int p, XmlElement *newPatternOn, int poolNote[128])
	{
		// generates the pool notes in poop p for variation v
		// v is variation to generate
		// p is poolnumber

		Random randomizer;

		XmlElement* newChild = nullptr;
		XmlElement* patternOn = patternData[variation[v].patternToUse].noteData; ///
		int patLenInTicks = variation[v].lenInTicks;

		XmlElement* patternChild;
		int previousTick;
		int note;
		bool firstNoteGenerated;

		// if NOT RUNNING - then the value of poolTickCursor is not reliable; because the 
		// first note in the pool may not be at time 0, and it may not be the first note at all!!! 
		// In that case set the poolTickCursor to the timestamp of the first note generated!!!
		// Assumes that at run start we do not regenerate the variations!!!

		if (runState == Topiary::Running) firstNoteGenerated = true;
		else firstNoteGenerated = false;

		if (!variation[v].enabled) return; // might not be decently initialized!!!
		if (!variation[v].enablePool[p]) return; // might not be decently initialized!!!

		//Logger::writeToLog("------------------- Start generation of pool " + String(p));

		// find poolIDCursor
		patternChild = patternOn->getFirstChildElement();
		if (patternChild == nullptr) return; // empty variation

		while (patternChild->getIntAttribute("ID") != variation[v].poolIdCursor[p])
		{
			//Logger::outputDebugString("ID =" + patternChild->getStringAttribute("ID") + " -- look for -- " + String(variation[v].poolIdCursor[p]));
			patternChild = patternChild->getNextElement();
		}

		
		jassert(variation[v].poolTickCursor[p] >= 0);
		while (variation[v].poolTickCursor[p] < patLenInTicks)
		{
			// careful: the ID we walk to may not be in this notepool (it might be the first ID that made the previous run loop over the patternlength
			// so check before generating the note !!!

			note = patternChild->getIntAttribute("Note");
			if (poolNote[note] == p)
			{
				// note randomization logic
				bool doNote = true;

				if (variation[v].randomizeNotes && variation[v].randomizeNotePool[p])
				{
					float rnd = randomizer.nextFloat();
					// decide whether we're generating this one or not
					if (rnd > ((float) variation[v].randomizeNotesValue / 100))
					{
						doNote = false;
					}
				};

				if (doNote)
				{
					// need some decision logic to make sure we generate the child

					newChild = new XmlElement("DATA");
					newChild->setAttribute("ID", patternChild->getStringAttribute("ID"));
					newChild->setAttribute("Note", patternChild->getStringAttribute("Note"));
					newChild->setAttribute("Channel", variation[v].poolChannel[p]);
					newChild->setAttribute("Length", patternChild->getStringAttribute("Length"));

					if (variation[v].randomizeVelocity && (variation[v].velocityPlus || variation[v].velocityMin)  && variation[v].velocityPool[p])
					{
						int vel = patternChild->getIntAttribute("Velocity");
						float rnd;
						int direction;
						if (variation[v].velocityPlus && variation[v].velocityMin)
						{
							rnd = randomizer.nextFloat();
							if (rnd > 0.5)
								direction = 1;
							else
								direction = -1;
						}
						else if (variation[v].velocityPlus)
							direction = 1;
						else direction = -1;

						rnd = randomizer.nextFloat();
						//int debug = direction * rnd * 128 * variation[v].velocityValue /100;
						vel = vel + (int) (direction * rnd * 128 * variation[v].velocityValue/50);   // originally / 100 but I want more of an effect
						if (vel > 127) vel = 127;
						if (vel < 0) vel = 0;
						newChild->setAttribute("Velocity", vel);
					}
					else
						newChild->setAttribute("Velocity", patternChild->getStringAttribute("Velocity"));

					newChild->setAttribute("midiType", Topiary::MidiType::NoteOn);
					// timestamp is variation[v].poolTickCursor !!!!
					// except if first note generated; then it's the timestamp of the note in the pattern!

					if (firstNoteGenerated)
					{
						newChild->setAttribute("Timestamp", String(variation[v].poolTickCursor[p]));
						jassert(variation[v].poolTickCursor[p] >= 0);
						jassert(newChild->getIntAttribute("Timestamp") >= 0);
					}
					else
					{
						jassert(variation[v].poolTickCursor[p] >= 0);
						variation[v].poolTickCursor[p] = patternChild->getIntAttribute("Timestamp");
						jassert(variation[v].poolTickCursor[p] >= 0);
						newChild->setAttribute("Timestamp", String(variation[v].poolTickCursor[p]));
						firstNoteGenerated = true;
						jassert(newChild->getIntAttribute("Timestamp") >= 0);
					}

					if (variation[v].swing && variation[v].swingPool[p])
					{
						// recalc the timestamp, based on the swing lookup table
						int timestamp = newChild->getIntAttribute("Timestamp");

						//Logger::outputDebugString("Timestamp pre " + String(timestamp));

						int base = ((int)(timestamp / Topiary::TICKS_PER_QUARTER)) * Topiary::TICKS_PER_QUARTER;

						//Logger::outputDebugString("Remainder pre " + String(timestamp-base));

						int remainder = swing(timestamp - base, variation[v].swingValue);
						newChild->setAttribute("Timestamp", base + remainder);

						//Logger::outputDebugString("Remainder post" + String (remainder));
						//Logger::outputDebugString("New Timestamp " + String(base + remainder) + " (DIFF: " + String (timestamp-base-remainder));

						jassert((remainder) <= Topiary::TICKS_PER_QUARTER);
						jassert(newChild->getIntAttribute("Timestamp") >= 0);
						
					}

					// we apply timing randomization AFTER possible swing !!!

					if (variation[v].randomizeTiming && (variation[v].timingPlus || variation[v].timingMin) && variation[v].timingPool[p])
					{
						int timestamp = newChild->getIntAttribute("Timestamp");
						float rnd;
						int direction = -1;
						if (variation[v].velocityPlus & variation[v].velocityMin)
						{
							rnd = randomizer.nextFloat();
							if (rnd > 0.5)
								direction = 1;
						}
						else if (variation[v].velocityPlus)
							direction = 1;
													

						rnd = randomizer.nextFloat();
						//int debug = direction * rnd * Topiary::TICKS_PER_QUARTER * variation[v].timingValue /100;
						timestamp = timestamp + (int)(direction * rnd * Topiary::TICKS_PER_QUARTER * variation[v].timingValue / 800);
						if (timestamp < 0) timestamp = 0;

						if (timestamp > (variation->lenInTicks-1)) timestamp = variation->lenInTicks - 1; // lenInTicks -1 because we need time for the note off event

						// make sure we do not run over the patternlenght with note off
						int length = newChild->getIntAttribute("Length");
						if ((length + timestamp) >= variation->lenInTicks)
						{
							length = variation->lenInTicks - timestamp - 1;
							newChild->setAttribute("Length", length);
							jassert(length > 0);
						}

						newChild->setAttribute("Timestamp", timestamp);
					}


					// the following 3 are incorrect!!! these are what's in the pattern definition, but won't match tick value.  That's OK
					// because nobody gets to see these :)
					newChild->setAttribute("Measure", patternChild->getStringAttribute("Measure"));
					newChild->setAttribute("Beat", patternChild->getStringAttribute("Beat"));
					newChild->setAttribute("Tick", patternChild->getStringAttribute("Tick"));


					newPatternOn->prependChildElement(newChild);

					//Logger::outputDebugString("Generating Note " + String(note) + " timestamp " + String(variation[v].poolTickCursor[p]));
					jassert(variation[v].poolTickCursor[p] >= 0);
					jassert(newChild->getIntAttribute("Timestamp") >= 0);
				}
			} // end if note in this pool

			// remember the tick of the last generated note; tick in the source pattern!
			previousTick = patternChild->getIntAttribute("Timestamp");

			//debug
			//jassert(previousTick < 950);

			//Logger::outputDebugString(" looking for next note ");

			// now find the next child that's in the pool; loop because it may take a couple of tries; a loop without loops is empty and caught in the caller: generateVariation()

			do
			{
				patternChild = patternChild->getNextElement();

				if (patternChild == nullptr) // run over pattern end
				{
					//Logger::outputDebugString(" running over pattern end");
					variation[v].poolTickCursor[p] += (variation[v].lenInTicks - previousTick) + 1; // +1 because we will reset to start of pool/pattern (and that start is 1st tick in that pattern/pool)
					
					patternChild = patternOn->getFirstChildElement();
					previousTick = patternChild->getIntAttribute("Timestamp");  // because first note need not be at time 0 !!!
				}
				else
				{
					if (patternChild->getIntAttribute("Timestamp") > variation[v].fullTickTo[p]) // run over pool end
					{
						//Logger::outputDebugString(" running over pool end");
						variation[v].poolTickCursor[p] += (patternChild->getIntAttribute("Timestamp") - previousTick);
						previousTick = patternChild->getIntAttribute("Timestamp");
					}
					else
					{
						// normal case
						if (patternChild->getIntAttribute("Timestamp") < variation[v].fullTickFrom[p])  // we are before the start of the pool; just keep walking
						{
							Logger::outputDebugString(" looking for pool start; now at tick " + patternChild->getStringAttribute("Timestamp") + " and tickFrom " + String(variation[v].fullTickFrom[p]));

							variation[v].poolTickCursor[p] += (patternChild->getIntAttribute("Timestamp") - previousTick);
							previousTick = patternChild->getIntAttribute("Timestamp");
						}
						else
						{
							// just take the step; tested and OK!
							variation[v].poolTickCursor[p] += (patternChild->getIntAttribute("Timestamp") - previousTick);
							previousTick = patternChild->getIntAttribute("Timestamp");
						}
					}
				}
				
				note = patternChild->getIntAttribute("Note");

			} // do this as long as the note is not in this pool and the pattern is not completely filled (in length)
			while ((poolNote[note] != p) && (variation[v].poolTickCursor[p] < patLenInTicks));


		}  // while length is less than whole pattern

		variation[v].poolIdCursor[p] = patternChild->getIntAttribute("ID");

		// the pooltickCursor was already set!!! but it has run over the patternlength; reset it to remainder (so minus the patternlength)
		variation[v].poolTickCursor[p] = variation[v].poolTickCursor[p] - patLenInTicks;

	} // generatePool

	///////////////////////////////////////////////////////////////////////

	int swing(int value, int deviation)
	{ 
		// value between 0 - ticksPerBeat; deviation -100 to 100
		jassert(deviation < 101);
		jassert(deviation > -101);

		int maxMidiValue = Topiary::TICKS_PER_QUARTER-1;
		int midMidiValue = (int) (maxMidiValue / 2);

		// This is our control point for the quadratic bezier curve
		// We want this to be between 0 (min) and 63.5 (max)
		double dev = (double)deviation;
		double controlPointX = (double) midMidiValue + ((dev / 100) * midMidiValue);

		// Get the percent position of the incoming value in relation to the max
		double t = (double)value / maxMidiValue;

		// The quadratic bezier curve formula
		// B(t) = ((1 - t) * (1 - t) * p0) + (2 * (1 - t) * t * p1) + (t * t * p2)

		// t  = the position on the curve between (0 and 1)
		// p0 = minMidiValue (0)
		// p1 = controlPointX (the bezier control point)
		// p2 = maxMidiValue (127)

		// Formula can now be simplified as:
		// B(t) = ((1 - t) * (1 - t) * minMidiValue) + (2 * (1 - t) * t * controlPointX) + (t * t * maxMidiValue)

		// What is the deviation from our value?
		
		int delta = (int) round((2.0 * (1 - t) * t * controlPointX) + (t * t * (double)maxMidiValue));

		//Logger::outputDebugString("swing delta: " + String(delta));
		return (value - delta) + value;
	
	} // swing

	///////////////////////////////////////////////////////////////////////

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(TopiaryBeatsModel)
};

