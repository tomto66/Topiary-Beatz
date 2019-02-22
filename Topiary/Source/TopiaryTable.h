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
#include "../JuceLibraryCode/JuceHeader.h"
#include "Topiary.h"

#if defined (BEATZ) || defined(CHORDZ)

class TOPIARYMODEL;


class TopiaryTable : public Component, 
					 public TableListBoxModel
{
public:
	TopiaryTable();

	int getNumRows() override;
	void paintRowBackground(Graphics& g, int, int, int, bool) override;
	void paintCell(Graphics&, int, int, int, int, bool) override;
	void sortOrderChanged(int, bool) override;
	Component* refreshComponentForCell(int, int, bool, Component*) override;
	int getColumnAutoSizeWidth(int) override;
	int getSelectedRow();
	void selectRow(int i);
	int getSelection(const int) const;
	void setSelection(const int, const int);
	String getText(const int, const int) const;
	String setText(const int, const int, const String&);
	void resized() override;	
	void setDataLists(XmlElement *h, XmlElement *d);
	void updateContent();
	bool isHeaderSet();
	void setHeader();
	void setBroadcaster(ActionBroadcaster *b, String msg);

#ifdef BEATZ
	void setModel(TOPIARYMODEL* m);
	void setPattern(int p); // so the table knows which pattern is loaded into it
#endif

private:
	TableListBox tableComponent{ {}, this };
	Font font{ 14.0f };
	std::unique_ptr<XmlElement> tableData;
	XmlElement* columnList = nullptr;
	XmlElement* dataList = nullptr;
	int numRows = 0;
	bool headerSet = false;
	ActionBroadcaster* broadcaster = nullptr;
	String broadcastMessage = "";
	int pattern; // needed to the table can know which pattern we are editing (only when editing patterns of course)

#ifdef BEATZ
	TOPIARYMODEL* model;
#endif

	class EditableTextCustomComponent : public Label
	{
	public:
		EditableTextCustomComponent(TopiaryTable& td)
			: owner(td)
		{
			setEditable(false, true, false);
		}

		/////////////////////////////////////////////////////////////////////

		void mouseDown(const MouseEvent& event) override
		{
			owner.tableComponent.selectRowsBasedOnModifierKeys(row, event.mods, false);

			Label::mouseDown(event);
		}

		/////////////////////////////////////////////////////////////////////

		void textWasEdited() override
		{
			String validatedText = owner.setText(columnId, row, getText());
			setText(validatedText, dontSendNotification);
		}

		/////////////////////////////////////////////////////////////////////

		void setRowAndColumn(const int newRow, const int newColumn)
		{
			row = newRow;
			columnId = newColumn;
			setText(owner.getText(columnId, row), dontSendNotification);
		}

		
	private:
		TopiaryTable& owner;
		int row, columnId;
		Colour textColour;
	}; // class EditableTextCustomComponent

	/////////////////////////////////////////////////////////////////////////

	class SelectionColumnCustomComponent : public Component
	{
	public:
		SelectionColumnCustomComponent(TopiaryTable& td)
			: owner(td)
		{
			addAndMakeVisible(toggleButton);

			toggleButton.onClick = [this] { owner.setSelection(row, (int)toggleButton.getToggleState()); };
		}

		void resized() override
		{
			toggleButton.setBoundsInset(BorderSize<int>(2));
		}

		void setRowAndColumn(int newRow, int newColumn)
		{
			row = newRow;
			columnId = newColumn;
			toggleButton.setToggleState((bool)owner.getSelection(row), dontSendNotification);
		}

	private:
		TopiaryTable& owner;
		ToggleButton toggleButton;
		int row, columnId;

#ifdef BEATZ

#endif
	};  // class SelectionColumnCustomComponent

	/////////////////////////////////////////////////////////////////////////

	class DataSorter
	{
	public:
		DataSorter(const String& attributeToSortBy, bool forwards)
			: attributeToSort(attributeToSortBy),
			direction(forwards ? 1 : -1)
		{}

		int compareElements(XmlElement* first, XmlElement* second) const
		{
			auto result = first->getStringAttribute(attributeToSort)
				.compareNatural(second->getStringAttribute(attributeToSort));

			if (result == 0)
				result = first->getStringAttribute("ID")
				.compareNatural(second->getStringAttribute("ID"));

			return direction * result;
		}

	private:
		String attributeToSort;
		int direction;
	}; // class DataSorter

	/////////////////////////////////////////////////////////////////////

	String getAttributeNameForColumnId(const int columnId) const
	{
		forEachXmlChildElement(*columnList, columnXml)
		{
			if (columnXml->getIntAttribute("columnId") == columnId)
				return columnXml->getStringAttribute("name");
		}

		return {};
	}

	/////////////////////////////////////////////////////////////////////


	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(TopiaryTable)
};

#endif  // of the top #if defined statement