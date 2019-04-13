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

#include "../JuceLibraryCode/JuceHeader.h"
#include "TopiaryBeatsPatternComponent.h"


TopiaryBeatsPatternComponent::TopiaryBeatsPatternComponent()
{   // size set by TabbedComponent!
	// Table for patterns
	patternTable.setSize(patternTW, patternTH);
	addAndMakeVisible(patternTable);
	
	addAndMakeVisible(patternCombo);
	patternCombo.setSize(200, 30);
	patternCombo.onChange = [this]
	{
		processPatternCombo();
	};

	addAndMakeVisible(patternLengthComponent);
	addAndMakeVisible(actionButtonsComponent);

} // TopiaryBeatsPatternComponent

/////////////////////////////////////////////////////////////////////////


TopiaryBeatsPatternComponent::~TopiaryBeatsPatternComponent()
{
	beatsModel->removeListener((ActionListener*)this);
} //~TopiaryBeatsPatternComponent

/////////////////////////////////////////////////////////////////////////

void TopiaryBeatsPatternComponent::setModel(TopiaryBeatsModel* m)
{
	beatsModel = m;
	beatsModel->setListener((ActionListener*)this);
	
	patternTable.setModel(beatsModel->getPattern(0)); // (0) just to get it started because it will have to validate note data!
	actionButtonsComponent.setParent(this);
	patternLengthComponent.setParent(this);
	
	beatsModel->setListener((ActionListener*)this);

	// trick to call the model and read 
	
	actionListenerCallback(MsgPatternList); // among other things, set the pattern combobox;

} // setModel

/////////////////////////////////////////////////////////////////////////

void TopiaryBeatsPatternComponent::paint(Graphics& g)
{
	auto area = getLocalBounds();

	g.fillAll(TopiaryColour::background);
	g.setColour(TopiaryColour::foreground);
	g.setFont(12.0f);
	patternTable.setBounds(10, 10, patternTW + 10, patternTH + 10);

	patternCombo.setBounds(420, 25, 200, 30);

	patternLengthComponent.setBounds(630, 10, patternLengthComponent.getWidth(), patternLengthComponent.getHeight());
	actionButtonsComponent.setBounds(420, 60, actionButtonsComponent.getWidth(), actionButtonsComponent.getHeight());
		
} // paint

/////////////////////////////////////////////////////////////////////////

void TopiaryBeatsPatternComponent::resized()
{
} //resized

/////////////////////////////////////////////////////////////////////////

void TopiaryBeatsPatternComponent::actionListenerCallback(const String &message)
{
	if (message.compare(MsgLoad) == 0)
	{
		// this means we just loaded a preset; model needs to be refreshed because the pattern data may have been overwritten
		//beatsModel->getPatternModel(0, &patternListHeader, &patternListData);  // @ initialization this will simply be an empty pattern
		//patternTable.setDataLists(patternListHeader, patternListData);
		patternTable.setModel(beatsModel->getPattern(0)); // (0) just to get it started because it will have to validate note data!
	}
	else if (message.compare(MsgPattern) == 0)
	{
		// pattern (may have) changed; update the table
		int rememberSelectedRow = patternTable.getSelectedRow();
		patternTable.updateContent();
		patternTable.selectRow(rememberSelectedRow);
		setButtonStates(); // is in include file
	}
	else if (message.compare(MsgPatternList) == 0)
	{
		// find the list of patterns loaded
		String patterns[8];
		int rememberPatternComboSelection = patternCombo.getSelectedItemIndex()-1;
		if (rememberPatternComboSelection < 0)
			rememberPatternComboSelection = 0;

		patternCombo.clear();
		for (int i = 0; i < 8; i++) patterns[i] = "";

		beatsModel->getPatterns(patterns);

		for (int i=0; i<8; i++)
		{
			if (patterns[i].compare(""))
				patternCombo.addItem(patterns[i], i + 1);
		}

		// if there are no patterns, disable yourself!!!
		if (beatsModel->getNumPatterns()>0)
		{
			this->setEnabled(true);
			if (patternCombo.getNumItems() > rememberPatternComboSelection)
			{
				patternCombo.setSelectedItemIndex(rememberPatternComboSelection);
				//patternTable.setModel(beatsModel->getPattern(rememberPatternComboSelection)); done by above line
			}
			else
			{
				patternCombo.setSelectedItemIndex(0);
				//patternTable.setModel(beatsModel->getPattern(0)); done by above line
			}

			//actionListenerCallback(MsgPattern);  // force reload of patterndata
		}
		else
		{
			this->setEnabled(false);
			patternCombo.setSelectedItemIndex(0, dontSendNotification);
		}
		// fill the combobox with the pattern names
	}

} // actionListenerCallback

/////////////////////////////////////////////////////////////////////////

void TopiaryBeatsPatternComponent::setPatternLength()
{
	// validate the input
	int len = patternLengthComponent.lengthEditor.getText().getIntValue();
	if ((len <= 0) || len > 16)
	{
		beatsModel->Log("Invalid pattern length.", Topiary::LogType::Warning);
		return;
	}
	else
	{
		beatsModel->setPatternLength(patternCombo.getSelectedId() - 1, len, patternLengthComponent.keepTail.getToggleState());
		patternTable.updateContent();
	}

} // setPatternLength

/////////////////////////////////////////////////////////////////////////

void TopiaryBeatsPatternComponent::deleteNote()
{
	// deletes selected note

	auto selection = patternTable.getSelectedRow();
	jassert(selection >= 0);

	beatsModel->deleteNote(patternCombo.getSelectedId() - 1, selection);
	//patternTable.updateContent(); not needed, model will post action MsgPattern

} // deleteNote

/////////////////////////////////////////////////////////////////////////

void TopiaryBeatsPatternComponent::addNote()
{
	// adds a note at the position selected in table
	int timestamp = 0;
	int note = 0;
	int velocity;
	int length;
	
	// see if there is something selected in the table
	if (patternTable.getNumRows() == 0)
		timestamp = 0;
	else if (patternTable.getSelectedRow() < 0)
		timestamp = 0;
	else
	{
		beatsModel->getNote(patternCombo.getSelectedId() - 1, patternTable.getSelectedRow(), note, velocity, timestamp, length);
	}
	beatsModel->addNote(patternCombo.getSelectedId() - 1, 0, 127, Topiary::TICKS_PER_QUARTER, timestamp);
	//int debug = patternTable.getSelectedRow();
	/*if (rememberSelectedRow < 0)
		patternTable.selectRow(1);
	else 
		patternTable.selectRow(rememberSelectedRow);
		*/

	actionListenerCallback(MsgPattern); // sets all buttons properly (e.g. enable the delete button) - will remember the selected row!

}  // addNote

/////////////////////////////////////////////////////////////////////////

void TopiaryBeatsPatternComponent::copyNote()
{
	// adds a note at the position selected in table
	int timestamp = 0;
	
	beatsModel->getNote(patternCombo.getSelectedId() - 1, patternTable.getSelectedRow(), pbufferNote, pbufferVelocity, timestamp, pbufferLength);
	
	// call setButtonStates so pase button comes on
	setButtonStates();

}  // copyNote

/////////////////////////////////////////////////////////////////////////

void TopiaryBeatsPatternComponent::pasteNote()
{
	// adds a note at the position selected in table
	int timestamp = 0;
	int length, note, velocity;

	jassert(pbufferNote != -1);

	// see if there is something selected in the table
	if (patternTable.getNumRows() == 0)
		timestamp = 0;
	else if (patternTable.getSelectedRow() < 0)
		timestamp = 0;
	else
	{
		beatsModel->getNote(patternCombo.getSelectedId() - 1, patternTable.getSelectedRow(), note, velocity, timestamp, length);
	}
	beatsModel->addNote(patternCombo.getSelectedId() - 1, pbufferNote, pbufferVelocity, pbufferLength, timestamp);
	

}  // pasteNote

/////////////////////////////////////////////////////////////////////////

void TopiaryBeatsPatternComponent::clearPattern()
{
	beatsModel->clearPattern(patternCombo.getSelectedId() - 1);
	actionListenerCallback(MsgPattern);

} // clearPattern

/////////////////////////////////////////////////////////////////////////

void TopiaryBeatsPatternComponent::deleteAllNotes()
{
	// deletes all notes equal to selected one from the pattern
	beatsModel->deleteAllNotes(patternCombo.getSelectedId() - 1, patternTable.getSelectedRow() + 1);
	actionListenerCallback(MsgPattern);

} // deleteAllNotes

/////////////////////////////////////////////////////////////////////////

