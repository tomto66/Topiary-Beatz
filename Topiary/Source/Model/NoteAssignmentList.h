///////////////////////////////////////////////////////////////////////////
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

#include "../../Topiary/Source/Model/TopiaryListModel.h"

class TopiaryPadzModel;

class NoteAssignmentList : public TopiaryListModel
{

public:
	NoteAssignmentList();
	~NoteAssignmentList();
	 
	void sortByID() override; 
	void del(int n) override;
	void add() override;
	void addToModel(XmlElement *model);
	void getFromModel(XmlElement *model);
	void validateTableEdit(int p, XmlElement* child, String attribute) override;
	void selectedRowsChanged(int lastRowSelected) override;
	void setRiffzModel(TopiaryPadzModel* m);
	void sortByNote();
	int findOffset(int note); // return the offset for the note given; return 0 if nothing known
	void redoPatternNames(int patternId, String patternName);

	static const int maxItems = 128;

	struct data // MUST match what has been defined in the headerlist data!!!
	{
		int ID;
		int note;
		String patternName;
		int patternId;
		int offset;
		String noteLabel;

	};

	data dataList[maxItems];

	void fillDataList(XmlElement* dList) override;

	void setIntByIndex(int row, int o, int newInt) override;

	void setStringByIndex(int row, int i, String newString) override;

	void renumber() override;


private:
	TOPIARYMODEL* model;

	void swap(int from, int to)
	{ 
		intSwap(dataList[from].ID, dataList[to].ID);
		stringSwap(dataList[from].patternName, dataList[to].patternName);
		intSwap(dataList[from].offset, dataList[to].offset);
		intSwap(dataList[from].note, dataList[to].note);
		intSwap(dataList[from].patternId, dataList[to].patternId);
		stringSwap(dataList[from].noteLabel, dataList[to].noteLabel);
	} // swap

}; // NoteAssignmentList

#endif