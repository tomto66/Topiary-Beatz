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
#include "../../Topiary/Source/Model/TopiaryListModel.h"

#ifdef TOPIARYMODEL
class TOPIARYMODEL; // needs to be a defined model!

class TopiaryPattern : public TopiaryListModel
{

public:
	TopiaryPattern();
	~TopiaryPattern();
	

	void sortByID() override; 
	void sortByTimestamp(bool keepIDs=false);
	void del(int n) override;
	void addNote(int measure, int beat, int tick, int timestamp, int note, int length, int velocity);
	void addAT(int measure, int beat, int tick, int timestamp, int value);
	void addCC(int measure, int beat, int tick, int timestamp, int CC, int value);
	void addPitch(int measure, int beat, int tick, int timestamp, int value);
	 
	void add();
	void addToModel(XmlElement *m);
	void getFromModel(XmlElement *m);
	void validateTableEdit(int p, XmlElement* child, String attribute) override;
	void setModel(TOPIARYMODEL* m);
	int findID(int ID); // returns index of the ID; creates new record with that ID if not found
	void duplicate(TopiaryPattern* p);
	void quantize(int ticks);

	static const int maxItems = 16000;

	struct data // MUST match what has been defined in the headerlist data!!!
	{
		int ID;
		int measure;
		int beat;
		int tick;
		int note;
		String label;
		int length;
		int velocity;
		int timestamp;  // not in header 
		int midiType;   // not in header
		int channel;	// not in header
		int value;		// not in header - either the pitch or the CC or AT value
		//int CC;			// not in header
		
	};

    int patLenInTicks; // not in header
	data dataList[maxItems];

	void fillDataList(XmlElement* dList) override;
	void setIntByIndex(int row, int o, int newInt) override;
	void setStringByIndex(int row, int i, String newString) override;
	
	void renumber() override;
	void swapPatternData(data a, data b);

private:
	TOPIARYMODEL* model;
	void swap(int from, int to)
	{
		intSwap(dataList[from].ID, dataList[to].ID);
		intSwap(dataList[from].measure, dataList[to].measure);
		intSwap(dataList[from].beat, dataList[to].beat);
		intSwap(dataList[from].tick, dataList[to].tick);
		intSwap(dataList[from].timestamp, dataList[to].timestamp);
		intSwap(dataList[from].note, dataList[to].note);
		intSwap(dataList[from].velocity, dataList[to].velocity);
		intSwap(dataList[from].length, dataList[to].length);
		intSwap(dataList[from].midiType, dataList[to].midiType);
		intSwap(dataList[from].channel, dataList[to].channel);
		//intSwap(dataList[from].CC, dataList[to].CC);
		intSwap(dataList[from].value, dataList[to].value);
		stringSwap(dataList[from].label, dataList[to].label);

	} // swap

}; // TopiaryPattern

#endif