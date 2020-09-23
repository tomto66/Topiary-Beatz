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

#include "TopiaryListModel.h"

TopiaryListModel::TopiaryListModel()
{
} // TopiaryListModel

/////////////////////////////////////////////////////////////////////////////

TopiaryListModel::~TopiaryListModel()
{
} // ~TopiaryListModel

/////////////////////////////////////////////////////////////////////////////

void TopiaryListModel::setIntByIndex(int row, int i, int newInt)
{
	UNUSED(row)
	UNUSED(i)
	UNUSED(newInt)
	jassert(false);
} // setIntByIndex

/////////////////////////////////////////////////////////////////////////////

void TopiaryListModel::setStringByIndex(int row, int i, String newString)
{
	// virtual
	UNUSED(row)
	UNUSED(i)
	UNUSED(newString)
	jassert(false);
}

/////////////////////////////////////////////////////////////////////////////

void TopiaryListModel::del(int n)
{
	// virtual
	UNUSED(n)
	jassert(false);
} // del

/////////////////////////////////////////////////////////////////////////////

void TopiaryListModel::add()
{
	jassert(false);
} // add

/////////////////////////////////////////////////////////////////////////////

void TopiaryListModel::sortByID()
{
	jassert(false);
} // sortByID

/////////////////////////////////////////////////////////////////////////////

void TopiaryListModel::renumber()
{
	jassert(false);
} // renumber

/////////////////////////////////////////////////////////////////////////////

void TopiaryListModel::fillDataList(XmlElement* dList)
{
	// virtual
	UNUSED(dList)
	jassert(false);
}  // fillDataList

/////////////////////////////////////////////////////////////////////////////

int TopiaryListModel::getColumnIndexByName(String name)
{
	for (int i = 0; i < headerListItems; i++)
			if (name.compare(headerList[i].name) == 0)
				return i;
	jassert(false); // column name not found
	return(-1);
	
} // getColumnNameByIndex

/////////////////////////////////////////////////////////////////////////////

int TopiaryListModel::getNumItems()
{
	return numItems;
}

/////////////////////////////////////////////////////////////////////////////

void TopiaryListModel::validateTableEdit(int p, XmlElement* child, String attribute)
{
	//jassert(false);
	UNUSED(p) 
	UNUSED(child)
	UNUSED(attribute)
} // validateTableEdit

/////////////////////////////////////////////////////////////////////////////

void TopiaryListModel::selectedRowsChanged(int lastRowSelected)
{
	UNUSED(lastRowSelected);
}

/////////////////////////////////////////////////////////////////////////////

