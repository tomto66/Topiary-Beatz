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

#include "../JuceLibraryCode/JuceHeader.h"
#include "../../Topiary/Topiary.h"
#include "TopiaryBeatsModel.h"

void TopiaryBeatsModel::saveStateToMemoryBlock(MemoryBlock& destData)
{
	addParametersToModel();  // this adds and XML element "Parameters" to the model
	AudioProcessor::copyXmlToBinary(*model, destData);
	model->deleteAllChildElementsWithTagName("Parameters");
}

/////////////////////////////////////////////////////////////////////////

void TopiaryBeatsModel::restoreStateFromMemoryBlock(const void* data, int sizeInBytes)
{	
	//return; // XXXXXXXXX
	model.reset(AudioProcessor::getXmlFromBinary(data, sizeInBytes));
	restoreParametersToModel();
}

/////////////////////////////////////////////////////////////////////////

void TopiaryBeatsModel::savePreset(File f)
{
	addParametersToModel();  // this adds and XML element "Parameters" to the model
	String myXmlDoc = model->createDocument(String());
	f.replaceWithText(myXmlDoc);
	//Logger::writeToLog(myXmlDoc);

	// now delete the no-longer-needed "Parameters" child
	model->deleteAllChildElementsWithTagName("Parameters");
}

/////////////////////////////////////////////////////////////////////////

void TopiaryBeatsModel::loadPreset(File f)
{
	model.reset(XmlDocument::parse(f));
	restoreParametersToModel();
}

/////////////////////////////////////////////////////////////////////////

#define xstr(s) str(s)
#define str(s) #s

TopiaryBeatsModel::TopiaryBeatsModel()
{
	logString = "";
	beatsLog(String("Topiary Beats V ")+String(xstr(JucePlugin_Version))+String(" (c) Tom Tollenaere 2018."), Topiary::LogType::License);
	beatsLog(String(""), Topiary::LogType::License);
	beatsLog(String("Topiary Beats is free software : you can redistribute it and/or modify"), Topiary::LogType::License);
	beatsLog(String("it under the terms of the GNU General Public License as published by"), Topiary::LogType::License);
	beatsLog(String("the Free Software Foundation, either version 3 of the License, or"), Topiary::LogType::License);
	beatsLog(String("(at your option) any later version."), Topiary::LogType::License);
	beatsLog(String(""), Topiary::LogType::License);
	beatsLog(String("Topiary Beats is distributed in the hope that it will be useful,"), Topiary::LogType::License);
	beatsLog(String("but WITHOUT ANY WARRANTY; without even the implied warranty of"), Topiary::LogType::License);
	beatsLog(String("MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the"), Topiary::LogType::License);
	beatsLog(String("GNU General Public License for more details."), Topiary::LogType::License);
	beatsLog(String(""), Topiary::LogType::License);

	//variationsDirty = true; // make sure screen initializes ok
	//broadcaster.sendActionMessage("variations"); - useless because the editor won't be there (yet)
	//transportDirty = true;
	overrideHostTransport = false;
	runState = Topiary::Stopped;
	//broadcaster.sendActionMessage("transport");
	//broadcaster.sendActionMessage("log");
	BPM = 120;
	numerator = 4; denominator = 4;
	
	/////////////////////////////////////
	// patternListHeader initialization
	/////////////////////////////////////
	
	patternListHeader->setTagName("PatternListHeader");
	XmlElement* child =  new XmlElement("COLUMN");
	child->setAttribute("columnId", "1");
	child->setAttribute("name", "ID");
	child->setAttribute("width","20");
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
	child->setAttribute("width", "40");
	child->setAttribute("type", "note");
	poolListHeader->addChildElement(child);

	child = new XmlElement("COLUMN");
	child->setAttribute("columnId", "3");
	child->setAttribute("name", "Label");
	child->setAttribute("width", "100");
	child->setAttribute("type", "char");
	poolListHeader->addChildElement(child);

	child = new XmlElement("COLUMN");
	child->setAttribute("columnId", "4");
	child->setAttribute("name", "Pool"); // which of the 3 pools this one is in
	child->setAttribute("width", "40");
	child->setAttribute("type", "int");
	child->setAttribute("min", "1");
	child->setAttribute("max", "4");
	poolListHeader->addChildElement(child);

	/*
	child = new XmlElement("COLUMN");
	child->setAttribute("columnId", "5");
	child->setAttribute("name", "Channel");
	child->setAttribute("width", "100");
	child->setAttribute("type", "int");
	child->setAttribute("min", "1");
	child->setAttribute("max", "16");
	poolListHeader->addChildElement(child);
	*/

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
	child->setAttribute("width", "80");
	child->setAttribute("type", "int");
	patternHeader->addChildElement(child);

	child = new XmlElement((char*)"COLUMN");
	child->setAttribute("columnId", "3");
	child->setAttribute("name", "Beat");
	child->setAttribute("width", "80");
	child->setAttribute("type", "int");
	patternHeader->addChildElement(child);


	child = new XmlElement((char*)"COLUMN");
	child->setAttribute("columnId", "4");
	child->setAttribute("name", "Tick");
	child->setAttribute("width", "80");
	child->setAttribute("type", "int");
	patternHeader->addChildElement(child);


	child = new XmlElement((char*)"COLUMN");
	child->setAttribute("columnId", "5");
	child->setAttribute("name", "Note");
	child->setAttribute("width", "80");
	child->setAttribute("type", "int");
	patternHeader->addChildElement(child);

	child = new XmlElement((char*)"COLUMN");
	child->setAttribute("columnId", "6");
	child->setAttribute("name", "Length");
	child->setAttribute("width", "80");
	child->setAttribute("type", "int");
	patternHeader->addChildElement(child);

	child = new XmlElement((char*)"COLUMN");
	child->setAttribute("columnId", "7");
	child->setAttribute("name", "Velocity");
	child->setAttribute("width", "80");
	child->setAttribute("type", "int");
	patternHeader->addChildElement(child);

    child = new XmlElement((char*)"COLUMN");
	child->setAttribute("columnId", "8");
	child->setAttribute("name", "Timestamp");
	child->setAttribute("width", "80");
	child->setAttribute("type", "int");
	patternHeader->addChildElement(child);

			
	/////////////////////////////////////
	// variations initialisation
	/////////////////////////////////////

	for (int i=0; i < 8; ++i)
	{
		variation[i].patternToUse = 1;				// index in patterndata
		variation[i].patternOn = nullptr;			// will be set when editing the variation, or when loading state
		variation[i].patternOff = nullptr;		
		
		for (int j=0; j < 4; j++)
		{
			variation[i].enablePool[j] = true;
			variation[i].measureTo[j] = -1;
			variation[i].beatFrom[j] = -1;
			variation[i].beatTo[j] = -1;
			variation[i].tickFrom[j] = -1;
			variation[i].tickTo[j] = -1; 
			variation[i].poolIdCursor[j]= 1;
			variation[i].poolTickCursor[j] = 0;
		}
		variation[i].lenInMeasures = 0;
		variation[i].lenInTicks = 0;
		variation[i].ending = false;									// indicates that once pattern played, we no longer generate notes! (but we keep running (status Ended) till done
				
		variation[i].name = "Variation " + String(i + 1);
		variation[i].enabled = false;
		for (int j=0; j < 4; j++)
		{
			variation[i].enablePool[j] = false;
			variation[i].poolChannel[j] = 10;
			variation[i].measureFrom[j] = 0;
			variation[i].measureTo[j] = -1;
			variation[i].beatFrom[j] = 0;
			variation[i].beatTo[j] = -1;
			variation[i].tickFrom[j] = 0;
			variation[i].tickTo[j] = -1;
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
		patternData[i].noteData->setAttribute("Index",String(i));
		ch = new XmlElement("PatternOff");
		patternData[i].noteOffData = ch;
		patternData[i].noteOffData->setAttribute("Index", String(i));

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
		model->addChildElement(patternData[i].noteOffData);
		//model->addChildElement(variation[i].shadowPatternOff); 
		//model->addChildElement(variation[i].shadowPatternOn);   do not save these - they wil be regenerated at load time
	}
	
} // TopiaryBeatsModel

//////////////////////////////////////////////////////////////////////////////////////////////////////


TopiaryBeatsModel::~TopiaryBeatsModel()
{		
	XmlElement toDelete("toDelete");

	for (int i = 0; i < 8; ++i)
	{
		// avoid dangling pointers
		variation[i].currentPatternChild = nullptr;
		variation[i].currentPatternChildOff = nullptr;

		// ensure these will get destroyed
		toDelete.addChildElement(variation[i].patternOff);
		toDelete.addChildElement(variation[i].patternOn);
		//jassert(variation[i].patternOn != nullptr);
		
	}
} //~optiaryBeatsModel


/////////////////////////////////////////////////////////////////////////

void TopiaryBeatsModel::getMasterModel(XmlElement** h, XmlElement** d, XmlElement** ph, XmlElement** pd)  {
	// returns the data andheaderlists
	*h = patternListHeader;
	*d = patternListData;
	*ph = poolListHeader;
	*pd = poolListData;
}

///////////////////////////////////////////////////////
// pattern methods
///////////////////////////////////////////////////////


int TopiaryBeatsModel::getNumPatterns() 
{
	return numPatterns;
} // getNumPatterns

///////////////////////////////////////////////////////

int TopiaryBeatsModel::getPatternLengthInMeasures(int i)  
{
	// length of pattern in Measures
	auto pattern = patternListData->getChildByAttribute("ID", String(i));
	if (pattern != nullptr)
		return pattern->getIntAttribute("Measures");
	else
		return 1;  // this should only happen during initialisation, before data is initialized (or restored from state)
} // getPatternLengthInMeasures

///////////////////////////////////////////////////////////////////////

void TopiaryBeatsModel::deletePattern(int i)
{   
	jassert(i > -1); // has to be a valid row to delete
	jassert(i < patternListData->getNumChildElements()); // number has to be smaller than number of children (it starts at 0 
	
	// now delete the pattern itself (the NOTEDATA)
														 
	//remove from the patternlist - find it first
	auto child = patternListData->getChildElement(i);
	removeFromModel("PatternData", child);

	// now delete the pattern itself (the NOTEDATA in patternData[i])
	patternData[i].noteData->deleteAllChildElements();
	patternData[i].noteOffData->deleteAllChildElements();
	beatsLog("Pattern deleted.", Topiary::LogType::Info);
	broadcaster.sendActionMessage("masterTables");

	// if there are no patterns all variations need to be disabled
	if (numPatterns == 0)
	{
		for (int j = 0; j < 8; j++ )
			variation[j].enabled = false;
		broadcaster.sendActionMessage("variationDefinition");  // because we may need to turn off the enable button there!
		broadcaster.sendActionMessage("variationEnables"); 
	}

	// if there are variations that use patterns higher than this one, lower their patternToUse by -1!
	for (int j=0; j < 8; j++)
	{
		if (variation[j].patternToUse > (i + 1))
			variation[j].patternToUse--;
	}

} // deletePattern

///////////////////////////////////////////////////////////////////////

void TopiaryBeatsModel::addPattern()
{
	int i = patternListData->getNumChildElements();

	if (i == 8)
	{
		beatsLog("Number of patterns is limited to 8.", Topiary::LogType::Warning);
		return;
	}

	XmlElement *newPattern = addToModel("PatternData");
	newPattern->setAttribute("ID", String(i + 1));
	newPattern->setAttribute("Name", "-- empty --");
	newPattern->setAttribute("Measures", "1");
	//newPattern->setAttribute("LenInTicks", "0");

	beatsLog("New empty pattern created.", Topiary::LogType::Info);
	broadcaster.sendActionMessage("masterTables");
} // addPattern

///////////////////////////////////////////////////////////////////////

void TopiaryBeatsModel::insertPatternFromFile(const File& fileToRead, int patternIndex)
{   // patternIndex starts at 0

	bool msg;
	jassert(patternIndex > -1);  // nothing selected in the model

	numPatterns = patternListData->getNumChildElements();
	jassert(patternIndex <= numPatterns);

	XmlElement* patternParent = patternListData->getChildElement(patternIndex);

	// now the pattern of the DATA objects; i.e; the PATTERN object is in patternData[patternIndex].noteData;
	auto pattern = patternData[patternIndex].noteData;
	// if there is data in the parent, delete it!
	if (pattern->getNumChildElements() != 0) pattern->deleteAllChildElements();

	auto patternOff = patternData[patternIndex].noteOffData;
	// if there is data in the parent, delete it!
	if (patternOff->getNumChildElements() != 0) patternOff->deleteAllChildElements();


	int patternMeasures = 0;
	int lenInTicks = 0;
	msg = loadMidiDrumPattern(fileToRead, patternData[patternIndex].noteData, patternData[patternIndex].noteOffData, patternIndex, patternMeasures, lenInTicks);
	if (!msg)
	{
		return; // file not read for some reason; reason is logged!
	}

	// all OK now, so we set the length and the name now
	// CAREFUL - len in measures & name are in XML, patlentinTicks is in the pattern[] structure!! (because that one is not edited in a table !!!!
	patternData[patternIndex].patLenInTicks = lenInTicks;
	patternParent->setAttribute("Name", fileToRead.getFileName());
	patternParent->setAttribute("Measures", patternMeasures);

	rebuildPool();
	broadcaster.sendActionMessage("masterTables");
	return;
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

		numPatterns ++;
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
		auto patternDataID = child->getIntAttribute("ID");
		patternListData->removeChildElement(child, true);
	    //now renumber by REALID
		renumberList(patternListData); 
		numPatterns--;	
		// now remove the patterns themselves!!!
		patternData[patternDataID].noteData->deleteAllChildElements();
		patternData[patternDataID].noteOffData->deleteAllChildElements();
		return;
	}
	if (!strcmp(type, "PoolData"))
	{
		auto poolDataID = child->getStringAttribute("ID");
		
		poolListData->removeChildElement(child, true);
		
		//now renumber by REALID
		renumberList(poolListData);
		numPoolNotes--;
		
		return;
	}
	jassert(false); // wrong type as input!!!

}


//////////////////////////////////////////////////////////////////////////////////
// NOTE POOL METHODS
//////////////////////////////////////////////////////////////////////////////////

void TopiaryBeatsModel::deletePool(int i)
{
	jassert(i > -1); // has to be a valid row to delete
	jassert(i < poolListData->getNumChildElements()); // number has to be smaller than number of children (it starts at 0 

	//remove from the poollist - find it first
	auto child = poolListData->getChildElement(i);
	removeFromModel("PoolData", child);
	broadcaster.sendActionMessage("masterTables");
	beatsLog("Note pool deleted.", Topiary::LogType::Info);
} // deletePool

///////////////////////////////////////////////////////////////////////

void TopiaryBeatsModel::addPool()
{
	int i = poolListData->getNumChildElements();

	XmlElement *newPool = addToModel("PoolData");
	newPool->setAttribute("ID", String(i + 1));
	newPool->setAttribute("Note", MidiMessage::getMidiNoteName(1, true, true, 5));
	newPool->setAttribute("Label", MidiMessage::getMidiNoteName(1, true, true, 5));
	newPool->setAttribute("Pool", "1");
	newPool->setAttribute("Channel", "10");
	newPool->setAttribute("NoteNumber", "1");
	broadcaster.sendActionMessage("masterTables");
	beatsLog("Pool note created.", Topiary::LogType::Info);
} //addPool

///////////////////////////////////////////////////////////////////////

void TopiaryBeatsModel::rebuildPool()
{  // looks at all patterns and rebuilds the pool
   // existing pool data is not overwritten!
   // called after inserting midi file; possibly also with a button
   // deleting a pattern does NOT rebuild the pool!

	SortedSet<int> set;
	// loop over all patterns and make set of inuque ones
	
	for (int p = 0; p < 8; p++)
	{
		forEachXmlChildElement(*patternData[p].noteData, child)
		{
			auto noteNumber = child->getIntAttribute("Note");
			set.add(noteNumber);			
		}
	}

	// loop over the pool and add "*a" to labels (indicating note not used)
	forEachXmlChildElement(*poolListData, child)
	{
		auto label = child->getStringAttribute("Label");
		if (label.indexOfWholeWord("**") == -1)
			child->setAttribute("Label", String("**") + label);
	}

	// loop over the set, see if in the pool
	// if so do housekeeping (note names and GM drum map names), if needed remove "**" from label 
	// if not, add to pool
	for (auto it = set.begin(); it != set.end(); ++it) 
	{
		auto poolNote = poolListData->getChildByAttribute("NoteNumber", String(*it));
		if (poolNote != nullptr)
		{
			// cleanup; translate noteNumber into readable note
			poolNote->setAttribute("Note", MidiMessage::getMidiNoteName(*it, true, true, 5));
			// remove the "**"
			poolNote->setAttribute("Label", poolNote->getStringAttribute("Label").substring(2));
			// find GM drum name and fill out - TO DO
		}
		else
		{
			XmlElement *newPool = addToModel("PoolData");
			
			newPool->setAttribute("Note", MidiMessage::getMidiNoteName(*it, true, true, 5));
			newPool->setAttribute("Label", MidiMessage::getMidiNoteName(*it, true, true, 5));
			newPool->setAttribute("Pool", "1");
			newPool->setAttribute("Channel", "10");
			newPool->setAttribute("NoteNumber",*it);
		}
	}
	// refesh the pool table!
	renumberPool(poolListData);
	broadcaster.sendActionMessage("masterTables");
}

///////////////////////////////////////////////////////////////////////

void TopiaryBeatsModel::setGMDrumMapLabels() 
{
	for (int i = 0; i < poolListData->getNumChildElements(); i++) {
		auto child = poolListData->getChildElement(i);
		int note = child->getIntAttribute("NoteNumber");
		child->setAttribute("Label", GMDrumMap(note));
	}
} // setGMDrumMapLabels

///////////////////////////////////////////////////////////////////////
// LOGGER
///////////////////////////////////////////////////////////////////////

String* TopiaryBeatsModel::getBeatsLog()
{
	//logDirty = false;
	return &logString;
}

///////////////////////////////////////////////////////////////////////

void TopiaryBeatsModel::beatsLog(String s, int logType)
{
	switch (logType)
	{
	case Topiary::LogType::Debug: 
		if (!logDebug) return;
		break;
	case Topiary::LogType::MidiIn:
		if (!logMidiIn) return;
		break;
	case Topiary::LogType::MidiOut:
		if (!logMidiOut) return;
		break;
	case Topiary::LogType::Warning:
		if (!logWarning) return;
		break;
	case Topiary::LogType::Info:
		if (!logInfo) return;
		break;
	case Topiary::LogType::Transport:
		if (!logTransport) return;
		break;
	case Topiary::LogType::Variations:
		if (!logVariations) return;
		break;
	case Topiary::LogType::License:
		break;
	default: return;
		break;
	}
		 
	Time t = Time::getCurrentTime();
	char pre[20];
	if (logType == Topiary::LogType::License)
		strcpy(pre, "");
	else
		sprintf(pre,"%02d:%02d:%02d:%03d : ", t.getHours(), t.getMinutes(), t.getSeconds(), t.getMilliseconds());
	
	logString.append(String(pre) +  s + newLine, 250);
	
	broadcaster.sendActionMessage("log");
	if (logType == Topiary::LogType::Warning) {
		broadcaster.sendActionMessage("warning");
		lastWarning = s;
	}
}  // beatsLog

String TopiaryBeatsModel::getLastWarning()
{
	return lastWarning;
}

///////////////////////////////////////////////////////////////////////

void TopiaryBeatsModel::setBeatsLogSettings(bool warning, bool midiIn, bool midiOut, bool debug, bool transport, bool variations, bool info)
{
	bool updateNeeded = false;

	if (logWarning != warning)
	{
		logWarning = warning;
		updateNeeded = true;
	}

	if (logMidiIn != midiIn)
	{
		logMidiIn = midiIn;
		updateNeeded = true;
	}

	if (logMidiOut != midiOut)
	{
		logMidiOut = midiOut;
		updateNeeded = true;
	}

	if (logDebug != debug)
	{
		logDebug = debug;
		updateNeeded = true;
	}
	
	if (logTransport != transport)
	{
		logTransport = transport;
		updateNeeded = true;
	}
	
	if (logVariations != variations)
	{
		logVariations = variations;
		updateNeeded = true;
	}
	
	if (logInfo != info)
	{
		logInfo = info;
		updateNeeded = true;
	}
	

	if (updateNeeded) 
		broadcaster.sendActionMessage("log");

} // setBeatsLogSettings

///////////////////////////////////////////////////////////////////////

void TopiaryBeatsModel::clearBeatsLog()
{
	logString = "";
	broadcaster.sendActionMessage("log");

} // clearBeatsLog

///////////////////////////////////////////////////////////////////////

void TopiaryBeatsModel::getBeatsLogSettings(bool& warning, bool& midiIn, bool& midiOut, bool& debug, bool &transport, bool &variations, bool &info)
{
	warning = logWarning;
	midiIn = logMidiIn;
	midiOut = logMidiOut;
	debug = logDebug;
	transport = logTransport;
	variations = logVariations;
	info = logInfo;
} // getBeatsLogSettings

///////////////////////////////////////////////////////////////////////
// TimeSignatures
///////////////////////////////////////////////////////////////////////

void TopiaryBeatsModel::setOverrideHostTransport(bool o)
{
	const GenericScopedLock<SpinLock> myScopedLock(lockModel);

	if (overrideHostTransport != o)
	{
		overrideHostTransport = o;
		//transportDirty = true;
		broadcaster.sendActionMessage("transport");
		// careful here - if we just set the runstate to Stopped - it might have been stopped already and variables may be undefined
		// dirty hack below
		runState = -1000000; // to force a runstate stopped below!!!
		setRunState(Topiary::Stopped);
		if (o)
		{
			beatsLog(String("Host transport overriden."), Topiary::LogType::Transport);
			recalcRealTime();
		}
		else
		{
			beatsLog(String("Host transport in control."), Topiary::LogType::Transport);
			recalcRealTime();
		}
		
	}
}

///////////////////////////////////////////////////////////////////////

void TopiaryBeatsModel::getTransportState (int& b, int& n, int& d, int& bs, bool& override, bool &waitFFN)
 // all transport variables, inc signature, BPM etc
{
	b = BPM;
	n = numerator;
	d = denominator;
	bs = runState;
	waitFFN = WFFN;
	override = overrideHostTransport;
	//if (clearDirty) transportDirty = false;
}

///////////////////////////////////////////////////////////////////////

void TopiaryBeatsModel::setNumeratorDenominator(int nu, int de)
{
	const GenericScopedLock<SpinLock> myScopedLock(lockModel);
	if ((numerator != nu)||(denominator != de))
	{
		numerator = nu;
		denominator = de;
		recalcRealTime();
		beatsLog(String("Signature set to ") + String(numerator) + String("/") + String(denominator), Topiary::LogType::Transport);
		//transportDirty = true;
		broadcaster.sendActionMessage("transport");
	}
}

///////////////////////////////////////////////////////////////////////

void TopiaryBeatsModel::setBPM(int n)
{
	const GenericScopedLock<SpinLock> myScopedLock(lockModel);
	if (n == 0) n = 120;  // do not allow 0 BPM
	if (BPM != n)
	{
		BPM = n;
		recalcRealTime(); // housekeeping!
		beatsLog(String("BPM set to ")+String(n), Topiary::LogType::Transport);
		//transportDirty = true;
		broadcaster.sendActionMessage("transport");
	}
} // setBPM

///////////////////////////////////////////////////////////////////////

void TopiaryBeatsModel::setRunState(int n)
{
//	const GenericScopedLock<SpinLock> myScopedLock(lockModel);
	int remember;
	remember = runState;  // needed because in 1 case setting to Armed should fail!!!

	if (runState != n)
	{
		runState = n;
		switch (n)
		{
		case  Topiary::Running :
			beatsLog("Start running.", Topiary::LogType::Transport);
			Logger::outputDebugString("RTcur to 0");
			// initialize the patterncursor
			patternCursorOn = 0;
			patternCursorOff = 0;
			blockCursor = 0;
			cursorToStop = (int64) -1;
			// and do in all patterns
			for (int i = 0; i < 8; ++i)
			{
				variation[i].currentPatternChild = nullptr;
				variation[i].currentPatternChildOff = nullptr;
			}
			//////////////////////////// CAREFUL - TODO !!!!
			// assumes startSampleTime gets set now in processor, except when called from editor; 
			// in that case set it here (write below!)
			broadcaster.sendActionMessage("transport");
			break;

		case Topiary::Stopped :
			
			// reset stuff
			patternCursorOn = 0;
			patternCursorOff = 0;
			blockCursor = 0;
			cursorToStop = (int64)-1;
			// and do in all patterns
			for (int i = 0; i < 8; ++i)
			{
				variation[i].currentPatternChild = nullptr;
				variation[i].currentPatternChildOff = nullptr;
			}
			beatsLog("Stopped.", Topiary::LogType::Transport);
			broadcaster.sendActionMessage("transport");
			break;
		case Topiary::Ending :
			beatsLog("Ending, cleaning up last notes.", Topiary::LogType::Transport);
			broadcaster.sendActionMessage("transport");
			break;
		case Topiary::Ended :
			// Note we use Ended, not Stopped - this only for the case of an ending pattern (not implemented yet (8/11/2018).  Because the transport 
			// may still be running, and if state is stopped and transport starts running it 
			// would restart or re-arm!!!
			// DELETE THIS STATE
			beatsLog("Ended, done with last notes.", Topiary::LogType::Transport);
			// reset stuff
			patternCursorOn = 0;
			patternCursorOff = 0;
			blockCursor = 0;
			cursorToStop = (int64)-1;
			// and do in all patterns
			for (int i = 0; i < 8; ++i)
			{
				variation[i].currentPatternChild = nullptr;
				variation[i].currentPatternChildOff = nullptr;
			}
			break;
		case Topiary::Armed:
			if (numPatterns == 0)
			{
				beatsLog("Cannot run because there is no pattern data loaded.", Topiary::LogType::Warning);
				runState = remember;
			}
			else
			{
				beatsLog("Armed, waiting for first note.", Topiary::LogType::Transport);
				broadcaster.sendActionMessage("transport");
			}
			break;
		default:
			break;
		}
		//transportDirty = true;
		broadcaster.sendActionMessage("transport");
	}
	
} // setRunState

///////////////////////////////////////////////////////////////////////

/*
bool TopiaryBeatsModel::isTransportDirty()
{
	return false; // transportDirty;
} // transportDirty
*/
///////////////////////////////////////////////////////////////////////


void TopiaryBeatsModel::processTransportControls(int buttonEnabled)  // buttonEnabled; 0=Stop; 1=start; 2=rec
{   
	// careful - transport has 3 buttons but runState has more states!!! need logic to convert here!
	const GenericScopedLock<SpinLock> myScopedLock(lockModel);
	// can only be called when overiden
	jassert(overrideHostTransport);

	if (buttonEnabled == 1)
	{   
		//  start button pressed
		if ((runState != Topiary::Running) && (runState != Topiary::Armed))
		{
			setRunState(Topiary::Armed);  // if override then either processblock will switch it to Running asap, or processblock will set it to Running at first note 
			// else do nothing otherwise it would restart!	
			broadcaster.sendActionMessage("transport");
		}
	}
	else
	{   // stop button pressed
		if (buttonEnabled == 0)
		{
			if (runState == Topiary::Armed)
			{
				setRunState(Topiary::Stopped);  // because then it never got started in the first place
				broadcaster.sendActionMessage("transport");
				return;
			}
			else
			{
				if (runState == Topiary::Running)
				{
					setRunState(Topiary::Ending);  // it will go to Stopped in processblock, when the time has come (depending on runStopQ)
					broadcaster.sendActionMessage("transport");
				}
				else
				{
					setRunState(Topiary::Stopped);
					broadcaster.sendActionMessage("transport");
				}
				return;
			}
			
		}
		else
		{
			jassert(false); // REC not written yet
		}

	}

} // processTransportControls


///////////////////////////////////////////////////////////////////////
//  VARIATIONS
///////////////////////////////////////////////////////////////////////

void TopiaryBeatsModel::getVariation( int& running, int& selected)
{
	running = variationRunning;
	selected = variationSelected;
	return;
} // getVariation

///////////////////////////////////////////////////////////////////////

void TopiaryBeatsModel::setVariation(int n)
{
	jassert(n < 9);
	jassert(n > 0);
	const GenericScopedLock<SpinLock> myScopedLock(lockModel);

	if (n != variationSelected)
	{   		
		variationSelected = n;
		if (runState == Topiary::Stopped)
			variationRunning = n;
		beatsLog(String("Variation ") + String(n) + String(" selected."), Topiary::LogType::Variations);
		//variationsDirty = true;
		broadcaster.sendActionMessage("variations");
	}
	
} // setVariation


///////////////////////////////////////////////////////////////////////
// GENERATOR STUFF
///////////////////////////////////////////////////////////////////////

void TopiaryBeatsModel::setSampleRate(double sr)
{
	if (sr != sampleRate)  // not really needed because also checked in processor !!!
	{
		sampleRate = sr;
		recalcRealTime(); // housekeeping
		beatsLog(String("Samplerate set to ")+String(sampleRate)+String("."), Topiary::LogType::Debug);
	}

} // setSampleRate

///////////////////////////////////////////////////////////////////////

void TopiaryBeatsModel::setStartTimes()
{ // time in samples when running really starts + other housekeeping
	//rtCursor = 0;
	blockCursor = 0;
	variationRunning = variationSelected;
	for (int i=0; i<4; i++)
		for (int j = 0; j < 8; j++)
		{
			// initialize variation regeneration
			variation[j].poolIdCursor[i] = 1;
			variation[j].poolTickCursor[i] = 0;
		}
	broadcaster.sendActionMessage("variations");  // so orange waiting button turn blue;
	
} // setStartTimes

///////////////////////////////////////////////////////////////////////

void TopiaryBeatsModel::setBlockSize(int blocksz)
{
	blockSize = blocksz;
	beatsLog(String("Blocksize ") + String(blockSize), Topiary::LogType::Debug);
}

///////////////////////////////////////////////////////////////////////

void TopiaryBeatsModel::generateMidi(MidiBuffer* midiBuffer)
{ // main Generator

  const GenericScopedLock<SpinLock> myScopedLock(lockModel);

	/*************************************************************************************************************************************************
	Uses a lot of model variables!  Summary of what is needed for what here

	variation[variationRunning - 1].patternOn;
	variation[variationRunning - 1].patternOff;
	variation[variationRunning - 1].currentPatternChild;
    variation[variationRunning - 1].currentPatternChildOff;
	variation[variationRunning - 1].lenInTicks;
	variation[variationRunning].lenInMeasures;   DO WE NEED THIS, THINK NOT!

	int64 blockCursor;					// sampletime of start of current block 
	int64 nextRTGenerationCursor;		// real time cursor of next event to generate 
	int blockSize;						// size of block to generate
	int patternCursorOn;				// ticks where we are within the variation/pattern - if we do nothing it should still advance with blocksize/samplesPerTick
	int patternCursorOff;				// ticks where we are within the variation/pattern - if we do nothing it should still advance with blocksize/samplesPerTick
	
    **************************************************************************************************************************************************/

  int64 rtCursorFrom;				// sampletime to start generating from
  int64 rtCursorTo;					// we generate in principle till here
  int64 rtCursor;					// where we are, between rtCursorFrom and rtCursorTo
  if (blockCursor == 0)  // blockCursor is updated at end of generation!
  {
		rtCursor = 0;
		patternCursorOn = 0;  
		patternCursorOff = 0;
		rtCursorFrom = 0;
		rtCursorTo = rtCursorFrom + blockSize;
		nextRTGenerationCursor = 0;
		measure = 0;
		beat = 0;
		tick = 0;
		noteOnSet.clear();
  }
  else
  {
		rtCursorFrom = blockCursor;
		rtCursorTo = rtCursorFrom + blockSize;
		rtCursor = rtCursorFrom;
		calcMeasureBeat();
		patternCursorOn = (int) floor(blockCursor / samplesPerTick);
		patternCursorOff = (int) floor(blockCursor / samplesPerTick);
  }


  //Logger::outputDebugString("Generate midi; patcur on/off " + String(patternCursorOff) + " / " + String(patternCursorOn));
  //Logger::outputDebugString("next RTcursor " + String(nextRTGenerationCursor));

  jassert(beat >= 0);
  jassert(measure >= 0);

  if (nextRTGenerationCursor > rtCursorTo)
  {
	  blockCursor = blockCursor + blockSize;
	  return;  // nothing to generate now
  }
  
  // be careful with below; when changing patterns need to see if below is really correct!!!!
  XmlElement* parentOn;
  XmlElement* parentOff;
  XmlElement* noteChildOn;
  XmlElement* noteChildOff;
  int parentLength; // in ticks

  parentOn = variation[variationRunning - 1].patternOn;
  parentOff = variation[variationRunning - 1].patternOff;
  noteChildOn = variation[variationRunning - 1].currentPatternChild;
  noteChildOff = variation[variationRunning - 1].currentPatternChildOff;
  parentLength = variation[variationRunning - 1].lenInTicks;

  //jassert(false);

  int nextPatternCursorOn;  // patternCursorOn is global and remembers the last one we generated
  int nextPatternCursorOff; // patternCursorOff is global and remembers the last one we generated
  int ticksTaken;
  int ticksTakenOn;
  int ticksTakenOff;
  MidiMessage msg;
  int noteNumber;
  int length;
  int channel;

  // normally next line does nothing, because the child should already be where expected
  // when switching variations mid-pattern this may actually do something

  patternCursorOn = (int)patternCursorOn % parentLength;  //////// because rtCursors are multi loops !!!!
  patternCursorOff = (int)patternCursorOff % parentLength;

  //Logger::outputDebugString("Next note on to generate afer current tick " + String(patternCursorOn));

  bool walkOn = walkToTick(parentOn, &noteChildOn, patternCursorOn);
  bool walkOff = walkToTick(parentOff, &noteChildOff, patternCursorOff);
  
  //Logger::outputDebugString(String("WalkOn ") + String((int)walkOn));
  //Logger::outputDebugString(String("WalkOff ") + String((int)walkOff));

  if (walkOn || walkOff)
  {
	  // set patternCursors where we are now, so the offsets in sample time are correct
	  patternCursorOn = (int) patternCursorOn % parentLength;  //////// because rtCursors are multi loops !!!!
	  patternCursorOff = (int) patternCursorOff % parentLength;
	   
	  //calcMeasureBeat();

	  while (rtCursor < rtCursorTo)
	  {
		  if (walkOn)
		  {
			  nextPatternCursorOn = noteChildOn->getIntAttribute("Timestamp");
			  
			  ticksTakenOn = nextPatternCursorOn - patternCursorOn;  // ticks taken in this timeframe
			  if (ticksTakenOn < 0)
			  {
				  //Logger::outputDebugString("PatternOnCursor looped over end");
				  ticksTakenOn += parentLength;
				  jassert(ticksTakenOn >= 0);
			  }
		  }
		  else
		  {
			  nextPatternCursorOn = 999999999;
			  ticksTakenOn = 999999999;
		  }

		  if (walkOff)
		  {
			  nextPatternCursorOff = noteChildOff->getIntAttribute("Timestamp");
			  ticksTakenOff = nextPatternCursorOff - patternCursorOff;
			  if (ticksTakenOff < 0)
			  {
				  ticksTakenOff += parentLength;
				  jassert(ticksTakenOff >= 0);
				  //Logger::outputDebugString("PatternOffCursor looped over end");
			  }
		  }
		  else
		  {
			  nextPatternCursorOff = 999999999;
			  ticksTakenOff = 999999999;
		  }

		  if (ticksTakenOff < ticksTakenOn)  // tick taken in this timeframe (not in the pattern!)
			  ticksTaken = ticksTakenOff;
		  else
			  ticksTaken = ticksTakenOn;


		  if ((rtCursor + (int64)(ticksTaken*samplesPerTick)) < rtCursorTo)
		  {
			  //Logger::outputDebugString("GENERATING A NOTE ------------>");
			  //Logger::outputDebugString(String("Next PatcursorOn ") + String(nextPatternCursorOn));
			  //Logger::outputDebugString(String("Next PatcursorOff ") + String(nextPatternCursorOff));
			  
			  if (ticksTakenOn < ticksTakenOff)
			  {
				  ////// GENERATE NOTE ON

				  length = noteChildOn->getIntAttribute("Length");
				  noteNumber = noteChildOn->getIntAttribute("Note");
				  channel = noteChildOn->getIntAttribute("Channel"); ///////// channel is not here - it's in the pool data !!!!
				  msg = MidiMessage::noteOn(10, noteNumber, (float)noteChildOn->getIntAttribute("Velocity") / 128);
 
				  // DEBUG LOGIC !!!!!!!!!
				  // outputting the pattern tick values + the tick value in the generated pattern
				  //int64 cursorInTicks = (int64)floor( (rtCursor + (int64)(ticksTaken*samplesPerTick) ) / samplesPerTick    );  
				  // now do that modulo the patternlenght in ticks
				  //cursorInTicks = cursorInTicks % parentLength;
				  //Logger::outputDebugString("Generated note at realtime pat tick " + String(cursorInTicks) + " / tick in pattern " + String(noteChildOn->getStringAttribute("Timestamp")));
				  /////////////////////////

				  patternCursorOn = nextPatternCursorOn;  // that is the tick of the event ze just generated
				  if (patternCursorOn >= parentLength) patternCursorOn = patternCursorOn - parentLength;
				  nextTick(parentOn, &noteChildOn);
				  //Logger::outputDebugString(String("NOTE ON --------"));
				  rtCursor = rtCursor + (int64)(ticksTaken * samplesPerTick);
				  noteOnSet.add(noteNumber);
			  }
			  else
			  {
				  ////// GENERATE NOTE OFF

				  length = noteChildOff->getIntAttribute("Length");
				  noteNumber = noteChildOff->getIntAttribute("Note");
				  channel = noteChildOff->getIntAttribute("Channel");  ///////// channel is not here - it's in the pool data !!!!
				  msg = MidiMessage::noteOff(10, noteNumber, (float)0.5);
				  //Logger::outputDebugString(String("Note event on pattern tick ") + String(patternCursorOff+ticksTaken));
				  patternCursorOff = nextPatternCursorOff;
				  if (patternCursorOff >= parentLength) patternCursorOff = patternCursorOff - parentLength;
				  nextTick(parentOff, &noteChildOff);
				  //Logger::outputDebugString(String("NOTE OFF --------"));
				  rtCursor = rtCursor + (int64)(ticksTaken * samplesPerTick);
				  noteOnSet.removeValue(noteNumber);
			  }

			  jassert((rtCursor - rtCursorFrom) >= 0);

			  midiBuffer->addEvent(msg, (int) (rtCursor-rtCursorFrom));
 			  
			  //nextRTGenerationCursor = rtCursor;
			  
			  if (ticksTakenOn < ticksTakenOff)
			  {
				  patternCursorOff = patternCursorOn;
			  }
			  else
				  patternCursorOn = patternCursorOff;
				
			  //Logger::outputDebugString(String("nxtcursor ") + String(nextRTGenerationCursor));

		  }  // generated a note (on or off)
		  else
		  {
			  // done for now; next event is over rtCursorTo 
			  // let's place ourselves ready for the next round
			  // so either walkOn and we have the next On note ready
			  // or walkOff and next Off note ready as well
			  // main goal is to set nextRTgenerationCursor!

			  //Logger::outputDebugString(String(" ++++++++++++++ done +++++++++++++++++++++"));

			  if (walkOn)
			  {
				  nextPatternCursorOn = noteChildOn->getIntAttribute("Timestamp");
				  ticksTakenOn = nextPatternCursorOn - patternCursorOn;
				  if (ticksTakenOn < 0)
				  {
					  ticksTakenOn += parentLength;
					  //Logger::outputDebugString(String("Running over end of pattern!!!"));
				  }
			  }
			  
			  if (walkOff)
			  {
				  nextPatternCursorOff = noteChildOff->getIntAttribute("Timestamp");
				  ticksTakenOff = nextPatternCursorOff - patternCursorOff;
				  if (ticksTakenOff < 0) ticksTakenOff += parentLength;
			  }

			  if (ticksTakenOff <= ticksTakenOn)
			  {
				  patternCursorOff = nextPatternCursorOff;
				  ticksTaken = ticksTakenOff;
				  if (patternCursorOff >= parentLength) patternCursorOff = patternCursorOff - parentLength;
			  }
			  else
			  {
				  patternCursorOn = nextPatternCursorOn;
				  ticksTaken = ticksTakenOn;
				  if (patternCursorOn >= parentLength) patternCursorOn = patternCursorOn - parentLength;
			  }
			  
			  if (ticksTakenOff <= ticksTakenOn)
				  patternCursorOn = patternCursorOff;
			  else
				  patternCursorOff = patternCursorOn;

			  // we set rtCursor at the time of the next event; will possibly break out of the loop if not within this block
			  
			  rtCursor = rtCursorTo; // force break out of the loop
			  nextRTGenerationCursor = rtCursorFrom + (int64)(ticksTaken * samplesPerTick);
			  int nextTick = (int) (nextRTGenerationCursor / samplesPerTick) % parentLength;
			  //Logger::outputDebugString("Next tick to generate (off nextRTcursor): " + String(nextTick));
		  }  // end loop over from --> to
	  } 
  }
  else
  {
	  // walk did not find a note to generate	
	  // so pattern is empty or no fitting note events found
	  if ((noteChildOn == nullptr) || (noteChildOff == nullptr))
	  {
		  // make sure our cursors keep running
		  // 		  nextRTGenerationCursor = rtCursorTo + 1;
		  //nextRTGenerationCursor = rtCursorTo;
		  patternCursorOn = +(int)(blockSize / samplesPerTick);
		  patternCursorOff = +(int)(blockSize / samplesPerTick);
	  }
	 
  }

  blockCursor = blockCursor + blockSize;
  
  calcMeasureBeat();

} // generateMidi

///////////////////////////////////////////////////////////////////////

bool TopiaryBeatsModel::processVariationSwitch() // called just before generateMidi - to see if a variation is changed, and if so whether to switch now (and make the switch)
{
	// next block goes from blockCursor to blockCursor+blockSize;
	// generation is at patternCursorOn within the pattern - which is variationRunning-1, not (yet) variationSelected-1
	// and both cursors go from 0 to the tickLength of that pattern
	// we also know that we are currently at rtCursor, and given rtCursor and patternCursorOn we can calculate when the current pattern started in realtime 

	//Logger::outputDebugString(String("Sel ")+String(variationSelected)+String(" Running ")+String(variationRunning));

	if (variationSelected == variationRunning) return false;

	if (blockCursor == 0)
	{
		runState = Topiary::Running;
		Logger::outputDebugString("Starting - runstate is Armed but we need to go!");  // careful that the WFFN functonality keeps working!!!
		return true;
	}
		// in this case we want to start asap (unless we Q on the transport, but that is done in the call before this one!!!
	
	//Logger::outputDebugString("Need to switch, now ???");
	//Logger::outputDebugString("blockcursor "+String(blockCursor)+" NextRTcursor "+String(nextRTGenerationCursor));

	// nothing if gong to happen now, so don't bother calculating
	if (nextRTGenerationCursor > (blockCursor + 2*blockSize))	return false;  

	// first decide whether we should switch in this block

	// we are at measure/beat/tick overall; time and tick apply to the pattern we are in, but not measure within the pattern so we calculate that by using patternCursorOn
	//int patternMeasure = patternCursorOn % (numerator * Topiary::TICKS_PER_QUARTER);

	Logger::outputDebugString("Need to switch, now ???  Something is going to happen next");

	int64 cursorToSwitch = 0;  // time the switch should happen; if nexRTcursor < ticksToSwith < blockCursor=blockSize then we know we want to switch;
	Logger::outputDebugString("VariationStartQ "+String(variationStartQ));

	switch (variationStartQ)
	{
	case (Topiary::Immediate): 
	{
		cursorToSwitch = blockCursor; // i.e. now
		Logger::outputDebugString(String("IMMEDIATE Sel ") + String(variationSelected) + String(" Running ") + String(variationRunning));
		break;
	}
	case (Topiary::Measure):
	{
		// moment of next measure = blockCursor + time to next beat (ticksperquarter - tick) + #beats to go till end of measure			
		cursorToSwitch = (int64) ( blockCursor + samplesPerTick * ( (Topiary::TICKS_PER_QUARTER - tick -1) + (numerator - beat - 1)* Topiary::TICKS_PER_QUARTER ) );
		Logger::outputDebugString(String("MEASURE Sel ") + String(variationSelected) + String(" Running ") + String(variationRunning));
		break;
	}
	case (Topiary::Quarter):
	{
		// moment of next beat = blockCursor + time to next beat (ticksperquarter - tick) 
		cursorToSwitch = (int64) ( blockCursor + samplesPerTick * ( (Topiary::TICKS_PER_QUARTER - tick - 1) ));
		Logger::outputDebugString(String("QUARTER Sel ") + String(variationSelected) + String(" Running ") + String(variationRunning)) ;
		break;
	}
	case (Topiary::Half):
	{
		// moment of next beat = blockCursor + time to next beat (ticksperquarter - tick) 
		cursorToSwitch = (int64) (blockCursor + samplesPerTick * ((Topiary::TICKS_PER_QUARTER - tick - 1) + Topiary::TICKS_PER_QUARTER));
		Logger::outputDebugString(String("IMMEDIATE Half ") + String(variationSelected) + String(" Running ") + String(variationRunning));
		break;
	}

	} // switch
	Logger::outputDebugString(String("BlockCursor ") + String(blockCursor));
	Logger::outputDebugString(String("CursorToSwitch ") + String(cursorToSwitch));
	
	//if ((nextRTGenerationCursor <= cursorToSwitch) && (cursorToSwitch < (blockCursor + blockSize))) 
	if (cursorToSwitch < (blockCursor + blockSize))
	{

		Logger::outputDebugString("Need to switch, now ??? ----------------------------> yes we're sure");
		// we are sure we're switching variations now
		// so decide where in the variation we start - we need to set patternCursorOn (and we will set patternCursorOff to the same value)

		switch (switchVariation)
		{
		case (Topiary::SwitchFromStart): patternCursorOn = 0;
			break;
		case (Topiary::SwitchWithinBeat): patternCursorOn = patternCursorOn % Topiary::TICKS_PER_QUARTER;
			break;
		case (Topiary::SwitchWithinMeasure): patternCursorOn = patternCursorOn % (Topiary::TICKS_PER_QUARTER*numerator);
			break;
		case (Topiary::SwitchWithinPattern):	patternCursorOn = patternCursorOn % (variation[variationSelected-1].lenInTicks);
			break;
		}

		
		patternCursorOff = patternCursorOn;
		beatsLog(String("Switch from variation ") + String(variationRunning) + String(" to ") + String(variationSelected), Topiary::LogType::Variations);
		variationRunning = variationSelected;
		broadcaster.sendActionMessage("variations");

		return true;
	}
	else return false;
	

} // processVariationSwitch

///////////////////////////////////////////////////////////////////////

bool TopiaryBeatsModel::switchingVariations()
{
	return (variationRunning != variationSelected);

} // switchingVariations

///////////////////////////////////////////////////////////////////////

void TopiaryBeatsModel::endNotesOn(MidiBuffer* midiBuffer)
{
	// end any notes that didn't get a note off yet
	for (auto i = noteOnSet.begin(); i != noteOnSet.end(); ++i)
	{
		MidiMessage msg = MidiMessage::noteOff(10, *i, (float)0.5);
		midiBuffer->addEvent(msg, 1);
	}

	noteOnSet.clear();
} // endNotesOn

///////////////////////////////////////////////////////////////////////

bool TopiaryBeatsModel::processEnding() // called just before generateMidi - to see if we need to stop, and if so whether to stop now (and stop if needed)
{
	// see if we are past the last note
	// we are going to set global variable cursorToStop, based on current block cursor; and only if it is not equal to -1 

	if (cursorToStop == -1)
	{
		switch (runStopQ) {
		case (Topiary::Immediate):
		{
			cursorToStop = blockCursor;
			break;
		}
		case (Topiary::WholePattern):
		{
			cursorToStop = (int64) ( blockCursor - (std::min(patternCursorOn, patternCursorOff)*samplesPerTick) /* start of this pattern */ + (variation[variationRunning].lenInTicks*samplesPerTick) );
			Logger::outputDebugString(String("blockCur ") + String(blockCursor) + String(" stopCursor ") + String(cursorToStop));

			break;
		}
		case (Topiary::Measure):
		{
			cursorToStop = (int64) ( blockCursor + samplesPerTick * ((Topiary::TICKS_PER_QUARTER - tick - 1) + (numerator - beat - 1)* Topiary::TICKS_PER_QUARTER) );
			break; 
		}
		case (Topiary::Half):
		{
			cursorToStop = (int64) ( blockCursor + samplesPerTick * ((Topiary::TICKS_PER_QUARTER - tick - 1) + Topiary::TICKS_PER_QUARTER) );
			break;
		}
		case (Topiary::Quarter):
		{
			cursorToStop = (int64) ( blockCursor + samplesPerTick * ((Topiary::TICKS_PER_QUARTER - tick - 1))) ;
			break;
		}

		} // switch
	}

	// now see if we're close enough to cursorToStop
	if (cursorToStop < (blockCursor + blockSize))
	{
		cursorToStop = -1;
		setRunState(Topiary::Stopped);
		return true;
	}
	else return false;

} // processEnding


///////////////////////////////////////////////////////////////////////
// Settings stuff
///////////////////////////////////////////////////////////////////////

void TopiaryBeatsModel::setWFFN(bool wffn)
{
	WFFN = wffn;
}

///////////////////////////////////////////////////////////////////////

bool TopiaryBeatsModel::getWFFN()
{
	return WFFN;
}

///////////////////////////////////////////////////////////////////////

void TopiaryBeatsModel::setNotePassThrough(bool echo)
{
	notePassThrough = echo;
} // setNotePassThrough

///////////////////////////////////////////////////////////////////////

bool TopiaryBeatsModel::getNotePassThrough()
{
	return notePassThrough;
} // getNotePassThrough

///////////////////////////////////////////////////////////////////////

void TopiaryBeatsModel::setVariationStartQ(int q)
{
	variationStartQ = q;
} // setVariationStartQ

///////////////////////////////////////////////////////////////////////

int TopiaryBeatsModel::getVariationStartQ()
{
	return variationStartQ;
} // getVariationStartQ

///////////////////////////////////////////////////////////////////////

void TopiaryBeatsModel::setRunStopQ(int q)
{
	runStopQ = q;
} // setRunStopQ

///////////////////////////////////////////////////////////////////////

int TopiaryBeatsModel::getRunStopQ()
{
	return runStopQ;
} // setRunStopQ

///////////////////////////////////////////////////////////////////////

void TopiaryBeatsModel::getTime(int& m, int& b)
{
	const GenericScopedLock<SpinLock> myScopedLock(lockModel); // wait if it's generating
	m = measure;
	b = beat;
	
} // setRunStopQ

///////////////////////////////////////////////////////////////////////

String TopiaryBeatsModel::getName()
{
	return name;
} // getName

///////////////////////////////////////////////////////////////////////

void TopiaryBeatsModel::setName(String n)
{
	name = n;
} // setName

///////////////////////////////////////////////////////////////////////

void TopiaryBeatsModel::setSwitchVariation(int a)
{
	switchVariation = a;
} // setSwitchVariation

///////////////////////////////////////////////////////////////////////

int TopiaryBeatsModel::getSwitchVariation()
{
	return switchVariation;
} // getSwitchVariation

///////////////////////////////////////////////////////////////////////
// Broadcaster & listeners
///////////////////////////////////////////////////////////////////////

void TopiaryBeatsModel::setListener(ActionListener *listener)
{
	broadcaster.addActionListener(listener);
} // setListener


void TopiaryBeatsModel::removeListener(ActionListener *listener)
{
	broadcaster.removeActionListener(listener);
} // setListener


///////////////////////////////////////////////////////////////////////
// Variation Parameters
///////////////////////////////////////////////////////////////////////


void TopiaryBeatsModel::getVariationDefinition(int i, bool& enabled, String& vname, int& patternId, bool enables[4], int poolLength[4][3][2])
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

	}
	// need to look up the ID in the items in the patternCombo, and set the patternCombo selection

} // getVariationDefinition

///////////////////////////////////////////////////////////////////////


bool TopiaryBeatsModel::setVariationDefinition(int i, bool enabled, String vname, int patternId, bool enables[4], int poolLength[4][3][2])
{
	// return false if we try to disable the last enabled variation - so we can return the button to enabled
	// also  refuse to enable a variation if there are no patterns!!!

	jassert(i < 8); // goes from 0-7
	variation[i].name = vname;
	variation[i].patternToUse = patternId;
	variation[i].lenInTicks = patternData[patternId].patLenInTicks;
	for (int j = 0; j < 4; j++)
	{
		variation[i].enablePool[j] = enables[j];
		variation[i].measureFrom[j] = poolLength[j][0][0];
		variation[i].measureTo[j] = poolLength[j][0][1];
		variation[i].beatFrom[j] = poolLength[j][1][0];
		variation[i].beatTo[j] = poolLength[j][1][1];
		variation[i].tickFrom[j] = poolLength[j][2][0];
		variation[i].tickTo[j] = poolLength[j][2][1];

 		// calculate the above in ticks
		variation[i].fullTickFrom[j] = variation[i].tickFrom[j] + variation[i].beatFrom[j] * Topiary::TICKS_PER_QUARTER + numerator * variation[i].measureFrom[j] * Topiary::TICKS_PER_QUARTER;
		variation[i].fullTickTo[j] = variation[i].tickTo[j] + variation[i].beatTo[j] * Topiary::TICKS_PER_QUARTER + numerator * variation[i].measureTo[j] * Topiary::TICKS_PER_QUARTER;

		variation[i].lenInTicks = patternData[variation[i].patternToUse-1].patLenInTicks;

		// reset variation generation cursors
		variation[i].poolIdCursor[j] = 1;
		variation[i].poolTickCursor[j] = 0;
	}
	bool refuse = false;
	if (enabled != variation[i].enabled)
	{
		refuse = true;
		int otherEnabled = -1;;  // one other enabled button; so that if we disable the currently 'running' variation, we auto-switch to a random running one
	
		// if we are disabling, make sure at least one other is disabled; if not refuse and log an error
		if (!enabled)
		{
			for (int j = 0; j < 8; j++)
			{
				if (j != i)
					if (variation[j].enabled)
					{
						refuse = false;
						if (otherEnabled == -1) 
							otherEnabled = j;
					}
			}
		}
		else refuse = false;

		if (!refuse)
		{
			enabled = variation[i].enabled = enabled;
			
			if (!enabled && (variationSelected == (i+1)))  // selected and running go from 1 to 8 not 0-7
			{
				// we disabled the currently selected variation - set anotherone as selected
				setVariation(otherEnabled+1); //setVariation works on 1-8, not 0-7
			}
			broadcaster.sendActionMessage("variationEnables");  // so that if needed variationbuttons are disabled/enabled
		}
	}

	broadcaster.sendActionMessage("variationDefinition");

	if (refuse) beatsLog("At least one variation must be selected!", Topiary::LogType::Warning);

	// regenerate!

	generateVariation(i);


	return refuse;

} // setVariationDefinition

///////////////////////////////////////////////////////////////////////

void TopiaryBeatsModel::getVariationEnables(bool enables[8])
{
	for (int i = 0; i < 8; i++)
		enables[i] = variation[i].enabled;

} // getVariationEnables

///////////////////////////////////////////////////////////////////////

void TopiaryBeatsModel::getPatterns(String pats[8])
{
	XmlElement* pattern;

	pattern = patternListData->getFirstChildElement();
	int i = 0;
	while (pattern != nullptr)
	{
		pats[i] = String(i+1) + " " + pattern->getStringAttribute("Name");
		pattern = pattern->getNextElement();
		i++;
	}
} // getPatterns

///////////////////////////////////////////////////////////////////////
// Autoation Parameters
///////////////////////////////////////////////////////////////////////

void TopiaryBeatsModel::setVariationControl(bool ccSwitching, int channel, int switches[8])
{
	ccVariationSwitching = ccSwitching;
	variationSwitchChannel = channel;

	for (int i = 0; i < 8; i++)
	{
		variationSwitch[i] = switches[i];
	}

} // setVariationControl

///////////////////////////////////////////////////////////////////////

void TopiaryBeatsModel::getVariationControl(bool& ccSwitching, int& channel, int switches[8]) 
{
	ccSwitching = ccVariationSwitching;
	channel = variationSwitchChannel;

	for (int i = 0; i < 8; i++)
	{
		switches[i] = variationSwitch[i];
	}

} // getVariationControl

///////////////////////////////////////////////////////////////////////

void TopiaryBeatsModel::processAutomation(MidiMessage& msg)
{
	// assert that called only calls this consistently: msg is CC if ccVariationSwithing; msg is NoteOn it not
	jassert(((ccVariationSwitching && msg.isController()) || ((!ccVariationSwitching) && msg.isNoteOn())));
	
	int channel = msg.getChannel();
	int note;
	int controller;

	if (ccVariationSwitching)
	{
		// we use midi CC messages for variation switching
		for (int i = 0; i < 8; i++)
		{
			controller = msg.getControllerNumber();
			if (controller == variationSwitch[i])
			{
				if ((variationSwitchChannel == 0) || (variationSwitchChannel == channel))
				{
					setVariation(i+1); // setVariation goes from 1-8; not 0-7!
					break;
				}
			}
		}
	}
	else
	{
		// we use note on events for variation switching
		for (int i = 0; i < 8; i++)
		{
			note = msg.getNoteNumber();
			if (note == variationSwitch[i])
			{
				if ((variationSwitchChannel == 0) || (variationSwitchChannel == channel))
				{
					setVariation(i + 1); // setVariation goes from 1-8; not 0-7!
					break;
				}
			}
		}
	}
} // processAutomation

////////////////////////////////////////////////////////////////////////////////////////////////////////////

void TopiaryBeatsModel::generateVariation(int i)
{
	// (re)generates the variation as a separate XmlElement
	// when done, it locks the model and swaps it in with the shadowpattern and destroys the old one
	// uses the variation's generation cursors, which need to be reset when the variation gets edited (and when program loads)
	

	auto newPatternOn = new XmlElement("ShadowPatternOn"); 
	auto newPatternOff = new XmlElement("ShadowPatternOff");

	XmlElement* patternOn;
	patternOn = patternData[i].noteData;
	
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
		note = child->getIntAttribute("NoteNumber");
		pool = child->getIntAttribute("Pool");
		poolNote[note] = pool-1;
		poolHasNotes[pool - 1] = true;
	} 

	XmlElement* newChild = nullptr;

	// set poolTo values if measureTo = -1;

	if (variation[i].enabled) // otherwise it may not have valid settings!!!
		for (int j = 0; j < 4; j++)
		{
			if (variation[i].measureTo[j] == -1)
			{
				XmlElement* pattern = patternListData->getChildByAttribute("ID", String(variation[i].patternToUse));
				if (pattern == nullptr)
				{
					// a pattern got deleted and no longer exists; autocorrect - set to 1; there will always be at least 1 pattern otherwise te variation editor is disabled!
					variation[i].patternToUse = 1;
					pattern = patternListData->getChildByAttribute("ID", String(variation[i].patternToUse));
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
		// only call generatePool if there are notes in the pool; otherwise generatePool will loo(p forever!!![j]) 
		if (poolHasNotes[j]) 
			generatePool(i, j, newPatternOn, poolNote);
	}
	
	// now generate the noteOff data
	newChild = nullptr;

	forEachXmlChildElement(*newPatternOn, child)
	{
		// create a copy of child with the variation parameters taken into account
		newChild = new XmlElement("DATA");

		newChild->setAttribute("ID", child->getStringAttribute("ID"));
		newChild->setAttribute("Note", child->getStringAttribute("Note"));
		newChild->setAttribute("Velocity", child->getStringAttribute("Velocity"));
		newChild->setAttribute("Timestamp", child->getIntAttribute("Timestamp") + child->getIntAttribute("Length")  );
		newChild->setAttribute("Measure", child->getStringAttribute("Measure"));
		newPatternOff->prependChildElement(newChild);
	}

	
	renumberNotes(newPatternOn);
	renumberNotes(newPatternOff);

	Logger::writeToLog("-------------------- VARIATION " + String(i) + " GENERATED ---------------------");
	//Logger::writeToLog("Measures: " + String(lenInMeasures) + " --- Ticks " + String(lenInTicks));

	String myXmlDoc2 = newPatternOn->createDocument(String());
	Logger::writeToLog(myXmlDoc2);

	// swap the patterns; SHOULD BE A CRITICAL SECTION
	// TODO - set the variation[i].currentPatternChild and variation[i].currentPatternChildOff; - or set those to nullptr and generateMidi will figure it out???

	XmlElement *remember;

	remember = newPatternOn;
	newPatternOn = variation[i].patternOn;
	variation[i].patternOn = remember;
	
	remember = newPatternOff;
	newPatternOff = variation[i].patternOff;
	variation[i].patternOff = remember;

	variation[i].lenInTicks = patternData[i].patLenInTicks;

	//variation[i].lenInMeasures = lenInMeasures;

	delete newPatternOff;
	delete newPatternOn; 
	
	
} // generateVariation

