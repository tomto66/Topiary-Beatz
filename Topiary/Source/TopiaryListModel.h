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
#include "Topiary.h"

class TopiaryListModel
{

public:
	TopiaryListModel();
	~TopiaryListModel();

	int getNumItems();
	virtual void sortByID(); // sort by index n (order in which the variables are in the struct
	virtual void del(int n);
	virtual void add();
	virtual void validateTableEdit(int p, XmlElement* child, String attribute);
	struct header
	{
		int columnID;
		String name;
		int width;
		int type;  // define the type values !!
		bool editable;
		int min = 0;
		int max = 0;
		bool visible;
	};

	static const int numHeaderColumns = 7;
	int numItems =0;
	int headerListItems = 1;
	static const int maxItems = 1;
	header headerList[20];  // max # columns in a header

	virtual void fillDataList(XmlElement* dList);

	virtual void setIntByIndex(int row, int o, int newInt);

	virtual void setStringByIndex(int row, int i, String newString);

	virtual void renumber();

	int getColumnIndexByName(String name);

private:
	
}; // TopiaryListModel


