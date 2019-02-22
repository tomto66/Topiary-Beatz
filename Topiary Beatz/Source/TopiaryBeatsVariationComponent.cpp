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
#include "../JuceLibraryCode/JuceHeader.h"
#include "../../Topiary/Source/Topiary.h"
#include "TopiaryBeatsModel.h"
#include "TopiaryBeatsVariationComponent.h"


TopiaryBeatsVariationComponent::TopiaryBeatsVariationComponent()
{   // size set by paint!
	variation = 0;

	variationDefinitionComponent.setParent(this);
	addAndMakeVisible(variationDefinitionComponent);
	
	addAndMakeVisible(enablePoolComponent);
	enablePoolComponent.setParent(this);

	addAndMakeVisible(poolLengthComponent);
	poolLengthComponent.setParent(this);

	addAndMakeVisible(poolChannelComponent);
	poolChannelComponent.setParent(this);

	addAndMakeVisible(randomNoteComponent);
	randomNoteComponent.setParent(this);

	addAndMakeVisible(swingComponent);
	swingComponent.setParent(this);

	addAndMakeVisible(velocityComponent);
	velocityComponent.setParent(this);

	addAndMakeVisible(timingComponent);
	timingComponent.setParent(this);
	

} // TopiaryBeatsVariationComponent() 

/////////////////////////////////////////////////////////////////////////

TopiaryBeatsVariationComponent::~TopiaryBeatsVariationComponent()
{
	beatsModel->removeListener((ActionListener*)this);

} // ~TopiaryBeatsVariationComponent

/////////////////////////////////////////////////////////////////////////

void TopiaryBeatsVariationComponent::setModel(TopiaryBeatsModel* m)
{
	beatsModel = m;	
	beatsModel->setListener((ActionListener*)this);
	actionListenerCallback(MsgMaster); // need to call this so we can fill the patternCombo !!!
	
	variationDefinitionComponent.variationCombo.setSelectedId(1);  // this will trigger a call to getVariationDefinition which gets the data
	
} //setModel

/////////////////////////////////////////////////////////////////////////


void TopiaryBeatsVariationComponent::setVariationDefinition()
{
	// write to model
	if (beatsModel->getNumPatterns() == 0)
	{
		initializing = true; // don't do anything
		return;
	}
	
	if ((beatsModel->getNumPatterns() == 1) & (initializing==true))
	{
		initializing = false; // don't do anything but un-set initializing
	}

	
	bool enables[4];
	int poolLength[4][3][2];
	int poolChannel[4];
	

	enables[0] = enablePoolComponent.enableButton1.getToggleState();
	enables[1] = enablePoolComponent.enableButton2.getToggleState();
	enables[2] = enablePoolComponent.enableButton3.getToggleState();
	enables[3] = enablePoolComponent.enableButton4.getToggleState();


	if (variation != (variationDefinitionComponent.variationCombo.getSelectedId() - 1))
	{
		variation = variationDefinitionComponent.variationCombo.getSelectedId() - 1;
	}

	jassert(variation > -1);

	int selectedPatternId = variationDefinitionComponent.patternCombo.getSelectedId() -1;

	for (int j = 0; j < 4; j++)
	{
		poolLength[j][0][0] = poolLengthComponent.poolStart[j].measureEditor.getText().getIntValue();
		poolLength[j][0][1] = poolLengthComponent.poolStop[j].measureEditor.getText().getIntValue();
		poolLength[j][1][0] = poolLengthComponent.poolStart[j].beatEditor.getText().getIntValue();
		poolLength[j][1][1] = poolLengthComponent.poolStop[j].beatEditor.getText().getIntValue();
		poolLength[j][2][0] = poolLengthComponent.poolStart[j].tickEditor.getText().getIntValue();
		poolLength[j][2][1] = poolLengthComponent.poolStop[j].tickEditor.getText().getIntValue();

	}

	poolChannel[0] = poolChannelComponent.poolChannelEditor1.getText().getIntValue();
	poolChannel[1] = poolChannelComponent.poolChannelEditor2.getText().getIntValue();
	poolChannel[2] = poolChannelComponent.poolChannelEditor3.getText().getIntValue();
	poolChannel[3] = poolChannelComponent.poolChannelEditor4.getText().getIntValue();

	bool refused = beatsModel->validateVariationDefinition(variation, variationDefinitionComponent.enableButton.getToggleState(),  variationDefinitionComponent.nameEditor.getText(), selectedPatternId, poolLength);


	// channels are not validated in the model; because that validation does not depend on the model state

	if ((poolChannelComponent.poolChannelEditor1.getText().getIntValue() < 1) || (poolChannelComponent.poolChannelEditor1.getText().getIntValue() > 16))
	{
		beatsModel->Log("Output channel for pool 1 should be > 0 and < 16.", Topiary::LogType::Warning);
		refused = true;
	}
	if ((poolChannelComponent.poolChannelEditor2.getText().getIntValue() < 1) || (poolChannelComponent.poolChannelEditor2.getText().getIntValue() > 16))
	{
		beatsModel->Log("Output channel for pool 2 should be > 0 and < 16.", Topiary::LogType::Warning);
		refused = true;
	}
	if ((poolChannelComponent.poolChannelEditor3.getText().getIntValue() < 1) || (poolChannelComponent.poolChannelEditor3.getText().getIntValue() > 16))
	{
		beatsModel->Log("Output channel for pool 3 should be > 0 and < 16.", Topiary::LogType::Warning);
		refused = true;
	}
	if ((poolChannelComponent.poolChannelEditor4.getText().getIntValue() < 1) || (poolChannelComponent.poolChannelEditor4.getText().getIntValue() > 16))
	{
		beatsModel->Log("Output channel for pool 4 should be > 0 and < 16.", Topiary::LogType::Warning);
		refused = true;
	}

	if (!refused)
	{
		beatsModel->setVariationDefinition(variation, variationDefinitionComponent.enableButton.getToggleState(), variationDefinitionComponent.nameEditor.getText(), selectedPatternId, enables, poolLength, poolChannel);  //// TODO last parameter
		beatsModel->Log("Variation "+String(variation+1)+" saved.", Topiary::LogType::Info);
	}
	else
		variationDefinitionComponent.enableButton.setToggleState(false, dontSendNotification);

	setRandomizeNotes();
	setSwing();
	setVelocity();
	setTiming();

	initializing = false;

}  // setVariationDefinition

/////////////////////////////////////////////////////////////////////////

void TopiaryBeatsVariationComponent::getVariationDefinition()
{
	// get from model

	if (variation != (variationDefinitionComponent.variationCombo.getSelectedId() - 1) )
	{
		variation = variationDefinitionComponent.variationCombo.getSelectedId() - 1;
		if (variation != -1 && !getVariationCalledFromChangeInVariationButton)  // because otherwise the variation is already set in the editor
			beatsModel->setVariation(variation); // so that the variation buttons follow
	}

	if (variation <0 ) return;  // this should never happen, except when initializing

	bool enable;
	String vname;
	int patternId;

	bool enables[4];
	int poolLength[4][3][2];
	int poolChannel[4];

	beatsModel->getVariationDefinition(variation, enable, vname, patternId, enables, poolLength, poolChannel);
	variationDefinitionComponent.enableButton.setToggleState(enable, dontSendNotification);
	variationDefinitionComponent.nameEditor.setText(vname, dontSendNotification);

	enablePoolComponent.enableButton1.setToggleState(enables[0], dontSendNotification);
	enablePoolComponent.enableButton2.setToggleState(enables[1], dontSendNotification);
	enablePoolComponent.enableButton3.setToggleState(enables[2], dontSendNotification);
	enablePoolComponent.enableButton4.setToggleState(enables[3], dontSendNotification);

	poolChannelComponent.poolChannelEditor1.setText(String(poolChannel[0]));
	poolChannelComponent.poolChannelEditor2.setText(String(poolChannel[1]));
	poolChannelComponent.poolChannelEditor3.setText(String(poolChannel[2]));
	poolChannelComponent.poolChannelEditor4.setText(String(poolChannel[3]));

	for (int j = 0; j < 4; j++)
	{
		poolLengthComponent.poolStart[j].measureEditor.setText(String(poolLength[j][0][0]));
		poolLengthComponent.poolStop[j].measureEditor.setText(String(poolLength[j][0][1]));
		poolLengthComponent.poolStart[j].beatEditor.setText(String(poolLength[j][1][0]));
		poolLengthComponent.poolStop[j].beatEditor.setText(String(poolLength[j][1][1]));
		poolLengthComponent.poolStart[j].tickEditor.setText(String(poolLength[j][2][0]));
		poolLengthComponent.poolStop[j].tickEditor.setText(String(poolLength[j][2][1]));
	}

	// find the appropriate item in patternCombo, and select it; if nothing sel = variationDefinitionComponent.patternCombo.getSelectedId();
	int items = variationDefinitionComponent.patternCombo.getNumItems();
	
	int selectedId;
	bool found = false;
	if (patternId >= 0) // do nothing if it was -1 - should be set to 0 below as found will stay false
	{
		for (int i = 0; i < items; i++)
		{
			// careful: item (i=0) will have ID 1!!!
			selectedId = i + 1;  
			if ((patternId+1) == selectedId) // because patternID goes 0-7 and the items in the combo are 1 higher!!!
			{
				found = true;
				if (variationDefinitionComponent.patternCombo.getSelectedId() != selectedId)
					variationDefinitionComponent.patternCombo.setSelectedId(selectedId, dontSendNotification);
				
			}
		}
	}
	
	if (!found)
	{
		variationDefinitionComponent.patternCombo.setSelectedId(0);
	}

	getRandomizeNotes();
	getSwing();
	getVelocity();
	getTiming();

} // getVariationDefinition

/////////////////////////////////////////////////////////////////////////

void TopiaryBeatsVariationComponent::getRandomizeNotes()
{
	// get note randomisation from model
	bool enable;
	bool pool[4];
	int value;

	beatsModel->getRandomizeNotes(variationDefinitionComponent.variationCombo.getSelectedId()-1, enable, pool, value);

	randomNoteComponent.onButton.setToggleState(enable, dontSendNotification);
	randomNoteComponent.pool1Button.setToggleState(pool[0], dontSendNotification);
	randomNoteComponent.pool2Button.setToggleState(pool[1], dontSendNotification);
	randomNoteComponent.pool3Button.setToggleState(pool[2], dontSendNotification);
	randomNoteComponent.pool4Button.setToggleState(pool[3], dontSendNotification);
	randomNoteComponent.percentSlider.setValue(value, dontSendNotification);

} // getRandomizeNotes

/////////////////////////////////////////////////////////////////////////

void TopiaryBeatsVariationComponent::setRandomizeNotes()
{
	// set note randomisation to model

	bool pool[4];

	pool[0] = randomNoteComponent.pool1Button.getToggleState();
	pool[1] = randomNoteComponent.pool2Button.getToggleState();
	pool[2] = randomNoteComponent.pool3Button.getToggleState();
	pool[3] = randomNoteComponent.pool4Button.getToggleState();

	beatsModel->setRandomizeNotes(variationDefinitionComponent.variationCombo.getSelectedId()-1, randomNoteComponent.onButton.getToggleState(), pool, (int) randomNoteComponent.percentSlider.getValue());

} // setRandomizeNotes

/////////////////////////////////////////////////////////////////////////

void TopiaryBeatsVariationComponent::setSwing()
{
	// set note randomisation to model

	bool pool[4];

	pool[0] = swingComponent.pool1Button.getToggleState();
	pool[1] = swingComponent.pool2Button.getToggleState();
	pool[2] = swingComponent.pool3Button.getToggleState();
	pool[3] = swingComponent.pool4Button.getToggleState();

	beatsModel->setSwing(variationDefinitionComponent.variationCombo.getSelectedId() - 1, swingComponent.onButton.getToggleState(), pool, (int)swingComponent.swingSlider.getValue());

} // setSwing

/////////////////////////////////////////////////////////////////////////

void TopiaryBeatsVariationComponent::getSwing()
{
	// get note randomisation from model
	bool enable;
	bool pool[4];
	int value;

	beatsModel->getSwing(variationDefinitionComponent.variationCombo.getSelectedId() - 1, enable, pool, value);

	swingComponent.onButton.setToggleState(enable, dontSendNotification);
	swingComponent.pool1Button.setToggleState(pool[0], dontSendNotification);
	swingComponent.pool2Button.setToggleState(pool[1], dontSendNotification);
	swingComponent.pool3Button.setToggleState(pool[2], dontSendNotification);
	swingComponent.pool4Button.setToggleState(pool[3], dontSendNotification);
	swingComponent.swingSlider.setValue(value, dontSendNotification);

} // getSwing

/////////////////////////////////////////////////////////////////////////

void TopiaryBeatsVariationComponent::setTiming()
{
	// set note randomisation to model

	bool pool[4];

	pool[0] = timingComponent.pool1Button.getToggleState();
	pool[1] = timingComponent.pool2Button.getToggleState();
	pool[2] = timingComponent.pool3Button.getToggleState();
	pool[3] = timingComponent.pool4Button.getToggleState();

	beatsModel->setRandomizeTiming(variationDefinitionComponent.variationCombo.getSelectedId() - 1, timingComponent.onButton.getToggleState(), pool, 
									(int)timingComponent.timingSlider.getValue(),
									timingComponent.plusButton.getToggleState(),
									timingComponent.minusButton.getToggleState());

} // setTiming

/////////////////////////////////////////////////////////////////////////

void TopiaryBeatsVariationComponent::getTiming()
{
	// get note randomisation from model
	bool enable;
	bool pool[4], plus, min;
	int value;

	beatsModel->getRandomizeTiming(variationDefinitionComponent.variationCombo.getSelectedId() - 1, enable, pool, value, plus, min);

	timingComponent.onButton.setToggleState(enable, dontSendNotification);
	timingComponent.pool1Button.setToggleState(pool[0], dontSendNotification);
	timingComponent.pool2Button.setToggleState(pool[1], dontSendNotification);
	timingComponent.pool3Button.setToggleState(pool[2], dontSendNotification);
	timingComponent.pool4Button.setToggleState(pool[3], dontSendNotification);
	timingComponent.timingSlider.setValue(value, dontSendNotification);
	timingComponent.plusButton.setToggleState(plus, dontSendNotification);
	timingComponent.minusButton.setToggleState(min, dontSendNotification);

} // getTiming


/////////////////////////////////////////////////////////////////////////

void TopiaryBeatsVariationComponent::setVelocity()
{
	// set note randomisation to model

	bool pool[4];

	pool[0] = velocityComponent.pool1Button.getToggleState();
	pool[1] = velocityComponent.pool2Button.getToggleState();
	pool[2] = velocityComponent.pool3Button.getToggleState();
	pool[3] = velocityComponent.pool4Button.getToggleState();

	beatsModel->setRandomizeVelocity(variationDefinitionComponent.variationCombo.getSelectedId() - 1, velocityComponent.onButton.getToggleState(), pool, 
									(int)velocityComponent.velocitySlider.getValue(),
									velocityComponent.plusButton.getToggleState(),
									velocityComponent.minusButton.getToggleState() 	);

} // setVelocity

/////////////////////////////////////////////////////////////////////////

void TopiaryBeatsVariationComponent::getVelocity()
{
	// get note randomisation from model
	bool enable;
	bool pool[4], plus, min;
	int value;

	beatsModel->getRandomizeVelocity (variationDefinitionComponent.variationCombo.getSelectedId() - 1, enable, pool, value, plus, min);

	velocityComponent.onButton.setToggleState(enable, dontSendNotification);
	velocityComponent.pool1Button.setToggleState(pool[0], dontSendNotification);
	velocityComponent.pool2Button.setToggleState(pool[1], dontSendNotification);
	velocityComponent.pool3Button.setToggleState(pool[2], dontSendNotification);
	velocityComponent.pool4Button.setToggleState(pool[3], dontSendNotification);
	velocityComponent.velocitySlider.setValue(value, dontSendNotification);
	velocityComponent.plusButton.setToggleState(plus, dontSendNotification);
	velocityComponent.minusButton.setToggleState(min, dontSendNotification);

} // getVelocity

/////////////////////////////////////////////////////////////////////////

void  TopiaryBeatsVariationComponent::actionListenerCallback(const String &message)
{
	if (message.compare(MsgVariationDefinition) == 0)
	{
		getVariationDefinition();  
		// be sure that the mastertables are read first so the patternCombo is set correctly !!!
	}
	else if (message.compare(MsgMaster) == 0)
	{
		// fill the patternCombo; careful, there may already be stuff there so clear it and then set it back where it was
		int remember = variationDefinitionComponent.patternCombo.getSelectedId();

		variationDefinitionComponent.patternCombo.clear();

		String patterns[8];
		for (int i = 0; i < 8; i++) patterns[i] = "";

		beatsModel->getPatterns(patterns);
		//int i = 0;
		//while (patterns[i].compare("") && (i<8))
		for (int i=0; i<8; i++)
		{
			if (patterns[i].compare(""))
				variationDefinitionComponent.patternCombo.addItem(patterns[i], i+1);		
		}

				// if there are no patterns, disable yourself!!!
		if (beatsModel->getNumPatterns()>0)
			this->setEnabled(true);
		else
			this->setEnabled(false);

		if (remember <= variationDefinitionComponent.patternCombo.getNumItems())
			variationDefinitionComponent.patternCombo.setSelectedId(remember, dontSendNotification);
		else
			variationDefinitionComponent.patternCombo.setSelectedId(1, dontSendNotification);
	}
	else if (message.compare(MsgVariationSelected) == 0)
	{
		// set the combo to the selected variation
		int unused, newVariation;
		beatsModel->getVariation(newVariation, unused);
		if (variation != newVariation)
		{
			variationDefinitionComponent.variationCombo.setSelectedId(newVariation + 1, dontSendNotification);
			getVariationCalledFromChangeInVariationButton = true;
			getVariationDefinition();
			getVariationCalledFromChangeInVariationButton = false;
			UNUSED(unused)
		}
	}

}  // actionListenerCallback

/////////////////////////////////////////////////////////////////////////

void TopiaryBeatsVariationComponent::paint(Graphics& g)
{
	UNUSED(g)
	
	variationDefinitionComponent.setBounds(10, 10, variationDefinitionComponent.width, variationDefinitionComponent.heigth);
	enablePoolComponent.setBounds(10, 150, enablePoolComponent.width, enablePoolComponent.heigth);
	poolLengthComponent.setBounds(160, 10, poolLengthComponent.width, poolLengthComponent.heigth);
	poolChannelComponent.setBounds(10, 270, poolChannelComponent.width, poolChannelComponent.heigth);
	randomNoteComponent.setBounds(350, 10, randomNoteComponent.width, randomNoteComponent.heigth);
	swingComponent.setBounds(450, 10, swingComponent.width, swingComponent.heigth);
	velocityComponent.setBounds(550, 10, velocityComponent.width, velocityComponent.heigth);
	timingComponent.setBounds(650, 10, timingComponent.width, timingComponent.heigth);
} // paint

//////////////////////////////////////////////////

void TopiaryBeatsVariationComponent::resized()
{
} // resized

//////////////////////////////////////////////////


