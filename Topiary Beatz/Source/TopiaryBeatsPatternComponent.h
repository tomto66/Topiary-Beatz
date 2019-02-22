/////////////////////////////////////////////////////////////////////////////
/*
This file is part of Topiary Beatz, Copyright Tom Tollenaere 2018-19.

Topiary Beats is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

Topiary Beats is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Topiary Beats. If not, see <https://www.gnu.org/licenses/>.
*/
/////////////////////////////////////////////////////////////////////////////


#pragma once
#include"TopiaryBeatsModel.h"
//#include "../../Topiary/Source/Topiary.h"
//#include"../../Topiary/Source/TopiaryTable.h"  
#include "TopiaryBeatsPatternChildren.h"

class TopiaryBeatsPatternComponent : public Component, ActionListener
{
public:
	TopiaryBeatsPatternComponent();
	~TopiaryBeatsPatternComponent();

	void paint(Graphics&) override;
	void resized() override;
	void setModel(TopiaryBeatsModel* m);
	void actionListenerCallback(const String &message) override;
	void setPatternLength(); 
	void deleteNote(); // deletes selected note
	void addNote(); // adds a note at the position selected in table
	void copyNote();
	void pasteNote();


private:
	TopiaryBeatsModel* beatsModel;
	TopiaryTable patternTable;
	int patternTW = 380;
	int patternTH = 360;
	XmlElement *patternListHeader = nullptr;
	XmlElement *patternListData = nullptr;
	
	ComboBox patternCombo;
	PatternLengthComponent patternLengthComponent;
	ActionButtonsComponent actionButtonsComponent;

	// copy/paste buffer
	int pbufferNote = -1;			// -1 means uninitialized
	int pbufferVelocity = -1;
	int pbufferLength = -1;

	///////////////////////////////////////////////////////////////////////////////////////

	void processPatternCombo() // call when pattern combobox changed
	{
		beatsModel->getPatternModel(patternCombo.getSelectedId()-1, &patternListHeader, &patternListData);  // @ initialization this will simply be an empty pattern
		patternTable.setDataLists(patternListHeader, patternListData);
		patternTable.updateContent();
		patternTable.setPattern(patternCombo.getSelectedId() - 1);
		
		patternLengthComponent.lengthEditor.setText(String(beatsModel->getPatternLengthInMeasures( patternCombo.getSelectedId() - 1)));
		beatsModel->setPatternTableHeaders(patternCombo.getSelectedId() - 1); // make sure the table allow valid measure inputs
		setButtonStates();

	} // processPatternCombo

	///////////////////////////////////////////////////////////////////////////////////////

	void setButtonStates()
	{

		int numRows = patternTable.getNumRows();
		//int selRow = patternTable.getSelectedRow();

		if (pbufferNote == -1)
			actionButtonsComponent.pasteButton.setEnabled(false);
		else
			actionButtonsComponent.pasteButton.setEnabled(true);

		// make sure that if there are notes, at least one is selected
		if (patternTable.getSelectedRow() <0)
			if (numRows > 0)
				patternTable.selectRow(0);

		// called from actionListener; enable/dcisable buttons depending on pattern state
		if (patternTable.getNumRows() == 0)
		{		
			actionButtonsComponent.deleteButton.setEnabled(false);
			actionButtonsComponent.copyButton.setEnabled(false);
		}
		else if (patternTable.getSelectedRow() >= 0)
		{
			actionButtonsComponent.deleteButton.setEnabled(true);
			actionButtonsComponent.copyButton.setEnabled(true);
		}
		else
		{
			// nothing selected
			actionButtonsComponent.deleteButton.setEnabled(false);
			actionButtonsComponent.copyButton.setEnabled(false);
		}

	}  // setButtonStates

	///////////////////////////////////////////////////////////////////////////////////////

	

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(TopiaryBeatsPatternComponent)
};

/////////////////////////////////////////////////////////////////////
