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
	int getNumPatterns();
	void deletePattern(int i);
	void addPattern();
	void addPool();
	void deletePool(int i);
	void insertPatternFromFile(const File& fileToRead, int patternIndex);
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
	void beatsLog(String s);
	//bool isLogDirty();

	////// Transport

	//bool isTransportDirty();
	void setBPM(int bpm);
	void setNumeratorDenominator(int n, int d);
	void getTransportState(int& b, int& n, int& d, int& bs, bool& o, bool &waitFFN);
	void setOverrideHostTransport(bool o);
	void setRunState(int n); 
	void processTransportControls(int buttonEnabled); // // buttonEnabled; 0=Stop; 1=start; 2=rec
	

	////// Variations

	//bool isVariationsDirty();
	void setVariation(int i);
	void getVariation(int& running, int& selected);

	struct Variation {
		
		int patternToUse;				// index in patterndata
		int lenInTicks;
		int lenInMeasures;
		XmlElement* patternOn;			// pattern On note events - points to patterndata!!!
		XmlElement* patternOff;			// pattern Off note events
		XmlElement* currentPatternChild;    // where we are in generation
		XmlElement* currentPatternChildOff; // where we are in generation
		String name;
		bool playPool1, playPool2, playPool3, playPool4;
		bool ending;					// indicates that once pattern played, we no longer generate notes! (but we keep running (status Ended)
		XmlElement *shadowPatternOn;	// shadow pattern On note events
		XmlElement *shadowPatternOff;	// shadow pattern Off note events - copy of patterndata, so holds its own elements!!! (which get destroyed when this gets dvariation[i].shadowPatternOn.setAttribute("Index", String(i));estroyed)
		bool shadow;					// if true generator should use shadowpatterns
		// lots more to come!
	};
		
	// Generator stuff

	void setSampleRate(double sr);
	void setBlockSize(int blocksz);
	void setStartTimes();
	void generateMidi(MidiBuffer* midiBuffer);
	//static const Colour TOGGLE_COLOUR;
	//int processRunQ(int64 timeInSamples);  // called just before generateMidi - to see is status armed needs to be changed to running; status is returned as int
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
	//void setRunStartQ(int q);
	//int getRunStartQ();
	void setRunStopQ(int q);
	int getRunStopQ();
	String getName();
	void setName(String n);

	void getTime(int& b, int& m);

	//bool isMasterTablesDirty();
	//void clearMasterTablesDirty();

	////////// Broadcasters & Listeners

	void setListener(ActionListener *listener);
	void removeListener(ActionListener *listener);

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
		XmlElement *noteData;						// for the walker and nextTick in generation; keep track of which pattern (on and off) is playing
		XmlElement *noteOffData;					
		XmlElement *currentPatternChild;			// and which child we were on during generation
		XmlElement *currentPatternChildOff;
		int numNotes = 0;
		int notesRealID = 1;
		int patLenInTicks;
		
	} patternData[8];

		
	int numPatterns = 0;
	int patternsRealID = 1;
	int numPoolNotes = 0 ;
	int poolNotesRealID = 1;
		

	std::unique_ptr<XmlElement> model; 

	/////////// Logger 

	String logString;

	/////////// Transport
	
	bool overrideHostTransport;
	int denominator = 0; // b in a/b
	int numerator = 0;  // a in a/b
	int BPM = 120;
	int runState; 

	//int64 variationStartTriggerTime;  // what is this??
	//int64 variationStopTriggerTime;   // what is this?? Needed?
		
	/////////// Variations

	int variationSelected = 1; 
	int variationRunning = 1; // this may differ from variationSelected during switch trigger time !!!

	Variation variation[8];  // struct to hold variation detail

	/////////// Generator variables

	double sampleRate, ticksPerBeat, samplesPerTick;  // housekeeping in recalcRealTime() !
	
	int64 rtCursorFrom;					// sampletime to start generating from
	int64 rtCursorTo;					// we generated in principle till here
	int64 rtCursor;						// sampletime how far we are in generation between rtCursorFrom and rtCursorTo
	int64 blockCursor;					// sampletime of start of current block
	int64 nextRTGenerationCursor;		// real time cursor of next event to generate
	int blockSize;						// size of block to generate 
	//int patternCursor;
	int patternCursorOn;				// ticks we are at within the variation/pattern for notes on
	int patternCursorOff;				// ticks we are at within the variation/pattern for notes off
	int lastTickOn;						// last generated on note in ticks within the pattern
	int lastTickOff;					// last generated off note in ticks within the pattern
	
	int variationStartQ = 100;			// when to switch variations
	//int runStartQ = 100;				// when to start running relative to transport
	int runStopQ = 100;					// when to stop running
	bool WFFN = false;					// start at first note in if true; otherwize immediate
	int switchVariation =  1;			// when new pattern starts, it always starts at beginning - enumerator
										// if not, we try to stay within same measure/beat as the running pattern
										// if the running pattern is shorter than the new one, we start within the first measure
	bool notePassThrough = false;		// echo incoming notes out
	int measure = 0;					// keep track of where we are in generation
	int beat = 0;						// keep track of where we are in generation
	int tick = 0;						// keep track of where we are in generation
	int64 cursorToStop = -1;			// used when running, set once, to cursor where we should stop; needs to be set to -1 when running stops (typically in processEnding() but also when arming 

	SortedSet<int> noteOnSet;			// keep track of which notes are still on
	// loop over all patterns and make set of inuque ones

	
	/////////// Dirty variables

	//bool variationsDirty = false;
	//bool transportDirty = false;
	//bool logDirty = false;
	//bool masterTablesDirty = false; // so next paint can update the tables
	
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
			beatsLog("File " + fileToRead.getFileName() + " does not exist.)");  // file doesn't exist
			return false;
		}
		FileInputStream inputStream(fileToRead);

		if (!inputStream.openedOk())
		{
			beatsLog("Cannot open file " + fileToRead.getFileName() + ".");
			return false;  // failed to open
		}

		double bpm = 120.0; // default 
		int num = 4; // default 
		int den = 4; // default 

		int ticksPerFrame = 0;
		int framesPerSecond = 0;
		int ticksPerQuarter = 0;
		int timeStampMeasure = 0;

		MidiFile midifile;
		if (!midifile.readFrom(inputStream))
		{
			beatsLog("Invalid MIDI format in file " + fileToRead.getFileName() + ".");
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
					lenInTicks = (int) timeStamp; // passed on to caller
					newNote->setAttribute("Timestamp", (int)floor(timeStamp));
					timeStampMeasure = (int)floor(timeStamp / (num*Topiary::TICKS_PER_QUARTER));
					timeStamp = timeStamp - (timeStampMeasure * num*Topiary::TICKS_PER_QUARTER);
					int timeStampBeat = (int) floor(timeStamp / Topiary::TICKS_PER_QUARTER);
					timeStamp = timeStamp - (timeStampBeat * Topiary::TICKS_PER_QUARTER);
					int timeStampTicks = (int) timeStamp;
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
					lenInTicks = (int) timeStamp; // passed on to caller
					child->setAttribute("Length", (int)floor(timeStamp - child->getStringAttribute("Timestamp").getIntValue()));

					// so child is our note ON event; fill the note OFF event
					newNoteOff->setAttribute("ID", child->getStringAttribute("ID"));
					newNoteOff->setAttribute("REALID", child->getStringAttribute("REALID"));
					newNoteOff->setAttribute("Note", child->getStringAttribute("Note"));
					newNoteOff->setAttribute("Timestamp", (int) timeStamp);
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
		beatsLog("File " + fileToRead.getFileName() + " imported.");

		
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

	void addIntToModel(XmlElement *p, int i, char* iname, int index)
	{
		auto parameter = new XmlElement("Parameter");
		parameter->setAttribute("Name", iname);
		parameter->setAttribute("Value", i);
		parameter->setAttribute("Index", index);
		p->addChildElement(parameter);
	}

	void addBoolToModel(XmlElement *p, bool b, char* bname)
	{
		auto parameter = new XmlElement("Parameter");
		parameter->setAttribute("Name", bname);
		parameter->setAttribute("Value", (int)b);
		p->addChildElement(parameter);
	}

	void addStringToModel(XmlElement *p, String value, char* sname)
	{
		auto parameter = new XmlElement("Parameter");
		parameter->setAttribute("Name", sname);
		parameter->setAttribute("Value", value);
		p->addChildElement(parameter);
	}

	void addParametersToModel()
	{
		auto parameters = new XmlElement("Parameters");
		model->addChildElement(parameters);

		addStringToModel(parameters, name, "name");
		addIntToModel(parameters, BPM, "BPM");
		addIntToModel(parameters, numerator, "numerator");
		addIntToModel(parameters, denominator ,"denominator");
		addIntToModel(parameters, variationSelected, "variationSelected");
		addIntToModel(parameters, switchVariation, "switchVariation");
		//addIntToModel(parameters, runStartQ, "runStartQ");
		addIntToModel(parameters, runStopQ, "runStopQ");
		addIntToModel(parameters, variationStartQ, "variationStartQ");
		addBoolToModel(parameters, WFFN, "WFFN");
		addStringToModel(parameters, name, "name");
		addBoolToModel(parameters, overrideHostTransport, "overrideHostTransport");
		addBoolToModel(parameters, notePassThrough, "notePassThrough");
		
		addIntToModel(parameters, numPatterns, "numPatterns");
		addIntToModel(parameters, patternsRealID, "patternsRealID");
		addIntToModel(parameters, numPoolNotes, "numPoolNotes");
		addIntToModel(parameters, poolNotesRealID, "poolNotesRealId");

		for (int i = 0; i < 8; i++) {
			addIntToModel(parameters, patternData[i].numNotes, "numNotes", i);
			addIntToModel(parameters, patternData[i].notesRealID, "notesRealID", i);
			addIntToModel(parameters, patternData[i].patLenInTicks, "patLenInTicks", i);

			// still to do - variation variables!!!
			addIntToModel(parameters, variation[i].lenInTicks, "lenInTicks", i);
			addIntToModel(parameters, variation[i].lenInMeasures, "lenInMeasures", i);
			addIntToModel(parameters, variation[i].patternToUse, "patternToUse", i);
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
					if (parameterName.compare("overrideHostTransport") == 0) overrideHostTransport = (bool) parameter->getIntAttribute("Value");
					if (parameterName.compare("denominator") == 0) denominator = parameter->getIntAttribute("Value");
					if (parameterName.compare("variationSelected") == 0)
					{
						variationSelected = parameter->getIntAttribute("Value");
						variationRunning = variationSelected;
					}

					if (parameterName.compare("switchVariation") == 0) switchVariation = parameter->getIntAttribute("Value");
					//if (parameterName.compare("runStartQ") == 0) runStartQ = parameter->getIntAttribute("Value");
					if (parameterName.compare("runStopQ") == 0) runStopQ = parameter->getIntAttribute("Value");
					if (parameterName.compare("variationStartQ") == 0) variationStartQ = parameter->getIntAttribute("Value");
					if (parameterName.compare("name") == 0) name = parameter->getStringAttribute("Value");

					if (parameterName.compare("WFFN") == 0)	WFFN = (bool)parameter->getIntAttribute("Value");
					if (parameterName.compare("notePassThrough") == 0) 	notePassThrough = (bool)parameter->getIntAttribute("Value");
										
					if (parameterName.compare("numPatterns") == 0) numPatterns = parameter->getIntAttribute("Value");
					if (parameterName.compare("patternsRealID") == 0) patternsRealID = parameter->getIntAttribute("Value");
					if (parameterName.compare("numPoolNotes") == 0) numPoolNotes = parameter->getIntAttribute("Value");
					if (parameterName.compare("poolNotesRealID") == 0) poolNotesRealID = parameter->getIntAttribute("Value");

					if (parameterName.compare("numNotes") == 0)  patternData[parameter->getIntAttribute("Index")].numNotes = parameter->getIntAttribute("Value");
					if (parameterName.compare("notesRealID") == 0)  patternData[parameter->getIntAttribute("Index")].notesRealID = parameter->getIntAttribute("Value");

					//if (parameterName.compare("PatternOff") == 0) patternData[parameter->getIntAttribute("Index")].noteOffData = parameter; // not needed - find those with patterntouse
					//if (parameterName.compare("Pattern") == 0) patternData[parameter->getIntAttribute("Index")].noteData = parameter;

					// variation array variables
					// No need to save the shadowPatterns; these are generated on the fly (delete lines below)
					//if (parameterName.compare("ShadowPatternOn") == 0) variation[parameter->getIntAttribute("Index")].shadowPatternOn = parameter; shadowpatterns are on the fly generated
					//if (parameterName.compare("ShadowPatternOff") == 0) variation[parameter->getIntAttribute("Index")].shadowPatternOff = parameter;
					if (parameterName.compare("lenInTicks") == 0) variation[parameter->getIntAttribute("Index")].lenInTicks = parameter->getIntAttribute("Value");
					if (parameterName.compare("lenInMeasures") == 0) variation[parameter->getIntAttribute("Index")].lenInMeasures = parameter->getIntAttribute("Value");
					if (parameterName.compare("patternToUse") == 0) variation[parameter->getIntAttribute("Index")].patternToUse = parameter->getIntAttribute("Value");

					// patterndata array variables
					if (parameterName.compare("patLenInTicks") == 0)  patternData[parameter->getIntAttribute("Index")].patLenInTicks = parameter->getIntAttribute("Value");
					if (parameterName.compare("numNotes") == 0)  patternData[parameter->getIntAttribute("Index")].numNotes = parameter->getIntAttribute("Value");
					if (parameterName.compare("notesRealID") == 0)  patternData[parameter->getIntAttribute("Index")].notesRealID = parameter->getIntAttribute("Value");

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

		// variation noteOn and noteOff needs to refer to patternData
		// temporarily point shadowpatterns to patternData - to fix later
		for (int i = 0; i < 8; i++)
		{
			variation[i].patternOff = patternData[variation[i].patternToUse].noteOffData;
			variation[i].patternOn = patternData[variation[i].patternToUse].noteData;
			//variation[i].shadowPatternOn = patternData[variation[i].patternToUse].noteOffData;
			//variation[i].shadowPatternOff = patternData[variation[i].patternToUse].noteData;  LEAK
		}

	    // now delete the no-longer-needed "Parameters" child
		model->deleteAllChildElementsWithTagName("Parameters");
		//variationsDirty = true;
		//transportDirty = true;
		broadcaster.sendActionMessage("transport");
		//logDirty = true;
		broadcaster.sendActionMessage("log");
		broadcaster.sendActionMessage("masterTables");
		//masterTablesDirty = true;

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
		ticksPerBeat = Topiary::TICKS_PER_QUARTER *4 /denominator;
		samplesPerTick = (double)sampleRate / ((double)ticksPerBeat * BPM / 60.0); 
		beatsLog("Samples per tick" + String(samplesPerTick));
		
	} // recalcRealTime

	///////////////////////////////////////////////////////////////////////

	void calcMeasureBeat()
	{
		int64 cursorInTicks = (int64) floor(blockCursor / samplesPerTick);  // use BlockCursor instead of rtCursor as rtCursor might jump back 7 forth
		int newMeasure = (int) floor(cursorInTicks / (ticksPerBeat* denominator)) +1;
				
		int newBeat = (int)floor(cursorInTicks / ticksPerBeat);
		newBeat = (newBeat % denominator) + 1;

		jassert((newMeasure > 0));
		jassert((newMeasure < 10000));

		//if ((beat != newBeat) || (measure != newMeasure))
		if (beat != newBeat)
		{
			beat = newBeat;
			measure = newMeasure;
			broadcaster.sendActionMessage("timing");
		}
	} // calcMeasureBeat

	///////////////////////////////////////////////////////////////////////

	bool walkToTick(XmlElement* parent, XmlElement** child, int toTick)
	{ // find the first child on or after this tick, starting from current child; 
	  // caller has to make sure that child is child of parent, or pass nullptr to initialize
	  // return false if pattern is empty or nothing to do within the blockSize; if empty then child == nullptr; 
      // This loops around the pattern! To avoid infinite loop, take blockSize into account; only loop over pattern
	  // if the amount of ticks walked is less than blocksize

		// DOES NOT TAKE LIMITED PATTERN LENGTH INTO ACCOUNT (yet) - we will do that in the variations later
		
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
			// we only return true if first child is < blockSize away from where we are now; actually conservative, can be blockSize/samplesPerTick !!!
			childTick = (*child)->getIntAttribute("Timestamp");
			if (childTick <= (blockSize / samplesPerTick))
				return true;
			else
				return false;

		}
		return true;
	} // walkToTick

	///////////////////////////////////////////////////////////////////////

	void nextTick(XmlElement* parent, XmlElement** child)
	{	// assert that parent has at least 1 child of each!!! (do a walk to a tick first!)
		// this one loops around the pattern

		// DOES NOT TAKE LIMITED PATTERN LENGTH INTO ACCOUNT (yet) - we will do that in the variations later

		*child = (*child)->getNextElement();
		if (*child == nullptr)
		{   
			*child = parent->getFirstChildElement();
			Logger::outputDebugString(String("-------- LOOPING OVER END OFF PATTERN ==================== "));
		}

	}  // nextTick
	
	

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(TopiaryBeatsModel)
};
	
