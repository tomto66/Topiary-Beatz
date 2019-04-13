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
#include "../../Topiary/Source/TopiaryListModel.h"

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

#undef TOPIARYLISTMODEL
#undef TOPIARYTABLE
#define TOPIARYLISTMODEL TopiaryPoolList
#define TOPIARYTABLE PoolListTable

class TOPIARYTABLE : public Component,
	public TableListBoxModel
{
public:
	TOPIARYTABLE();


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
	void updateContent();
	bool isHeaderSet();
	void setHeader();
	void setModel(TOPIARYLISTMODEL* model);

private:
	TableListBox tableComponent{ {}, this };
	Font font{ 14.0f };
	std::unique_ptr<XmlElement> tableData;
	XmlElement* dataList;

	int numRows = 0;
	bool headerSet = false;
	TOPIARYLISTMODEL* model;

	class EditableTextCustomComponent : public Label
	{
	public:
		EditableTextCustomComponent(TOPIARYTABLE& td)
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
		TOPIARYTABLE& owner;
		int row, columnId;
		Colour textColour;
	}; // class EditableTextCustomComponent

	/////////////////////////////////////////////////////////////////////////

	class SelectionColumnCustomComponent : public Component
	{
	public:
		SelectionColumnCustomComponent(TOPIARYTABLE& td)
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
		TOPIARYTABLE& owner;
		ToggleButton toggleButton;
		int row, columnId;


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
		return model->headerList[columnId - 1].name;
	}

	/////////////////////////////////////////////////////////////////////


	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(TOPIARYTABLE)
};
   //////////////////////////////////////////////////////////////////


