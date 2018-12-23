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


#pragma once
#include "../JuceLibraryCode/JuceHeader.h"
#include "../../Topiary/Topiary.h"

class TopiaryBeatsModel
{
public:
	TopiaryBeatsModel();
	~TopiaryBeatsModel();

	String name = "New Beats Pattern"; // name of this preset

	void getMasterModel(XmlElement** h, XmlElement** d, XmlElement** ph, XmlElement** pd);
	void getPatternModel(int patternIndex, XmlElement** h, XmlElement** d);
	int getNumPatterns();
	void getPatterns(String pats[8]);
	int getPatternLengthInMeasures(int i);  // needed in variationComponent for validating pool length values;
	void deletePattern(int i);
	void addPattern();
	void addPool();
	void deletePool(int i);
	bool insertPatternFromFile(int patternIndex);
	XmlElement* addToModel(char* type);
	XmlElement* addToModel(char *type, int index);
	void removeFromModel(char *type, XmlElement *child);
	void rebuildPool();
	void setGMDrumMapLabels();

	void savePreset(File f);
	void loadPreset(File f);
	void saveStateToMemoryBlock(MemoryBlock& destData);
	void restoreStateFromMemoryBlock(const void* data, int sizeInBytes);

	////// Logger 
	String* getBeatsLog();
	String getLastWarning();
	void beatsLog(String s, int logType);
	void setBeatsLogSettings(bool warning, bool midiIn, bool midiOut, bool debug, bool transport, bool variations, bool info);
	void clearBeatsLog();
	void getBeatsLogSettings(bool& warning, bool& midiIn, bool& midiOut, bool& debug, bool &transport, bool &variations, bool &info);
	void getMidiLogSettings(bool &in, bool &out);
	void logMidi(bool in, MidiMessage &msg); // in denotes msg in or msg out

	////// Transport

	void setBPM(int bpm);
	void setNumeratorDenominator(int n, int d);
	void getTransportState(int& b, int& n, int& d, int& bs, bool& o, bool &waitFFN);
	void setOverrideHostTransport(bool o);
	void setRunState(int n);
	int getRunState();
	void processTransportControls(int buttonEnabled); // // buttonEnabled; 0=Stop; 1=start; 2=rec


	////// Variations

	void setVariation(int i);
	void getVariation(int& running, int& selected);
	void getVariationEnables(bool enables[8]);

	void getVariationDefinition(int i, bool& enabled, String& vname, int& patternId, bool enables[4], int poolLength[4][3][2], int poolChannel[4]);   // pass variation Definition on to VariationComponent in editor
	void setVariationDefinition(int i, bool enabled, String vname, int patternId, bool enables[4], int poolLength[4][3][2], int poolChannel[4]);      // set variation Definition parameters from editor; return false if we try to disable the last enabled variation
	bool validateVariationDefinition(int i, bool enable, String vname, int patternId, int poolLength[4][3][2]);
	void generateVariation(int i); // Generates the variation; at the end , swaps it in (using spinlock)

	struct Variation {

		int patternToUse;				// index in patterndata
		int lenInTicks;
		int lenInMeasures;
		XmlElement* patternOn;			// pattern On note events - points to patterndata!!!
		XmlElement* patternOff;			// pattern Off note events
		XmlElement* currentPatternChild;    // where we are in generation - not in patterns but in shadowpatterns!
		XmlElement* currentPatternChildOff; // where we are in generation - not in patterns but in shadowpatterns!

		bool ending;					// indicates that once pattern played, we no longer generate notes! (but we keep running (status Ended)
		//XmlElement *shadowPatternOn;	// shadow pattern On note events
		//XmlElement *shadowPatternOff;	// shadow pattern Off note events - copy of patterndata, so holds its own elements!!! (which gets destroyed when this gets destroyed)

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

		// lots more to come!
	};

	// Generator stuff

	void setSampleRate(double sr);
	void setBlockSize(int blocksz);
	void setStartTimes();
	void generateMidi(MidiBuffer* midiBuffer);

	bool processEnding();
	bool processVariationSwitch();
	void endNotesOn(MidiBuffer* midiBuffer); // send noteOff events for all notes and empty noteOnSet
	bool switchingVariations();

	void setWFFN(bool wffn);
	bool getWFFN();
	void setNotePassThrough(bool echo);
	bool getNotePassThrough();
	void setSwitchVariation(int a);
	int getSwitchVariation();
	void setVariationStartQ(int q);
	int getVariationStartQ();
	void setRunStopQ(int q);
	int getRunStopQ();
	String getName();
	void setName(String n);
	void getTime(int& b, int& m);

	////////// Broadcasters & Listeners

	void setListener(ActionListener *listener);
	void removeListener(ActionListener *listener);
	void sendActionMessage(String s);  // needed because the tables are updated in the editor, and the tables resort the data !!!
	ActionBroadcaster* getBroadcaster();

	// broadcaster messages
#define MsgTransport "t"
#define MsgLog "l"
#define MsgMasterTables "m"
#define MsgVariationDefinition "d"
#define MsgVariationEnables "e"
#define MsgVariationAutomation "a"  // variationcontrol
#define MsgVariationSelected "s"
#define MsgWarning "w"
#define MsgPattern "p"
#define MsgTiming "T"

	////////// Automation

	void setVariationControl(bool ccSwitching, int channel, int switches[8]);
	void getVariationControl(bool& ccSwitching, int& channel, int switches[8]);
	void processAutomation(MidiMessage& msg);

#define NUMBEROFQANTIZERS 10

private:

	SpinLock lockModel;

	ActionBroadcaster broadcaster;

	XmlElement *patternListHeader = new XmlElement("PatternListHeader");
	XmlElement *patternListData = new XmlElement("PatternListData");

	XmlElement *poolListHeader = new XmlElement("PoolListHeader");
	XmlElement *poolListData = new XmlElement("PoolListData");

	XmlElement *patternHeader = new XmlElement("PatternHeader");
	struct Pattern
	{
		XmlElement *noteData;
		XmlElement *noteOffData;
		int numNotes = 0;
		int notesRealID = 1;
		int patLenInTicks = 0;
	} patternData[8];

	int numPatterns = 0;
	int patternsRealID = 1;
	int numPoolNotes = 0;
	int poolNotesRealID = 1;

	std::unique_ptr<XmlElement> model;

	/////////// Logger 

	String logString;
	String lastWarning;

	bool logWarning = true;
	bool logMidiIn = false;
	bool logMidiOut = false;
	bool logDebug = false;
	bool logTransport = true;
	bool logVariations = true;
	bool logInfo = true;

	/////////// Transport

	bool overrideHostTransport;
	int denominator = 0; // b in a/b
	int numerator = 0;  // a in a/b
	int BPM = 120;
	int runState;

	/////////// Variations

	int variationSelected = 1;
	int variationRunning = 1; // this may differ from variationSelected during switch trigger time !!!

	Variation variation[8];  // struct to hold variation detail

	/////////// Generator variables

	double sampleRate, ticksPerBeat, samplesPerTick;  // housekeeping in recalcRealTime() !

	int64 blockCursor;					// sampletime of start of current block
	int64 nextRTGenerationCursor;		// real time cursor of next event to generate
	int blockSize;						// size of block to generate 
	int patternCursorOn;				// ticks within the variation/pattern for next note on
	int patternCursorOff;				// ticks within the variation/pattern for next note off

	int variationStartQ = 100;			// when to switch variations
	int runStopQ = 100;					// when to stop running
	bool WFFN = false;					// start at first note in if true; otherwize immediate
	int switchVariation = 1;			// when new pattern starts, it always starts at beginning - enumerator
										// if not, we try to stay within same measure/beat as the running pattern
										// if the running pattern is shorter than the new one, we start within the first measure
	bool notePassThrough = false;		// echo incoming notes out
	int measure = 0;					// keep track of where we are in generation
	int beat = 0;						// keep track of where we are in generation
	int tick = 0;						// keep track of where we are in generation
	int64 cursorToStop = -1;			// used when running, set once, to cursor where we should stop; needs to be set to -1 when running stops (typically in processEnding() but also when arming 

	//SortedSet<int> noteOnSet;			// keep track of which notes are still on

	/////////////// Automation

	int variationSwitch[8];     // either notes for each variation, of CC numbers
	bool ccVariationSwitching;  // if false then we're using notes
	int variationSwitchChannel; // midi channel for variation switching; 0 == omni

	String filePath;

	/////////////////////////////////////////////////////////////////////////

	class DataSorter
	{
	public:
		DataSorter(const String& attributeToSortBy, bool forwards)
			: attributeToSort(attributeToSortBy),
			direction(forwards ? 1 : -1)
		{}

		int compareElements(XmlElement* first, XmlElement* second) const
		{
			auto result = first->getStringAttribute(attributeToSort)
				.compareNatural(second->getStringAttribute(attributeToSort));

			if (result == 0)
				result = first->getStringAttribute("ID")
				.compareNatural(second->getStringAttribute("ID"));

			return direction * result;
		}

	private:
		String attributeToSort;
		int direction;
	}; // class DataSorter

	///////////////////////////////////////////////////////////////////////

	void renumberList(XmlElement *list)
	{  // resort by RealID and reset all IDs
		DataSorter sorter("REALID", true);
		list->sortChildElements(sorter);
		XmlElement* child = list->getFirstChildElement();
		int id = 1;
		while (child != nullptr)
		{
			child->setAttribute("ID", id);
			id++;
			child = child->getNextElement();
		}
	} // renumberList

	///////////////////////////////////////////////////////////////////////

	void renumberNotes(XmlElement *list)
	{  // resort by timestamp and set the Ids in that order
		DataSorter sorter("Timestamp", true);
		list->sortChildElements(sorter);
		XmlElement* child = list->getFirstChildElement();
		int id = 1;
		while (child != nullptr)
		{
			child->setAttribute("ID", id);
			id++;
			child = child->getNextElement();
		}
	} // renumberNotes

	void renumberPool(XmlElement *list)
	{  // resort by timestamp and set the Ids in that order
		DataSorter sorter("NoteNumber", true);
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

	bool loadMidiDrumPattern(const File& fileToRead, XmlElement *noteList, XmlElement *noteOffList, int patternIndex, int& measures, int& lenInTicks)
	{   // fileToRead and noteList must have been initialized!!
		// noteList will receive children that have tag NOTEDATA
		// if the input file contains more than 1 real track, the result will
		// be undefined and possibly mix note data from different channels
		// note data need not be on Channel10 (any channel will do as long as it's one track and one channel

		//jassert(noteList->getTagName().compare("PATTERNDATA"));

		if (!fileToRead.existsAsFile())
		{
			beatsLog("File " + fileToRead.getFileName() + " does not exist.)", Topiary::LogType::Warning);  // file doesn't exist
			return false;
		}
		FileInputStream inputStream(fileToRead);

		if (!inputStream.openedOk())
		{
			beatsLog("Cannot open file " + fileToRead.getFileName() + ".", Topiary::LogType::Warning);
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
			beatsLog("Invalid MIDI format in file " + fileToRead.getFileName() + ".", Topiary::LogType::Warning);
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
					auto newNoteOff = new XmlElement("OFFDATA");

					Logger::writeToLog(message.getDescription());
					Logger::writeToLog(String(message.getTimeStamp()));
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
					newNoteOff->setAttribute("ID", child->getStringAttribute("ID"));
					newNoteOff->setAttribute("REALID", child->getStringAttribute("REALID"));
					newNoteOff->setAttribute("Note", child->getStringAttribute("Note"));
					newNoteOff->setAttribute("Timestamp", (int)timeStamp);
					noteOffList->prependChildElement(newNoteOff);
				}
				if (message.isMetaEvent())
				{
					if (message.isTempoMetaEvent())
					{
						//getTempoMetaEventTickLength(short timeFormat)
						Logger::writeToLog(String("TEMPO META EVENT tempo seconds per quarter note: " + String(message.getTempoSecondsPerQuarterNote())));
						bpm = (60 / message.getTempoSecondsPerQuarterNote());
						Logger::writeToLog(String("=====> BPM " + String(bpm)));
					}
					if (message.isTimeSignatureMetaEvent())
					{
						message.getTimeSignatureInfo(num, den);
						Logger::writeToLog(String("SIGNATURE META EVENT: " + String(num) + String("/") + String(den)));
					}
				}
			}

			patternData[patternIndex].numNotes = noteList->getNumChildElements();
			patternData[patternIndex].notesRealID = patternData[patternIndex].numNotes;

			measures = timeStampMeasure; // because that gets passed on to  caller !! and we do +1 at the end!
			measures++;
			int finalLength = measures * num * Topiary::TICKS_PER_QUARTER;
			lenInTicks = finalLength;
			renumberNotes(noteList);
			renumberNotes(noteOffList);

			/* not efficient - would be simpler to do this with while loop before restoring: can't get that to work */
			forEachXmlChildElement(*noteOffList, child)
			{
				if (child->getIntAttribute("Timestamp") == finalLength)
				{
					child->setAttribute("Timestamp", child->getIntAttribute("Timestamp") - 1);
				}
			}

			// summary:
			Logger::writeToLog(String("************ BPM: ") + String(bpm) + String(" signature ") + String(num) + String("/") + String(den));
			Logger::writeToLog(String("************ From header timeframe:  tickperQ ") + String(timeFormat) + String("ticksperFrame ") +
				String(ticksPerFrame) + String("framesPerSecond ") + String(framesPerSecond));
			String myXmlDoc2 = noteList->createDocument(String());
			Logger::writeToLog(myXmlDoc2);

			String myXmlDoc = noteOffList->createDocument(String());
			Logger::writeToLog(myXmlDoc);


		}
		beatsLog("File " + fileToRead.getFileName() + " imported.", Topiary::LogType::Info);

		if ((num != numerator) || (den != denominator))
			beatsLog("Time signature in file is different from plugin timesignature; results may be unexpected!", Topiary::LogType::Warning);

		return true;
	}; // loadMidiDrumPattern

	//////////////////////////////////////////////////////////////////////////////////////////

	char* GMDrumMap(int i)
	{
		switch (i) {
		case 35: return "Acoustic Bass Drum";
		case 36: return "Bass Drum ";
		case 37: return "Side Stick";
		case 38: return "Acoustic Snare";
		case 39: return"Hand Clap";
		case 40: return "Electric Snare";
		case 41: return "Low Floor Tom";
		case 42: return "Closed Hi Hat";
		case 43: return "Closed Hi Hat";
		case 44: return "Pedal Hi Hat";
		case 45: return "Low Tom";
		case 46: return "Open Hi Hat";
		case 47: return "Low-mid Tom";
		case 48: return "Hi-mid Tom";
		case 49: return"Crash Cymbal 1";
		case 50: return "High Tom";
		case 51: return "Ride Cymbal 1";
		case 52: return "Chinese Cymbal";
		case 53: return "Ride Bell";
		case 54: return "Tambourine";
		case 55: return "Splash Cymbal";
		case 56: return "Cowbell";
		case 57: return "Crash Cymbal2";
		case 58: return "Vibraslap";
		case 59: return "Ride Cymbal 2";
		case 60: return "Hi Bongo";
		case 61: return "Low Bongo";
		case 62: return "Mute Hi Conga";
		case 63: return "Open Hi Conga";
		case 64: return "Low Conga";
		case 65: return "High Timbale";
		case 66: return "Low Timbale";
		case 67: return "High Agogo";
		case 68: return "Low Agogo";
		case 69: return "Cabasa";
		case 70: return "Maracas";
		case 71: return "Short Whistle";
		case 72: return "Long Whistle";
		case 73: return "Short Guiro";
		case 74: return "Long Guiro";
		case 75: return "Claves";
		case 76: return "Hi Wood Block";
		case 77: return "Low Wood Block";
		case 78: return "Mute Cuica";
		case 79: return "Open Cuica";
		case 80: return "Mute Triangle";
		case 81: return "Open Triangle";
		default: return "-------";
		}
	} // GMDrumMap


	//////////////////////////////////////////////////////////////////////////////////////////////////
	// load & save stuff stuff	
	//////////////////////////////////////////////////////////////////////////////////////////////////

	void addIntToModel(XmlElement *p, int i, char* iname)
	{
		auto parameter = new XmlElement("Parameter");
		parameter->setAttribute("Name", iname);
		parameter->setAttribute("Value", i);
		p->addChildElement(parameter);
	}

	//////////////////////////////////////////////////////////////////////////////////////////////////

	void addIntToModel(XmlElement *p, int i, char* iname, int index)
	{
		auto parameter = new XmlElement("Parameter");
		parameter->setAttribute("Name", iname);
		parameter->setAttribute("Value", i);
		parameter->setAttribute("Index", index);
		p->addChildElement(parameter);
	}

	//////////////////////////////////////////////////////////////////////////////////////////////////

	void addBoolToModel(XmlElement *p, bool b, char* bname)
	{
		auto parameter = new XmlElement("Parameter");
		parameter->setAttribute("Name", bname);
		parameter->setAttribute("Value", (int)b);
		p->addChildElement(parameter);
	}

	//////////////////////////////////////////////////////////////////////////////////////////////////

	void addBoolToModel(XmlElement *p, bool b, char* bname, int index)
	{
		auto parameter = new XmlElement("Parameter");
		parameter->setAttribute("Name", bname);
		parameter->setAttribute("Value", (int)b);
		parameter->setAttribute("Index", index);
		p->addChildElement(parameter);
	}

	//////////////////////////////////////////////////////////////////////////////////////////////////

	void addStringToModel(XmlElement *p, String value, char* sname)
	{
		auto parameter = new XmlElement("Parameter");
		parameter->setAttribute("Name", sname);
		parameter->setAttribute("Value", value);
		p->addChildElement(parameter);
	}

	//////////////////////////////////////////////////////////////////////////////////////////////////

	void addStringToModel(XmlElement *p, String value, char* sname, int index)
	{
		auto parameter = new XmlElement("Parameter");
		parameter->setAttribute("Name", sname);
		parameter->setAttribute("Value", value);
		parameter->setAttribute("Index", index);
		p->addChildElement(parameter);
	}

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

		addIntToModel(parameters, numPatterns, "numPatterns");
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

			// still to do - variation variables!!!
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

			// automation
			addIntToModel(parameters, variationSwitch[i], "variationSwitch", i);
			addBoolToModel(parameters, ccVariationSwitching, "ccVariationSwitching");
		}

	} // addParametersToModel

	//////////////////////////////////////////////////////////////////////////////////////////////////

	void restoreParametersToModel()
	{ // model now has a child "Parameters"; set all non-XML parameters to their new values

		//auto parameters = model->getChildByName("Parameters");
		auto child = model->getFirstChildElement();
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
					if (parameterName.compare("overrideHostTransport") == 0) overrideHostTransport = (bool)parameter->getIntAttribute("Value");
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

					if (parameterName.compare("numPatterns") == 0) numPatterns = parameter->getIntAttribute("Value");
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

					// patterndata array variables
					if (parameterName.compare("patLenInTicks") == 0)  patternData[parameter->getIntAttribute("Index")].patLenInTicks = parameter->getIntAttribute("Value");					if (parameterName.compare("numNotes") == 0)  patternData[parameter->getIntAttribute("Index")].numNotes = parameter->getIntAttribute("Value");
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

			if (tagName.compare("PatternOff") == 0) patternData[child->getIntAttribute("Index")].noteOffData = child;
			if (tagName.compare("Pattern") == 0) patternData[child->getIntAttribute("Index")].noteData = child;

			//if (tagName.compare("ShadowPatternOn") == 0) variation[child->getIntAttribute("Index")].shadowPatternOn = child;  LEAK
			//if (tagName.compare("ShadowPatternOff") == 0) variation[child->getIntAttribute("Index")].shadowPatternOff = child;

			child = child->getNextElement();

		} // foreach parameters

		// do some housekeeping to set variables always contained in the model object

		for (int i = 0; i < 8; i++)
		{
			// housekeeping auxciliary values for pools
			variation[i].lenInTicks = patternData[variation[i].patternToUse].patLenInTicks;

			for (int j = 0; j < 4; j++)
			{
				variation[i].fullTickFrom[j] = variation[i].tickFrom[j] + variation[i].beatFrom[j] * Topiary::TICKS_PER_QUARTER + numerator * variation[i].measureFrom[j] * Topiary::TICKS_PER_QUARTER;
				variation[i].fullTickTo[j] = variation[i].tickTo[j] + variation[i].beatTo[j] * Topiary::TICKS_PER_QUARTER + numerator * variation[i].measureTo[j] * Topiary::TICKS_PER_QUARTER;
			}
			// create the variations
			generateVariation(i);
			
		}

		// now delete the no-longer-needed "Parameters" child
		model->deleteAllChildElementsWithTagName("Parameters");

		// if there are no patterns; all variations need to be disabled!!!

		if (numPatterns == 0)
		{
			for (int i = 0; i < 8; i++)
				variation[i].enabled = false;
		}

		// inform editor
		broadcaster.sendActionMessage(MsgTransport);
		broadcaster.sendActionMessage(MsgLog);
		broadcaster.sendActionMessage(MsgMasterTables);
		broadcaster.sendActionMessage(MsgVariationEnables);		// so that if needed variationbuttons are disabled/enabled
		broadcaster.sendActionMessage(MsgVariationDefinition);	// inform editor of variation settings;
		broadcaster.sendActionMessage(MsgVariationAutomation);	// inform editor of variation automation settings;

		setRunState(Topiary::Stopped);

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
		beatsLog("Samples per tick" + String(samplesPerTick), Topiary::LogType::Debug);

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
		// i is index of pattern to use!
		// v is variation
		// p is poolnumber

		XmlElement* newChild = nullptr;
		XmlElement* patternOn = patternData[v].noteData;
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

		Logger::writeToLog("------------------- Start generation of pool " + String(p));

		// find poolIDCursor
		patternChild = patternOn->getFirstChildElement();
		while (patternChild->getIntAttribute("ID") != variation[v].poolIdCursor[p])
		{
			Logger::outputDebugString("ID =" + patternChild->getStringAttribute("ID") + " -- look for -- " + String(variation[v].poolIdCursor[p]));
			patternChild = patternChild->getNextElement();
		}

		//XmlElement* patternH = patternListData->getChildByAttribute("ID", String(variation[v].patternToUse));

		while (variation[v].poolTickCursor[p] < patLenInTicks)
		{
			// careful: the ID we walk to may not be in this notepool (it might be the first ID that made the previous run loop over the patternlength
			// so check before generating the note !!!

			note = patternChild->getIntAttribute("Note");
			if (poolNote[note] == p)
			{

				// need some decision logic to make sure we generate the child

				newChild = new XmlElement("DATA");
				newChild->setAttribute("ID", patternChild->getStringAttribute("ID"));
				newChild->setAttribute("Note", patternChild->getStringAttribute("Note"));
				newChild->setAttribute("Velocity", patternChild->getStringAttribute("Velocity"));

				// timestamp is variation[v].poolTickCursor !!!!
				// except if first note note generated; then it's the timestamp of the note in the pattern!

				if (firstNoteGenerated)
					newChild->setAttribute("Timestamp", String(variation[v].poolTickCursor[p]));
				else
				{
					variation[v].poolTickCursor[p] = patternChild->getIntAttribute("Timestamp");
					newChild->setAttribute("Timestamp", String(variation[v].poolTickCursor[p]));
					firstNoteGenerated = true;
				}
				// the following 3 are incorrect!!! these are what's in the pattern definition, but won't match tick value.  That's OK
				// because nobody gets to see these :)

				newChild->setAttribute("Measure", patternChild->getStringAttribute("Measure"));
				newChild->setAttribute("Beat", patternChild->getStringAttribute("Beat"));
				newChild->setAttribute("Tick", patternChild->getStringAttribute("Tick"));
				newChild->setAttribute("Channel", variation[v].poolChannel[p]);
				newChild->setAttribute("Length", patternChild->getStringAttribute("Length"));

				newPatternOn->prependChildElement(newChild);

				Logger::outputDebugString("Generating Note " + String(note) + " timestamp " + String(variation[v].poolTickCursor[p]));
				jassert(variation[v].poolTickCursor[p] >= 0);
				
			}

			// remember the tick of the last generated note; tick in the source pattern!
			previousTick = patternChild->getIntAttribute("Timestamp");

			//debug
			//jassert(previousTick < 950);

			Logger::outputDebugString(" looking for next note ");

			// now find the next child that's in the pool; loop because it may take a couple of tries; a loop without loops is empty and caught in the caller: generateVariation()

			do
			{
				patternChild = patternChild->getNextElement();

				if (patternChild == nullptr) // run over pattern end
				{
					Logger::outputDebugString(" running over pattern end");
					variation[v].poolTickCursor[p] += (variation[v].lenInTicks - previousTick) + 1; // +1 because we will reset to start of pool/pattern (and that start is 1st tick in that pattern/pool)
					
					patternChild = patternOn->getFirstChildElement();
					previousTick = patternChild->getIntAttribute("Timestamp");  // because first note need not be at time 0 !!!
				}
				else
				{
					if (patternChild->getIntAttribute("Timestamp") > variation[v].fullTickTo[p]) // run over pool end
					{
						Logger::outputDebugString(" running over pool end");
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

	void swapXmlElementPointers(XmlElement** a, XmlElement** b)
	{
		XmlElement* remember;
		remember = *a;
		*a = *b;
		*b = remember;
	}

	///////////////////////////////////////////////////////////////////////

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(TopiaryBeatsModel)
};

