/////////////////////////////////////////////////////////////////////////////
/*
This file is part of Topiary Beatz, Copyright Tom Tollenaere 2018-20.

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
#include "TopiaryBeatsMasterChildren.h"
#include "../../Topiary/Source/Components/TopiaryTable.h"

class TopiaryBeatsMasterComponent : public Component, ActionListener
{
public:
	TopiaryBeatsMasterComponent();
	~TopiaryBeatsMasterComponent();

	void paint(Graphics&) override;
	void resized() override;
	void setModel(TopiaryBeatsModel* m);
	void setSettings();
	void loadPreset();
	void savePreset();

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
	TopiaryButton auditionButton;

	int poolTW = 249;
	int poolTH = 240;


	// settings stuff
	SettingComponent settingComponent;

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
			auditionButton.setEnabled(false);
		}
		else
		{
			deletePoolButton.setEnabled(true);
			auditionButton.setEnabled(true);
		}
	}

	//////////////////////////////////////////////////////

	void getSettings()
	{  
		// called when settings change
		
		settingComponent.WFFNButton.setToggleState(beatsModel->getWFFN(), dontSendNotification);
		settingComponent.notePassThroughButton.setToggleState(beatsModel->getNotePassThrough(), dontSendNotification);
		settingComponent.quantizeVariationStartCombo.setSelectedId(beatsModel->getVariationStartQ(), dontSendNotification);
		settingComponent.quantizeRunStopCombo.setSelectedId(beatsModel->getRunStopQ(), dontSendNotification);
		settingComponent.switchVariationCombo.setSelectedId(beatsModel->getSwitchVariation(), dontSendNotification);
		settingComponent.forceOutputChannelButton.setToggleState(beatsModel->getFixedOutputChannels(), dontSendNotification);
		auto name = beatsModel->getName();
		settingComponent.nameEditor.setText(name, dontSendNotification);
		
		// disable when host overridden
		int b, n, d, bs;
		bool override, wf;
		beatsModel->getTransportState(b, n, d, bs, override, wf);
		
	} // getSettings
	
	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(TopiaryBeatsMasterComponent)
};

/////////////////////////////////////////////////////////////////////


