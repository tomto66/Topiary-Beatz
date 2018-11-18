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
	beatsLog(String("Topiary Beats V ")+String(xstr(JucePlugin_Version))+String(" (c) Tom Tollenaere 2018."));
	beatsLog(String(""));
	beatsLog(String("Topiary Beats is free software : you can redistribute it and/or modify"));
	beatsLog(String("it under the terms of the GNU General Public License as published by"));
	beatsLog(String("the Free Software Foundation, either version 3 of the License, or"));
	beatsLog(String("(at your option) any later version."));
	beatsLog(String(""));
	beatsLog(String("Topiary Beats is distributed in the hope that it will be useful,"));
	beatsLog(String("but WITHOUT ANY WARRANTY; without even the implied warranty of"));
	beatsLog(String("MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the"));
	beatsLog(String("GNU General Public License for more details."));
	beatsLog(String(""));

	//variationsDirty = true; // make sure screen initializes ok
	//broadcaster.sendActionMessage("variations"); - useless because the editor won't be there (yet)
	//transportDirty = true;
	overrideHostTransport = false;
	runState = Topiary::Stopped;
	//broadcaster.sendActionMessage("transport");
	//broadcaster.sendActionMessage("log");
	BPM = 60;
	numerator = 2; denominator = 2;
	
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

	child = new XmlElement("COLUMN");
	child->setAttribute("columnId", "5");
	child->setAttribute("name", "Channel");
	child->setAttribute("width", "100");
	child->setAttribute("type", "int");
	child->setAttribute("min", "1");
	child->setAttribute("max", "16");
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
		variation[i].patternToUse = i;								// index in patterndata
		variation[i].patternOn = patternData[i].noteData;			// pattern On note events not needed - found via patternToUse
		variation[i].patternOff = patternData[i].noteOffData;		// pattern Off note events
		variation[i].name = "Pattern 1";
		variation[i].playPool1 = true;
		variation[i].playPool2 = true;
		variation[i].playPool3 = true;
		variation[i].playPool4 = true;
		variation[i].lenInMeasures = 0;
		variation[i].lenInTicks = 0;
		variation[i].ending = false;									// indicates that once pattern played, we no longer generate notes! (but we keep running (status Ended) till done
		//child = new XmlElement("ShadowPatternOn"); LEAK
		//variation[i].shadowPatternOn = child;
		//variation[i].shadowPatternOn->setAttribute("Index", String(i));
		//child = new XmlElement("ShadowPatternOff");
		//variation[i].shadowPatternOff = child;
		//variation[i].shadowPatternOff->setAttribute("Index", String(i));
		variation[i].shadow = false;									// if true generator should use shadowpatterns

	}
		
	/////////////////////////////////////
	// Patterndata initialization
	/////////////////////////////////////

	for (int i = 0; i < 8; ++i)
	{
		auto ch = new XmlElement("Pattern");
		patternData[i].noteData = ch;
		patternData[i].noteData->setAttribute("Index",String(i));
		ch = new XmlElement("PatternOff");
		patternData[i].noteOffData = ch;
		patternData[i].noteOffData->setAttribute("Index", String(i));
	}

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
		//model->addChildElement(variation[i].shadowPatternOn);   LEAK
	}
	
}
TopiaryBeatsModel::~TopiaryBeatsModel()
{		
	// avoid dangling pointers to children in patterns
	// the children themselves will be destroyed when the pattern parent is deallocated!
	for (int i = 0; i < 8; ++i)
	{
		variation[i].currentPatternChild = nullptr;
		variation[i].currentPatternChildOff = nullptr;
	}
}


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


int TopiaryBeatsModel::getNumPatterns() {
	return numPatterns;
}

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
	// patternData[i].noteData  TODO
	// patternData[i].noteOffData TODO
	beatsLog("Pattern deleted.");
}

///////////////////////////////////////////////////////////////////////

void TopiaryBeatsModel::addPattern()
{
	int i = patternListData->getNumChildElements();

	if (i == 8)
	{
		beatsLog("Number of patterns is limited to 8.");
		return;
	}

	XmlElement *newPattern = addToModel("PatternData");
	newPattern->setAttribute("ID", String(i + 1));
	newPattern->setAttribute("Name", "-- empty --");
	newPattern->setAttribute("Measures", "1");
	newPattern->setAttribute("LenInTicks", "0");

	beatsLog("New empty pattern created.");
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
	// CAREFUL - len in leasure & name are in XML, patlentinTicks is in the pattern[] structure!! (because that one is not edited in a table !!!!
	patternData[patternIndex].patLenInTicks = lenInTicks;
	patternParent->setAttribute("Name", fileToRead.getFileName());
	patternParent->setAttribute("Measures", patternMeasures);

	// temporary variation administration
	variation[patternIndex].lenInTicks = lenInTicks;
	variation[patternIndex].lenInMeasures = patternMeasures;
	variation[patternIndex].patternOn = patternData[patternIndex].noteData;
	variation[patternIndex].patternOff = patternData[patternIndex].noteOffData;
	rebuildPool();

	return;
} // insdertPatternFromFile

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
}


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
}

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

	beatsLog("Note pool deleted.");
} // deletePool

///////////////////////////////////////////////////////////////////////

void TopiaryBeatsModel::addPool()
{
	int i = poolListData->getNumChildElements();

	XmlElement *newPool = addToModel("PoolData");
	newPool->setAttribute("ID", String(i + 1));
	newPool->setAttribute("Note", MidiMessage::getMidiNoteName(1, true, true, 4));
	newPool->setAttribute("Label", MidiMessage::getMidiNoteName(1, true, true, 4));
	newPool->setAttribute("Pool", "1");
	newPool->setAttribute("Channel", "10");
	newPool->setAttribute("NoteNumber", "1");

	beatsLog("Pool note created.");
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
			auto noteNumber = topiaryStoi(child->getStringAttribute("Note") );
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
			poolNote->setAttribute("Note", MidiMessage::getMidiNoteName(*it, true, true, 4));
			// remove the "**"
			poolNote->setAttribute("Label", poolNote->getStringAttribute("Label").substring(2));
			// find GM drum name and fill out - TO DO
		}
		else
		{
			XmlElement *newPool = addToModel("PoolData");
			
			newPool->setAttribute("Note", MidiMessage::getMidiNoteName(*it, true, true, 4));
			newPool->setAttribute("Label", MidiMessage::getMidiNoteName(*it, true, true, 4));
			newPool->setAttribute("Pool", "1");
			newPool->setAttribute("Channel", "10");
			newPool->setAttribute("NoteNumber",*it);
		}
	}
	// refesh the pool table!
	renumberPool(poolListData);
}

///////////////////////////////////////////////////////////////////////

void TopiaryBeatsModel::setGMDrumMapLabels() 
{
	for (int i = 0; i < poolListData->getNumChildElements(); i++) {
		auto child = poolListData->getChildElement(i);
		int note = topiaryStoi(child->getStringAttribute("NoteNumber"));
		child->setAttribute("Label", GMDrumMap(note));
	}
} // setGMDrumMapLabels

///////////////////////////////////////////////////////////////////////
// LOGGER
///////////////////////////////////////////////////////////////////////

String* TopiaryBeatsModel::getBeatsLog()
{
	logDirty = false;
	return &logString;
}

///////////////////////////////////////////////////////////////////////

void TopiaryBeatsModel::beatsLog(String s)
{
	Time t = Time::getCurrentTime();
	char pre[20];
	sprintf(pre,"%02d:%02d:%02d:%03d : ", t.getHours(), t.getMinutes(), t.getSeconds(), t.getMilliseconds());
	logString.append(String(pre) +  s + newLine, 250);
	logDirty = true;
	broadcaster.sendActionMessage("log");
}

bool TopiaryBeatsModel::isLogDirty()
{
	return logDirty;
}

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
			beatsLog(String("Host transport overriden."));
			recalcRealTime();
		}
		else
		{
			beatsLog(String("Host transport in control."));
			recalcRealTime();
		}
		
	}
}

///////////////////////////////////////////////////////////////////////

void TopiaryBeatsModel::getTransportState (int& b, int& n, int& d, int& bs, bool& override, bool &waitFFN, bool clearDirty)
 // all transport variables, inc signature, BPM etc
{
	b = BPM;
	n = numerator;
	d = denominator;
	bs = runState;
	waitFFN = WFFN;
	override = overrideHostTransport;
	if (clearDirty) transportDirty = false;
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
		beatsLog(String("Signature set to ") + String(numerator) + String("/") + String(denominator));
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
		beatsLog(String("BPM set to ")+String(n));
		//transportDirty = true;
		broadcaster.sendActionMessage("transport");
	}
} // setBPM

///////////////////////////////////////////////////////////////////////

void TopiaryBeatsModel::setRunState(int n)
{
//	const GenericScopedLock<SpinLock> myScopedLock(lockModel);
	if (runState != n)
	{
		runState = n;
		switch (n)
		{
		case  Topiary::Running :
			beatsLog("Start running.");
			Logger::outputDebugString("RTcur to 0");
			// initialize the patterncursor
			patternCursorOn = 0;
			patternCursorOff = 0;
			lastTickOff = 0;
			lastTickOn = 0;
			rtCursor = 0;
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
			lastTickOff = 0;
			lastTickOn = 0;
			rtCursor = 0;
			blockCursor = 0;
			cursorToStop = (int64)-1;
			// and do in all patterns
			for (int i = 0; i < 8; ++i)
			{
				variation[i].currentPatternChild = nullptr;
				variation[i].currentPatternChildOff = nullptr;
			}
			beatsLog("Stopped.");
			broadcaster.sendActionMessage("transport");
			break;
		case Topiary::Ending :
			beatsLog("Ending, cleaning up last notes.");
			broadcaster.sendActionMessage("transport");
			break;
		case Topiary::Ended :
			// Note we use Ended, not Stopped - this only for the case of an ending pattern (not implemented yet (8/11/2018).  Because the transport 
			// may still be running, and if state is stopped and transport starts running it 
			// would restart or re-arm!!!
			// DELETE THIS STATE
			beatsLog("Ended, done with last notes.");
			// reset stuff
			patternCursorOn = 0;
			patternCursorOff = 0;
			lastTickOff = 0;
			lastTickOn = 0;
			rtCursor = 0;
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
			beatsLog("Armed, waiting for first note.");
			broadcaster.sendActionMessage("transport");
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
		beatsLog(String("Variation ") + String(n) + String(" selected."));
		variationsDirty = true;
		broadcaster.sendActionMessage("variations");
	}
	
} // setVariation

///////////////////////////////////////////////////////////////////////

bool TopiaryBeatsModel::isVariationsDirty()
{
	return variationsDirty;
} // variationsDirty

///////////////////////////////////////////////////////////////////////

bool TopiaryBeatsModel::isMasterTablesDirty()
{
	return masterTablesDirty;
} // tablesDirty

void TopiaryBeatsModel::clearMasterTablesDirty()
{
	masterTablesDirty = false;
} // tablesDirty

///////////////////////////////////////////////////////////////////////
// GENERATOR STUFF
///////////////////////////////////////////////////////////////////////

void TopiaryBeatsModel::setSampleRate(double sr)
{
	if (sr != sampleRate)  // not really needed because also checked in processor !!!
	{
		sampleRate = sr;
		recalcRealTime(); // housekeeping
		beatsLog(String("Samplerate set to ")+String(sampleRate)+String("."));
	}

} // setSampleRate

///////////////////////////////////////////////////////////////////////

void TopiaryBeatsModel::setStartTimes()
{ // time in samples when running really starts + other housekeeping
	rtCursor = 0;
	blockCursor = 0;
	variationRunning = variationSelected;
	broadcaster.sendActionMessage("variations");
	
} // setStartTimes

///////////////////////////////////////////////////////////////////////

void TopiaryBeatsModel::setBlockSize(int blocksz)
{
	blockSize = blocksz;
	beatsLog(String("Blocksize ") + String(blockSize));
}

///////////////////////////////////////////////////////////////////////

void TopiaryBeatsModel::generateMidi(MidiBuffer* midiBuffer)
{ // main Generator

  const GenericScopedLock<SpinLock> myScopedLock(lockModel);

  
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
  }


  jassert(beat >= 0);
  jassert(measure >= 0);

  //Logger::outputDebugString(String("GENERATE -----------------------------------------------"));
  //Logger::outputDebugString(String("blkcursor ") + String(blockCursor));
  //Logger::outputDebugString(String("rtFcursor ") + String(rtCursorFrom));
  //Logger::outputDebugString(String("rtTcursor ") + String(rtCursorTo));
  //Logger::outputDebugString(String("nxtcursor ") + String(lastTickOn));
  //Logger::outputDebugString(String("nxtcursor ") + String(lastTickOff));

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
  int parentLengthInMeasures; 

  parentOn = variation[variationRunning - 1].patternOn;
  parentOff = variation[variationRunning - 1].patternOff;
  noteChildOn = variation[variationRunning - 1].currentPatternChild;
  noteChildOff = variation[variationRunning - 1].currentPatternChildOff;
  parentLength = variation[variationRunning - 1].lenInTicks;
  parentLengthInMeasures = variation[variationRunning].lenInMeasures;

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
  bool walkOn = walkToTick(parentOn, &noteChildOn, patternCursorOn);
  bool walkOff = walkToTick(parentOff, &noteChildOff, patternCursorOff);
  
  Logger::outputDebugString(String("WalkOn ") + String((int)walkOn));
  Logger::outputDebugString(String("WalkOff ") + String((int)walkOff));

  if (walkOn || walkOff)
  {
	  //Logger::outputDebugString(String(">>>>>>>>>>>>>>>>>>> From to in ticks ") + String(rtCursorFrom / samplesPerTick) + String(" / ") + String((int)rtCursorTo / samplesPerTick));
	  //Logger::outputDebugString(String(">>>>>>>>>>>>>>>>>>> Next RTGenCursor ") + String(nextRTGenerationCursor / samplesPerTick));

	  // set patternCursors where we are now, so the offsets in sample time are correct
	  patternCursorOn = (int) patternCursorOn % parentLength;  //////// because rtCursors are multi loops !!!!
	  patternCursorOff = (int) patternCursorOff % parentLength;
	   
	  calcMeasureBeat();

	  while (rtCursor < rtCursorTo)
	  {
		  Logger::outputDebugString(String("Next WHILE LOOP ITERATION"));
		  Logger::outputDebugString(String(">>>>>>>>>>>>>>>>>>> From to in ticks ") + String(rtCursorFrom / samplesPerTick) + String(" ? ") + String((int)rtCursorTo / samplesPerTick));
		  Logger::outputDebugString(String(">>>>>>>>>>>>>>>>>>> Next RTGenCursor ") + String(nextRTGenerationCursor / samplesPerTick));
		  //patternCursors run in Tick time
		  //rtCursors run in sample time!!!    
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
				  length = noteChildOn->getIntAttribute("Length");
				  noteNumber = noteChildOn->getIntAttribute("Note");
				  channel = noteChildOn->getIntAttribute("Channel"); ///////// channel is not here - it's in the pool data !!!!
				  msg = MidiMessage::noteOn(10, noteNumber, (float)noteChildOn->getIntAttribute("Velocity") / 128);

				  //Logger::outputDebugString(String("Note event on pattern tick ") + String(patternCursorOn+ticksTaken));
				  patternCursorOn = nextPatternCursorOn;
				  if (patternCursorOn >= parentLength) patternCursorOn = patternCursorOn - parentLength;
				  nextTick(parentOn, &noteChildOn);
				  //Logger::outputDebugString(String("NOTE ON --------"));
				  rtCursor = rtCursor + (int64)(ticksTaken * samplesPerTick);
				  lastTickOn = patternCursorOn;
				  if (lastTickOn >= parentLength) lastTickOn = lastTickOn - parentLength;
				  noteOnSet.add(noteNumber);
			  }
			  else
			  {
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
				  lastTickOff = patternCursorOff;  
				  if (lastTickOff >= parentLength) lastTickOff = lastTickOff - parentLength;
				  noteOnSet.removeValue(noteNumber);
			  }

			  jassert((rtCursor - rtCursorFrom) >= 0);

			  midiBuffer->addEvent(msg, (int) (rtCursor-rtCursorFrom));

			  //
			  //Logger::outputDebugString(String("NoteNumber ") + String(noteNumber));
			  //Logger::outputDebugString(String("Note event on tick ") + String((int)(rtCursor / samplesPerTick)));
			  //Logger::outputDebugString(String("ParentLen ") + String(parentLength));
			  //Logger::outputDebugString(String("TicksTaken    ") + String(ticksTaken));
			  //Logger::outputDebugString(String("TicksTakenOn  ") + String(ticksTakenOn));
			  //Logger::outputDebugString(String("TicksTakenOff ") + String(ticksTakenOff));
			  //Logger::outputDebugString(String("LastTickOff ") + String(lastTickOff));
			  //Logger::outputDebugString(String("LastTickOn ") + String(lastTickOn));
			  //Logger::outputDebugString(String("PatternCursorOff ") + String(patternCursorOff));
			  //Logger::outputDebugString(String("PatternCursorOn ") + String(patternCursorOn));
			  //Logger::outputDebugString(String("rtCursor  ") + String(rtCursor));
			  //Logger::outputDebugString(String("rtFcursor ") + String(rtCursorFrom));
			  //Logger::outputDebugString(String("rtToCursor ") + String(rtCursorTo));
			  //Logger::outputDebugString(String("SampleTime  ------------------------------->") + String(rtCursor-rtCursorFrom));
			  			  
			  nextRTGenerationCursor = rtCursor;
			  
			  if (ticksTakenOn < ticksTakenOff)
			  {
				  patternCursorOff = patternCursorOn;
			  }
			  else
				  patternCursorOn = patternCursorOff;
				
			  //Logger::outputDebugString(String("nxtcursor ") + String(nextRTGenerationCursor));

		  }
		  else
		  {
			  // done for now, but let's place ourselves ready for the next round

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
			  else
			  {
				  nextPatternCursorOn = 999999999;
				  ticksTakenOn = 999999999;
			  }

			  if (walkOff)
			  {
				  nextPatternCursorOff = noteChildOff->getIntAttribute("Timestamp");
				  ticksTakenOff = nextPatternCursorOff - patternCursorOff;
				  if (ticksTakenOff < 0) ticksTakenOff += parentLength;
			  }
			  else
			  {
				  nextPatternCursorOff = 999999999;
				  ticksTakenOff = 999999999;
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
			  //Logger::outputDebugString(String(" ++++++++++++++ done +++++++++++++++++++++"));
			  //Logger::outputDebugString(String("TicksTakenOn ") + String(ticksTakenOn));
			  //Logger::outputDebugString(String("TicksTakenOf ") + String(ticksTakenOff));

			  if (ticksTakenOff <= ticksTakenOn)
				  patternCursorOn = patternCursorOff;
			  else
				  patternCursorOff = patternCursorOn;

			  nextRTGenerationCursor = rtCursor + (int64)(ticksTaken * samplesPerTick);
			  rtCursor = nextRTGenerationCursor;
			  // we use RTGeneration cursor to rapidly drop out of next call if it does not fall within the generation window; apart from that we do not need it
			  

			  //Logger::outputDebugString(String("NextPatcursorOn ") + String(patternCursorOn));
			  //Logger::outputDebugString(String("NextPatcursorOf ") + String(patternCursorOff));
			  //Logger::outputDebugString(String("rtCursor  ") + String(rtCursor));
			  //Logger::outputDebugString(String("rtFcursor ") + String(rtCursorFrom));
			  //Logger::outputDebugString(String("rtTcursor ") + String(rtCursorTo));
			  //Logger::outputDebugString(String("nxtcursor ") + String(lastTickOn));
			  //Logger::outputDebugString(String("nxtcursor ") + String(lastTickOff));
			  //Logger::outputDebugString(String("measure: ") + String(measure) + String(" beat: ") + String(beat) + String(" tick ") + String(tick));
		  
		  }
	  }
  }
  else
  {
	  // pattern is empty or no fitting note events found
	  if ((noteChildOn == nullptr) || (noteChildOff == nullptr))
	  {
		  // make sure our cursors keep running
		  nextRTGenerationCursor = rtCursorTo + 1;
		  patternCursorOn = +(int)(blockSize / samplesPerTick);
		  patternCursorOff = +(int)(blockSize / samplesPerTick);
	  }
	 
  }

  blockCursor = blockCursor + blockSize;
  /*
  int newTick = patternCursorOn % Topiary::TICKS_PER_QUARTER;
  if (newTick > tick) tick = newTick; // to aboid jumpiness
  
  int newBeat = (int)patternCursorOn / (numerator * (Topiary::TICKS_PER_QUARTER));
  if (newBeat > beat) beat = newBeat;
  
  measure = (int)patternCursorOn / (numerator * (Topiary::TICKS_PER_QUARTER) * denominator);
  if (measure > variation[variationRunning].lenInMeasures) measure = 1;
  if (measure > 2)
	  measure = 100;
	  */
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
		beatsLog(String("---------------------------- >Switch from variation ") + String(variationRunning) + String(" to ") + String(variationSelected));
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

void TopiaryBeatsModel::setRunStartQ(int q)
{
	runStartQ = q;
} // setRunStartQ

///////////////////////////////////////////////////////////////////////

int TopiaryBeatsModel::getRunStartQ()
{
	return runStartQ;
} // getRunStartQ

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
// Broacaster & listeners
///////////////////////////////////////////////////////////////////////

void TopiaryBeatsModel::setListener(ActionListener *listener)
{
	broadcaster.addActionListener(listener);
} // setListener


void TopiaryBeatsModel::removeListener(ActionListener *listener)
{
	broadcaster.removeActionListener(listener);
} // setListener
