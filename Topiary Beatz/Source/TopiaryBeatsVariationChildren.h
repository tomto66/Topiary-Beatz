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

class RandomNoteComponent : public Component
{
public:
	TopiaryLookAndFeel rndLookAndFeel;
	TextButton pool1Button;
	TextButton pool2Button;
	TextButton pool3Button;
	TextButton pool4Button;

	TextButton onButton;
	Slider	percentSlider;

	int width = 65;
	int heigth = 95;

	RandomNoteComponent();
	~RandomNoteComponent();
	void resized();
	void paint(Graphics& g) override;

private:

	
	static const int buttonH = 20;
	static const int pbuttonH = 12;
	static const int sliderH = 100;
	static const int sliderV = 65;
	static const int sliderTW = 60;
	static const int sliderTH = 15;

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(RandomNoteComponent)

}; // RandomNoteComponent

/////////////////////////////////////////////////////////////////////////////

class VariationDefinitionComponent : public Component
{
public:
	TopiaryLookAndFeel vdLookAndFeel;
	
	ComboBox variationCombo;
	TextEditor nameEditor;

	TextEditor fromBeatEditor;
	TextEditor toBeatEditor;
	TextEditor fromMeasureEditor;
	TextEditor toMeasureEditor;
	TextEditor fromTickEditor;
	TextEditor toTickEditor;

	int width = 400;
	int heigth = 100;

	VariationDefinitionComponent();
	~VariationDefinitionComponent();

	void resized();
	void paint(Graphics& g) override;

private:

	static const int eW = 120; // editor & combo
	static const int eH = 14; 
	static const int beW = 50; // measures, beats and ticks
	static const int meW = 50;
	static const int teW = 50;

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(VariationDefinitionComponent)

}; // VariationDefinitionComponent
