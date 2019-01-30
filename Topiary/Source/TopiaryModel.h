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


#pragma once
#include "Topiary.h"
#include "TopiaryModel.h"
#include "TopiaryThread.h"


class TopiaryModel
{
public:
	TopiaryModel();
	~TopiaryModel();

	void savePreset(String msg, String extension);
	void loadPreset(String msg, String extension);
	virtual void saveStateToMemoryBlock(MemoryBlock& destData);
	virtual void restoreStateFromMemoryBlock(const void* data, int sizeInBytes);
	virtual void addParametersToModel();
	virtual void restoreParametersToModel();

	int getNumPatterns();

	////// Logger 

	String* getLog();
	String getLastWarning();
	void Log(String s, int logType);
	void setLogSettings(bool warning, bool midiIn, bool midiOut, bool debug, bool transport, bool variations, bool info);
	void clearLog();
	void getLogSettings(bool& warning, bool& midiIn, bool& midiOut, bool& debug, bool &transport, bool &variations, bool &info);
	void getMidiLogSettings(bool &in, bool &out);
	void logMidi(bool in, MidiMessage &msg); // in denotes msg in or msg out

	////// Transport

	void setBPM(int bpm);
	void setNumeratorDenominator(int n, int d);
	void getTransportState(int& b, int& n, int& d, int& bs, bool& o, bool &waitFFN);
	virtual void setOverrideHostTransport(bool o);
	void removeRecordButton();
	void setRunState(int n);
	int getRunState();
	void processTransportControls(int buttonEnabled); // // buttonEnabled; 0=Stop; 1=start; 2=rec
	
	////// Variations

	virtual int getVariationLenInTicks(int v);
	virtual void setVariation(int v);
	virtual bool getVariationEnabled(int v);
	virtual void initializeVariationsForRunning();

	// Generator stuff

	void setSampleRate(double sr);
	void setBlockSize(int blocksz);
	void setStartTimes();
	void endNotesOn(MidiBuffer* midiBuffer);
	
	bool processEnding();
	void outputModelEvents(MidiBuffer& buffer);		// see if anything needs to be done by model; e.g. output non-generation stuff like CC messages (can be out-IFDEFed for plugins that don't need it
	virtual void outputVariationEvents();			// potentially generate events when variation button is pressed (outside of running) - certainly needed for presetz
	virtual void generateMidi(MidiBuffer* midiBuffer);
	virtual bool processVariationSwitch();
	virtual bool switchingVariations();
	virtual void getVariationDetailForGenerateMidi(XmlElement** parent, XmlElement** noteChild, int& parentLength, bool& ending, bool& ended);
	virtual void setEnded(); // flag and ending variation has actually ended
	virtual void threadRunner(); // does generation of patterns on a separate thread

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
#define MsgVariationEnables "e"
#define MsgVariationAutomation "a"  // variationcontrol
#define MsgVariationSelected "s"
#define MsgWarning "w"
#define MsgPattern "p"
#define MsgTiming "T"
#define MsgMaster "m"
#define MsgVariationDefinition "d" // 
#define MsgRealTimeParameter "r" // signal that something in realtime needs updating in editor

	////////// Automation

	void setVariationControl(bool ccSwitching, int channel, int switches[8]);
	void getVariationControl(bool& ccSwitching, int& channel, int switches[8]);
	void processAutomation(MidiMessage& msg);
	virtual void processCC(MidiMessage& msg, MidiBuffer* midiBuffer) ;
	virtual void processCC(MidiMessage& msg);

protected:

	SpinLock lockModel;
	ActionBroadcaster broadcaster;
	std::unique_ptr<XmlElement> model;
	TopiaryThread topiaryThread;

	String name;

	/////////// Logger 

	String logString;
	String lastWarning;

	bool logWarning = true;
	bool logMidiIn = false;
	bool logMidiOut = false;
	bool logDebug = false;
	bool logTransport = false;
	bool logVariations = false;
	bool logInfo = false;
	
	/////////// Transport

	bool overrideHostTransport = true;
	int denominator = 0; // b in a/b
	int numerator = 0;  // a in a/b
	int BPM = 120;
	int runState;
	

	/////////// Variations

	int variationSelected = 0;
	int variationRunning = 0; // this may differ from variationSelected during switch trigger time !!!

	/////////// Generator variables

	double sampleRate, ticksPerBeat, samplesPerTick;  // housekeeping in recalcRealTime() !
	int64 blockCursor;					// sampletime of start of current block
	int64 nextRTGenerationCursor;		// real time cursor of next event to generate
	int blockSize;						// size of block to generate 
	int patternCursor;					// patternCursor  to disappear in the future
	int numPatterns = -1;				// set to 1 in Presetz (as long as it's not -1 in presetz :)
	int variationStartQ = Topiary::Quantization::Immediate;			// when to switch variations
	int runStopQ = Topiary::Quantization::Immediate;				// when to stop running
	bool WFFN = false;												// start at first note in if true; otherwize immediate
	int switchVariation = Topiary::VariationSwitch::SwitchFromStart;// when new pattern starts, it always starts at beginning - enumerator
																	// if not, we try to stay within same measure/beat as the running pattern
																	// if the running pattern is shorter than the new one, we start within the first measure
	bool notePassThrough = false;		// echo incoming notes out
	int measure = 0;					// keep track of where we are in generation
	int beat = 0;						// keep track of where we are in generation
	int tick = 0;						// keep track of where we are in generation
	int64 cursorToStop = -1;			// used when running, set once, to cursor where we should stop; needs to be set to -1 when running stops (typically in processEnding() but also when arming 
	int patternCursorOffset = 0;		// used in conjunction with switchVariation; the offset of the patterncursor with respect to the blockCursor

	int transitioningFrom = -1;  // set when we are generating
	int transitioningTo = -1;

	int threadRunnerState; // NothingToDo --> Generating --> DoneGenerating (and back to nothingToGenerate when what was generated was applied (the latter for presetz only for now)

	MidiBuffer modelEventBuffer;		// for msgs caused by the editor
	

	/////////////// Automation

	int variationSwitch[8];				// either notes for each variation, of CC numbers
	bool ccVariationSwitching = true;	// if false then we're using notes
	int variationSwitchChannel = 0;		// midi channel for variation switching; 0 == omni

	String filePath;

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

	void renumberByID(XmlElement *list)
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
	} // renumberByID

	///////////////////////////////////////////////////////////////////////

	void renumberByTimestamp(XmlElement *list)
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
	} // renumberByTiumestamp

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

	void addIntToModel(XmlElement *p, int i, char* iname, int index1, int index2)
	{
		auto parameter = new XmlElement("Parameter");
		parameter->setAttribute("Name", iname);
		parameter->setAttribute("Value", i);
		parameter->setAttribute("Index1", index1);
		parameter->setAttribute("Index2", index2);
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

		tick = cursorInTicks % Topiary::TICKS_PER_QUARTER;
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

	bool walkToTick(XmlElement* parent, XmlElement** child, int toTick)
	{ 
		// find the first child on or after this tick, starting from current child; 
		// caller has to make sure that child is child of parent, or pass nullptr to initialize
		// return false if pattern is empty or nothing to do within the blockSize; if empty then child == nullptr; 
		// This loops around the pattern! To avoid infinite loop, take blockSize into account; only loop over pattern
		// if the amount of ticks walked is less than blocksize

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

	///////////////////////////////////////////////////////////////////////

	void swapXmlElementPointers(XmlElement** a, XmlElement** b)
	{
		XmlElement* remember;
		remember = *a;
		*a = *b;
		*b = remember;
	}

	///////////////////////////////////////////////////////////////////////

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(TopiaryModel)
};

