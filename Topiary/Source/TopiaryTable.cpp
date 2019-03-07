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
#include "TopiaryTable.h"

#if defined(BEATZ) || defined(CHORDZ)

TopiaryTable::TopiaryTable()
{
	tableComponent.setColour(ListBox::outlineColourId, Colours::grey);
	tableComponent.setOutlineThickness(1);
	model = nullptr;
	dummyDataList.reset(new XmlElement("dummyDataList"));
	XmlElement* child = new XmlElement("dummyDataList");
	dummyDataList->addChildElement(child);
	addAndMakeVisible(tableComponent);
}

//////////////////////////////////////////////////////////////////////////////////////

void TopiaryTable::setHeader()
{
	headerSet = true;
}

//////////////////////////////////////////////////////////////////////////////////////

bool TopiaryTable::isHeaderSet()
{
	return headerSet;
}

//////////////////////////////////////////////////////////////////////////////////////

void TopiaryTable::setDataLists(XmlElement *h, XmlElement *d)
{
		columnList = h;
		dataList = d;
		if (dataList == nullptr)
			dataList = dummyDataList->getFirstChildElement(); // may happen at initialization - dummyDataList is just a temporary XmlElement

		if (dataList != nullptr)
			numRows = dataList->getNumChildElements();
		else
			jassert(false);

		//addAndMakeVisible(tableComponent);

		if ((columnList != nullptr) && !headerSet) // if the column has already been set, we are probably loading a new preset; ignore 
		{
			forEachXmlChildElement(*columnList, columnXml)
			{  
				// type is for internal use, not to show in the header
				tableComponent.getHeader().addColumn(columnXml->getStringAttribute("name"),
														columnXml->getIntAttribute("columnId"),
														columnXml->getIntAttribute("width"),
														10,
														400,
														//TableHeaderComponent::defaultFlags);
														TableHeaderComponent::visible + TableHeaderComponent::sortable);
						
			}
			headerSet = true;
		}
		
		tableComponent.getHeader().setSortColumnId(1, true);
		
		tableComponent.setMultipleSelectionEnabled(false);
		if (numRows > 0) selectRow(1);
		tableComponent.getHeader().setStretchToFitActive(true);
	} // setDataLists


//////////////////////////////////////////////////////////////////////////////////////


int TopiaryTable::getNumRows() 
	{
		return numRows;
	}

//////////////////////////////////////////////////////////////////////////////////////

void TopiaryTable::paintRowBackground(Graphics& g, int rowNumber, int /*width*/, int /*height*/, bool rowIsSelected) 
	{
		auto alternateColour = getLookAndFeel().findColour(ListBox::backgroundColourId)
			.interpolatedWith(getLookAndFeel().findColour(ListBox::textColourId), 0.03f);
		if (rowIsSelected)
			g.fillAll(TopiaryColour::brightBlue);
		else if (rowNumber % 2)
			g.fillAll(alternateColour);
	}

//////////////////////////////////////////////////////////////////////////////////////

void TopiaryTable::paintCell(Graphics& g, int rowNumber, int columnId, int width, int height, bool rowIsSelected) 
{
	UNUSED(rowIsSelected)
	// below controls the cell text color; in the demo the ID gets a different color (dark blue - we've taken that out so rowIsSelected is no longer used really)
	// cell background is in paintRowBackground
	
	g.setColour(getLookAndFeel().findColour(ListBox::textColourId));
	g.setFont(font);

	if (auto* rowElement = dataList->getChildElement(rowNumber))
	{
		auto text = rowElement->getStringAttribute(getAttributeNameForColumnId(columnId));
		g.drawText(text, 2, 0, width - 4, height, Justification::centredLeft, true);
	}

	g.setColour(getLookAndFeel().findColour(ListBox::backgroundColourId));
	g.fillRect(width - 1, 0, 1, height);
} // paintCell

//////////////////////////////////////////////////////////////////////////////////////

void TopiaryTable::sortOrderChanged(int newSortColumnId, bool isForwards) 
	{

		if (newSortColumnId != 0)
		{
			DataSorter sorter(getAttributeNameForColumnId(newSortColumnId), isForwards);
			dataList->sortChildElements(sorter);

			tableComponent.updateContent();
		}
	}

//////////////////////////////////////////////////////////////////////////////////////

Component* TopiaryTable::refreshComponentForCell(int rowNumber, int columnId, bool /*isRowSelected*/,
		Component* existingComponentToUpdate) 
	{
		if (columnId != 1)
		{
			const auto& columnName = tableComponent.getHeader().getColumnName(columnId);
			//see if this column is editable
			auto columnDefinition = columnList->getChildByAttribute("name", columnName);

			auto editable = columnDefinition->getStringAttribute("editable");

			if (editable.compare("false")) {

				auto* textLabel = static_cast<EditableTextCustomComponent*> (existingComponentToUpdate);

				if (textLabel == nullptr) textLabel = new EditableTextCustomComponent(*this);

				textLabel->setRowAndColumn(rowNumber, columnId);
				return textLabel;
			}
			else return nullptr;
		}

		jassert(existingComponentToUpdate == nullptr);
		return nullptr;
	}

//////////////////////////////////////////////////////////////////////////////////////

int TopiaryTable::getColumnAutoSizeWidth(int columnId) 
	{
		if (columnId == 9)
			return 50;

		int widest = 32;

		for (auto i = getNumRows(); --i >= 0;)
		{
			if (auto* rowElement = dataList->getChildElement(i))
			{
				auto text = rowElement->getStringAttribute(getAttributeNameForColumnId(columnId));

				widest = jmax(widest, font.getStringWidth(text));
			}
		}

		return widest + 8;
	}

//////////////////////////////////////////////////////////////////////////////////////

int TopiaryTable::getSelectedRow()
{
	return tableComponent.getSelectedRow(0);
}

//////////////////////////////////////////////////////////////////////////////////////

void TopiaryTable::selectRow(int i)
{
	tableComponent.selectRow(i);
}

//////////////////////////////////////////////////////////////////////////////////////

int TopiaryTable::getSelection(const int rowNumber) const
	{  
		return dataList->getChildElement(rowNumber)->getIntAttribute("Select");
	}

//////////////////////////////////////////////////////////////////////////////////////

void TopiaryTable::setSelection(const int rowNumber, const int newSelection)
	{
		dataList->getChildElement(rowNumber)->setAttribute("Select", newSelection);
	}

//////////////////////////////////////////////////////////////////////////////////////

String TopiaryTable::getText(const int columnNumber, const int rowNumber) const
{
	return dataList->getChildElement(rowNumber)->getStringAttribute(getAttributeNameForColumnId(columnNumber));
}

//////////////////////////////////////////////////////////////////////////////////////

String TopiaryTable::setText(const int columnNumber, const int rowNumber, const String& newText)
{
	String validatedText;
	// check input, type etc here!!!
	const auto& columnName = tableComponent.getHeader().getColumnName(columnNumber);
	// find the column definition in columnList

	auto columnDefinition = columnList->getChildByAttribute("name", columnName);

	auto type = columnDefinition->getStringAttribute("type");
	
	if (type.compare("int") == 0)
	{
		// see if valid integer; if not make it 1 or minimum
		int i = newText.getIntValue();

		// if valid integer, see if it's beween the bounds
		int min = columnDefinition->getIntAttribute("min");
		int max = columnDefinition->getIntAttribute("max");

		if (i >= min)
		{
			if (i > max) i = max;
		}
		else i = min;
		validatedText = String(i);
	}
	
	else if (type.compare("noteLabel") == 0)
	{
		validatedText = validateNote(newText);
		int noteNumber = validNoteNumber(validatedText);
		dataList->getChildElement(rowNumber)->setAttribute("NoteNumber", noteNumber);
	}
	else
		validatedText = newText;
	

	dataList->getChildElement(rowNumber)->setAttribute(columnName, validatedText);

	//if (broadcaster != nullptr)
	//{
	//	broadcaster->sendActionMessage(broadcastMessage);
	//}

#ifdef TOPIARYMODEL
	if (model != nullptr) // i.e. we are editing a pattern or pool notes (otherwise model == nullptr)
	{
		model->validateTableEdit(pattern, dataList->getChildElement(rowNumber), columnName);
		int remember = tableComponent.getSelectedRow(0);
		setDataLists(columnList, dataList);  // force refresh
		selectRow(remember);
	}
#endif
	return validatedText;
} // setText

//////////////////////////////////////////////////////////////////////////////////////

void TopiaryTable::updateContent()
{
	if (dataList != nullptr)
		numRows = dataList->getNumChildElements();
	else
		numRows = 0;
	tableComponent.updateContent();
}

//////////////////////////////////////////////////////////////////////////////////////

void TopiaryTable::resized() 
{
	tableComponent.setBoundsInset(BorderSize<int>(8));
} // resized

//////////////////////////////////////////////////////////////////////////////////////

//void TopiaryTable::setBroadcaster(ActionBroadcaster *b, String msg) 
//{
//	jassert(false); // think no longer needed
//	broadcaster = b;
//	broadcastMessage = msg;
//}

//////////////////////////////////////////////////////////////////////////////////////

#if defined(BEATZ) || defined(CHORDZ)
void TopiaryTable::setModel(TOPIARYMODEL* m)
{
	model = m;
}

//////////////////////////////////////////////////////////////////////////////////////

void TopiaryTable::setPattern(int p)
{
	// set pattern index
	pattern = p;
}
#endif

//////////////////////////////////////////////////////////////////////////////////////

#endif // main #if defined