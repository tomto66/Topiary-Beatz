/////////////////////////////////////////////////////////////////////////////
/*
This file is part of Topiary, Copyright Tom Tollenaere 2019-21.

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

#ifdef TOPIARYMODEL
#include "NoteAssignmentList.h"

#ifdef RIFFZ
#include "TopiaryRiffzModel.h"
#endif
#ifdef PADZ
#include "TopiaryPadzModel.h"
#endif
 
NoteAssignmentList::NoteAssignmentList()
{
	// initialize the headerlist data
	headerListItems = 6;
	// headerlist IDs start at 1;
	// datalist IDs start at 1;
	
	headerList[0].columnID = 1;
	headerList[0].name = "ID";
	headerList[0].width = 20;
	headerList[0].type = Topiary::HeaderType::Int;
	headerList[0].editable = false;
	headerList[1].visible = true;

	headerList[1].columnID = 2;
	headerList[1].name = "NoteNumber";
	headerList[1].width = 50;
	headerList[1].type = Topiary::HeaderType::Int;
	headerList[1].editable = false;
	headerList[1].visible = false;

	headerList[2].columnID = 3;
	headerList[2].name = "Note";
	headerList[2].width = 50;
	headerList[2].type = Topiary::HeaderType::NoteLabel;
	headerList[2].editable = false;
	headerList[2].visible = true;

	headerList[3].columnID = 4;
	headerList[3].name = "Pattern";
	headerList[3].width = 110;
	headerList[3].type = Topiary::HeaderType::String;
	headerList[3].editable = false;
	headerList[3].visible = true;

	headerList[4].columnID = 5;
	headerList[4].name = "PatternId";
	headerList[4].width = 40;
	headerList[4].type = Topiary::HeaderType::Int;
	headerList[4].editable = false;
	headerList[4].min = 1;
	headerList[4].max = 4;
	headerList[4].visible = false;

	headerList[5].columnID = 6;
	headerList[5].name = "Offset";
	headerList[5].width = 40;
	headerList[5].type = Topiary::HeaderType::Int;
	headerList[5].editable = false;
	headerList[5].min = -128;
	headerList[5].max = 128;
	headerList[5].visible = true;

	numItems = 0; // empty list

} // NoteAssignmentList

/////////////////////////////////////////////////////////////////////////////

NoteAssignmentList::~NoteAssignmentList()
{
}

/////////////////////////////////////////////////////////////////////////////

void NoteAssignmentList::setIntByIndex(int row, int i, int newInt)
{
	jassert(false); // should never happen, table not editable
	UNUSED(row);
	UNUSED(i);
	UNUSED(newInt);

} // setIntByID

/////////////////////////////////////////////////////////////////////////////

void NoteAssignmentList::setStringByIndex(int row, int i, String newString)
{
	jassert(false); // should never happen; table not editable
	UNUSED(row);
	UNUSED(i);
	UNUSED(newString);
}

/////////////////////////////////////////////////////////////////////////////

void NoteAssignmentList::del(int n)
{
	jassert(n >= 0);
	jassert(n < numItems);
	for (int i = n; i < (numItems - 1); i++)
	{
		swap(i, i + 1);
	}

	numItems--;
	// set the IDs correctly
	for (int i = 0; i < numItems; i++)
		dataList[i].ID = i + 1;

} // del

/////////////////////////////////////////////////////////////////////////////

void NoteAssignmentList::add()
{
	jassert(numItems < (maxItems + 1));

	// adds new empty one
	dataList[numItems].note = 1;
	dataList[numItems].noteLabel = noteNumberToString(1);
	dataList[numItems].offset = 0;
	dataList[numItems].ID = numItems + 1;
	dataList[numItems].patternId = -1;
	dataList[numItems].patternName = "UNASSIGNED";
	numItems++;

} // add

/////////////////////////////////////////////////////////////////////////////

void NoteAssignmentList::sortByID()
{
	// sorts and then renumbers by ID (one might have been deleted)
	// IDs to delete are set to Topiary::ToDeleteID

	for (int i = 0; i <= numItems; i++)
	{
		for (int j = i + 1; j < numItems; j++)
		{
			if (dataList[i].ID > dataList[j].ID)
			{
				swap(i, j);
			}
		}
	};

	renumber();

} // sortByID

/////////////////////////////////////////////////////////////////////////////

void NoteAssignmentList::sortByNote()
{
	// sorts and then renumbers by ID (one might have been deleted)
	// IDs to delete are set to Topiary::ToDeleteID

	for (int i = 0; i <= numItems; i++)
	{
		for (int j = i + 1; j < numItems; j++)
		{
			if (dataList[i].note > dataList[j].note)
			{
				swap(i, j);
			}
		}
	};

	renumber();

} // sortByID

/////////////////////////////////////////////////////////////////////////////

void NoteAssignmentList::renumber()
{
	for (int i = 0; i < numItems; i++)
		dataList[i].ID = i + 1;
} // renumber


/////////////////////////////////////////////////////////////////////////////

void NoteAssignmentList::fillDataList(XmlElement* dList)
{
	dList->deleteAllChildElements();
	for (int i = 0; i < numItems; i++)
	{
		XmlElement *child = new XmlElement("NoteAssignment");
		child->setAttribute("Note", dataList[i].noteLabel); 
		child->setAttribute("ID", dataList[i].ID);
		child->setAttribute("Offset", dataList[i].offset);
		child->setAttribute("NoteNumber", dataList[i].note);
		child->setAttribute("Pattern", dataList[i].patternName);
		child->setAttribute("PatternId", dataList[i].patternId);
		dList->addChildElement(child);
	}
}  // fillDataList

/////////////////////////////////////////////////////////////////////////////

void NoteAssignmentList::addToModel(XmlElement* m)
{
	// adds the noteAssignment data as XmlElement; *model should be <noteAssignments>
	// for variation v 

	jassert(m->getTagName().equalsIgnoreCase("noteAssignments"));

	for (int p = 0; p < numItems; p++)
	{
		XmlElement* child = new XmlElement("noteAssignment");
		m->addChildElement(child);
		child->setAttribute("ID", dataList[p].ID);
		child->setAttribute("note", dataList[p].note);
		child->setAttribute("patternId", dataList[p].patternId);
		child->setAttribute("offset", dataList[p].offset);

	}

} // addToModel

/////////////////////////////////////////////////////////////////////////////

void NoteAssignmentList::getFromModel(XmlElement *m)
{
	// adds the pattern data as XmlElement; *model should be <PatternList>
	jassert(m->getTagName().equalsIgnoreCase("noteAssignments"));

	XmlElement* child = m->getFirstChildElement();
	int index = 0;
	while (child != nullptr)
	{
		dataList[index].ID = child->getIntAttribute("ID");
		dataList[index].note = child->getIntAttribute("note");
		dataList[index].offset = child->getIntAttribute("offset");
		dataList[index].patternId = child->getIntAttribute("patternId");

		child = child->getNextElement();
		index++;
	}

	numItems = index;

} // getFromModel

 
/////////////////////////////////////////////////////////////////////////////

void NoteAssignmentList::validateTableEdit(int p, XmlElement* child, String attribute)
{
	UNUSED(p);
	UNUSED(child);
	UNUSED(attribute);

	jassert(false);  // should not happen, table not editable
} // validateTableEdit

/////////////////////////////////////////////////////////////////////////////

void NoteAssignmentList::setRiffzModel(TopiaryPadzModel* m)
{
	model = m;
}  // setRiffzModel

/////////////////////////////////////////////////////////////////////////////

void NoteAssignmentList::selectedRowsChanged(int lastRowSelected)
{
	UNUSED(lastRowSelected)
	model->broadcaster.sendActionMessage(MsgSelectedNoteAssignmentRowsChanged); // may need to disable the variation!
}

/////////////////////////////////////////////////////////////////////////////

int NoteAssignmentList::findOffset(int note)
{
	// loop over all noteAssignments till you find this note, return -1 if nothing found
	
	int offset = Topiary::NoNoteAssigned; 
	for (int i=0; i<numItems;i++)
		if (dataList[i].note == note)
		{
			offset = dataList[i].offset;
			i = numItems;
		}
	return offset;
} // findOffset

/////////////////////////////////////////////////////////////////////////////

void NoteAssignmentList::redoPatternNames(int patternId, String patternName)
{
	// called when patternID changes name - references here should be renamed
	for (int i = 0; i < numItems; i++)
		if (dataList[i].patternId == patternId)
			dataList[i].patternName = patternName;
} // redoPatternNames

#endif // TOPIARYMODEL