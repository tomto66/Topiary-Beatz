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
#include "TopiaryPatternList.h"

TopiaryPatternList::TopiaryPatternList()
{
	// initialize the headerlist data
	headerListItems = 3;
	// headerlist IDs start at 1;
	// datalist IDs start at 1;
	
	headerList[0].columnID = 1;
	headerList[0].name = "ID";
	headerList[0].width = 20;
	headerList[0].type = Topiary::HeaderType::Int;
	headerList[0].editable = false;
	headerList[0].visible = true;

	headerList[1].columnID = 2;
	headerList[1].name = "Name";
	headerList[1].width = 140;
	headerList[1].type = Topiary::HeaderType::String;
	headerList[1].editable = true;
	headerList[1].visible = true;

	headerList[2].columnID = 3;
	headerList[2].name = "Measures";
	headerList[2].width = 70;
	headerList[2].type = Topiary::HeaderType::Int;
	headerList[2].editable = false;
	headerList[2].visible = true;

	numItems = 0; // empty list
	
} // TopiaryPatternList

/////////////////////////////////////////////////////////////////////////////

TopiaryPatternList::~TopiaryPatternList()
{
} 

/////////////////////////////////////////////////////////////////////////////

void TopiaryPatternList::setIntByIndex(int row, int i, int newInt)
{
	
	jassert(row < numItems);
	jassert(i < headerListItems);
	if (i == 0)
		dataList[row].ID = newInt;
	else if (i == 2)
		dataList[row].measures = newInt;
	else
	{
		jassert(false); // wrong type
	}
} // setIntByID

/////////////////////////////////////////////////////////////////////////////

void TopiaryPatternList::setStringByIndex(int row, int i, String newString)
{
	
	jassert(row < numItems);
	jassert(i < headerListItems);
	if (i == 1)
		dataList[row].name = newString;
	else
	{  
		jassert(false); // wrong type
	}
} // setStringByIndex

/////////////////////////////////////////////////////////////////////////////

void TopiaryPatternList::del(int n)
{
	jassert(n >= 0);
	jassert(n < numItems);
	for (int i = n; i <(numItems-1); i++)
	{
		swap(i, i + 1);
	}

	numItems--;
	for (int i = 0; i < numItems; i++)
		dataList[i].ID = i + 1;
} // del

/////////////////////////////////////////////////////////////////////////////

void TopiaryPatternList::add()
{
	jassert(numItems < (maxItems + 1));

	// adds new empty one
	dataList[numItems].name = "--empty--";
	dataList[numItems].measures = 1;
	dataList[numItems].ID = numItems+1;
	numItems++;

	// ToDo: create the pattern itself!!!!
}

/////////////////////////////////////////////////////////////////////////////

void TopiaryPatternList::duplicate(int i)
{
	add();
	dataList[numItems - 1].name = dataList[i].name + "(dup)";
	dataList[numItems - 1].measures = dataList[i].measures;
	dataList[numItems].ID = numItems;
}

/////////////////////////////////////////////////////////////////////////////

void TopiaryPatternList::sortByID()
{
	// sorts and then renumbers by ID (one might have been deleted)
	// IDs to delete are set to Topiary::ToDeleteID

	for (int i=0; i<=numItems; i++)
	{
		for (int j=i+1; j<numItems; j++)
		{
			if (dataList[i].ID > dataList[j].ID)
			{
				swap(i,j);
			}
		}		
	};

	// renumber
	for (int i=0; i< numItems; i++)
		dataList[i].ID = i+1;
		
} // sortByID

/////////////////////////////////////////////////////////////////////////////

void TopiaryPatternList::fillDataList(XmlElement* dList)
{
	dList->deleteAllChildElements();
	for (int i=0; i<numItems;i++)
	{
		XmlElement *child = new XmlElement("Data");
		child->setAttribute("Name",dataList[i].name);
		child->setAttribute("ID",dataList[i].ID);
		child->setAttribute("Measures",dataList[i].measures);
		dList->addChildElement(child);
	}
}  // fillDataList

/////////////////////////////////////////////////////////////////////////////

void TopiaryPatternList::addToModel(XmlElement *model)
{
	// adds the pattern data as XmlElement; *model should be <PatternList>
	jassert(model->getTagName().equalsIgnoreCase("PatternList"));

	for (int p = 0; p < numItems; p++)
	{
		XmlElement* child = new XmlElement("PatternData");
		model->addChildElement(child);
		child->setAttribute("ID", dataList[p].ID);
		child->setAttribute("name", dataList[p].name);
		child->setAttribute("measures", dataList[p].measures);
	}

} // addToModel

/////////////////////////////////////////////////////////////////////////////

void TopiaryPatternList::getFromModel(XmlElement *model)
{
	// adds the pattern data as XmlElement; *model should be <PatternList>
	jassert(model->getTagName().equalsIgnoreCase("PatternList"));

	XmlElement* child = model->getFirstChildElement();
	int index = 0;
	while (child != nullptr)
	{
		dataList[index].ID = child->getIntAttribute("ID");
		dataList[index].name = child->getStringAttribute("name");
		dataList[index].measures = child->getIntAttribute("measures");
		index++;
		child = child->getNextElement();
	}

	numItems = index;

} // getFromModel


