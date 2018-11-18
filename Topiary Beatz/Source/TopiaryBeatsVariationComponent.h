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

class TopiaryBeatsVariationComponent : public Component
{
public:
	TopiaryBeatsVariationComponent();
	~TopiaryBeatsVariationComponent();

	void paint(Graphics&) override;
	void resized() override;
	void setModel(TopiaryBeatsModel* m);

	

private:

	TopiaryBeatsModel* beatsModel;
	TopiaryLookAndFeel topiaryLookAndFeel;

	RandomNoteComponent randomNoteComponent;
	VariationDefinitionComponent variationDefinitionComponent;


	ComboBox variationCombo;
	static const int buttonH = 20;
	static const int sliderH = 80;
	static const int sliderV = 80;
	static const int sliderTW = 60;
	static const int sliderTH = 20;

	//////////////////////////////////////////////////////

	void getSettings()
	{


	} // getSettings

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(TopiaryBeatsVariationComponent)
};
