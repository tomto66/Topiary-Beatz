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
#include"../../Topiary/TopiaryTable.h"  // temp for testing
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
	void actionListenerCallback(const String &message) override;
	TopiaryBeatsModel* beatsModel;

private:
	   
	//RandomNoteComponent randomNoteComponent;
	VariationDefinitionComponent variationDefinitionComponent;
	EnablePoolComponent enablePoolComponent;
	PoolLengthComponent poolLengthComponent;
	PoolChannelComponent poolChannelComponent;

	int variation; // the one we are working on; 0-7
	
	static const int buttonH = 20;
	static const int sliderH = 80;
	static const int sliderV = 80;
	static const int sliderTW = 60;
	static const int sliderTH = 20;

	bool initializing = true; // to avoid unneeded warnings @init. set to true when get/set detects numPatterns = 0; set to false as soons as a patternId of 1 gets written.

	//////////////////////////////////////////////////////

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(TopiaryBeatsVariationComponent)
};
