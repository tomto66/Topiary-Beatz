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
#include "TopiaryBeatsModel.h"
#include "TopiaryBeatsMasterComponent.h"


TopiaryBeatsMasterComponent::TopiaryBeatsMasterComponent()
{   // size set by TabbedComponent!

	////////////////////////////////////////
	// Pattern stuff
	////////////////////////////////////////

	// Table for patterns
	patternsTable.setSize(patternTW, patternTH);
	addAndMakeVisible(patternsTable);

	//  Insert Pattern button
	insertPatternButton.setSize(buttonW, buttonH);
	addAndMakeVisible(insertPatternButton);
	insertPatternButton.setButtonText("Load MIDI file");
	insertPatternButton.onClick = [this] {
		auto selection = patternsTable.getSelectedRow();
		jassert(selection >= 0);

		beatsModel->insertPatternFromFile(selection);
		
		beatsModel->sendActionMessage(MsgMaster); // tables resort the data!
	};

	// Duplicate Pattern button
	duplicatePatternButton.setSize(buttonW, buttonH);
	addAndMakeVisible(duplicatePatternButton);
	duplicatePatternButton.setButtonText("Duplicate pattern");
	duplicatePatternButton.onClick = [this] {

		auto selection = patternsTable.getSelectedRow();
		jassert(selection >= 0);

		//beatsModel->duplicatePattern(selection);
		//patternsTable.updateContent();
		
		beatsModel->sendActionMessage(MsgMaster); // tables resort the data!
	};

	// Delete Pattern button
	deletePatternButton.setSize(buttonW, buttonH);
	addAndMakeVisible(deletePatternButton);
	deletePatternButton.setButtonText("Delete pattern");
	deletePatternButton.onClick = [this] {

		auto selection = patternsTable.getSelectedRow();
		jassert(selection >= 0);
		
		beatsModel->deletePattern(selection);
		patternsTable.updateContent();
		//patternsTable.selectRow(1);   // select the first row
		//setButtonStates();
		beatsModel->sendActionMessage(MsgMaster); // tables resort the data!
	};


	// Add Pattern button
	newPatternButton.setSize(buttonW, buttonH);
	addAndMakeVisible(newPatternButton);
	newPatternButton.setButtonText("Add pattern");
	newPatternButton.onClick = [this] {
		beatsModel->addPattern();
		patternsTable.updateContent();
		//patternsTable.selectRow(beatsModel->getNumPatterns() - 1);   // select the new row
		//setButtonStates();
		beatsModel->sendActionMessage(MsgMaster); // tables resort the data!
		patternsTable.selectRow(beatsModel->getNumPatterns()-1);   // select the new row
	};


	////////////////////////////////////////
	// Pool stuff
	////////////////////////////////////////

	// Table for pool notes
	poolTable.setSize(poolTW, poolTH);
	addAndMakeVisible(poolTable);

	// Delete Pattern button
	deletePoolButton.setSize(buttonW, buttonH);
	addAndMakeVisible(deletePoolButton);
	deletePoolButton.setButtonText("Delete note");
	deletePoolButton.onClick = [this] {

		auto selection = poolTable.getSelectedRow();
		jassert(selection > -1);
		
		beatsModel->deletePoolNote(selection);
		beatsModel->generateAllVariations();
		beatsModel->sendActionMessage(MsgMaster); // tables resort the data!
		poolTable.updateContent();
		poolTable.selectRow(0);   // select the first row
	};


	// Add Pattern button
	newPoolButton.setSize(buttonW, buttonH);
	addAndMakeVisible(newPoolButton);
	newPoolButton.setButtonText("Add note");
	newPoolButton.onClick = [this] {
		beatsModel->addPoolNote();
		beatsModel->sendActionMessage(MsgMaster); // tables resort the data!
		poolTable.selectRow(beatsModel->getNumPatterns()-1 );   // select the new row
	};

	// Regenerate Button
	regeneratePoolButton.setSize(buttonW, buttonH);
	addAndMakeVisible(regeneratePoolButton);
	regeneratePoolButton.setButtonText("Rebuild");
	regeneratePoolButton.onClick = [this] {
		beatsModel->rebuildPool(false);
		beatsModel->generateAllVariations();
		beatsModel->sendActionMessage(MsgMaster); // tables resort the data!
	};

	GMDrumMapButton.setSize(buttonW, buttonH);
	GMDrumMapButton.setButtonText("GM Drum Map");
	GMDrumMapButton.onClick = [this] {
		beatsModel->setGMDrumMapLabels();
		poolTable.updateContent();
	};
	addAndMakeVisible(GMDrumMapButton);

	// Clean Pool Button
	cleanPoolButton.setSize(buttonW, buttonH);
	addAndMakeVisible(cleanPoolButton);
	cleanPoolButton.setButtonText("Clean");
	cleanPoolButton.onClick = [this] {
		beatsModel->rebuildPool(true); // true makes it clean!
		beatsModel->generateAllVariations();
		beatsModel->sendActionMessage(MsgMaster); // tables resort the data!
	};

	setButtonStates();

	/////////////////////////////////
	// Settings
	/////////////////////////////////

	// WFFNButton

	WFFNButton.setClickingTogglesState(true);
	WFFNButton.setLookAndFeel(&topiaryLookAndFeel);
	WFFNButton.setSize(100, buttonH);
	addAndMakeVisible(WFFNButton);
	WFFNButton.setButtonText("WFFN");
	WFFNButton.onClick = [this]
	{
		setSettings();
	};

	// notePassThoughButton

	notePassThroughButton.setClickingTogglesState(true);
	notePassThroughButton.setLookAndFeel(&topiaryLookAndFeel);
	notePassThroughButton.setSize(100, buttonH);
	addAndMakeVisible(notePassThroughButton);
	notePassThroughButton.setButtonText("Echo MIDI");
	notePassThroughButton.onClick = [this]
	{
		setSettings();
	};

	// quantizeVariationStartCombo;

	quantizeVariationStartCombo.setSize(100, buttonH);
	addAndMakeVisible(quantizeVariationStartCombo);
	quantizeVariationStartCombo.setSize(100, buttonH);
	quantizeVariationStartCombo.addItem("Immediate", Topiary::Immediate);
	//quantizeVariationStartCombo.addItem("Whole variation", TopiaryBeatsModel::WholePattern);
	quantizeVariationStartCombo.addItem("Measure", Topiary::Measure);
	//quantizeVariationStartCombo.addItem("1/2", Topiary::Half);
	//quantizeVariationStartCombo.addItem("1/3", 4);
	quantizeVariationStartCombo.addItem("Beat", Topiary::Quarter);
	//quantizeVariationStartCombo.addItem("1/8", 6);
	//quantizeVariationStartCombo.addItem("1/16", 7);
	//quantizeVariationStartCombo.addItem("1/32", 8);

	quantizeVariationStartCombo.onChange = [this]
	{
		setSettings();
	};

	// quantizeRunStop

	quantizeRunStopCombo.setSize(100, buttonH);
	addAndMakeVisible(quantizeRunStopCombo);
	quantizeRunStopCombo.setSize(100, buttonH);
	quantizeRunStopCombo.addItem("Immediate", 100);
	quantizeRunStopCombo.addItem("Whole variation", Topiary::WholePattern);
	quantizeRunStopCombo.addItem("Measure", 1);
	quantizeRunStopCombo.addItem("1/2", 3);
	//quantizeRunStopCombo.addItem("1/3", 4);
	quantizeRunStopCombo.addItem("1/4", 5);
	//quantizeRunStopCombo.addItem("1/8", 6);
	//quantizeRunStopCombo.addItem("1/16", 7);
	//quantizeRunStopCombo.addItem("1/32", 8);

	quantizeRunStopCombo.onChange = [this]
	{
		setSettings();
	};

	addAndMakeVisible(saveButton);
	saveButton.setSize(100, buttonH);
	saveButton.setButtonText("Save");
	saveButton.onClick = [this]
	{
		
		beatsModel->savePreset("Please select Topiay Beats file to load...", "*.tbe" );
	};
	
	addAndMakeVisible(loadButton);
	loadButton.setSize(100, buttonH);
	loadButton.setButtonText("Load");
	loadButton.onClick = [this]
	{		
		beatsModel->loadPreset("Please select Topiary Beats file to load...", "*.tbe");
		//setModel(beatsModel); // does this make sense? NO it doesn't - replaced by actionlistener for MsgLoad
		actionListenerCallback(MsgLoad);
		getSettings();
		patternsTable.updateContent();
		poolTable.updateContent();
		setButtonStates();
	};

	addAndMakeVisible(nameEditor);
	nameEditor.setSize(210, buttonH);
	
	nameEditor.onReturnKey = [this]
	{
		setSettings();
	};
	nameEditor.onFocusLost = [this]
	{
		setSettings();
	};
	

	// switchVariationCombo

	switchVariationCombo.setSize(150, buttonH);
	addAndMakeVisible(switchVariationCombo);
	switchVariationCombo.addItem("Switch from start", 1);
	switchVariationCombo.addItem("Switch within beat", 2);
	switchVariationCombo.addItem("Switch within measure", 3);
	switchVariationCombo.addItem("Switch within pattern", 4);
	
	switchVariationCombo.onChange = [this]
	{
		setSettings();
	};	

} // TopiaryBeatsMasterComponent() 

/////////////////////////////////////////////////////////////////////////

TopiaryBeatsMasterComponent::~TopiaryBeatsMasterComponent()
{
	beatsModel->removeListener((ActionListener*)this);
} // ~TopiaryBeatsMasterComponent

/////////////////////////////////////////////////////////////////////////

void TopiaryBeatsMasterComponent::setModel(TopiaryBeatsModel* m)
{
	beatsModel = m;
	beatsModel->getMasterModel(&patternListHeader, &patternListData, &poolListHeader, &poolListData);

	patternsTable.setDataLists(patternListHeader, patternListData);
	patternsTable.setBroadcaster(beatsModel->getBroadcaster(), MsgMaster);

	poolTable.setDataLists(poolListHeader, poolListData);
	poolTable.setModel(beatsModel); // because it will have to validate note data!
	beatsModel->setListener((ActionListener*)this);

	// trick to call the model and read 
	actionListenerCallback(MsgMaster);
	actionListenerCallback(MsgTransport);

}

/////////////////////////////////////////////////////////////////////////

void TopiaryBeatsMasterComponent::paint(Graphics& g)
{
	int lineSize = 2;
	int labelOffset = 15;
	
	g.fillAll(TopiaryColour::background);
	g.setColour(TopiaryColour::foreground);
	g.setFont(12.0f);

	//// Patterns

	g.drawText("Patterns", 10, 10, 200, labelOffset, juce::Justification::centredLeft);
	g.drawRoundedRectangle((float)lineSize + 10, (float)labelOffset + 10, (float)patternButtonOffsetX + buttonW + 20, (float)patternTH + (2 * lineSize + 5), (float)4, (float)lineSize);

	patternsTable.setBounds(20, 30, patternTW - 5, patternTH);
	insertPatternButton.setBounds(patternButtonOffsetX, 40, buttonW, buttonH);
	newPatternButton.setBounds(patternButtonOffsetX, 70, buttonW, buttonH);
	deletePatternButton.setBounds(patternButtonOffsetX, 100, buttonW, buttonH);
	duplicatePatternButton.setBounds(patternButtonOffsetX, 130, buttonW, buttonH);

	//// Pool

	g.drawText("Note Pool", 440, 10, 200, labelOffset, juce::Justification::centredLeft);
	g.drawRoundedRectangle((float)lineSize + 440, (float)labelOffset + 10, (float)545, (float)patternTH + (2 * lineSize + 5), (float)4, (float)lineSize);
	poolTable.setBounds(455, 30, poolTW - 5, poolTH);

	cleanPoolButton.setBounds(patternButtonOffsetX + 450, 40, buttonW, buttonH);
	newPoolButton.setBounds(patternButtonOffsetX + 450, 70, buttonW, buttonH);
	deletePoolButton.setBounds(patternButtonOffsetX + 450, 100, buttonW, buttonH);
	GMDrumMapButton.setBounds(patternButtonOffsetX + 450, 130, buttonW, buttonH);
	regeneratePoolButton.setBounds(patternButtonOffsetX + 450, 160, buttonW, buttonH);
	
	//// Settings

	g.drawText("Settings", 10, 290, 800, labelOffset, juce::Justification::centredLeft);
	g.drawRoundedRectangle((float)lineSize + 10, (float)305, (float)975, (float)60, (float)4, (float)lineSize);

	WFFNButton.setBounds(20, 310, 100, buttonH);

	notePassThroughButton.setBounds(130, 310, 100, buttonH);

	g.setColour(Colours::white);
	//g.drawText("Q Run Start:", 240, 310, 100, labelOffset + 3, juce::Justification::centredLeft);
	//quantizeRunStartCombo.setBounds(310, 310, 100, buttonH);

	g.drawText("Q Variation Start:", 250, 310, 200, labelOffset + 3, juce::Justification::centredLeft);
	quantizeVariationStartCombo.setBounds(345, 310, 110, buttonH);

	g.drawText("Q Variation End:", 470, 310, 200, labelOffset + 3, juce::Justification::centredLeft);
	quantizeRunStopCombo.setBounds(555, 310, 100, buttonH);

	g.drawText("Q Variation Switch:", 675, 310, 200, labelOffset + 3, juce::Justification::centredLeft);
	switchVariationCombo.setBounds(780, 310, 150, buttonH);
	
	nameEditor.setBounds(20, 340, 210, buttonH);
	saveButton.setBounds(250, 340,100,buttonH);   
	loadButton.setBounds(360, 340, 100, buttonH);
	
} // paint

void TopiaryBeatsMasterComponent::resized()
{
} // resized

///////////////////////////////////////////////////////////////////////////

void TopiaryBeatsMasterComponent::actionListenerCallback(const String &message)
{
	if (message.compare(MsgLoad) == 0)
	{
		// refresh the tables because data adresses may have changed
		beatsModel->getMasterModel(&patternListHeader, &patternListData, &poolListHeader, &poolListData);
		patternsTable.setDataLists(patternListHeader, patternListData);
		poolTable.setDataLists(poolListHeader, poolListData);
	}

	else if (message.compare(MsgMaster) == 0)
	{
		// trigger update of pooltable & masterTable
		int remember = patternsTable.getSelectedRow();
		patternsTable.updateContent();
		
		if ((remember <= patternsTable.getNumRows()) && (remember >= 0))
		{
			patternsTable.selectRow(0);  // needed otherwise the number of measures is not correctly updated (strange)
			patternsTable.selectRow(remember);
		}
		else
			patternsTable.selectRow(0);

		poolTable.updateContent();
		poolTable.selectRow(0);
		setButtonStates();
	}
	if (message.compare(MsgTransport) == 0)
		getSettings();
} // actionListenerCallback

///////////////////////////////////////////////////////////////////////////