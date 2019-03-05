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

#include "TopiaryBeatsModel.h"

// following has std model code that can be included (cannot be in TopiaryModel because of variable definitions)
#include "../../Topiary/Source/TopiaryTable.cpp"
#include"../../Topiary/Source/TopiaryModelIncludes.cpp"


void TopiaryBeatsModel::saveStateToMemoryBlock(MemoryBlock& destData)
{
	addParametersToModel();  // this adds an XML element "Parameters" to the model
	AudioProcessor::copyXmlToBinary(*model, destData);
	model->deleteAllChildElementsWithTagName("Parameters");
}

/////////////////////////////////////////////////////////////////////////

void TopiaryBeatsModel::restoreStateFromMemoryBlock(const void* data, int sizeInBytes)
{
	//return; 
	model.reset(AudioProcessor::getXmlFromBinary(data, sizeInBytes));
	restoreParametersToModel();
}

/////////////////////////////////////////////////////////////////////////

TopiaryBeatsModel::TopiaryBeatsModel()
{

	Log(String("Topiary Beatz V ") + String(xstr(JucePlugin_Version)) + String(" (c) Tom Tollenaere 2018-2019."), Topiary::LogType::License);
	Log(String(""), Topiary::LogType::License);
	Log(String("Topiary Beatz is free software : you can redistribute it and/or modify"), Topiary::LogType::License);
	Log(String("it under the terms of the GNU General Public License as published by"), Topiary::LogType::License);
	Log(String("the Free Software Foundation, either version 3 of the License, or"), Topiary::LogType::License);
	Log(String("(at your option) any later version."), Topiary::LogType::License);
	Log(String(""), Topiary::LogType::License);
	Log(String("Topiary Beatz is distributed in the hope that it will be useful,"), Topiary::LogType::License);
	Log(String("but WITHOUT ANY WARRANTY; without even the implied warranty of"), Topiary::LogType::License);
	Log(String("MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the"), Topiary::LogType::License);
	Log(String("GNU General Public License for more details."), Topiary::LogType::License);
	Log(String(""), Topiary::LogType::License);
	Log(String("VST PlugIn Technology by Steinberg Media Technologies."), Topiary::LogType::License);
	Log(String(""), Topiary::LogType::License);

	/////////////////////////////////////
	// patternListHeader initialization
	/////////////////////////////////////

	patternListHeader->setTagName("PatternListHeader");
	XmlElement* child = new XmlElement("COLUMN");
	child->setAttribute("columnId", "1");
	child->setAttribute("name", "ID");
	child->setAttribute("width", "20");
	child->setAttribute("type", "int");
	patternListHeader->addChildElement(child);

	child = new XmlElement((char*)"COLUMN");
	child->setAttribute("columnId", "2");
	child->setAttribute("name", "Name");
	child->setAttribute("width", "140");
	child->setAttribute("type", "char");
	patternListHeader->addChildElement(child);

	child = new XmlElement((char*)"COLUMN");
	child->setAttribute("columnId", "3");
	child->setAttribute("name", "Measures");
	child->setAttribute("width", "70");
	child->setAttribute("type", "int");
	child->setAttribute("min", "0");
	child->setAttribute("max", "100");
	child->setAttribute("editable", "false");
	patternListHeader->addChildElement(child);


	/////////////////////////////////////
	// notepoolHeader initialisation
	/////////////////////////////////////

	poolListHeader->setTagName("PoolListHeader");
	child = new XmlElement("COLUMN");
	child->setAttribute("columnId", "1");
	child->setAttribute("name", "ID");
	child->setAttribute("width", "20");
	child->setAttribute("type", "int");
	poolListHeader->addChildElement(child);

	child = new XmlElement("COLUMN");
	child->setAttribute("columnId", "2");
	child->setAttribute("name", "Note"); // actually the keybd index, eg A#3
	child->setAttribute("width", "50");
	child->setAttribute("type", "note");
	child->setAttribute("editable", "false");
	poolListHeader->addChildElement(child);

	child = new XmlElement("COLUMN");
	child->setAttribute("columnId", "3");
	child->setAttribute("name", "Label"); // actually the keybd index, eg A#3
	child->setAttribute("width", "50");
	child->setAttribute("type", "noteLabel");
	poolListHeader->addChildElement(child);

	child = new XmlElement("COLUMN");
	child->setAttribute("columnId", "4");
	child->setAttribute("name", "Description");
	child->setAttribute("width", "110");
	child->setAttribute("type", "char");
	poolListHeader->addChildElement(child);

	child = new XmlElement("COLUMN");
	child->setAttribute("columnId", "5");
	child->setAttribute("name", "Pool"); // which of the 4 pools this one is in
	child->setAttribute("width", "40");
	child->setAttribute("type", "int");
	child->setAttribute("min", "1");
	child->setAttribute("max", "4");
	poolListHeader->addChildElement(child);


	/////////////////////////////////////
	// patternHeader initialisation
	/////////////////////////////////////

	patternHeader->setTagName("PatternHeader");
	child = new XmlElement("COLUMN");
	child->setAttribute("columnId", "1");
	child->setAttribute("name", "ID");
	child->setAttribute("width", "20");
	child->setAttribute("type", "int");
	patternHeader->addChildElement(child);

	child = new XmlElement((char*)"COLUMN");
	child->setAttribute("columnId", "2");
	child->setAttribute("name", "Measure");
	child->setAttribute("width", "70");
	child->setAttribute("type", "int");
	//child->setAttribute("editable", "false");
	child->setAttribute("min", "0");
	child->setAttribute("max", "15");
	patternHeader->addChildElement(child);

	child = new XmlElement((char*)"COLUMN");
	child->setAttribute("columnId", "3");
	child->setAttribute("name", "Beat");
	child->setAttribute("width", "40");
	child->setAttribute("type", "int");
	//child->setAttribute("editable", "false");
	child->setAttribute("min", "0");
	child->setAttribute("max", String(numerator - 1));
	patternHeader->addChildElement(child);
	
	child = new XmlElement((char*)"COLUMN");
	child->setAttribute("columnId", "4");
	child->setAttribute("name", "Tick");
	child->setAttribute("width", "40");
	child->setAttribute("type", "int");
	child->setAttribute("min", "0");
	child->setAttribute("max", String(Topiary::TICKS_PER_QUARTER - 1));
	//child->setAttribute("editable", "false");
	patternHeader->addChildElement(child);


	child = new XmlElement((char*)"COLUMN");
	child->setAttribute("columnId", "5");
	child->setAttribute("name", "Note");
	child->setAttribute("width", "40");
	child->setAttribute("type", "note");
	child->setAttribute("editable", "false");
	patternHeader->addChildElement(child);

	child = new XmlElement((char*)"COLUMN");
	child->setAttribute("columnId", "6");
	child->setAttribute("name", "Label");
	child->setAttribute("width", "40");
	child->setAttribute("type", "noteLabel");
	//child->setAttribute("editable", "false");
	patternHeader->addChildElement(child);


	child = new XmlElement((char*)"COLUMN");
	child->setAttribute("columnId", "7");
	child->setAttribute("name", "Length");
	child->setAttribute("width", "50");
	child->setAttribute("type", "int");
	child->setAttribute("min", "0");
	child->setAttribute("max", String(10000));
	//child->setAttribute("editable", "false");
	patternHeader->addChildElement(child);

	child = new XmlElement((char*)"COLUMN");
	child->setAttribute("columnId", "8");
	child->setAttribute("name", "Velocity");
	child->setAttribute("width", "50");
	child->setAttribute("type", "int");
	//child->setAttribute("editable", "false");
	child->setAttribute("min", "0");
	child->setAttribute("max", "127");

	patternHeader->addChildElement(child);

	/*
	child = new XmlElement((char*)"COLUMN");
	child->setAttribute("columnId", "8");
	child->setAttribute("name", "Timestamp");
	child->setAttribute("width", "80");
	child->setAttribute("type", "int");
	child->setAttribute("editable", "false");
	patternHeader->addChildElement(child);
	*/
	/////////////////////////////////////
	// variations initialisation
	/////////////////////////////////////

	for (int i = 0; i < 8; ++i)
	{
		variation[i].patternToUse = -1;				// index in patterndata
		variation[i].pattern = nullptr;			// will be set when editing the variation, or when loading state
		

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
		variation[i].ending = false;									// indicates that once pattern played, we no longer generate notes! (but we keep running (status Ended) till done
		variation[i].ended = false;

		variation[i].name = "Variation " + String(i + 1);
		variation[i].enabled = false;
		variation[i].randomizeNotes = false;
		variation[i].randomizeNotesValue = 100;
		variation[i].swing = false;
		variation[i].swingValue = 0;
		variation[i].randomizeVelocity = false;
		variation[i].velocityValue = 0;
		variation[i].velocityPlus = true;
		variation[i].velocityMin = true;

		variation[i].randomizeTiming = false;
		variation[i].timingValue = 0;
		variation[i].timingPlus = true;
		variation[i].timingMin = true;

		for (int j = 0; j < 4; j++)
		{
			variation[i].enablePool[j] = false;
			variation[i].poolChannel[j] = 10;
			variation[i].measureFrom[j] = 0;
			variation[i].measureTo[j] = -1;
			variation[i].beatFrom[j] = 0;
			variation[i].beatTo[j] = -1;
			variation[i].tickFrom[j] = 0;
			variation[i].tickTo[j] = -1;

			variation[i].randomizeNotePool[j] = false;
			variation[i].swingPool[j] = false;
			variation[i].velocityPool[j] = false;
			variation[i].timingPool[j] = false;
		}
	}

	/////////////////////////////////////
	// Patterndata initialization
	/////////////////////////////////////

	int ccStart = 22;

	for (int i = 0; i < 8; ++i)
	{
		auto ch = new XmlElement("Pattern");
		patternData[i].noteData = ch;
		patternData[i].noteData->setAttribute("Index", String(i));

		variationSwitch[i] = ccStart;
		ccStart++;
	}

	ccVariationSwitching = true;
	variationSwitchChannel = 0;

	model.reset(new XmlElement("beatsModel")); // keep all lists under the main model object to avoid leaking
	model->setAttribute("Version", JucePlugin_VersionString);
	model->addChildElement(patternHeader);
	model->addChildElement(patternListHeader);
	model->addChildElement(patternListData);
	model->addChildElement(poolListData);
	model->addChildElement(poolListHeader);
	for (int i = 0; i < 8; i++) {
		model->addChildElement(patternData[i].noteData);
	}

	threadRunnerState = Topiary::ThreadRunnerState::NothingToDo;
	topiaryThread.notify();

} // TopiaryBeatsModel

//////////////////////////////////////////////////////////////////////////////////////////////////////


TopiaryBeatsModel::~TopiaryBeatsModel()
{
	XmlElement toDelete("toDelete");

	for (int i = 0; i < 8; ++i)
	{
		// avoid dangling pointers
		variation[i].currentPatternChild = nullptr;
		
		// ensure these will get destroyed
		toDelete.addChildElement(variation[i].pattern);
		
	}
} //~TopiaryBeatsModel


/////////////////////////////////////////////////////////////////////////

void TopiaryBeatsModel::getMasterModel(XmlElement** h, XmlElement** d, XmlElement** ph, XmlElement** pd) {
	// returns the data andheaderlists
	*h = patternListHeader;
	*d = patternListData;
	*ph = poolListHeader;
	*pd = poolListData;

} // getMasterModelModel

/////////////////////////////////////////////////////////////////////////

void TopiaryBeatsModel::getPatternModel(int patternIndex, XmlElement** h, XmlElement** d)
{
	jassert(patternIndex >= -1);
	if (numPatterns > 0)
	{
		jassert(patternIndex < numPatterns);
	}

	*h = patternHeader;
	if (patternIndex == -1)
		*d = nullptr;
	else
		*d = patternData[patternIndex].noteData;

	//Logger::writeToLog("Notedata ["+String(patternIndex)+"] ---------------------------------------------------->");
	//String myXmlDoc2 = patternData[patternIndex].noteData->createDocument(String());
	//Logger::writeToLog(myXmlDoc2);
	//Logger::writeToLog("Notedata ---------------------------------------------------->");

} // getPatternModel

///////////////////////////////////////////////////////
// pattern methods
///////////////////////////////////////////////////////

int TopiaryBeatsModel::getPatternLengthInMeasures(int i)
{
	// length of pattern in Measures

	jassert( (i<8) && (i>=-1));

	auto pattern = patternListData->getChildByAttribute("ID", String(i+1)); // pattern IDS are 1-8
	if (pattern != nullptr)
		return pattern->getIntAttribute("Measures");
	else
		return 1;  // this should only happen during initialisation, before data is initialized (or restored from state)
} // getPatternLengthInMeasures

//////////////////////////////////////////////////////////////////////

void TopiaryBeatsModel::setPatternLengthInMeasures(int i, int l)
{
	// l: length of pattern in Measures
	// i: pattern index

	jassert((i < 8) && (i >= 0));

	auto pattern = patternListData->getChildByAttribute("ID", String(i + 1)); // pattern IDS are 1-8
	if (pattern != nullptr)
		return pattern->setAttribute("Measures",l);
	else
		jassert(false);  // this should only happen during initialisation, before data is initialized (or restored from state)

} // setPatternLengthInMeasures

///////////////////////////////////////////////////////////////////////

void TopiaryBeatsModel::deletePattern(int deletePattern)
{
	jassert(deletePattern > -1); // has to be a valid row to delete
	jassert(deletePattern < patternListData->getNumChildElements()); // number has to be smaller than number of children (it starts at 0)
	
	// remove from the patternlist - find it first
	auto child = patternListData->getChildElement(deletePattern);
	removeFromModel("PatternData", child);

	// now delete the pattern itself (the NOTEDATA in patternData[i])
	// careful! the pattern noteData and noteOffData now needs to be reshuffled - we do that a bit further down!

	//String myXmlDoc2;

	//Logger::outputDebugString("Pattern[" + String(deletePattern) + "] before delete children --------------------------------------");
	//myXmlDoc2 = patternData[deletePattern].noteData->createDocument(String());
	//Logger::writeToLog(myXmlDoc2);
	//Logger::outputDebugString("Pattern[" + String(deletePattern+1) + "] before delete children --------------------------------------");
	//myXmlDoc2 = patternData[deletePattern+1].noteData->createDocument(String());
	//Logger::writeToLog(myXmlDoc2);

	patternData[deletePattern].noteData->deleteAllChildElements();
	patternData[deletePattern].patLenInTicks = 0; 
	patternData[deletePattern].notesRealID = 1;
	patternData[deletePattern].numNotes = 0;

	//Logger::outputDebugString("Pattern[" + String(deletePattern) + "] AFTER delete children --------------------------------------");
	//myXmlDoc2 = patternData[deletePattern].noteData->createDocument(String());
	//Logger::writeToLog(myXmlDoc2);
	//Logger::outputDebugString("Pattern[" + String(deletePattern + 1) + "] AFTER delete children --------------------------------------");
	//myXmlDoc2 = patternData[deletePattern + 1].noteData->createDocument(String());
	//Logger::writeToLog(myXmlDoc2);
	
	// shift all patterns above the one deleted, down by 1

	for (int j = deletePattern; j < 7; j++) // <7 because we are swapping  with pattern j+1 !!!
	{
		//Logger::outputDebugString("Pattern[" + String(j) + "] before swap");
		//myXmlDoc2 = patternData[j].noteData->createDocument(String());
		//Logger::writeToLog(myXmlDoc2);
		swapXmlElementPointers(&(patternData[j].noteData), &(patternData[j + 1].noteData));
		patternData[j].patLenInTicks = patternData[j+1].patLenInTicks;
		patternData[j].notesRealID = patternData[j+1].notesRealID;
		patternData[j].numNotes = patternData[j+1].numNotes;

		// also lower the index attribute in the patterndata!!!
		// patternData[j].noteData->setAttribute("Index", patternData[j].noteData->getIntAttribute("Index") - 1);

		//Logger::outputDebugString("-------------------------------------------");
		//Logger::outputDebugString("Pattern[" + String(j) + "] after swap");
		//myXmlDoc2 = patternData[j].noteData->createDocument(String());
		//Logger::writeToLog(myXmlDoc2);
		//Logger::outputDebugString("===========================================");
	}

	// and clean  up the indexes in the patterndata
	for (int p=0; p<8; p++)
		patternData[p].noteData->setAttribute("Index", p);


	Log("Pattern "+String(deletePattern)+" deleted.", Topiary::LogType::Info);
	// broadcaster.sendActionMessage(MsgMasterTables); not needed done in editor

	// if there are variations that use this pattern, they need to be re-initialized (send in -1 in the initialize code)
	for (int j = 0; j < 8; j++)
	{
		if (variation[j].patternToUse == deletePattern)
			initializePatternToVariation(-1, j);
	}
			
	broadcaster.sendActionMessage(MsgVariationEnables); // may need to disable the variation!

	// if there are variations that use patterns higher than this one, lower their patternToUse by -1!
	// patternToUse  is pattern number, not index i// and also shift the noteOn/Off data down!

	for (int j = 0; j < 8; j++)
	{
		if (variation[j].patternToUse > (deletePattern))
		{
			variation[j].patternToUse--;
		}
	}
	broadcaster.sendActionMessage(MsgVariationDefinition);  // something may have changed to the currently shown variation (it might be disabled)
	

} // deletePattern

///////////////////////////////////////////////////////////////////////

void TopiaryBeatsModel::addPattern()
{
	int i = patternListData->getNumChildElements();
	
	if (i == 8)
	{
		Log("Number of patterns is limited to 8.", Topiary::LogType::Warning);
		return;
	}

	XmlElement *newPattern = addToModel("PatternData");
	newPattern->setAttribute("ID", String(i + 1));
	newPattern->setAttribute("Name", "-- empty --");
	newPattern->setAttribute("Measures", "1");
	patternData[i].patLenInTicks = Topiary::TICKS_PER_QUARTER*numerator;

	Log("New empty pattern created.", Topiary::LogType::Info);

	if (i == 0)
		initializeVariations();

	//broadcaster.sendActionMessage(MsgMasterTables); not needed done in editor
} // addPattern

///////////////////////////////////////////////////////////////////////

bool TopiaryBeatsModel::insertPatternFromFile(int patternIndex)
{   // patternIndex starts at 0
	jassert(patternIndex > -1);  // nothing selected in the model

	auto directory = File::getSpecialLocation(File::userHomeDirectory);

	if (filePath.compare("") == 0)
		directory = File::File(filePath);

	FileChooser myChooser("Please select MIDI file to load...", filePath, "*.mid");

	bool success = false;

	if (myChooser.browseForFileToOpen())
	{
		auto f = myChooser.getResult();

		filePath = f.getParentDirectory().getFullPathName();

		jassert(patternIndex > -1);  // nothing selected in the model

		numPatterns = patternListData->getNumChildElements();
		jassert(patternIndex < numPatterns);

		XmlElement* patternParent = patternListData->getChildElement(patternIndex);

		// now the pattern of the DATA objects; i.e; the PATTERN object is in patternData[patternIndex].noteData;
		auto pattern = patternData[patternIndex].noteData;
		// if there is data in the parent, delete it!
		if (pattern->getNumChildElements() != 0) pattern->deleteAllChildElements();

		int patternMeasures = 0;
		int lenInTicks = 0;
		success = loadMidiDrumPattern(f, patternData[patternIndex].noteData, patternIndex, patternMeasures, lenInTicks);

		if (success)
		{
			// all OK now, so we set the length and the name now
			// CAREFUL - len in measures & name are in XML, patlenInTicks is in the pattern[] structure!! (because that one is not edited in a table !!!!
			patternData[patternIndex].patLenInTicks = lenInTicks;
			patternParent->setAttribute("Name", f.getFileName());
			patternParent->setAttribute("Measures", patternMeasures);
			
			rebuildPool(true);
			//broadcaster.sendActionMessage(MsgMasterTables); not needed; done in the editor

			bool deassigned = false;

			// if any variations use this pattern, unassign them 
			for (int v = 0; v < 8; v++)
			{
				if (variation[v].patternToUse == patternIndex)
				{
					initializePatternToVariation(-1, v);
					Log("Variation " + String(v + 1) + " deassigned from this pattern.", Topiary::LogType::Warning);
					broadcaster.sendActionMessage(MsgWarning);  // warn the user in the header
					deassigned = true;
				}
			}

			if (deassigned)
			{
				broadcaster.sendActionMessage(MsgVariationDefinition);
			}
			
		}
		
	}
	else
		success = false;

	return success;

} // insertPatternFromFile

///////////////////////////////////////////////////////////////////////


XmlElement* TopiaryBeatsModel::addToModel(char* type, int index)
{   // this one has an int parameter, because note data is stored in array[8] !!!
	if (!strcmp(type, "NOTEDATA"))
	{
		XmlElement *child = new XmlElement("DATA");
		child->setAttribute("REALID", patternData[index].notesRealID);
		patternData[index].noteData->addChildElement(child);

		patternData[index].numNotes++;
		patternData[index].notesRealID++;
		return child;
	}

	jassert(false); // wrong type as input!!!
	return nullptr;
} // addToModel

///////////////////////////////////////////////////////////////////////

XmlElement* TopiaryBeatsModel::addToModel(char* type)
{
	if (!strcmp(type, "PatternData"))
	{
		XmlElement *child = new XmlElement("PatternData");
		child->setAttribute("REALID", patternsRealID);
		patternListData->addChildElement(child);
		numPatterns = patternListData->getNumChildElements();
		
		patternsRealID++;
		return child;
	}
	if (!strcmp(type, "PoolData"))
	{
		XmlElement *child = new XmlElement("PoolData");
		child->setAttribute("REALID", poolNotesRealID);
		poolListData->addChildElement(child);

		numPoolNotes++;
		poolNotesRealID++;
		return child;
	}

	jassert(false); // wrong type as input!!!
	return nullptr;
} // addToModel

///////////////////////////////////////////////////////////////////////

void TopiaryBeatsModel::removeFromModel(char *type, XmlElement *child)
{
	if (!strcmp(type, "PatternData"))
	{
		//auto patternDataID = child->getIntAttribute("ID");
		patternListData->removeChildElement(child, true);
		//now renumber by REALID
		renumberByID(patternListData);
		numPatterns = patternListData->getNumChildElements();
		// removing the patterns  NOT DONE HERE - DONE BY CALLER !!!!
		
		return;
	}
	if (!strcmp(type, "PoolData"))
	{
		auto poolDataID = child->getStringAttribute("ID");

		poolListData->removeChildElement(child, true);

		//now renumber by REALID
		renumberByID(poolListData);
		numPoolNotes--;

		return;
	}
	jassert(false); // wrong type as input!!!

}


//////////////////////////////////////////////////////////////////////////////////
// NOTE POOL METHODS
//////////////////////////////////////////////////////////////////////////////////

void TopiaryBeatsModel::deletePoolNote(int i)
{
	// deletes a note from the note list

	jassert(i > -1); // has to be a valid row to delete
	jassert(i < poolListData->getNumChildElements()); // number has to be smaller than number of children (it starts at 0 

	//remove from the poollist - find it first
	auto child = poolListData->getChildElement(i);
	removeFromModel("PoolData", child);
	broadcaster.sendActionMessage(MsgMaster);
	Log("Note deleted.", Topiary::LogType::Info);
	//String myXmlDoc = poolListData->createDocument(String());
	//Logger::writeToLog(myXmlDoc);

} // deletePoolNote

///////////////////////////////////////////////////////////////////////

void TopiaryBeatsModel::addPoolNote()
{
	// adds a note to the pool list

	int i = poolListData->getNumChildElements();

	XmlElement *newPool = addToModel("PoolData");
	newPool->setAttribute("ID", String(i + 1));
	newPool->setAttribute("Note", 1);
	newPool->setAttribute("Label", MidiMessage::getMidiNoteName(1, true, true, 5));
	newPool->setAttribute("Pool", "1");
	newPool->setAttribute("Channel", "10");
	//newPool->setAttribute("NoteNumber", "1");
	rebuildPool(false);
	broadcaster.sendActionMessage(MsgMaster);
	Log("Note created.", Topiary::LogType::Info);

} //addPoolNote

///////////////////////////////////////////////////////////////////////

void TopiaryBeatsModel::rebuildPool(bool clean)
{	// looks at all patterns and rebuilds the pool
	// existing pool data is not overwritten!
	// called after inserting midi file, recording and pattern edits
	// deleting a pattern does NOT rebuild the pool!
	// if (clean) then notes not in patterns are deleted
	// generate a warning if there are duplicate notes in the pool

	SortedSet<int> set;
	// loop over all patterns and make set of unique ones

	for (int p = 0; p < 8; p++)
	{
		if (patternData[p].noteData->getNumChildElements() != 0)
			forEachXmlChildElement(*patternData[p].noteData, child)
			{
				set.add(child->getIntAttribute("Note"));
			}
	}

	// loop over the pool and add "**" to descriptions (indicating note not used)
	forEachXmlChildElement(*poolListData, child)
	{
		auto label = child->getStringAttribute("Description");
		if (!label.contains("**"))
			child->setAttribute("Description", String("**") + label);
		if (clean)
			child->setTagName("DELETE"); // se we can delete is hereafter
	}

	

	// loop over the set, see if in the pool
	// if so do housekeeping (note names and GM drum map names), if needed remove "**" from label 
	// if not, add to pool
	for (auto it = set.begin(); it != set.end(); ++it)
	{
		auto poolNote = poolListData->getChildByAttribute("Note", String(*it));
		if (poolNote != nullptr)
		{
			// cleanup; translate noteNumber into readable note
			if (poolNote->getStringAttribute("Label").compare("")==0) // if no label, just add the note name
				poolNote->setAttribute("Label", MidiMessage::getMidiNoteName(*it, true, true, 5));

			String debug2 = poolNote->getStringAttribute("Description");
			String debug1 = poolNote->getStringAttribute("Description").substring(0, 1);
			String debug3 = poolNote->getStringAttribute("Description").substring(2);
			if (poolNote->getStringAttribute("Description").substring(0, 2).compare("**") == 0) // if note in the pool, delete the ** and reset to poolnote
			{
				poolNote->setAttribute("Description", poolNote->getStringAttribute("Description").substring(2));
				debug2 = poolNote->getStringAttribute("Description");
				poolNote->setTagName("PoolData");
			}
			
		}
		else
		{
			XmlElement *newPool = addToModel("PoolData");

			newPool->setAttribute("Note", *it);
			newPool->setAttribute("Label", MidiMessage::getMidiNoteName(*it, true, true, 5));
			newPool->setAttribute("Pool", "1");
			newPool->setAttribute("Channel", "10");
			newPool->setAttribute("Description", "");
			
		}
	}

	poolListData->deleteAllChildElementsWithTagName("DELETE"); // the remaining ones!
	// refesh the pool table!
	renumberPool(poolListData);
	broadcaster.sendActionMessage(MsgMaster);


	// warn if there are duplicate notes in the set

	set.clear();

	forEachXmlChildElement(*poolListData, child)
	{
		int note = child->getIntAttribute("Note");
		if (set.contains(note))
			Log("Note " + child->getStringAttribute("Label") + " is duplicate in the note pool.", Topiary::LogType::Warning);
		else
			set.add(child->getIntAttribute("Note"));
	}
}

///////////////////////////////////////////////////////////////////////

void TopiaryBeatsModel::setGMDrumMapLabels()
{
	for (int i = 0; i < poolListData->getNumChildElements(); i++) {
		auto child = poolListData->getChildElement(i);
		int note = child->getIntAttribute("Note");
		// remember if there was a ** in the description and keep that if it was there
		String twoStars = child->getStringAttribute("Description").substring(0, 2);
		if (!twoStars.equalsIgnoreCase("**"))
			twoStars == "";
		child->setAttribute("Description", twoStars + String( MidiMessage::getRhythmInstrumentName(note)));
	}
} // setGMDrumMapLabels


///////////////////////////////////////////////////////////////////////
//  VARIATIONS
///////////////////////////////////////////////////////////////////////

void TopiaryBeatsModel::initializeVariationsForRunning()
{
	// careful; this code is different in various plugins - don't think it's generic!!!
	for (int v = 0; v < 8; v++)
	{
		variation[v].ended = false;
		variation[v].currentPatternChild = nullptr;
		for (int p=0;p<4;p++)
			variation[v].poolTickCursor[p] = 0;
	}
} // initializeVariationsForRunning

///////////////////////////////////////////////////////////////////////
// Variation Parameters
///////////////////////////////////////////////////////////////////////

void TopiaryBeatsModel::getVariationDefinition(int i, bool& enabled, String& vname, int& patternId, bool enables[4], int poolLength[4][3][2], int poolChannel[4])
{
	jassert(i < 8); // goes from 0-7
	vname = variation[i].name;
	enabled = variation[i].enabled;
	patternId = variation[i].patternToUse;
	for (int j = 0; j < 4; j++)
	{
		enables[j] = variation[i].enablePool[j];
		poolLength[j][0][0] = variation[i].measureFrom[j];
		poolLength[j][0][1] = variation[i].measureTo[j];
		poolLength[j][1][0] = variation[i].beatFrom[j];
		poolLength[j][1][1] = variation[i].beatTo[j];
		poolLength[j][2][0] = variation[i].tickFrom[j];
		poolLength[j][2][1] = variation[i].tickTo[j];
		poolChannel[j] = variation[i].poolChannel[j];
	}
	// need to look up the ID in the items in the patternCombo, and set the patternCombo selection

} // getVariationDefinition

///////////////////////////////////////////////////////////////////////

void TopiaryBeatsModel::setVariationDefinition(int i, bool enabled, String vname, int patternId, bool enables[4], int poolLength[4][3][2], int poolChannel[4])
{
	// write to model
	// assume the data has been validated first!
	// make sure that overrideHost is set to TRUE if there are no enabled variations!!!

	jassert(i < 8); // goes from 0-7

	if (numPatterns == 0) return;  // should not really happen unless @ initialization - but in that case we shouldn't write to the model

	// if patternId == -1 and that means we are not touching a not-initialized variation - do nothing
	if (patternId == -1) return;

	int lenInMeasures = getPatternLengthInMeasures(patternId);

	variation[i].name = vname;
	variation[i].enabled = enabled;

	if (!enabled)
	{
		// make sure to enable hostOverride if there are no enabled variations
		// otherwise host might try to run the plugin when these is nothing to run - it might hang
		bool ok = false;
		for (int v = 0; v < 8; v++)
		{
			if (variation[v].enabled)
				ok = true;
		}

		if (!ok && !overrideHostTransport)
			setOverrideHostTransport(true);
	}

	// if we are changing patterns there is no need to pass in from/to - those will be reinitialized
	if (patternId == variation[i].patternToUse)
	{
		for (int j = 0; j < 4; j++)
		{
			variation[i].enablePool[j] = enables[j];
			variation[i].measureFrom[j] = poolLength[j][0][0];
			variation[i].measureTo[j] = poolLength[j][0][1];
			variation[i].beatFrom[j] = poolLength[j][1][0];
			variation[i].beatTo[j] = poolLength[j][1][1];
			variation[i].tickFrom[j] = poolLength[j][2][0];
			variation[i].tickTo[j] = poolLength[j][2][1];
			if (variation[i].measureTo[j] == -1)
			{
				variation[i].tickTo[j] = Topiary::TICKS_PER_QUARTER - 1;
				variation[i].beatTo[j] = numerator - 1;
				variation[i].measureTo[j] = lenInMeasures - 1;
			}

			// calculate the above in ticks
			variation[i].fullTickFrom[j] = variation[i].tickFrom[j] + variation[i].beatFrom[j] * Topiary::TICKS_PER_QUARTER + numerator * variation[i].measureFrom[j] * Topiary::TICKS_PER_QUARTER;
			variation[i].fullTickTo[j] = variation[i].tickTo[j] + variation[i].beatTo[j] * Topiary::TICKS_PER_QUARTER + numerator * variation[i].measureTo[j] * Topiary::TICKS_PER_QUARTER;

			variation[i].lenInTicks = patternData[variation[i].patternToUse].patLenInTicks;
		}
	}
	else
		initializePatternToVariation(patternId, i);

	for (int j = 0; j < 4; j++)
	{
		// reset variation generation cursors
		variation[i].poolIdCursor[j] = 1;
		variation[i].poolTickCursor[j] = 0;
		variation[i].poolChannel[j] = poolChannel[j];
	}
	
	// regenerate!

	generateVariation(i);
	broadcaster.sendActionMessage(MsgVariationDefinition);
	broadcaster.sendActionMessage(MsgVariationEnables);  // may need to update the enable buttons

}  // setVariationDefinition

//////////////////////////////////////////////////////////////////////

bool TopiaryBeatsModel::validateVariationDefinition(int i, bool enable, String vname, int patternId, int poolLength[4][3][2])
{
	// return false if parameters do not make sense
	// refuse to enable a variation if there are no patterns
	// refuse to enable a variation if the pool settings make no sense and do not match e.g. length of pattern
	// if patternId == -1 check if it was -1 in the model; if so give is a pass; otherwise complain that pattern needs to be selected

	jassert(i < 8); // goes from 0-7

	if (numPatterns == 0) return false;  // in this case the variations tab should be disabled anyway

	bool refuse = false;

	if (enable && (patternId == -1))
	{
		Log("Variation " + String(i + 1) + ": Pattern needs to be selected.", Topiary::LogType::Warning);
		refuse = true;
		return refuse;
	}

	if ((patternId < 0) && (variation[i].patternToUse == -1)) return false;  // don't do further checks - pattern was uninitialized to start with

	if ((patternId < 0) && (variation[i].patternToUse != -1))
	{
		Log("Variation "+String(i+1)+": Pattern needs to be selected.", Topiary::LogType::Warning);
		refuse = true;
		return refuse;
	}

	if (!name.compare("")) {
		Log("Variation needs a name.", Topiary::LogType::Warning);
		refuse = true;
	}

	// when changing the pattern; do not validate the from-to : those will be reinitializen later

	if (patternId != variation[i].patternToUse)
	{
		return false;
	}

	int lenInTicks = patternData[patternId].patLenInTicks;
	if (lenInTicks < 1)
	{
		Log("Pattern must have length > 0 in order to be used here.", Topiary::LogType::Warning);
		return true;
	}

	int lenInMeasures = getPatternLengthInMeasures(patternId);

	if (lenInMeasures < 1)
	{
		Log("Pattern needs at least 1 measure in order to be used here.", Topiary::LogType::Warning);
		return true;
	}

	for (int j = 0; j < 4; j++)
	{
		int measureFrom = poolLength[j][0][0];
		int measureTo = poolLength[j][0][1];
		int beatFrom = poolLength[j][1][0];
		int beatTo = poolLength[j][1][1];
		int tickFrom = poolLength[j][2][0];
		int tickTo = poolLength[j][2][1];

		if (measureTo == -1)
		{
			tickTo = Topiary::TICKS_PER_QUARTER - 1;
			beatTo = numerator - 1;
			measureTo = lenInMeasures - 1;
		}

		// calculate the above in ticks
		int fullTickFrom = tickFrom + beatFrom * Topiary::TICKS_PER_QUARTER + numerator * measureFrom * Topiary::TICKS_PER_QUARTER;
		int fullTickTo = tickTo + beatTo * Topiary::TICKS_PER_QUARTER + numerator * measureTo * Topiary::TICKS_PER_QUARTER;

		if (fullTickTo < fullTickFrom)
		{
			Log("Pool " + String(j + 1) + " length does not make sense.", Topiary::LogType::Warning);
			refuse = true;
		}

		if ((measureFrom < 0) || (measureFrom >= lenInMeasures) || (measureFrom < 0) || (measureTo >= lenInMeasures))
		{
			Log("Pool "+String(j)+": measure values should be >=0 and <" + String(lenInMeasures), Topiary::LogType::Warning);
			refuse = true;
		}
		if ((beatFrom < 0) || (beatFrom >= numerator) || (beatFrom < 0) || (beatTo >= numerator))
		{
			Log("Beat values should be >=0 and <" + String(numerator), Topiary::LogType::Warning);
			refuse = true;
		}
		if ((tickFrom < 0) || (tickFrom >= Topiary::TICKS_PER_QUARTER) || (tickFrom < 0) || (tickTo >= Topiary::TICKS_PER_QUARTER))
		{
			Log("Tick values should be >=0 and <" + String(Topiary::TICKS_PER_QUARTER), Topiary::LogType::Warning);
			refuse = true;
		}
	}

	if (refuse) Log("Current values will not be remembered!", Topiary::LogType::Warning);

	return refuse;

} // validateVariationDefinition

///////////////////////////////////////////////////////////////////////

void TopiaryBeatsModel::setRandomizeNotes(int v, bool enable, bool enablePool[4], int value)
{
	variation[v].randomizeNotes = enable;
	variation[v].randomizeNotesValue = value;
	for (int i = 0; i < 4; i++)
	{
		variation[v].randomizeNotePool[i] = enablePool[i];
	}
	generateVariation(v);
	
} // getRandomNotes

///////////////////////////////////////////////////////////////////////

void TopiaryBeatsModel::getRandomizeNotes(int v, bool &enable, bool enablePool[4], int &value)
{
	enable = variation[v].randomizeNotes;
	value = variation[v].randomizeNotesValue;
	for (int i = 0; i < 4; i++)
	{
		enablePool[i] = variation[v].randomizeNotePool[i];
	}

} // setRandomNotes

///////////////////////////////////////////////////////////////////////

void TopiaryBeatsModel::setSwing(int v, bool enable, bool enablePool[4], int value)
{
	variation[v].swing = enable;
	variation[v].swingValue = value;
	for (int i = 0; i < 4; i++)
	{
		variation[v].swingPool[i] = enablePool[i];
	}

	generateVariation(v);

} // getSwing

///////////////////////////////////////////////////////////////////////

void TopiaryBeatsModel::getSwing(int v, bool &enable, bool enablePool[4], int &value)
{
	enable = variation[v].swing;
	value = variation[v].swingValue;
	for (int i = 0; i < 4; i++)
	{
		enablePool[i] = variation[v].swingPool[i];
	}

} // getSwing

///////////////////////////////////////////////////////////////////////

void TopiaryBeatsModel::setRandomizeVelocity(int v, bool enable, bool enablePool[4], int value, bool plus, bool min)
{
	variation[v].randomizeVelocity = enable;
	variation[v].velocityValue = value;
	variation[v].velocityPlus = plus;
	variation[v].velocityMin = min;
	for (int i = 0; i < 4; i++)
	{
		variation[v].velocityPool[i] = enablePool[i];
	}

	generateVariation(v);

} // getRandomizeVelocity

///////////////////////////////////////////////////////////////////////

void TopiaryBeatsModel::getRandomizeVelocity(int v, bool &enable, bool enablePool[4], int &value, bool &plus, bool &min)
{
	enable = variation[v].randomizeVelocity;
	value = variation[v].velocityValue;
	plus = variation[v].velocityPlus;
	min = variation[v].velocityMin;
	for (int i = 0; i < 4; i++)
	{
		enablePool[i] = variation[v].velocityPool[i];
	}

} // getRandomizeVelocity

///////////////////////////////////////////////////////////////////////

void TopiaryBeatsModel::setRandomizeTiming(int v, bool enable, bool enablePool[4], int value, bool plus, bool min)
{
	variation[v].randomizeTiming = enable;
	variation[v].timingValue = value;
	variation[v].timingPlus = plus;
	variation[v].timingMin = min;
	for (int i = 0; i < 4; i++)
	{
		variation[v].timingPool[i] = enablePool[i];
	}

	generateVariation(v);

} // getRandomizeTiming

///////////////////////////////////////////////////////////////////////

void TopiaryBeatsModel::getRandomizeTiming(int v, bool &enable, bool enablePool[4], int &value, bool &plus, bool &min)
{
	enable = variation[v].randomizeTiming;
	value = variation[v].timingValue;
	plus = variation[v].timingPlus;
	min = variation[v].timingMin;

	for (int i = 0; i < 4; i++)
	{
		enablePool[i] = variation[v].timingPool[i];
	}

} // getRandomizeTiming

///////////////////////////////////////////////////////////////////////

void TopiaryBeatsModel::getPatterns(String pats[8])
{
	XmlElement* pattern;

	pattern = patternListData->getFirstChildElement();
	int i = 0;
	while (pattern != nullptr)
	{
		pats[i] = String(i + 1) + " " + pattern->getStringAttribute("Name");
		pattern = pattern->getNextElement();
		i++;
	}

} // getPatterns

////////////////////////////////////////////////////////////////////////////////////////////////////////////

void TopiaryBeatsModel::generateVariation(int i)
{
	// (re)generates variation[i].pattern as a separate XmlElement
	// when done, it locks the model and swaps it in with the shadowpattern and destroys the old one
	// uses the variation's generation cursors, which need to be reset when the variation gets edited (and when program loads)

	jassert((i < 8) && (i >= 0));

	auto newPattern = new XmlElement("Pattern");
	auto newOffPattern = new XmlElement("OffPattern");

	XmlElement* pattern;
	pattern = patternData[variation[i].patternToUse].noteData;

	int poolNote[128];  // will contain the pool number for the note
	bool poolHasNotes[4];

	int note;
	int pool;

	// poolNote is helper datastructure; given a note, it returns the pool the note is in (careful for -1s; pool numbers in variation are 0-3 but in screen are 1-4

	for (int j = 0; j < 128; j++) poolNote[j] = -1;
	for (int j = 0; j < 4; j++) poolHasNotes[j] = false;

	forEachXmlChildElement(*poolListData, child)
	{
		// loop over the pool notes and put them in the appropriate set
		note = child->getIntAttribute("Note");
		pool = child->getIntAttribute("Pool");
		poolNote[note] = pool - 1;
		poolHasNotes[pool - 1] = true;
	}

	// set poolTo values if measureTo = -1;

	if (variation[i].enabled) // otherwise it may not have valid settings!!!
		for (int j = 0; j < 4; j++)
		{
			if (variation[i].measureTo[j] == -1)
			{
				XmlElement* patternToUse = patternListData->getChildByAttribute("ID", String(variation[i].patternToUse +1));
				if (patternToUse == nullptr)
				{
					// a pattern got deleted and no longer exists; autocorrect - set to 1; there will always be at least 1 pattern otherwise te variation editor is disabled!
					// this should never happen!
					jassert(false);
				}
				variation[i].measureTo[j] = pattern->getIntAttribute("Measures") - 1;
				variation[i].beatTo[j] = numerator - 1;
				variation[i].tickTo[j] = Topiary::TICKS_PER_QUARTER - 1;
			}
		}

	////////////////////////////
	// GENERATE THE POOL NOTES
	////////////////////////////

	for (int j = 0; j < 4; j++)
	{
		// only call generatePool if there are notes in the pool; otherwise generatePool will loop forever!!!) 
		if (poolHasNotes[j])
			generatePool(i, j, newPattern, poolNote);
	}

	XmlElement* newChild = nullptr;
	
	// generate the note off data 
	
	forEachXmlChildElement(*newPattern, child)
	{
		// create a copy of child with the variation parameters taken into account
		newChild = newOffPattern->createNewChildElement("OFFDATA");

		newChild->setAttribute("ID", child->getStringAttribute("ID"));
		newChild->setAttribute("Note", child->getStringAttribute("Note"));
		newChild->setAttribute("Velocity", child->getStringAttribute("Velocity"));
		newChild->setAttribute("Timestamp", child->getIntAttribute("Timestamp") + child->getIntAttribute("Length"));
		newChild->setAttribute("Measure", child->getStringAttribute("Measure"));
		newChild->setAttribute("Channel", child->getStringAttribute("Channel"));
	} 

	// add the off notes to newPattern
	
	forEachXmlChildElement(*newOffPattern, child)
	{
		// create a copy of child with the variation parameters taken into account
		newChild = newPattern->createNewChildElement("OFFDATA");
		newChild->setAttribute("ID", child->getStringAttribute("ID"));
		newChild->setAttribute("Note", child->getStringAttribute("Note"));
		newChild->setAttribute("Velocity", child->getStringAttribute("Velocity"));
		newChild->setAttribute("Timestamp", child->getIntAttribute("Timestamp") + child->getIntAttribute("Length"));
		newChild->setAttribute("Measure", child->getStringAttribute("Measure"));
		newChild->setAttribute("Channel", child->getStringAttribute("Channel"));
		newChild->setAttribute("midiType", Topiary::MidiType::NoteOff);
	}
	
	renumberByTimestamp(newPattern);

	Logger::writeToLog("-------------------- VARIATION " + String(i) + " GENERATED ---------------------");
	//Logger::writeToLog("Measures: " + String(lenInMeasures) + " --- Ticks " + String(lenInTicks));

	String myXmlDoc2 = newPattern->createDocument(String());
	Logger::writeToLog(myXmlDoc2); 

	// swap the patterns; 
	// TODO - set the variation[i].currentPatternChild - or set those to nullptr and generateMidi will figure it out???

	swapXmlElementPointers(&(variation[i].pattern), &newPattern);

	delete newPattern;
	delete newOffPattern;

} // generateVariation

////////////////////////////////////////////////////////////////////////////////////////////////////////////

void  TopiaryBeatsModel::generateAllVariations()
{
	for (int v = 0; v < 8; v++)
		for (int p = 0; p < 4; p++) variation[v].poolTickCursor[p] = 0;

	for (int v = 0; v < 8; v++)
	{
		generateVariation(v);
	}
} // generateAllVariations()

////////////////////////////////////////////////////////////////////////////////////

void TopiaryBeatsModel::threadRunner()
{	
	{
		const GenericScopedLock<SpinLock> myScopedLock(lockModel);
		threadRunnerState = Topiary::ThreadRunnerState::Generating;
	}
	generateAllVariations(); // this one swaps in the variations using the spinlock
	{
		const GenericScopedLock<SpinLock> myScopedLock(lockModel);
		threadRunnerState = Topiary::ThreadRunnerState::NothingToDo;
	}

} // threadRunner

////////////////////////////////////////////////////////////////////////////////////
// move to modelincludes when done

void TopiaryBeatsModel::regenerateVariationsForPattern(int p)
{
	// regenerate the variations using this pattern
	for (int v = 0; v < 8; v++)
	{
		if (variation[v].enabled && (variation[v].patternToUse == p))
			generateVariation(v);
	}

} // regenerateVariationsForPattern

////////////////////////////////////////////////////////////////////////////////////

void TopiaryBeatsModel::setPatternLength(int p, int l, bool keepTail)
{
	// set LinInTicks, based on l in measures
	// delete all events possbily after the ticklength
	bool warned = false;

	int newLenInTicks = l * Topiary::TICKS_PER_QUARTER * denominator;
	int tickDelta = patternData[p].patLenInTicks - newLenInTicks;
	int measureDelta = tickDelta / (Topiary::TICKS_PER_QUARTER * denominator);

	if (patternData[p].patLenInTicks != newLenInTicks)
	{
		// delete late note events
		XmlElement *child = patternData[p].noteData->getFirstChildElement();
		XmlElement *remember = nullptr;
		bool deleted;

		while (child != nullptr)
		{
			deleted = false;
			if (tickDelta >0) // only delete notes if the pattern gets shorter
				if ( ((newLenInTicks <= child->getIntAttribute("Timestamp")) && !keepTail) ||
					 (keepTail && (child->getIntAttribute("Timestamp")< tickDelta))  )
				{
					// delete the event
					remember = child; // cannot delete the child here; we still need its nextElement; use bool deleted  & remember for that!
					warned = true;
					deleted = true;
				
					//int debugTS = child->getIntAttribute("Timestamp");
					//int debugL = child->getIntAttribute("Length");	
					if ((child->getIntAttribute("Timestamp") + child->getIntAttribute("Length")) > newLenInTicks)
						child->setAttribute("Length", child->getIntAttribute("Length") - child->getIntAttribute("Timestamp"));
				}

			if (keepTail)
			{
				// correct measure, beat, tick & timestamps
				child->setAttribute("Timestamp", child->getIntAttribute("Timestamp") - tickDelta);
				child->setAttribute("Measure", child->getIntAttribute("Measure") - measureDelta);
			}

			// make sure note lenght never runs over total patternlength
			if ((child->getIntAttribute("Timestamp") + child->getIntAttribute("Length")) >= newLenInTicks)
				child->setAttribute("Length", String(newLenInTicks - child->getIntAttribute("Timestamp") -1));

			child = child->getNextElement();
			if (deleted)
				patternData[p].noteData->removeChildElement(remember, true);  // true deletes the child
		}

		patternData[p].patLenInTicks = newLenInTicks;
		setPatternLengthInMeasures(p, l);
		renumberByTimestamp(patternData[p].noteData);
		// regenerate variations using this pattern
		regenerateVariationsForPattern(p);
		if (warned)
			Log("Pattern was shortened and MIDI events were lost.", Topiary::LogType::Warning);

	}

} // setPatternLength

////////////////////////////////////////////////////////////////////////////////////

void TopiaryBeatsModel::deleteNote(int p ,int n)
{
	// deletes note with ID n from pattern p
	
	auto child = patternData[p].noteData->getChildElement(n);
	patternData[p].noteData->removeChildElement(child, true);
	broadcaster.sendActionMessage(MsgPattern);

	Log("Note deleted.", Topiary::LogType::Info);
	regenerateVariationsForPattern(p);
	rebuildPool(false);

	//String myXmlDoc = poolListData->createDocument(String());
	//Logger::writeToLog(myXmlDoc);
}

////////////////////////////////////////////////////////////////////////////////////

void TopiaryBeatsModel::addNote(int p, int n, int v, int l, int t) 
{
	// adds note n in pattern p, with velocity v at time t
	XmlElement* newNote = new XmlElement("Note");
	newNote->setAttribute("Note", n);
	newNote->setAttribute("Timestamp", t);
	newNote->setAttribute("Velocity", v);
	newNote->setAttribute("Length", l);
	newNote->setAttribute("Measure", (int) (t / (denominator*Topiary::TICKS_PER_QUARTER)));
	t = t % (denominator*Topiary::TICKS_PER_QUARTER);
	newNote->setAttribute("Beat", (int)(t / Topiary::TICKS_PER_QUARTER));
	t = t % Topiary::TICKS_PER_QUARTER;
	newNote->setAttribute("Tick", t);
	newNote->setAttribute("Label", MidiMessage::getMidiNoteName(n, true, true, 5));
	patternData[p].noteData->prependChildElement(newNote);
	renumberByTimestamp(patternData[p].noteData); // will create the ID
	broadcaster.sendActionMessage(MsgPattern);
	regenerateVariationsForPattern(p);

}  // addNote

////////////////////////////////////////////////////////////////////////////////////

void TopiaryBeatsModel::getNote(int p, int ID, int& note, int &velocity, int &timestamp, int &length)  // get note with id ID from pattern p
{
	// get note with id ID from pattern p
	XmlElement* child = patternData[p].noteData->getChildByAttribute("ID", String(ID));
	jassert(child != nullptr);
	note = child->getIntAttribute("Note");
	velocity = child->getIntAttribute("Velocity");
	length = child->getIntAttribute("Length");
	timestamp = child->getIntAttribute("Timestamp");

} // get Note

////////////////////////////////////////////////////////////////////////////////////

void TopiaryBeatsModel::setPatternTableHeaders(int p)
{
	// called when pattern p is selected
	// called when pattern length changed
	// adapts the max values for measure & beat

	XmlElement *child;

	child = patternHeader->getChildByAttribute("name", "Beat");
	child->setAttribute("max", String(numerator - 1));
	child = patternHeader->getChildByAttribute("name", "Measure");
	child->setAttribute("max", String((int) (patternData[p].patLenInTicks / Topiary::TICKS_PER_QUARTER / denominator) -1));

} // setPatternTableHeaders

////////////////////////////////////////////////////////////////////////////////////

void TopiaryBeatsModel::validateNoteEdit(int p, XmlElement* child, String attribute)
{
	// called by TopiaryTable
	// careful - can be called when editing patterns but also when editing note pool entries!!
	// do processing of user edits to notes and make sure all is consistent

	if (attribute.compare("Label") == 0) 
	{
		// set the note value correctly
		//int debug = validNoteNumber(child->getStringAttribute("Label"));
		child->setAttribute("Note", validNoteNumber(child->getStringAttribute("Label")));
		child->setAttribute("Label", validateNote(child->getStringAttribute("Label")));
	}
	else if ((attribute.compare("Measure") == 0) || (attribute.compare("Beat") == 0) || (attribute.compare("Tick") == 0))
	{
		// recalculate timestamp
		int timestamp = child->getIntAttribute("Tick") +
			child->getIntAttribute("Beat")* Topiary::TICKS_PER_QUARTER +
			child->getIntAttribute("Measure") * Topiary::TICKS_PER_QUARTER * denominator;

		child->setAttribute("Timestamp", String(timestamp));
		renumberByTimestamp(patternData[p].noteData);
	}

	// what follows only if we are editing patterns, i.e, the element has Timestamp as an attribute

	if (child->hasAttribute("Timestamp"))
	{

		// check note does not run over pattern end
		// make sure note lenght never runs over total patternlength
		if ((child->getIntAttribute("Timestamp") + child->getIntAttribute("Length")) >= patternData[p].patLenInTicks)
		{
			child->setAttribute("Length", String(patternData[p].patLenInTicks - child->getIntAttribute("Timestamp") - 1));
			Log("Note shortened so it does not run over pattern end.", Topiary::LogType::Warning);
		}

		renumberByTimestamp(patternData[p].noteData);

		// regenerate variations that depend on this one
		for (int v = 0; v < 8; v++)
		{
			if (variation[v].patternToUse == p)
				generateVariation(v);
		}
		rebuildPool(false);
		broadcaster.sendActionMessage(MsgPattern);
	}
} // validateNoteEdit

///////////////////////////////////////////////////////////////////////////////////////

void TopiaryBeatsModel::swapVariation(int from, int to)
{
	jassert((from < 8) && (from >= 0));
	jassert((to < 8) && (to >= 0));
	{
		const GenericScopedLock<SpinLock> myScopedLock(lockModel);

		intSwap(variation[from].patternToUse, variation[to].patternToUse);
		intSwap(variation[from].lenInTicks, variation[to].lenInTicks);
		intSwap(variation[from].lenInMeasures, variation[to].lenInMeasures);
		boolSwap(variation[from].ended, variation[to].ended);
		boolSwap(variation[from].ending, variation[to].ending);
		boolSwap(variation[from].enabled, variation[to].enabled);

		//XmlElement* pattern;				// pattern  events
		//XmlElement* currentPatternChild;    // where we are in generation 

		stringSwap(variation[from].name, variation[to].name);
		boolSwap(variation[from].randomizeNotes, variation[to].randomizeNotes);
		intSwap(variation[from].randomizeNotesValue, variation[to].randomizeNotesValue);

		boolSwap(variation[from].swing, variation[to].swing);
		intSwap(variation[from].swingValue, variation[to].swingValue);

		boolSwap(variation[from].randomizeVelocity, variation[to].randomizeVelocity);
		intSwap(variation[from].velocityValue, variation[to].velocityValue);
		boolSwap(variation[from].velocityPlus, variation[to].velocityPlus);
		boolSwap(variation[from].velocityMin, variation[to].velocityMin);

		boolSwap(variation[from].randomizeVelocity, variation[to].randomizeVelocity);

		boolSwap(variation[from].randomizeTiming, variation[to].randomizeTiming);
		intSwap(variation[from].timingValue, variation[to].timingValue);
		boolSwap(variation[from].timingPlus, variation[to].timingPlus);
		boolSwap(variation[from].timingMin, variation[to].timingMin);

		for (int i = 0; i < 4; i++)
		{
			boolSwap(variation[from].enablePool[i], variation[to].enablePool[i]);
			intSwap(variation[from].measureFrom[i], variation[to].measureFrom[i]);
			intSwap(variation[from].measureTo[i], variation[to].measureTo[i]);
			intSwap(variation[from].beatFrom[i], variation[to].beatFrom[i]);
			intSwap(variation[from].beatTo[i], variation[to].beatTo[i]);
			intSwap(variation[from].tickFrom[i], variation[to].tickFrom[i]);
			intSwap(variation[from].tickTo[i], variation[to].tickTo[i]);
			intSwap(variation[from].fullTickFrom[i], variation[to].fullTickFrom[i]);
			intSwap(variation[from].fullTickTo[i], variation[to].fullTickTo[i]);

			intSwap(variation[from].poolChannel[i], variation[to].poolChannel[i]);
			intSwap(variation[from].poolTickCursor[i], variation[to].poolTickCursor[i]);
			intSwap(variation[from].poolIdCursor[i], variation[to].poolIdCursor[i]);

			boolSwap(variation[from].randomizeNotePool[i], variation[to].randomizeNotePool[i]);
			boolSwap(variation[from].swingPool[i], variation[to].swingPool[i]);
			boolSwap(variation[from].velocityPool[i], variation[to].velocityPool[i]);
			boolSwap(variation[from].timingPool[i], variation[to].timingPool[i]);
		}		
	} // end scope of lock

	generateAllVariations();
	broadcaster.sendActionMessage(MsgMaster);
	broadcaster.sendActionMessage(MsgVariationEnables);
	Log("Variation " + String(from + 1) + " swapped with " + String(to + 1) + ".", Topiary::LogType::Info);


} // swapVariation

///////////////////////////////////////////////////////////////////////////////////////

void TopiaryBeatsModel::copyVariation(int from, int to)
{
	jassert((from < 8) && (from >= 0));
	jassert((to < 8) && (to >= 0));
	{
		const GenericScopedLock<SpinLock> myScopedLock(lockModel);

		variation[to].patternToUse = variation[from].patternToUse;
		variation[to].lenInTicks = variation[from].lenInTicks;
		variation[to].lenInMeasures = variation[from].lenInMeasures;
		variation[to].ending = variation[from].ending;
		variation[to].ended = variation[from].ended;
		variation[to].enabled = variation[from].enabled;
		variation[to].name = variation[from].name + String(" copy ");
		variation[to].randomizeNotes = variation[from].randomizeNotes;
		variation[to].randomizeNotesValue = variation[from].randomizeNotesValue;
		variation[to].swing = variation[from].swing;
		variation[to].swingValue = variation[from].swingValue;
		variation[to].randomizeVelocity = variation[from].randomizeVelocity;
		variation[to].velocityMin = variation[from].velocityMin;
		variation[to].velocityPlus = variation[from].velocityPlus;
		variation[to].randomizeTiming = variation[from].randomizeTiming;
		variation[to].timingValue = variation[from].timingValue;
		variation[to].timingMin = variation[from].timingMin;
		variation[to].timingPlus = variation[from].timingPlus;

		for (int i = 0; i < 4; i++)
		{
			variation[to].enablePool[i] = variation[from].enablePool[i];
			variation[to].beatFrom[i] = variation[from].beatFrom[i];
			variation[to].beatTo[i] = variation[from].beatTo[i];
			variation[to].measureFrom[i] = variation[from].measureFrom[i];
			variation[to].measureTo[i] = variation[from].measureTo[i];
			variation[to].tickFrom[i] = variation[from].tickFrom[i];
			variation[to].tickTo[i] = variation[from].tickTo[i];
			variation[to].fullTickFrom[i] = variation[from].fullTickFrom[i];
			variation[to].fullTickTo[i] = variation[from].fullTickTo[i];

			variation[to].poolChannel[i] = variation[from].poolChannel[i];
			variation[to].poolTickCursor[i] = variation[from].poolTickCursor[i];
			variation[to].poolIdCursor[i] = variation[from].poolIdCursor[i];
			variation[to].randomizeNotePool[i] = variation[from].randomizeNotePool[i];
			variation[to].swingPool[i] = variation[from].swingPool[i];
			variation[to].velocityPool[i] = variation[from].velocityPool[i];
			variation[to].timingPool[i] = variation[from].timingPool[i];
		}

		variation[to].currentPatternChild = nullptr;
		// pattern and currentPatternChild are not copied; former is recreated @generateVariation and latter is nullptr;
	} // end oif lock scope

	generateAllVariations();
	broadcaster.sendActionMessage(MsgMaster);
	broadcaster.sendActionMessage(MsgVariationEnables);
	Log("Variation " + String(from + 1) + " copied to " + String(to + 1) + ".", Topiary::LogType::Info);


} // copyVariation

///////////////////////////////////////////////////////////////////////////////////////

bool TopiaryBeatsModel::midiLearn(MidiMessage m)
{
	// called by processor; if midiLearn then learn based on what came in
	const GenericScopedLock<SpinLock> myScopedLock(lockModel);
	bool remember = learningMidi;
	if (learningMidi)
	{
		bool note = m.isNoteOn();
		bool cc = m.isController();

		if (note || cc)
		{
			// check the Id to learn; tells us what to set
			if ((midiLearnID >= TopiaryLearnMidiId::variationSwitch) && (midiLearnID < (TopiaryLearnMidiId::variationSwitch + 8)))
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

		}
	}

	return remember;

} // midiLearn

///////////////////////////////////////////////////////////////////////////////////////

void TopiaryBeatsModel::record(bool b)
{
	
	const GenericScopedLock<SpinLock> myScopedLock(lockModel);

	// check if pattern being recorded is also the one in the variation selected!

	if (patternSelectedInPatternEditor != variation[variationSelected].patternToUse)
	{
		Log("Pattern is not in variation selected.", Topiary::LogType::Warning);
		return;
	}

	// check there are pool(s) enabled; otherwise we don't know the channel when recording
	if (b) {
		bool noNotes = true;

		for (int p = 0; p < 4; p++)
		{
			if (variation[variationSelected].enablePool[p])
				noNotes = false;
		}

		if (noNotes)
		{
			Log("Enable at least one note pool in variation.", Topiary::LogType::Warning);
			Log("Can't know which channel to output to.", Topiary::LogType::Warning);
			return;
		}
	}
	else if (recordingMidi)
		processMidiRecording();

	recordingMidi = b;
	// inform transport
	broadcaster.sendActionMessage(MsgTransport);
}

///////////////////////////////////////////////////////////////////////////////////////

void TopiaryBeatsModel::processMidiRecording()
{
	// process recorded events and add to pattern
	
	XmlElement*event = variation[variationSelected].pattern->getFirstChildElement();
	XmlElement *patternParent = patternData[variation[variationSelected].patternToUse].noteData;

	//String myXmlDoc = patternParent->createDocument(String());
	//Logger::writeToLog(myXmlDoc);
	//Logger::outputDebugString("RECORDING in non-empty pattern --> inserted!");

	while (event != nullptr)
	{
		if (event->hasTagName("RECDATA"))
		{
			if (event->getIntAttribute("midiType") == Topiary::MidiType::NoteOn)
			{
				XmlElement* newChild;
				newChild = new XmlElement("INSERTED");  // the RECDATA elements will get inserted in the pattern when recording done; use tag insertes do we van find the note off for it
				newChild->setAttribute("ID", 0); // dummy
				newChild->setAttribute("Note", event->getIntAttribute("Note"));
				newChild->setAttribute("Label", MidiMessage::getMidiNoteName(event->getIntAttribute("Note"), true, true, 5));

				newChild->setAttribute("Velocity", event->getIntAttribute("Velocity"));
				int timestamp = event->getIntAttribute("Timestamp");
				newChild->setAttribute("Timestamp", timestamp);
				newChild->setAttribute("Length", 30);
				int measur = (int)floor(timestamp / (numerator * Topiary::TICKS_PER_QUARTER));
				newChild->setAttribute("Measure", measur);

				timestamp = timestamp - measur * (numerator * Topiary::TICKS_PER_QUARTER);
				int bea = (int)floor(timestamp / Topiary::TICKS_PER_QUARTER);
				newChild->setAttribute("Beat", bea);

				int tic = timestamp - bea * Topiary::TICKS_PER_QUARTER;
				newChild->setAttribute("Tick", tic);

				patternParent->prependChildElement(newChild);
				//String myXmlDoc = patternParent->createDocument(String());
				//Logger::writeToLog(myXmlDoc);
				//Logger::outputDebugString("RECORDING in non-empty pattern --> inserted!");
			}
			else if (event->getIntAttribute("midiType") == Topiary::MidiType::NoteOff)
			{
				// find the note and set the length
				int timestamp = event->getIntAttribute("Timestamp"); // timestamp of end of note
				int note = event->getIntAttribute("Note");
				XmlElement *patChild = patternParent->getFirstChildElement();
				bool cont = true;

				while (cont && (patChild != nullptr))
				{
					if (patChild->hasTagName("INSERTED") && (patChild->getIntAttribute("Note") == note))
					{
						// we found the note to end
						patChild->setAttribute("Length", timestamp - patChild->getIntAttribute("Timestamp"));
						patChild->setTagName("Note"); //so we don't cover it again
						cont = false;
					}
					patChild = patChild->getNextElement();
				}
			}
			else jassert(false); // should not happen; should be note on or note off event for now
		}
		event = event->getNextElement();
	}
	// resort 
	renumberByTimestamp(patternParent);

	// rebuild the pool list
	rebuildPool(false);
	// inform pattern tab
	broadcaster.sendActionMessage(MsgPattern);

}  // processMidiRecording

///////////////////////////////////////////////////////////////////////////////////////

void TopiaryBeatsModel::duplicatePattern(int p)
{
	jassert(p > -1); // has to be a valid row to delete

	if (patternListData->getNumChildElements() == 8)
	{
		Log("Number of patterns is limited to 8.", Topiary::LogType::Warning);
		return;
	}
	
	patternData[numPatterns].numNotes = patternData[p].numNotes;
	patternData[numPatterns].notesRealID = patternData[p].notesRealID;
	patternData[numPatterns].patLenInTicks = patternData[p].patLenInTicks;

	XmlElement *newPattern = addToModel("PatternData");
	patternData[numPatterns].noteData = newPattern;
	newPattern->setAttribute("ID", String(p + 1));
	newPattern->setAttribute("Name", patternData[p].noteData->getStringAttribute("Name")+"(copy)");
	newPattern->setAttribute("Measures", patternData[p].noteData->getIntAttribute("Measures"));
	numPatterns++;

	Log("Duplicate pattern created.", Topiary::LogType::Info);

} // duplicatePattern

///////////////////////////////////////////////////////////////////////////////////////

#include "../../Topiary/Source/TopiaryMidiLearnEditor.cpp"