/////////////////////////////////////////////////////////////////////////////
/*
This file is part of Topiary Beats, Copyright Tom Tollenaere 2018.

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
	beatsModel->getPatternModel(0, &patternListHeader, &patternListData);  // @ initialization this will simply be an empty pattern

	patternTable.setDataLists(patternListHeader, patternListData);
	
	beatsModel->setListener((ActionListener*)this);

	// trick to call the model and read 
	
	actionListenerCallback(MsgMasterTables); // among other things, set the pattern combobox;

} // setModel

/////////////////////////////////////////////////////////////////////////

void TopiaryBeatsPatternComponent::paint(Graphics& g)
{
	auto area = getLocalBounds();

	g.fillAll(TopiaryColour::background);
	g.setColour(TopiaryColour::foreground);
	g.setFont(12.0f);
	patternTable.setBounds(10, 10, patternTW + 10, patternTH + 10);

	patternCombo.setBounds(600, 10, 200, 30);

} // paint

/////////////////////////////////////////////////////////////////////////

void TopiaryBeatsPatternComponent::resized()
{
} //resized

/////////////////////////////////////////////////////////////////////////

void TopiaryBeatsPatternComponent::actionListenerCallback(const String &message)
{
	if (message.compare(MsgPattern) == 0)
	{
		// pattern (may have) changed; set the table such that the 
	}
	if (message.compare(MsgMasterTables) == 0)
	{
		// find the list of patterns loaded
		String patterns[8];
		patternCombo.clear();
		for (int i = 0; i < 8; i++) patterns[i] = "";

		beatsModel->getPatterns(patterns);
		int i = 0;
		while (patterns[i].compare(""))
		{
			patternCombo.addItem(patterns[i], i + 1);
			i++;
		}

		// if there are no patterns, disable yourself!!!
		if (beatsModel->getNumPatterns())
		{
			this->setEnabled(true);
			patternCombo.setSelectedItemIndex(0);
			actionListenerCallback(MsgPattern);  // force reload of patterndata
		}
		else
			this->setEnabled(false);
		// fill the combobox with the pattern names
	}

} // actionListenerCallback


