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
//#include"../../Topiary/Source/TopiaryTable.h"  // included in model.h
#include"TopiaryBeatsModel.h"

//class TopiaryBeatsMasterComponent;

class TopiaryBeatsMasterComponent : public Component, ActionListener
{
public:
	TopiaryBeatsMasterComponent();
	~TopiaryBeatsMasterComponent();

	void paint(Graphics&) override;
	void resized() override;
	void setModel(TopiaryBeatsModel* m);

private:
	TopiaryBeatsModel* beatsModel;
	TopiaryLookAndFeel topiaryLookAndFeel;
	void actionListenerCallback(const String &message);
	// patterns stuff
	TopiaryTable patternsTable;
	XmlElement *patternListHeader = nullptr;
	XmlElement *patternListData = nullptr;
	TextButton insertPatternButton;
	TextButton duplicatePatternButton;
	TextButton deletePatternButton;
	TextButton newPatternButton;
	TextButton overloadPatternButton;

	int buttonH = 20;
	int buttonW = 100;
	int patternButtonOffsetX = 280;

	int patternTW = 253;
	int patternTH = 240;

	// pool stuff
	TopiaryTable poolTable;
	XmlElement *poolListHeader = nullptr;
	XmlElement *poolListData = nullptr;
	
	TextButton deletePoolButton;
	TextButton newPoolButton;
	TextButton GMDrumMapButton;
	TextButton regeneratePoolButton;
	TextButton cleanPoolButton;

	int poolTW = 250;
	int poolTH = 240;


	// settings stuff

	TopiaryButton WFFNButton;
	TopiaryButton notePassThroughButton;
	ComboBox switchVariationCombo;
	ComboBox quantizeVariationStartCombo;
	ComboBox quantizeRunStopCombo;
	TextButton saveButton;
	TextButton loadButton;
	TextEditor nameEditor;

	//////////////////////////////////////////////////////

	void setButtonStates() {
		// check which buttons to disable/enable
		// if no data in table, then delete is disbled and load/insert data disabled
		if (patternsTable.getNumRows() == 0) {
			insertPatternButton.setEnabled(false);
			duplicatePatternButton.setEnabled(false);
			deletePatternButton.setEnabled(false);
			overloadPatternButton.setEnabled(false);
		}
		else
		{
			if (patternsTable.getSelectedRow() < 0)
			{
				 // nothing selected
				insertPatternButton.setEnabled(false);
				duplicatePatternButton.setEnabled(false);
				deletePatternButton.setEnabled(false);
				overloadPatternButton.setEnabled(false);
			}
			else
			{
				insertPatternButton.setEnabled(true);
				duplicatePatternButton.setEnabled(true);
				deletePatternButton.setEnabled(true);	
				overloadPatternButton.setEnabled(true);
			}
				
		}
		
		if (patternsTable.getNumRows() == 8)
			newPatternButton.setEnabled(false);
		else newPatternButton.setEnabled(true);

		if (poolTable.getNumRows() == 0)
		{
			deletePoolButton.setEnabled(false);
		}
		else
		{
			deletePoolButton.setEnabled(true);
		}
	}

	//////////////////////////////////////////////////////

	void setSettings()
	{  
		// called when settings changes
		
		beatsModel->setWFFN(WFFNButton.getToggleState());
		beatsModel->setNotePassThrough(notePassThroughButton.getToggleState());
		beatsModel->setVariationStartQ(quantizeVariationStartCombo.getSelectedId());
		//beatsModel->setRunStartQ(quantizeRunStartCombo.getSelectedId());
		beatsModel->setRunStopQ(quantizeRunStopCombo.getSelectedId());
		beatsModel->setName(nameEditor.getText());
		beatsModel->setSwitchVariation(switchVariationCombo.getSelectedId());

	} // setSettings

	//////////////////////////////////////////////////////


	void getSettings()
	{  
		// called when settings change
		
		WFFNButton.setToggleState(beatsModel->getWFFN(), dontSendNotification);
		notePassThroughButton.setToggleState(beatsModel->getNotePassThrough(), dontSendNotification);
		quantizeVariationStartCombo.setSelectedId(beatsModel->getVariationStartQ(), dontSendNotification);
		//quantizeRunStartCombo.setSelectedId(beatsModel->getRunStartQ(), dontSendNotification);
		quantizeRunStopCombo.setSelectedId(beatsModel->getRunStopQ(), dontSendNotification);
		switchVariationCombo.setSelectedId(beatsModel->getSwitchVariation(), dontSendNotification);
		auto name = beatsModel->getName();
		nameEditor.setText(name, dontSendNotification);
		
		// disable when host overridden
		int b, n, d, bs;
		bool override, wf;
		beatsModel->getTransportState(b, n, d, bs, override, wf);
		
	} // getSettings
	
	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(TopiaryBeatsMasterComponent)
};

/////////////////////////////////////////////////////////////////////


