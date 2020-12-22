/////////////////////////////////////////////////////////////////////////////
/*
This file is part of Topiary Beatz, Copyright Tom Tollenaere 2018-21.

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
#include "TopiaryPoolList.h"
#include "../../Topiary/Source/Model/TopiaryPattern.h"
#include "../../Topiary/Source/Model/TopiaryVariation.h"
#include "../../Topiary/Source/Model/TopiaryNoteOffBuffer.h"
#include "../../Topiary/Source/Components/TopiaryMidiLearnEditor.h"
#include "../../Topiary/Source/Model/TopiaryPatternList.h"

class TopiaryBeatsModel : public TopiaryModel
{
public:


	TopiaryBeatsModel();
	~TopiaryBeatsModel();

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
	void setRunState(int n) override;
	
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

	void generateVariation(int i, int eightToGenerate); // Generates the variation;
	void generateAllVariations(int measureToGenerate);

	void setOverrideHostTransport(bool o) override;
	void setNumeratorDenominator(int nu, int de) override;

	struct Variation {

		int patternToUse;					// index in patterndata
		int lenInMeasures;
		TopiaryVariation pattern;			// pattern  events in the variation
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

	void setLockState(bool state);
	bool getLockState();
	void saveState();
	void restoreState();
	void processPluginParameters();

#define NUMBEROFQUANTIZERS 10

	juce::AudioParameterFloat* rndNoteOccurrence;
	juce::AudioParameterBool* boolNoteOccurrence;

	juce::AudioParameterFloat* swingAmount;
	juce::AudioParameterBool* boolSwing;

	juce::AudioParameterFloat* rndVelocity;
	juce::AudioParameterBool* boolVelocity;

	juce::AudioParameterFloat* rndTiming;
	juce::AudioParameterBool* boolTiming;

private:
	TopiaryPatternList patternList;
	TopiaryPoolList poolList;
	TopiaryPattern patternData[8];
	Variation variation[8];  // struct to hold variation detail
	TopiaryNoteOffBuffer noteOffBuffer;
	bool fixedOutputChannels; // force those the same for every variation
	
	float prevRndNoteOccurrence;
	int prevBoolNoteOccurrence;
	float prevSwingAmount;
	int prevBoolSwing;
	float prevRndVelocity;
	int prevBoolVelocity;
	float prevRndTiming;
	int prevBoolTiming;

	///////////////////////////////////////////////////////////////////////

#include "..//..//Topiary/Source/Model/LoadMidiPattern.cpp.h"	
#include "..//..//Topiary/Source/Model/Swing.cpp.h"
#include "..//..//Topiary/Source/Model/RecalcRealTime.cpp.h"

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

		addToModel(parameters, name, "name");
		addToModel(parameters, BPM, "BPM");
		addToModel(parameters, numerator, "numerator");
		addToModel(parameters, denominator, "denominator");
		addToModel(parameters, switchVariation, "switchVariation");
		addToModel(parameters, runStopQ, "runStopQ");
		addToModel(parameters, variationStartQ, "variationStartQ");
		addToModel(parameters, WFFN, "WFFN");
		addToModel(parameters, name, "name");
		addToModel(parameters, overrideHostTransport, "overrideHostTransport");
		addToModel(parameters, notePassThrough, "notePassThrough");
		addToModel(parameters, fixedOutputChannels, "fixedOutputChannels");
		addToModel(parameters, logMidiIn, "logMidiIn");
		addToModel(parameters, logMidiOut, "logMidiOut");
		addToModel(parameters, logDebug, "logDebug");
		addToModel(parameters, logTransport, "logTransport");
		addToModel(parameters, logVariations, "logVariations");
		addToModel(parameters, logInfo, "logInfo");
		addToModel(parameters, filePath, "filePath");

		addToModel(parameters, poolList.getNumItems(), "numPoolNotes");
		
		addToModel(parameters, midiChannelListening, "variationSwitchChannel");
		addToModel(parameters, ccVariationSwitching, "ccVariationSwitching");
		addToModel(parameters, lockState, "lockState");

		for (int i = 0; i < 8; i++) {
			
			// Variations
			
			addToModel(parameters, variation[i].lenInMeasures, "lenInMeasures", i);
			addToModel(parameters, variation[i].patternToUse, "patternToUse", i);
			addToModel(parameters, variation[i].name, "variationName", i);
			addToModel(parameters, variation[i].enabled, "variationEnabled", i);
			addToModel(parameters, variation[i].type, "variationType", i);
			addToModel(parameters, variation[i].enablePool[0], "enablePool0", i);
			addToModel(parameters, variation[i].enablePool[1], "enablePool1", i);
			addToModel(parameters, variation[i].enablePool[2], "enablePool2", i);
			addToModel(parameters, variation[i].enablePool[3], "enablePool3", i);

			addToModel(parameters, variation[i].poolChannel[0], "poolChannel0", i);
			addToModel(parameters, variation[i].poolChannel[1], "poolChannel1", i);
			addToModel(parameters, variation[i].poolChannel[2], "poolChannel2", i);
			addToModel(parameters, variation[i].poolChannel[3], "poolChannel3", i);

			addToModel(parameters, variation[i].randomizeNotes, "randomizeNotes", i);
			addToModel(parameters, variation[i].randomizeNotesValue, "randomizeNotesValue", i);
			addToModel(parameters, variation[i].randomizeNotePool[0], "randomizeNotePool0", i);
			addToModel(parameters, variation[i].randomizeNotePool[1], "randomizeNotePool1", i);
			addToModel(parameters, variation[i].randomizeNotePool[2], "randomizeNotePool2", i);
			addToModel(parameters, variation[i].randomizeNotePool[3], "randomizeNotePool3", i);

			addToModel(parameters, variation[i].swing, "swing", i);
			addToModel(parameters, variation[i].swingValue, "swingValue", i);
			addToModel(parameters, variation[i].swingPool[0], "swingPool0", i);
			addToModel(parameters, variation[i].swingPool[1], "swingPool1", i);
			addToModel(parameters, variation[i].swingPool[2], "swingPool2", i);
			addToModel(parameters, variation[i].swingPool[3], "swingPool3", i);

			addToModel(parameters, variation[i].randomizeVelocity, "randomizeVelocity", i);
			addToModel(parameters, variation[i].velocityValue, "velocityValue", i);
			addToModel(parameters, variation[i].velocityPool[0], "velocityPool0", i);
			addToModel(parameters, variation[i].velocityPool[1], "velocityPool1", i);
			addToModel(parameters, variation[i].velocityPool[2], "velocityPool2", i);
			addToModel(parameters, variation[i].velocityPool[3], "velocityPool3", i);
			addToModel(parameters, variation[i].velocityPlus, "velocityPlus", i);
			addToModel(parameters, variation[i].velocityMin, "velocityMin", i);

			addToModel(parameters, variation[i].randomizeTiming, "randomizeTiming", i);
			addToModel(parameters, variation[i].timingValue, "timingValue", i);
			addToModel(parameters, variation[i].timingPool[0], "timingPool0", i);
			addToModel(parameters, variation[i].timingPool[1], "timingPool1", i);
			addToModel(parameters, variation[i].timingPool[2], "timingPool2", i);
			addToModel(parameters, variation[i].timingPool[3], "timingPool3", i);
			addToModel(parameters, variation[i].timingPlus, "timingPlus", i);
			addToModel(parameters, variation[i].timingMin, "timingMin", i);

			addToModel(parameters, variation[i].offsetVelocity, "offsetVelocity", i);
			addToModel(parameters, variation[i].velocityOffset[0], "velocityOffset0", i);
			addToModel(parameters, variation[i].velocityOffset[1], "velocityOffset1", i);
			addToModel(parameters, variation[i].velocityOffset[2], "velocityOffset2", i);
			addToModel(parameters, variation[i].velocityOffset[3], "velocityOffset3", i);
			
			addToModel(parameters, variation[i].swingQ, "swingQ", i);

			// automation
			addToModel(parameters, variationSwitch[i], "variationSwitch", i);

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
						if (parameterName.compare("lockState") == 0) lockState = parameter->getBoolAttribute("Value");

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
						if (parameterName.compare("variationSwitchChannel") == 0)  midiChannelListening = parameter->getIntAttribute("Value");

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
		broadcaster.sendActionMessage(MsgLockState);	// inform editor of variation automation settings;	
		
	} // restoreParametersToModel

	//////////////////////////////////////////////////////////////////////////////////////////////////
	// Generator stuff	
	//////////////////////////////////////////////////////////////////////////////////////////////////

	

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

	void generatePool(int v, int p, int poolNote[128], int eightToGenerate, int tickFrom, int tickTo)  
	{
		// generates the pool notes in pool p for variation v
		// v is variation to generate
		// p is poolnumber
		// if eightToGenerate == -1 it regenerates for all measures
		// if not it generates eight' measure "eighttoGenerate" 

		Logger::outputDebugString("Generating pool");

		Random randomizer;
		TopiaryVariation* var = &(variation[v].pattern);
		TopiaryPattern* pat = &(patternData[variation[v].patternToUse]);

		var->patLenInTicks = pat->patLenInTicks; // make sure length is correct

		int note;
		int vIndex;

		if (!variation[v].enabled) return; // might not be decently initialized!!!
		if (!variation[v].enablePool[p]) return; // might not be decently initialized!!!


		for (int pIndex = 0; pIndex < pat->numItems; pIndex++)
		{
			// if (((*pat).dataList[pIndex].measure == measureToGenerate) || (measureToGenerate == -1))
			if ((((*pat).dataList[pIndex].timestamp >= tickFrom) && ((*pat).dataList[pIndex].timestamp < tickTo)) || (eightToGenerate == -1))
			{
				note = (*pat).dataList[pIndex].note;

				if (poolNote[note] == p) // note in this pool
				{
					// note randomization logic
					bool doNote = true;
					vIndex = var->findID(pIndex + 1);

					if (variation[v].randomizeNotes && variation[v].randomizeNotePool[p])
					{
						float rnd = randomizer.nextFloat();
						// decide whether we're generating this one or not
						if (rnd > ((float)variation[v].randomizeNotesValue / 100))
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
					int timestamp = pat->dataList[pIndex].timestamp;
					var->dataList[vIndex].timestamp = timestamp;
					if (doNote)
						if (variation[v].randomizeVelocity && (variation[v].velocityPlus || variation[v].velocityMin) && variation[v].velocityPool[p])
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
							vel = vel + (int)(direction * rnd * 128 * variation[v].velocityValue / 50);   // originally / 100 but I want more of an effect

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
						if (variation[v].swingQ == Topiary::SwingQButtonIds::SwingQ8)
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

							if (timestamp > (var->patLenInTicks - 1)) timestamp = var->patLenInTicks - 1; // lenInTicks -1 because we need time for the note off event

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
	

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(TopiaryBeatsModel)
};

