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

class TopiaryBeatsPatternComponent;

////////////////////////////////////////////////////////////////////////////////
// PatternLenghComponent
// Editor to set/edit pattern length
////////////////////////////////////////////////////////////////////////////////

class PatternLengthComponent : public Component
{

public:

	TextEditor lengthEditor;
	ToggleButton keepTail; // when shortening; keep the tail
	
	PatternLengthComponent();
	~PatternLengthComponent();
	void resized();
	void paint(Graphics& g) override;
	void setParent(TopiaryBeatsPatternComponent* p);
	int width = 240;
	int heigth = 45;

private:
	TopiaryBeatsPatternComponent* parent;
	static const int eW = 120; //  size
	static const int eH = 20;
	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PatternLengthComponent)
};

////////////////////////////////////////////////////////////////////////////////
// ActionButtonsComponent
// Lots of action buttons
////////////////////////////////////////////////////////////////////////////////

class ActionButtonsComponent : public Component
{

public:
	
	ActionButtonsComponent();
	~ActionButtonsComponent();
	void resized();
	void paint(Graphics& g) override;
	void setParent(TopiaryBeatsPatternComponent* p);
	int width = 555;
	int heigth = 200;
	TextButton deleteButton;	// deletes currently selected; disabled if nothing selected
	TextButton addButton;		// adds at current selection; adds at 0 0 0 if nothing selected
	TextButton copyButton;		// copies selected note
	TextButton pasteButton;		// pastes at current selection; disabled when nothing in paste buffer
	TextButton clearButton;		// clears the pattern
	TextButton deleteAllNotesButton;  // deletes all notes in the pattern that are same as selected one; disabled if nothing selected
	TextButton quantizeButton;
	ComboBox quantizeCombo;

private:
	TopiaryBeatsPatternComponent* parent;

	static const int bW = 120; //  size
	static const int bH = 20;
	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ActionButtonsComponent)
};