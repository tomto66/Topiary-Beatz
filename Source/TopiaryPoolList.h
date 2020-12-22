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

class TopiaryBeatsModel;

class TopiaryPoolList : public TopiaryListModel
{

public:
	TopiaryPoolList();
	~TopiaryPoolList();
	
	void sortByID() override; 
	void sortByNote();
	void del(int n) override;
	void add() override;
	void addToModel(XmlElement *model);
	void getFromModel(XmlElement *model);
	void validateTableEdit(int p, XmlElement* child, String attribute) override;
	void setBeatsModel(TopiaryBeatsModel* m);

	static const int maxItems = 128;

	struct data // MUST match what has been defined in the headerlist data!!!
	{
		int ID;
		int note;
		String label;
		String description;
		int pool;
	};

	data dataList[maxItems];

	void fillDataList(XmlElement* dList) override;

	void setIntByIndex(int row, int o, int newInt) override;

	void setStringByIndex(int row, int i, String newString) override;

	void renumber() override;

	int isNoteInPool(int n);

private:
	TopiaryBeatsModel* beatsModel;

	void swap(int from, int to)
	{ 
		intSwap(dataList[from].ID, dataList[to].ID);
		intSwap(dataList[from].pool, dataList[to].pool);
		intSwap(dataList[from].note, dataList[to].note);
		stringSwap(dataList[from].label, dataList[to].label);
		stringSwap(dataList[from].description, dataList[to].description);

	} // swap

}; // TopiaryPoolList

//////////////////////////////////////////////////////////////////


