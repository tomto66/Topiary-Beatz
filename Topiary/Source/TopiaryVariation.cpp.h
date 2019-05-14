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
#include "TopiaryVariation.h"
#ifdef TOPIARYMODEL

TopiaryVariation::TopiaryVariation()
{
	// never in a table so no header	
	numItems = 0; // empty list

} // TopiaryPattern

/////////////////////////////////////////////////////////////////////////////

TopiaryVariation::~TopiaryVariation()
{
}

/////////////////////////////////////////////////////////////////////////////

void TopiaryVariation::del(int n)
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

void TopiaryVariation::add()
{
	jassert(numItems < (MAXVARIATIONITEMS + 1));
	// adds new one
	dataList[numItems].ID = numItems + 1;
	numItems++;

} // add


/////////////////////////////////////////////////////////////////////////////

void TopiaryVariation::sortByID()
{
	// sorts and then renumbers by ID (one might have been deleted)
	// IDs to delete are set to Topiary::ToDeleteID

	bool done;

	for (int i = 0; i < (numItems-1); i++)
	{
		done = true;
		for (int j = i+1; j < numItems; j++)
		{
			if (dataList[j].ID < dataList[i].ID)
			{
				swap(i, j);
				done = false;
			}

			if ((dataList[j].ID > dataList[j + 1].ID) && (j < (numItems - 1)))
			{
				swap(j+1, j);
				j--;
				done = false;
			}
		}
		if (done)
			i = numItems;
	};

	// DEBUG check if sorted
	for (int i = 0; i < (numItems - 1); i++)
		if (dataList[i].ID > dataList[i + 1].ID) jassert(false);

	renumber();

} // sortByID

/////////////////////////////////////////////////////////////////////////////

void TopiaryVariation::sortByTimestamp(bool keepIDs)
{
	// sorts and then renumbers by ID (one might have been deleted)
	// IDs to delete are set to Topiary::ToDeleteID

	bool done=true;

	for (int i = 0; i < (numItems-1); i++)
	{
		done = true;
		for (int j = i+1; j < numItems; j++)
		{
			if (dataList[i].timestamp > dataList[j].timestamp)
			{
				/*
				for (int k = 0; k < numItems; k++)
				{
					if ((k == j)) Logger::outputDebugString(">>>>> swap "+String(i)+" "+String(j));
					Logger::outputDebugString("<" + String(k) + "> <ID" + String(dataList[k].ID) + "> Note: " + String(dataList[k].note) + " timestamp " + String(dataList[k].timestamp));
				}
				*/
				swap(i, j);
				done = false;
			}
		
			if ((dataList[j].timestamp > dataList[j + 1].timestamp) && (j < (numItems - 1)))
			{
				/*
				for (int k = 0; k < numItems; k++)
				{
					if ((k == (j+1))) Logger::outputDebugString(">>>>> swap" + String(j+1) + " " + String(j));
					else if ((dataList[k].timestamp > dataList[k + 1].timestamp)) Logger::outputDebugString(">>");
					Logger::outputDebugString("<" + String(k) + "> <ID" + String(dataList[k].ID) + "> Note: " + String(dataList[k].note) + " timestamp " + String(dataList[k].timestamp));
				}*/
				swap(j+1, j);
				j--; // because j+1 might also be smaller than i !!!!
				done = false;
			}
		}
		if (done)
			i = numItems;
	};

	// DEBUG check if sorted
	for (int i = 0; i < (numItems - 1); i++)
		if (dataList[i].timestamp > dataList[i + 1].timestamp)
		{
			/*
			for (int k = 0; k < numItems; k++)
			{
				if ((dataList[k].timestamp > dataList[k + 1].timestamp)) Logger::outputDebugString(">>");
				Logger::outputDebugString("<" + String(k) + "> <ID" + String(dataList[k].ID) + "> Note: " + String(dataList[k].note) + " timestamp " + String(dataList[k].timestamp));
			}*/
			jassert(false);
		}

	if (!keepIDs)
		renumber();

} // sortByID

/////////////////////////////////////////////////////////////////////////////


void TopiaryVariation::renumber()
{
	for (int i = 0; i < numItems; i++)
		dataList[i].ID = i + 1;
} // renumber

/////////////////////////////////////////////////////////////////////////////

int TopiaryVariation::findID(int ID)
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

#endif