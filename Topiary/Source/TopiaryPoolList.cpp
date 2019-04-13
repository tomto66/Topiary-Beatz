/////////////////////////////////////////////////////////////////////////////
/*
This file is part of Topiary, Copyright Tom Tollenaere 2018-19.

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
#include "TopiaryPoolList.h"
#include "TopiaryBeatsModel.h"

TopiaryPoolList::TopiaryPoolList()
{
	// initialize the headerlist data
	headerListItems = 5;
	// headerlist IDs start at 1;
	// datalist IDs start at 1;

	headerList[0].columnID = 1;
	headerList[0].name = "ID";
	headerList[0].width = 20;
	headerList[0].type = Topiary::HeaderType::Int;
	headerList[0].editable = false;
	headerList[1].visible = true;

	headerList[1].columnID = 2;
	headerList[1].name = "Note";
	headerList[1].width = 50;
	headerList[1].type = Topiary::HeaderType::Int;
	headerList[1].editable = false;
	headerList[1].visible = true;

	headerList[2].columnID = 3;
	headerList[2].name = "Label";
	headerList[2].width = 50;
	headerList[2].type = Topiary::HeaderType::NoteLabel;
	headerList[2].editable = true;
	headerList[2].visible = true;

	headerList[3].columnID = 4;
	headerList[3].name = "Description";
	headerList[3].width = 110;
	headerList[3].type = Topiary::HeaderType::String;
	headerList[3].editable = true;
	headerList[3].visible = true;

	headerList[4].columnID = 5;
	headerList[4].name = "Pool";
	headerList[4].width = 40;
	headerList[4].type = Topiary::HeaderType::Int;
	headerList[4].editable = true;
	headerList[4].min = 1;
	headerList[4].max = 4;
	headerList[4].visible = true;

	

	numItems = 0; // empty list

} // TopiaryPoolList

/////////////////////////////////////////////////////////////////////////////

TopiaryPoolList::~TopiaryPoolList()
{
}

/////////////////////////////////////////////////////////////////////////////

void TopiaryPoolList::setIntByIndex(int row, int i, int newInt)
{

	jassert(row < numItems);
	jassert(i < headerListItems);
	if (i == 4)
		dataList[row].pool = newInt;
	else if (i == 1)
		dataList[row].note = newInt;
	else
	{
		jassert(false); // wrong type
	}

} // setIntByID

/////////////////////////////////////////////////////////////////////////////

void TopiaryPoolList::setStringByIndex(int row, int i, String newString)
{

	jassert(row < numItems);
	jassert(i < headerListItems);
	if (i == 2)
		dataList[row].label = newString;
	else if (i == 3)
		dataList[row].description = newString;
	else
	{
		jassert(false); // wrong type
	}
}

/////////////////////////////////////////////////////////////////////////////

void TopiaryPoolList::del(int n)
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

void TopiaryPoolList::add()
{
	jassert(numItems < (maxItems + 1));

	// adds new empty one
	dataList[numItems].note = 1;
	dataList[numItems].label = MidiMessage::getMidiNoteName(1, true, true, 5);
	dataList[numItems].pool = 1;
	dataList[numItems].ID = numItems + 1;
	numItems++;

} // add

/////////////////////////////////////////////////////////////////////////////

void TopiaryPoolList::sortByID()
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

void TopiaryPoolList::sortByNote()
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

void TopiaryPoolList::renumber()
{
	for (int i = 0; i < numItems; i++)
		dataList[i].ID = i + 1;
} // renumber

/////////////////////////////////////////////////////////////////////////////

int TopiaryPoolList::isNoteInPool(int n)
{
	for (int i = 0; i < numItems; i++)
		if (dataList[i].note == n)
			return i;

	return -1;
}

/////////////////////////////////////////////////////////////////////////////

void TopiaryPoolList::fillDataList(XmlElement* dList)
{
	dList->deleteAllChildElements();
	for (int i = 0; i < numItems; i++)
	{
		XmlElement *child = new XmlElement("Pool");
		child->setAttribute("Note", dataList[i].note);
		child->setAttribute("ID", dataList[i].ID);
		child->setAttribute("Pool", dataList[i].pool);
		child->setAttribute("Label", dataList[i].label);
		child->setAttribute("Description", dataList[i].description);
		dList->addChildElement(child);
	}
}  // fillDataList

/////////////////////////////////////////////////////////////////////////////

void TopiaryPoolList::addToModel(XmlElement *model)
{
	// adds the pattern data as XmlElement; *model should be <Pattern>
	jassert(model->getTagName().equalsIgnoreCase("PoolList"));

	for (int p = 0; p < numItems; p++)
	{
		XmlElement* child = new XmlElement("Data");
		model->addChildElement(child);
		child->setAttribute("ID", dataList[p].ID);
		child->setAttribute("note", dataList[p].note);
		child->setAttribute("pool", dataList[p].pool);
		child->setAttribute("label", dataList[p].label);
		child->setAttribute("description", dataList[p].description);
	}

} // addToModel

/////////////////////////////////////////////////////////////////////////////

void TopiaryPoolList::getFromModel(XmlElement *model)
{
	// adds the pattern data as XmlElement; *model should be <PatternList>
	jassert(model->getTagName().equalsIgnoreCase("PoolList"));

	XmlElement* child = model->getFirstChildElement();
	int index = 0;
	while (child != nullptr)
	{
		dataList[index].ID = child->getIntAttribute("ID");
		dataList[index].note = child->getIntAttribute("note");
		dataList[index].pool = child->getIntAttribute("pool");
		dataList[index].label = child->getStringAttribute("label");
		dataList[index].description = child->getStringAttribute("description");
		child = child->getNextElement();
		index++;
	}

	numItems = index;

} // getFromModel


/////////////////////////////////////////////////////////////////////////////

void TopiaryPoolList::validateTableEdit(int p, XmlElement* child, String attribute)
{
	// called by TopiaryTable
	// careful - can be called when editing patterns but also when editing note pool entries!!
	// do processing of user edits to notes and make sure all is consistent

	beatsModel->validateTableEdit(p, child, attribute);

} // validateTableEdit

/////////////////////////////////////////////////////////////////////////////

void TopiaryPoolList::setBeatsModel(TopiaryBeatsModel* m)
{
	beatsModel = m;
}