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
/////////////////////////////////////////////////////////////////////////////

This code has a generic variation component that can be included in every Topiary plugin.

CAREFUL: needs symbols:
- TOPIARYMODEL e.g. TOPIARYBEATSMODEL

*/
/////////////////////////////////////////////////////////////////////////////

#pragma once

#ifdef TOPIARYMODEL

#include "../../Topiary/Source/Model/TopiaryListModel.h"

class TopiaryVariation : public TopiaryListModel
{

public:
	TopiaryVariation();
	~TopiaryVariation();

	void sortByID() override; 
	void sortByTimestamp(bool keepIDs=false);
	void del(int n) override;
	void add();
	int findID(int ID); // returns index of the ID; creates new record with that ID if not found

	struct data // MUST match what has been defined in the headerlist data!!!
	{
		int ID;
		int note;
		int length;
		int velocity;
		int timestamp;  
		int midiType;   
		int channel;	
		int value;		
		int CC;		
	};

    int patLenInTicks; // not in header
	data dataList[MAXVARIATIONITEMS];

	void renumber() override;
	void swapPatternData(data a, data b);

private:
	
	void swap(int from, int to)
	{
		int rememberN;
		String rememberS;

		rememberN = dataList[from].ID;
		dataList[from].ID = dataList[to].ID;
		dataList[to].ID = rememberN;

		rememberN = dataList[from].timestamp;
		dataList[from].timestamp = dataList[to].timestamp;
		dataList[to].timestamp = rememberN;
		
		rememberN = dataList[from].note;
		dataList[from].note = dataList[to].note;
		dataList[to].note = rememberN;

		rememberN = dataList[from].length;
		dataList[from].length = dataList[to].length;
		dataList[to].length = rememberN;
		
		rememberN = dataList[from].velocity;
		dataList[from].velocity = dataList[to].velocity;
		dataList[to].velocity = rememberN;

		rememberN = dataList[from].midiType;
		dataList[from].midiType = dataList[to].midiType;
		dataList[to].midiType = rememberN;

		rememberN = dataList[from].channel;
		dataList[from].channel = dataList[to].channel;
		dataList[to].channel = rememberN;

		rememberN = dataList[from].CC;
		dataList[from].CC = dataList[to].CC;
		dataList[to].CC = rememberN;

		rememberN = dataList[from].value;
		dataList[from].value = dataList[to].value;
		dataList[to].value = rememberN;

	} // swap

}; // TopiaryVariation

#endif