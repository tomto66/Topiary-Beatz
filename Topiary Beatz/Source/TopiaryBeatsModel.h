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

#include "TopiaryBeats.h"
#include "TopiaryPatternList.h"
#include "TopiaryPoolList.h"
#include "TopiaryPattern.h"
#include "../../Topiary/Source/TopiaryVariation.h"
#include "TopiaryNoteOffBuffer.h"

#include "../../Topiary/Source/TopiaryMidiLearnEditor.h"

class TopiaryBeatsModel : public TopiaryModel
{
public:

	enum SwingQButtonIds
	{
		SwingQ4 = 3004,
		SwingQ8 = 3008,
		SwingQRadioID = 3000
	};

	TopiaryBeatsModel();
	~TopiaryBeatsModel();

	String name = "New Beatz Pattern"; // name of this preset
	void quantize(int p, int ticks);

	bool getFixedOutputChannels();
	void setFixedOutputChannels(bool b);

	void getPatterns(String pats[8]);
	
	int getPatternLengthInMeasures(int i);  // needed in variationComponent for validating pool length values;
	void setPatternLengthInMeasures(int i, int l);
	void deletePattern(int i);
	void addPattern();
	void duplicatePattern(int p);
	void clearPattern(int p);

	void setPatternLength(int p, int l, bool keepTail);
	void deleteNote(int p, int n);				// deletes the note with ID n from pattern p
	void getNote(int p, int ID, int& note, int &velocity, int &timestamp, int &length);  // get note with id ID from pattern p
	void addNote(int p, int n, int v, int l, int t);	// adds note n in pattern p, with velocity v at time t
	void deleteAllNotes(int p, int n);  // deletes all occurrence of note n (id of event) in the pattern

	void regenerateVariationsForPattern(int p);

	void addPoolNote();
	void deletePoolNote(int i);
	void rebuildPool(bool clean);
	
	void setGMDrumMapLabels();
	bool insertPatternFromFile(int patternIndex, bool overload);
	void validateTableEdit(int p, XmlElement* child, String attribute); // see if user edits to this attribute make sense and do housekeeping

	void saveStateToMemoryBlock(MemoryBlock& destData) override;
	void restoreStateFromMemoryBlock(const void* data, int sizeInBytes) override;
	bool processVariationSwitch() override;
	bool switchingVariations() override;
	void initializeVariationsForRunning() override;
	void initializePreviousSteadyVariation();
	void generateMidi(MidiBuffer* midiBuffer, MidiBuffer* recBuffer) override;
	
	////// Variations

	void setVariation(int i) override;
	void getVariation(int& running, int& selected);
	void getVariationEnables(bool enables[8]);
	bool getVariationEnabled(int v);
	int getVariationLenInTicks(int v);

	void getVariationDefinition(int i, bool& enabled, String& vname, int& patternId, bool enables[4], int poolChannel[4], int& type);   // pass variation Definition on to VariationComponent in editor
	void setVariationDefinition(int i, bool enabled, String vname, int patternId, bool enables[4], int poolChannel[4], int type);      // set variation Definition parameters from editor; return false if we try to disable the last enabled variation
	bool validateVariationDefinition(int i, bool enable, String vname, int patternId);
	void setRandomizeNotes(int v, bool enable, bool enablePool[4], int value);
	void getRandomizeNotes(int v, bool &enable, bool enablePool[4], int &value);
	void setSwing(int v, bool enable, bool enablePool[4], int value);
	void getSwing(int v, bool &enable, bool enablePool[4], int &value);
	void setRandomizeVelocity(int v, bool enable, bool enablePool[4], int value, bool plus, bool min);
	void getRandomizeVelocity(int v, bool &enable, bool enablePool[4], int &value, bool &plus, bool &min);
	void setRandomizeTiming(int v, bool enable, bool enablePool[4], int value, bool plus, bool min);
	void getRandomizeTiming(int v, bool &enable, bool enablePool[4], int &value, bool &plus, bool &min);
	void getVelocityOffset(int v, bool& offsetVelocity, int velocityOffset[4]);
	void setVelocityOffset(int v, bool offsetVelocity, int velocityOffset[4]);
	void setSwingQ(int v, int q);
	int getSwingQ(int v);

	void generateVariation(int i, int measureToGenerate); // Generates the variation;
	void generateAllVariations(int measureToGenerate);

	void setOverrideHostTransport(bool o) override;
	void setNumeratorDenominator(int nu, int de) override;

	struct Variation {

		int patternToUse;					// index in patterndata
		int lenInMeasures;
		TopiaryVariation pattern;				// pattern  events in the variation
		int currentPatternChild;			// where we are in generation 
		
		int type;
		bool ended;

		bool enabled = false;
		String name = "";
		bool enablePool[4];
		int poolChannel[4];		// output channel for this pool in this variation

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

		// velocity offset
		bool offsetVelocity;
		int velocityOffset[4];

		int swingQ;
	};

	void swapVariation(int from, int to) override;
	void copyVariation(int from, int to) override;
	bool midiLearn(MidiMessage m); // called by processor
	void record(bool b) override; // tells model to record or not; at end of recording it processes the new notes
	void processMidiRecording() override; // add recorded events to the pattern

	TopiaryPatternList* getPatternList();
	TopiaryPoolList* getPoolList();
	TopiaryPattern* getPattern(int p);
	int getNumPatterns();

	bool walkToTick(TopiaryVariation* parent, int& childIndex, int toTick);
	void nextTick(TopiaryVariation* parent, int& childIndex);


	void outputNoteOn(int noteNumber);
	void outputNoteOff(int noteNumber);

#define NUMBEROFQUANTIZERS 10

	

private:
	TopiaryPatternList patternList;
	TopiaryPoolList poolList;
	TopiaryPattern patternData[8];
	Variation variation[8];  // struct to hold variation detail
	NoteOffBuffer noteOffBuffer;
	bool fixedOutputChannels; // force those the same for every variation
	
	///////////////////////////////////////////////////////////////////////

	bool loadMidiDrumPattern(const File& fileToRead, int patternIndex, int& measures, int& lenInTicks)
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
		model.reset(new XmlElement("Beatz"));
		auto patternListData = new XmlElement("PatternList");
		model->addChildElement(patternListData);
		// this one will have a series of <PatternData>
		patternList.addToModel(patternListData);

		auto poolListData = new XmlElement("PoolList");
		// this one will have a series of <PoolData>
		model->addChildElement(poolListData);
		poolList.addToModel(poolListData);
		
		auto patternDataa = new XmlElement("PatternData");
		// this one will have a series of <Pattern>
		model->addChildElement(patternDataa);
		for (int p = 0; p < patternList.numItems; p++)
			patternData[p].addToModel(patternDataa);
		
		auto parameters = new XmlElement("Parameters");
		model->addChildElement(parameters);

		addStringToModel(parameters, name, "name");
		addIntToModel(parameters, BPM, "BPM");
		addIntToModel(parameters, numerator, "numerator");
		addIntToModel(parameters, denominator, "denominator");
		addIntToModel(parameters, switchVariation, "switchVariation");
		addIntToModel(parameters, runStopQ, "runStopQ");
		addIntToModel(parameters, variationStartQ, "variationStartQ");
		addBoolToModel(parameters, WFFN, "WFFN");
		addStringToModel(parameters, name, "name");
		addBoolToModel(parameters, overrideHostTransport, "overrideHostTransport");
		addBoolToModel(parameters, notePassThrough, "notePassThrough");
		addBoolToModel(parameters, fixedOutputChannels, "fixedOutputChannels");
		addBoolToModel(parameters, logMidiIn, "logMidiIn");
		addBoolToModel(parameters, logMidiOut, "logMidiOut");
		addBoolToModel(parameters, logDebug, "logDebug");
		addBoolToModel(parameters, logTransport, "logTransport");
		addBoolToModel(parameters, logVariations, "logVariations");
		addBoolToModel(parameters, logInfo, "logInfo");
		addStringToModel(parameters, filePath, "filePath");

		addIntToModel(parameters, poolList.getNumItems(), "numPoolNotes");
		
		addIntToModel(parameters, variationSwitchChannel, "variationSwitchChannel");
		addBoolToModel(parameters, ccVariationSwitching, "ccVariationSwitching");

		for (int i = 0; i < 8; i++) {
			
			// Variations
			
			addIntToModel(parameters, variation[i].lenInMeasures, "lenInMeasures", i);
			addIntToModel(parameters, variation[i].patternToUse, "patternToUse", i);
			addStringToModel(parameters, variation[i].name, "variationName", i);
			addBoolToModel(parameters, variation[i].enabled, "variationEnabled", i);
			addIntToModel(parameters, variation[i].type, "variationType", i);
			addBoolToModel(parameters, variation[i].enablePool[0], "enablePool0", i);
			addBoolToModel(parameters, variation[i].enablePool[1], "enablePool1", i);
			addBoolToModel(parameters, variation[i].enablePool[2], "enablePool2", i);
			addBoolToModel(parameters, variation[i].enablePool[3], "enablePool3", i);

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

			addBoolToModel(parameters, variation[i].offsetVelocity, "offsetVelocity", i);
			addIntToModel(parameters, variation[i].velocityOffset[0], "velocityOffset0", i);
			addIntToModel(parameters, variation[i].velocityOffset[1], "velocityOffset1", i);
			addIntToModel(parameters, variation[i].velocityOffset[2], "velocityOffset2", i);
			addIntToModel(parameters, variation[i].velocityOffset[3], "velocityOffset3", i);
			
			addIntToModel(parameters, variation[i].swingQ, "swingQ", i);

			// automation
			addIntToModel(parameters, variationSwitch[i], "variationSwitch", i);

		}

	} // addParametersToModel

	//////////////////////////////////////////////////////////////////////////////////////////////////

	void restoreParametersToModel()
	{
		
		overrideHostTransport = true; // otherwise we might get very weird effects if the host were running
		setRunState(Topiary::Stopped);

		auto child = model->getFirstChildElement();
		jassert(child->getTagName().equalsIgnoreCase("PatternList"));
		patternList.getFromModel(child);
	
		child = child->getNextElement();
		jassert(child->getTagName().equalsIgnoreCase("PoolList"));
		poolList.getFromModel(child);

		child = child->getNextElement();
		jassert(child->getTagName().equalsIgnoreCase("PatternData"));

		// load all patternData
		XmlElement *pchild = child->getFirstChildElement();
		int p = 0;
		while (pchild != nullptr)
		{
			patternData[p].getFromModel(pchild);
			pchild = pchild->getNextElement();
			p++;
		}

		child = child->getNextElement();
		bool rememberOverride = true; // we do not want to set that right away!
		jassert(child->getTagName().equalsIgnoreCase("Parameters"));
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

						if (parameterName.compare("switchVariation") == 0) switchVariation = parameter->getIntAttribute("Value");
						if (parameterName.compare("runStopQ") == 0) runStopQ = parameter->getIntAttribute("Value");
						if (parameterName.compare("variationStartQ") == 0) variationStartQ = parameter->getIntAttribute("Value");
						if (parameterName.compare("name") == 0) name = parameter->getStringAttribute("Value");

						if (parameterName.compare("WFFN") == 0)	WFFN = parameter->getBoolAttribute("Value");
						if (parameterName.compare("notePassThrough") == 0) 	notePassThrough = parameter->getBoolAttribute("Value");
						if (parameterName.compare("fixedOutputChannels") == 0) 	fixedOutputChannels = parameter->getBoolAttribute("Value");
						if (parameterName.compare("logMidiIn") == 0) logMidiIn = parameter->getBoolAttribute("Value");
						if (parameterName.compare("logMidiOut") == 0)	logMidiOut = parameter->getBoolAttribute("Value");
						if (parameterName.compare("logDebug") == 0)	logDebug = parameter->getBoolAttribute("Value");
						if (parameterName.compare("logTransport") == 0)	logTransport = parameter->getBoolAttribute("Value");
						if (parameterName.compare("logVariations") == 0) logVariations = parameter->getBoolAttribute("Value");
						if (parameterName.compare("logInfo") == 0)	logInfo = parameter->getBoolAttribute("Value");

						if (parameterName.compare("filePath") == 0)	filePath = parameter->getStringAttribute("Value");

						//if (parameterName.compare("lenInTicks") == 0) variation[parameter->getIntAttribute("Index")].lenInTicks = parameter->getIntAttribute("Value");
						if (parameterName.compare("lenInMeasures") == 0) variation[parameter->getIntAttribute("Index")].lenInMeasures = parameter->getIntAttribute("Value");
						if (parameterName.compare("patternToUse") == 0) variation[parameter->getIntAttribute("Index")].patternToUse = parameter->getIntAttribute("Value");
						if (parameterName.compare("variationEnabled") == 0) variation[parameter->getIntAttribute("Index")].enabled = parameter->getBoolAttribute("Value");
						if (parameterName.compare("variationType") == 0) variation[parameter->getIntAttribute("Index")].type = parameter->getIntAttribute("Value");
						if (parameterName.compare("enablePool0") == 0) variation[parameter->getIntAttribute("Index")].enablePool[0] = parameter->getBoolAttribute("Value");
						if (parameterName.compare("enablePool1") == 0) variation[parameter->getIntAttribute("Index")].enablePool[1] = parameter->getBoolAttribute("Value");
						if (parameterName.compare("enablePool2") == 0) variation[parameter->getIntAttribute("Index")].enablePool[2] = parameter->getBoolAttribute("Value");
						if (parameterName.compare("enablePool3") == 0) variation[parameter->getIntAttribute("Index")].enablePool[3] = parameter->getBoolAttribute("Value");

						if (parameterName.compare("poolChannel0") == 0) 
							variation[parameter->getIntAttribute("Index")].poolChannel[0] = parameter->getIntAttribute("Value");
						if (parameterName.compare("poolChannel1") == 0) variation[parameter->getIntAttribute("Index")].poolChannel[1] = parameter->getIntAttribute("Value");
						if (parameterName.compare("poolChannel2") == 0) variation[parameter->getIntAttribute("Index")].poolChannel[2] = parameter->getIntAttribute("Value");
						if (parameterName.compare("poolChannel3") == 0) variation[parameter->getIntAttribute("Index")].poolChannel[3] = parameter->getIntAttribute("Value");

						if (parameterName.compare("variationName") == 0) variation[parameter->getIntAttribute("Index")].name = parameter->getStringAttribute("Value");
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

						if (parameterName.compare("offsetVelocity") == 0) variation[parameter->getIntAttribute("Index")].offsetVelocity = parameter->getBoolAttribute("Value");
						if (parameterName.compare("velocityOffset0") == 0) variation[parameter->getIntAttribute("Index")].velocityOffset[0] = parameter->getIntAttribute("Value");
						if (parameterName.compare("velocityOffset1") == 0) variation[parameter->getIntAttribute("Index")].velocityOffset[1] = parameter->getIntAttribute("Value");
						if (parameterName.compare("velocityOffset2") == 0) variation[parameter->getIntAttribute("Index")].velocityOffset[2] = parameter->getIntAttribute("Value");
						if (parameterName.compare("velocityOffset3") == 0) variation[parameter->getIntAttribute("Index")].velocityOffset[3] = parameter->getIntAttribute("Value");

						if (parameterName.compare("swingQ") == 0) variation[parameter->getIntAttribute("Index")].swingQ = parameter->getIntAttribute("Value");

						// automation
						if (parameterName.compare("variationSwitch") == 0)  variationSwitch[parameter->getIntAttribute("Index")] = parameter->getIntAttribute("Value");
						if (parameterName.compare("ccVariationSwitching") == 0)  ccVariationSwitching = (bool)parameter->getIntAttribute("Value");
						if (parameterName.compare("variationSwitchChannel") == 0)  variationSwitchChannel = parameter->getIntAttribute("Value");

						parameter = parameter->getNextElement();
					}
					break;
				}

				child = child->getNextElement();

			} // foreach parameters

			// do some housekeeping to set variables always contained in the model object

			// if there are no patterns; all variations need to be disabled!!!

			if (patternList.getNumItems() == 0)
			{
				for (int i = 0; i < 8; i++)
					variation[i].enabled = false;
			}

			for (int i = 0; i < 8; i++)
			{
				// housekeeping auxciliary values for pools
				if (variation[i].patternToUse >=0)
					variation[i].pattern.patLenInTicks = patternData[variation[i].patternToUse].patLenInTicks;
			}

			

		} // end block protected by lock

		variationSelected = 0;
		setRunState(Topiary::Stopped);
		setOverrideHostTransport(rememberOverride);

		// generate the variations
		generateAllVariations(-1);
		
		// inform editor
		broadcaster.sendActionMessage(MsgLoad); // tell everyone we've just loaded something (table headers need to be re-set
		broadcaster.sendActionMessage(MsgTransport);
		broadcaster.sendActionMessage(MsgLog);
		broadcaster.sendActionMessage(MsgNotePool);
		broadcaster.sendActionMessage(MsgPatternList);
		broadcaster.sendActionMessage(MsgPattern);
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
		ticksPerBeat = Topiary::TicksPerQuarter * 4 / denominator;
		samplesPerTick = (double)sampleRate / ((double)ticksPerBeat * BPM / 60.0);
		Log("Samples per tick: " + String(samplesPerTick), Topiary::LogType::Info);

	} // recalcRealTime

	///////////////////////////////////////////////////////////////////////

	void initializeVariation(int i)
	{
		// full initialize to nothing!!!
		variation[i].patternToUse = -1;				// index in patterndata
		
		// be sure to keep the actual pattern here!!! if ever changed it will get overwritten by generateVariation

		for (int j = 0; j < 4; j++)
		{
			variation[i].enablePool[j] = true;
			
		}
		variation[i].lenInMeasures = 0;
		variation[i].pattern.patLenInTicks = 0;
		variation[i].type = Topiary::VariationTypeSteady;				// indicates that once pattern played, we no longer generate notes! (but we keep running (status Ended) till done

		variation[i].name = "Variation " + String(i + 1);
		variation[i].enabled = false;
		for (int j = 0; j < 4; j++)
		{
			if (j)  // always initialize pool1 as enabled
				variation[i].enablePool[j] = false;
			else
				variation[i].enablePool[j] = true;
			variation[i].poolChannel[j] = 10;
			
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
		
		// don't forget to set the length!!!
		
		if (p != -1)
		{
			variation[v].lenInMeasures = getPatternLengthInMeasures(p);
			variation[v].pattern.patLenInTicks = patternData[variation[v].patternToUse].patLenInTicks;
		}

	} // initializePatternToVariations

	///////////////////////////////////////////////////////////////////////

	void generatePool(int v, int p, int poolNote[128], int measureToGenerate)
	{
		// generates the pool notes in pool p for variation v
		// v is variation to generate
		// p is poolnumber
		// if measureToGenerate == -1 it regenerates for all measures
		// if not it generates for measureToGenerate - but if measure too long it resets to 0

		Logger::outputDebugString("Generating pool");
		
		Random randomizer;
		TopiaryVariation *var = &(variation[v].pattern);
		TopiaryPattern *pat = &(patternData[variation[v].patternToUse]);
		
		var->patLenInTicks = pat->patLenInTicks; // make sure length is correct

		int note;
		int vIndex;
		
		if (!variation[v].enabled) return; // might not be decently initialized!!!
		if (!variation[v].enablePool[p]) return; // might not be decently initialized!!!

	
		for (int pIndex=0; pIndex< pat->numItems; pIndex++)
		{
		   if ( ((*pat).dataList[pIndex].measure == measureToGenerate) || (measureToGenerate == -1) )
		   {
			note = (*pat).dataList[pIndex].note;

			if (poolNote[note] == p) // note in this pool
			{
				// note randomization logic
				bool doNote = true;
				vIndex = var->findID(pIndex+1);

				if (variation[v].randomizeNotes && variation[v].randomizeNotePool[p])
				{
					float rnd = randomizer.nextFloat();
					// decide whether we're generating this one or not
					if (rnd > ((float) variation[v].randomizeNotesValue / 100))
					{
						doNote = false;
					}
					
				};

				// if (doNote) we will generate a note on event; if not it will be a NOP event
				if (doNote)
					var->dataList[vIndex].midiType = Topiary::MidiType::NoteOn;
				else
					var->dataList[vIndex].midiType = Topiary::MidiType::NOP;

				var->dataList[vIndex].note = note;
				var->dataList[vIndex].channel = variation[v].poolChannel[p];
				var->dataList[vIndex].length = pat->dataList[pIndex].length;
				var->dataList[vIndex].velocity = pat->dataList[pIndex].velocity;
				int timestamp  = pat->dataList[pIndex].timestamp;			
				var->dataList[vIndex].timestamp = timestamp;
				if (doNote)
					if (variation[v].randomizeVelocity && (variation[v].velocityPlus || variation[v].velocityMin)  && variation[v].velocityPool[p])
					{
						int vel = pat->dataList[pIndex].velocity;
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

						if (variation[v].offsetVelocity)
							vel = vel + variation[v].velocityOffset[p];

						if (vel > 127) vel = 127;
						else if (vel < 0) vel = 0;
						var->dataList[vIndex].velocity = vel;

					} // velocity randomization
				
				if ((variation[v].offsetVelocity) && (doNote))
				{
					int vel = var->dataList[vIndex].velocity;
					vel = vel + variation[v].velocityOffset[p];

					if (vel > 127) vel = 127;
					else if (vel < 0) vel = 0;
					var->dataList[vIndex].velocity = vel;
				}
				if (variation[v].swing && variation[v].swingPool[p] && doNote)
				{
						// recalc the timestamp, based on the swing lookup table
						//Logger::outputDebugString("Timestamp pre " + String(timestamp));
					int base;

					//double debugFirst;
					//int debugSecond;
					//int debugThird;
					if (variation[v].swingQ == TopiaryBeatsModel::SwingQButtonIds::SwingQ8)
					{
						base = ((int)floor(timestamp / (Topiary::TicksPerQuarter / 2))) * (int)(Topiary::TicksPerQuarter / 2);
						//debugFirst = floor(timestamp / (Topiary::TicksPerQuarter / 2));
						//debugSecond = (int)(Topiary::TicksPerQuarter / 2);
						//debugThird = (int)floor(timestamp / (Topiary::TicksPerQuarter / 2));

					}
					else
						base = ((int)floor(timestamp / Topiary::TicksPerQuarter)) * Topiary::TicksPerQuarter;

					//Logger::outputDebugString("Remainder pre " + String(timestamp-base));

					int remainder = swing(timestamp - base, variation[v].swingValue, variation[v].swingQ);
					var->dataList[vIndex].timestamp = base + remainder;

					//Logger::outputDebugString("Remainder post" + String (remainder));
					//Logger::outputDebugString("New Timestamp " + String(base + remainder) + " (DIFF: " + String (timestamp-base-remainder));

					jassert((remainder) <= Topiary::TicksPerQuarter);
					jassert((base + remainder) >= 0);
						
				} // swing

				// we apply timing randomization AFTER possible swing !!!
				if (doNote)
					if (variation[v].randomizeTiming && (variation[v].timingPlus || variation[v].timingMin) && variation[v].timingPool[p])
					{
						timestamp = var->dataList[vIndex].timestamp;
						float rnd;
						int direction = -1;
						if (variation[v].velocityPlus && variation[v].velocityMin)
						{
							rnd = randomizer.nextFloat();
							if (rnd > 0.5)
								direction = 1;
						}
						else if (variation[v].velocityPlus)
							direction = 1;
													
						rnd = randomizer.nextFloat();
						//int debug = direction * rnd * Topiary::TicksPerQuarter * variation[v].timingValue /100;
						timestamp = timestamp + (int)(direction * rnd * Topiary::TicksPerQuarter * variation[v].timingValue / 800);
						if (timestamp < 0) timestamp = 0;

						if (timestamp > (var->patLenInTicks-1)) timestamp = var->patLenInTicks - 1; // lenInTicks -1 because we need time for the note off event

						// make sure we do not run over the patternlenght with note off
						int length = var->dataList[vIndex].length;
						if ((length + timestamp) >= var->patLenInTicks)
						{
							length = var->patLenInTicks - timestamp - 1;
							var->dataList[vIndex].length = length;
							jassert(length > 0);
						}
						var->dataList[vIndex].timestamp = timestamp;

					} // end randomize timing

				//Logger::outputDebugString("Generating Note " + String(var->dataList[vIndex].note) + " timestamp " + String(var->dataList[vIndex].timestamp));
				//auto debug = 0;
				
			} // end if note in this pool
		     } // end check for measureToGenerate
		}  // for children in original pattern
		
	} // generatePool

	///////////////////////////////////////////////////////////////////////

	int swing(int value, int deviation, int q) 
		// q indicates whetherwe are swinging quarter or eight based
	{ 
		// value between 0 - ticksPerBeat; deviation -100 to 100
		jassert(deviation < 101);
		jassert(deviation > -101);
		jassert(value >= 0);

		int maxMidiValue;
		
		if (q == TopiaryBeatsModel::SwingQButtonIds::SwingQ8)
			maxMidiValue = (int) (Topiary::TicksPerQuarter/2) - 1;
		else
			maxMidiValue = Topiary::TicksPerQuarter - 1;

		jassert(value <= maxMidiValue);

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

	

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(TopiaryBeatsModel)
};

