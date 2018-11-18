/////////////////////////////////////////////////////////////////////////
// TopiaryBeatsMasterComponent.h
// 30Sep18 Tom Tollenaere - Created
//
// Header for TopiaryBeatsMasterComponent
/////////////////////////////////////////////////////////////////////////////


#pragma once
#include"../../Topiary/TopiaryTable.h"  
#include"TopiaryBeatsModel.h"

class TopiaryBeatsMasterComponent : public Component
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

	// patterns stuff
	TopiaryTable patternsTable;
	XmlElement *patternListHeader = nullptr;
	XmlElement *patternListData = nullptr;
	TextButton insertPatternButton;
	TextButton duplicatePatternButton;
	TextButton deletePatternButton;
	TextButton newPatternButton;
	int buttonH = 20;
	int buttonW = 100;
	int patternButtonOffsetX = 280;

	int patternTW = 253;
	int patternTH = 240;

	TextButton GMDrumMapButton;

	// pool stuff
	TopiaryTable poolTable;
	XmlElement *poolListHeader = nullptr;
	XmlElement *poolListData = nullptr;
	
	TextButton deletePoolButton;
	TextButton newPoolButton;

	int poolTW = 350;
	int poolTH = 240;

	// settings stuff

	TopiaryButton WFFNButton;
	TopiaryButton notePassThroughButton;
	ComboBox switchVariationCombo;
	ComboBox quantizeVariationStartCombo;
	ComboBox quantizeRunStartCombo;
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
			
		}
		else
		{
			if (patternsTable.getSelectedRow() < 0)
			{
				 // nothing selected
				insertPatternButton.setEnabled(false);
				duplicatePatternButton.setEnabled(false);
				deletePatternButton.setEnabled(false);			
			}
			else
			{
				insertPatternButton.setEnabled(true);
				duplicatePatternButton.setEnabled(true);
				deletePatternButton.setEnabled(true);	
			}
				
		}
		
		if (patternsTable.getNumRows() == 8)
			newPatternButton.setEnabled(false);
		else newPatternButton.setEnabled(true);
	}

	//////////////////////////////////////////////////////

	void setSettings()
	{  
		// called when settings changes
		
		beatsModel->setWFFN(WFFNButton.getToggleState());
		beatsModel->setNotePassThrough(notePassThroughButton.getToggleState());
		beatsModel->setVariationStartQ(quantizeVariationStartCombo.getSelectedId());
		beatsModel->setRunStartQ(quantizeRunStartCombo.getSelectedId());
		beatsModel->setRunStopQ(quantizeRunStopCombo.getSelectedId());
		beatsModel->setName(nameEditor.getText());
		beatsModel->setSwitchVariation(switchVariationCombo.getSelectedId());

	} // setSettings

	//////////////////////////////////////////////////////


	void getSettings()
	{  // called when settings changes
		
		WFFNButton.setToggleState(beatsModel->getWFFN(), dontSendNotification);
		notePassThroughButton.setToggleState(beatsModel->getNotePassThrough(), dontSendNotification);
		quantizeVariationStartCombo.setSelectedId(beatsModel->getVariationStartQ(), dontSendNotification);
		quantizeRunStartCombo.setSelectedId(beatsModel->getRunStartQ(), dontSendNotification);
		quantizeRunStopCombo.setSelectedId(beatsModel->getRunStopQ(), dontSendNotification);
		switchVariationCombo.setSelectedId(beatsModel->getSwitchVariation(), dontSendNotification);
		nameEditor.setText(beatsModel->getName(), dontSendNotification);
		//allwaysRestartButton.setToggleState(beatsModel->getAlwaysRestart(), dontSendNotification);

		// disable when host overridden
		
			int b, n, d, bs;
			bool override, wf;
			beatsModel->getTransportState(b, n, d, bs, override, wf, false);
			if (override)
			{
				quantizeRunStartCombo.setEnabled(false);
			}
			else
			{	
				quantizeRunStartCombo.setEnabled(true);
			}
		

	} // getSettings
	
	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(TopiaryBeatsMasterComponent)
};

/////////////////////////////////////////////////////////////////////


