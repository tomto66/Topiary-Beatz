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
#include "TopiaryBeatsVariationChildren.h"

class TopiaryBeatsVariationComponent : public Component, ActionListener
{
public:
	TopiaryBeatsVariationComponent();
	~TopiaryBeatsVariationComponent();

	void paint(Graphics&) override;
	void resized() override;
	void setModel(TopiaryBeatsModel* m);
	void setVariationDefinition();  // write to model
	void getVariationDefinition();  // get data from model
	void getRandomizeNotes();
	void setRandomizeNotes();
	void setSwing();
	void getSwing();
	void setVelocity();
	void getVelocity();
	void getTiming();
	void setTiming();
	void setVelocityOffset();
	void getVelocityOffset();
	void setSwingQ();
	void getSwingQ();

	void actionListenerCallback(const String &message) override;
	TopiaryBeatsModel* beatsModel;

private:
	   
	
	VariationDefinitionComponent variationDefinitionComponent;
	EnablePoolComponent enablePoolComponent;
	VariationTypeComponent variationTypeComponent;
	PoolChannelComponent poolChannelComponent;
	RandomNoteComponent randomNoteComponent;
	SwingComponent swingComponent;
	VelocityComponent velocityComponent;
	TimingComponent timingComponent;
	VelocityOffsetComponent velocityOffsetComponent;
	SwingQComponent swingQComponent;

	int variation; // the one we are working on; 0-7
	
	bool initializing = true; // to avoid unneeded warnings @init. Set to true when get/set detects numPatterns = 0; set to false as soons as a patternId of 1 gets written.
	bool getVariationCalledFromChangeInVariationButton = false;

	//////////////////////////////////////////////////////

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(TopiaryBeatsVariationComponent)
};
