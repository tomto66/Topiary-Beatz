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

#include"TopiaryBeatsModel.h"
#include "../../Topiary/Topiary.h"
#include"../../Topiary/TopiaryTable.h"  

class TopiaryBeatsPatternComponent : public Component, ActionListener
{
public:
	TopiaryBeatsPatternComponent();
	~TopiaryBeatsPatternComponent();

	void paint(Graphics&) override;
	void resized() override;
	void setModel(TopiaryBeatsModel* m);
	void actionListenerCallback(const String &message) override;

private:
	TopiaryBeatsModel* beatsModel;
	TopiaryTable patternTable;
	int patternTW = 380;
	int patternTH = 360;
	XmlElement *patternListHeader = nullptr;
	XmlElement *patternListData = nullptr;
	
	ComboBox patternCombo;

	void processPatternCombo() // call when pattern combobox changed
	{
		beatsModel->getPatternModel(patternCombo.getSelectedId()-1, &patternListHeader, &patternListData);  // @ initialization this will simply be an empty pattern
		patternTable.setDataLists(patternListHeader, patternListData);
		patternTable.updateContent();
	} // processPatternCombo

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(TopiaryBeatsPatternComponent)
};

/////////////////////////////////////////////////////////////////////
