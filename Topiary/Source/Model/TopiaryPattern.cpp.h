/////////////////////////////////////////////////////////////////////////////
/*
This file is part of Topiary, Copyright Tom Tollenaere 2018-20.

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
#include "TopiaryPattern.h"

#ifdef TOPIARYMODEL
//#include MODELHEADERPATH

TopiaryPattern::TopiaryPattern()
{
	// initialize the headerlist data
	headerListItems = 9;
	// headerlist IDs start at 1;
	// datalist IDs start at 1;

	headerList[0].columnID = 1;
	headerList[0].name = "ID";
	headerList[0].width = 20;
	headerList[0].type = Topiary::HeaderType::Int;
	headerList[0].editable = false;
	headerList[0].visible = true;

	headerList[1].columnID = 2;
	headerList[1].name = "Measure";
	headerList[1].width = 70;
	headerList[1].type = Topiary::HeaderType::Int;
	headerList[1].editable = true;
	headerList[1].min = 0;
	headerList[1].max = 32;
	headerList[1].visible = true;

	headerList[2].columnID = 3;
	headerList[2].name = "Beat";
	headerList[2].width = 40;
	headerList[2].type = Topiary::HeaderType::Int;
	headerList[2].editable = true;
	headerList[2].min = 0;
	headerList[2].max = 3;   // careful - needs to be re-set if meter changes!!!
	headerList[2].visible = true;

	headerList[3].columnID = 4;
	headerList[3].name = "Tick";
	headerList[3].width = 40;
	headerList[3].type = Topiary::HeaderType::Int;
	headerList[3].editable = true;
	headerList[3].min = 0;
	headerList[3].max = Topiary::TicksPerQuarter - 1;
	headerList[3].visible = true;

	headerList[4].columnID = 5;
	headerList[4].name = "Note";
	headerList[4].width = 40;
	headerList[4].type = Topiary::HeaderType::Int;
	headerList[4].editable = false;
	headerList[4].min = 0;
	headerList[4].max = 127;
	headerList[4].visible = true;

	headerList[5].columnID = 6;
	headerList[5].name = "Label";
	headerList[5].width = 40;
	headerList[5].type = Topiary::HeaderType::NoteLabel;
	headerList[5].editable = true;
	headerList[5].visible = true;

	headerList[6].columnID = 7;
	headerList[6].name = "Length";
	headerList[6].width = 50;
	headerList[6].type = Topiary::HeaderType::Int;
	headerList[6].editable = true;
	headerList[6].min = 1;
	headerList[6].max = 10000;
	headerList[6].visible = true;

	headerList[7].columnID = 8;
	headerList[7].name = "Val-Vel";
	headerList[7].width = 50;
	headerList[7].type = Topiary::HeaderType::Int;
	headerList[7].editable = true;
	headerList[7].min = 0;
	headerList[7].max = 16383;
	headerList[7].visible = true;

	// timestamp not defined here because not in the table!!!

	headerList[8].columnID = 8;
	headerList[8].name = "MidiType";
	headerList[8].width = 50;
	headerList[8].type = Topiary::HeaderType::Int;
	headerList[8].editable = true;
	headerList[8].min = 0;
	headerList[8].max = 127;
	headerList[8].visible = false;
	numItems = 0; // empty list

} // TopiaryPattern

/////////////////////////////////////////////////////////////////////////////

TopiaryPattern::~TopiaryPattern()
{
}

/////////////////////////////////////////////////////////////////////////////

void TopiaryPattern::setIntByIndex(int row, int i, int newInt)
{

	jassert(row < numItems);
	jassert(i < headerListItems);
	switch(i)
	{
		case 0:
			dataList[row].ID = newInt;
			break;
		case 1:
			dataList[row].measure = newInt;
			break;
		case 2:
			dataList[row].beat = newInt;
			break;
		case 3:
			dataList[row].tick = newInt;
			break;
		case 4:
			dataList[row].note = newInt;
			break;
		case 6:
			dataList[row].length = newInt;
			break;
		case 7:
			dataList[row].velocity = newInt;
			break;
		default:
			jassert(false); // wrong type
	}	

} // setIntByID

/////////////////////////////////////////////////////////////////////////////

void TopiaryPattern::setStringByIndex(int row, int i, String newString)
{

	jassert(row < numItems);
	jassert(i < headerListItems);
	if (i == 5)
	{
		// label can only be edited if midiType = NoteOn
		if (dataList[row].midiType == Topiary::MidiType::NoteOn)
			dataList[row].label = newString;
	}
	else
	{
		jassert(false); // wrong type
	}
}

/////////////////////////////////////////////////////////////////////////////

void TopiaryPattern::del(int n)
{
	jassert(n >= 0);
	jassert(n < numItems);
	for (int i = n; i < (numItems - 1); i++)
	{
		swap(i, i + 1);
	}

	numItems--;
	for (int i = 0; i < numItems; i++)
		dataList[i].ID = i + 1;

} // del

/////////////////////////////////////////////////////////////////////////////

void TopiaryPattern::addNote(int measure, int beat, int tick, int timestamp, int note, int length, int velocity)
{
	jassert(numItems < (maxItems + 1));

	// adds new one
	dataList[numItems].note = note;
	dataList[numItems].label = noteNumberToString(note);
	dataList[numItems].ID = numItems + 1;
	dataList[numItems].measure = measure;
	dataList[numItems].beat = beat;
	dataList[numItems].tick = tick;
	dataList[numItems].timestamp = timestamp;
	dataList[numItems].velocity = velocity;
	dataList[numItems].length = length;
	dataList[numItems].midiType = Topiary::NoteOn;
	dataList[numItems].value = 0;
	numItems++;

} // addNote

void TopiaryPattern::addAT(int measure, int beat, int tick, int timestamp, int value)
{
	jassert(numItems < (maxItems + 1));

	// adds new one
	dataList[numItems].value = value;
	dataList[numItems].note = 0;
	dataList[numItems].label = "AT";
	dataList[numItems].ID = numItems + 1;
	dataList[numItems].measure = measure;
	dataList[numItems].beat = beat;
	dataList[numItems].tick = tick;
	dataList[numItems].timestamp = timestamp;
	dataList[numItems].velocity = 0;
	dataList[numItems].length = 0;
	dataList[numItems].midiType = Topiary::MidiType::AfterTouch;
	dataList[numItems].note = -1;
	numItems++;

} // addAT


void TopiaryPattern::addCC(int measure, int beat, int tick, int timestamp, int CC, int value)
{
	jassert(numItems < (maxItems + 1));
	 
	// adds new one
	dataList[numItems].note = CC; // note AND length have CC number!
	dataList[numItems].label = "CC";
	dataList[numItems].value = value;
	dataList[numItems].ID = numItems + 1;
	dataList[numItems].measure = measure;
	dataList[numItems].beat = beat;
	dataList[numItems].tick = tick;
	dataList[numItems].timestamp = timestamp;
	dataList[numItems].velocity = 0;
	dataList[numItems].length = CC;
	dataList[numItems].midiType = Topiary::MidiType::CC;
	dataList[numItems].note = CC;
	numItems++;

} // addCC


void TopiaryPattern::addPitch(int measure, int beat, int tick, int timestamp, int value)
{
	jassert(numItems < (maxItems + 1));

	// adds new one
	dataList[numItems].value = value;
	dataList[numItems].label = "Pitch";
	dataList[numItems].ID = numItems + 1;
	dataList[numItems].measure = measure;
	dataList[numItems].beat = beat;
	dataList[numItems].tick = tick;
	dataList[numItems].timestamp = timestamp;
	dataList[numItems].velocity = 0;
	dataList[numItems].length = 0;
	dataList[numItems].midiType = Topiary::MidiType::Pitch;
	dataList[numItems].note = -1;
	numItems++;

} // addPitch

/////////////////////////////////////////////////////////////////////////////

void TopiaryPattern::add()
{
	jassert(numItems < (maxItems + 1));

	// adds new one
	dataList[numItems].ID = numItems + 1;
	numItems++;

} // add


/////////////////////////////////////////////////////////////////////////////

void TopiaryPattern::sortByID()
{
	// sorts and then renumbers by ID (one might have been deleted)
	// IDs to delete are set to Topiary::ToDeleteID

	//bool done;

	for (int i = 0; i <= numItems; i++)
	{
		//done = true;
		for (int j = i + 1; j < numItems; j++)
		{
			if (dataList[i].ID > dataList[j].ID)
			{
				swap(i, j);
				//done = false;
			}
		}
		//if (done)
		//	i = numItems;
	};

	renumber();

} // sortByID

/////////////////////////////////////////////////////////////////////////////

void TopiaryPattern::sortByTimestamp(bool keepIDs)
{
	// sorts and then renumbers by ID (one might have been deleted)
	// IDs to delete are set to Topiary::ToDeleteID

	//bool done;

	for (int i = 0; i <= numItems; i++)
	{
		//done = true;
		for (int j = i + 1; j < numItems; j++)
		{
			if (dataList[i].timestamp > dataList[j].timestamp)
			{
				swap(i, j);
				//done = false;
			}
		}
		//if (done)
		//	i = numItems;
	};

	if (!keepIDs)
		renumber();

} // sortByID

/////////////////////////////////////////////////////////////////////////////


void TopiaryPattern::renumber()
{
	for (int i = 0; i < numItems; i++)
		dataList[i].ID = i + 1;
} // renumber

/////////////////////////////////////////////////////////////////////////////

void TopiaryPattern::duplicate(TopiaryPattern* p)
{
	// makes this pattern a duplicate of *p
	numItems = p->numItems;
	patLenInTicks = p->patLenInTicks;
	for (int i = 0; i < numItems; i++)
	{
		dataList[i].ID = p->dataList[i].ID;
		dataList[i].measure = p->dataList[i].measure;
		dataList[i].beat = p->dataList[i].beat;
		dataList[i].tick = p->dataList[i].tick;
		dataList[i].note = p->dataList[i].note;
		dataList[i].label = p->dataList[i].label;
		dataList[i].length= p->dataList[i].length;
		dataList[i].velocity = p->dataList[i].velocity;
		dataList[i].timestamp = p->dataList[i].timestamp;
		dataList[i].midiType = p->dataList[i].midiType;
		dataList[i].channel = p->dataList[i].channel;
		dataList[i].value = p->dataList[i].value;
		//dataList[i].CC = p->dataList[i].CC;
	}
} // duplicate

/////////////////////////////////////////////////////////////////////////////

void TopiaryPattern::fillDataList(XmlElement* dList)
{
	dList->deleteAllChildElements();
	for (int i = 0; i < numItems; i++)
	{
		XmlElement* child = new XmlElement("Note");

		child->setAttribute("ID", dataList[i].ID);
		child->setAttribute("Measure", dataList[i].measure);
		child->setAttribute("Beat", dataList[i].beat);
		child->setAttribute("Tick", dataList[i].tick);
		child->setAttribute("Timestamp", dataList[i].timestamp);
		if ((dataList[i].midiType < 1) || (dataList[i].midiType > 5))
			dataList[i].midiType = Topiary::MidiType::NoteOn;

		switch (dataList[i].midiType)
		{
			case Topiary::MidiType::NoteOn:
				child->setAttribute("Note", dataList[i].note);
				child->setAttribute("Length", dataList[i].length);
				child->setAttribute("Label", dataList[i].label);
				child->setAttribute("Val-Vel", dataList[i].velocity);
				break;
			case Topiary::MidiType::CC:
				child->setAttribute("Note", dataList[i].length);		// note and length have the CC number!!!
				child->setAttribute("Length", dataList[i].length);
				child->setAttribute("Label", "CC");
				child->setAttribute("Val-Vel", dataList[i].value);
				break;			
			case Topiary::MidiType::AfterTouch:
				child->setAttribute("Note", 0);
				child->setAttribute("Length", 0);
				child->setAttribute("Label", "AT");
				child->setAttribute("Val-Vel", dataList[i].value);
				break;				
			case Topiary::MidiType::Pitch:
				child->setAttribute("Note", 0);
				child->setAttribute("Length", 0);
				child->setAttribute("Label", "Pitch");
				child->setAttribute("Val-Vel", dataList[i].value);
				break;
		} // switch midiType
		
		dList->addChildElement(child);
	}
}  // fillDataList

/////////////////////////////////////////////////////////////////////////////

void TopiaryPattern::addToModel(XmlElement *m)
{
	// adds the pattern data as XmlElement; *m should be <PatternData>
	jassert(m->getTagName().equalsIgnoreCase("PatternData"));

	// wrap the whole thing in a <Pattern>
	auto* child = new XmlElement("Pattern");
	m->addChildElement(child);
	m = child;
	child = new XmlElement("PatLenInTicks");
	child->setAttribute("patLenInTicks", patLenInTicks);
	m->addChildElement(child);

	for (int p = 0; p < numItems; p++)
	{
			child = new XmlElement("Data");

			m->addChildElement(child);
			child->setAttribute("ID", dataList[p].ID);
			child->setAttribute("measure", dataList[p].measure);
			child->setAttribute("beat", dataList[p].beat);
			child->setAttribute("tick", dataList[p].tick);
			child->setAttribute("midiType",dataList[p].midiType);
			child->setAttribute("note", dataList[p].note);
			child->setAttribute("length", dataList[p].length);
			child->setAttribute("velocity", dataList[p].velocity);
			child->setAttribute("value", dataList[p].value);
			child->setAttribute("timestamp", dataList[p].timestamp);
			child->setAttribute("label", dataList[p].label);
			
	}

} // addToModel

/////////////////////////////////////////////////////////////////////////////

void TopiaryPattern::getFromModel(XmlElement *m)
{
	jassert(m->getTagName().equalsIgnoreCase("Pattern"));

	XmlElement* child = m->getFirstChildElement();
	jassert(child->getTagName() == "PatLenInTicks");

	patLenInTicks = child->getIntAttribute("patLenInTicks");
	child = child->getNextElement();
	int index = 0;
	while (child != nullptr)
	{
		dataList[index].ID = child->getIntAttribute("ID");
		dataList[index].note = child->getIntAttribute("note");
		dataList[index].measure = child->getIntAttribute("measure");
		dataList[index].beat = child->getIntAttribute("beat");
		dataList[index].tick = child->getIntAttribute("tick");
		dataList[index].length = child->getIntAttribute("length");
		dataList[index].velocity = child->getIntAttribute("velocity");
		dataList[index].timestamp = child->getIntAttribute("timestamp");
		dataList[index].midiType = child->getIntAttribute("midiType");
		
		if (dataList[index].midiType == Topiary::NoteOn)
			dataList[index].label = noteNumberToString(child->getIntAttribute("note"));
		else
			dataList[index].label = child->getStringAttribute("label");

		dataList[index].value = child->getIntAttribute("value");
		
		index++;
		child = child->getNextElement();
	}

	numItems = index;

} // getFromModel

/////////////////////////////////////////////////////////////////////////////

void TopiaryPattern::validateTableEdit(int p, XmlElement* child, String attribute)
{
	// called by TopiaryTable
	// careful - can be called when editing patterns but also when editing note pool entries!!
	// do processing of user edits to notes and make sure all is consistent

	model->validateTableEdit(p, child, attribute);
	
} // validateTableEdit

/////////////////////////////////////////////////////////////////////////////

void TopiaryPattern::setModel(TOPIARYMODEL* m)
{
	model = m;
} // setModel

/////////////////////////////////////////////////////////////////////////////

int TopiaryPattern::findID(int ID)
{
	// returns index of the ID; creates new record with that ID if not found
	int i = 0;
	while ((dataList[i].ID != ID) && (i < numItems))
		i++;

	if (i == numItems)
	{
		add();
		return (numItems-1);
	}
	else
		return i;

} // findID

/////////////////////////////////////////////////////////////////////////////

void TopiaryPattern::quantize(int ticks)
{
	// ticks indicates what to quantize to, e.g. beat/quarter == Topiary::TICKSPERQUARTER

	for (int i = 0; i < numItems; i++)
	{
		int deltaTick = dataList[i].timestamp % ticks;  //deltaTick is #ticks away from baseTick
		int baseTick = (int)floor(dataList[i].timestamp / ticks) * ticks;
		if ((deltaTick / ticks) > 0.5)
			deltaTick = ticks;
		else
			deltaTick = 0;
		dataList[i].timestamp = baseTick + deltaTick;
	}

} // quantize


#endif
