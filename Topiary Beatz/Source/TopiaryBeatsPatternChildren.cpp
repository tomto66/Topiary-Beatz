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
//#include "../JuceLibraryCode/JuceHeader.h"
//#include "../../Topiary/Source/Topiary.h"
#include "TopiaryBeatsModel.h"
#include "TopiaryBeatsPatternChildren.h"
#include "TopiaryBeatsPatternComponent.h"

////////////////////////////////////////////////////////////////////////////////
// PatternLenghComponent
// Editor to set/edit pattern length
////////////////////////////////////////////////////////////////////////////////

PatternLengthComponent::PatternLengthComponent()
{
	addAndMakeVisible(lengthEditor);
	lengthEditor.setSelectAllWhenFocused(true);
	lengthEditor.setEnabled(true);
	lengthEditor.setSize(eW, eH);

	lengthEditor.onFocusLost = [this]
	{
		parent->setPatternLength();
	};
	lengthEditor.onReturnKey = [this]
	{
		parent->setPatternLength();
	};

	addAndMakeVisible(keepTail);
	keepTail.setButtonText("Keep tail");
	keepTail.setClickingTogglesState(true);
	
	keepTail.onClick = [this]
	{
		// do nothing; it will get processed when the length is changed.
	};

	

	setSize(width, heigth);

} // PatternLenghtComponent

//////////////////////////////////////////////////////////////////////////////////////////////

PatternLengthComponent::~PatternLengthComponent()
{
} // ~PatternLenghtComponent

//////////////////////////////////////////////////////////////////////////////////////////////

void PatternLengthComponent::resized()
{
} // resized

//////////////////////////////////////////////////////////////////////////////////////////////

void PatternLengthComponent::setParent(TopiaryBeatsPatternComponent* p)
{
	parent = p;

} // setParent

//////////////////////////////////////////////////////////////////////////////////////////////

void PatternLengthComponent::paint(Graphics& g)
{
	//int separator = 5;
	int labelOffset = 15;
	int lineWidth = 2;
	g.fillAll(TopiaryColour::background);
	g.setColour(TopiaryColour::foreground);
	g.setFont(12.0f);

	g.drawText("Length in measures" , 0, 0, 200, (int)labelOffset, juce::Justification::centredLeft);
	auto area = getLocalBounds();
	auto recBounds = area.removeFromBottom(area.getHeight() - labelOffset);
	g.drawRoundedRectangle((float)recBounds.getX() + lineWidth, (float)recBounds.getY() + lineWidth, (float)recBounds.getWidth() - 2 * lineWidth, (float)recBounds.getHeight() - 2 * lineWidth, (float)lineWidth, (float)lineWidth);

	// turn it into inner rectangle
	auto inRecBounds = Rectangle<int>::Rectangle(recBounds.getX() + 3 * lineWidth, recBounds.getY() + 3 * lineWidth, recBounds.getWidth() - 6 * lineWidth, recBounds.getHeight() - 6 * lineWidth);

	auto eBounds = inRecBounds.removeFromTop(eH);
	auto tBounds = eBounds.removeFromRight(130);
	lengthEditor.setBounds(eBounds);
	keepTail.setBounds(tBounds);

} // paint

////////////////////////////////////////////////////////////////////////////////
// ActionButtonsComponent
// Lots of action buttons
////////////////////////////////////////////////////////////////////////////////

ActionButtonsComponent::ActionButtonsComponent()
{
	addAndMakeVisible(deleteButton);
	deleteButton.setSize(bW, bH);
	deleteButton.setButtonText("Delete");
	deleteButton.onClick = [this]
	{
		parent->deleteNote();
	};

	addAndMakeVisible(addButton);
	addButton.setSize(bW, bH);
	addButton.setButtonText("Add");
	addButton.onClick = [this]
	{
		parent->addNote();
	};

	addAndMakeVisible(copyButton);
	copyButton.setSize(bW, bH);
	copyButton.setButtonText("Copy");
	copyButton.onClick = [this]
	{
		parent->copyNote();
	};

	addAndMakeVisible(pasteButton);
	pasteButton.setSize(bW, bH);
	pasteButton.setButtonText("Paste");
	pasteButton.onClick = [this]
	{
		parent->pasteNote();
	};

	setSize(width, heigth);

} // ActionButtonsComponent

//////////////////////////////////////////////////////////////////////////////////////////////

ActionButtonsComponent::~ActionButtonsComponent()
{
} // ~ActinButtonsComponent

//////////////////////////////////////////////////////////////////////////////////////////////

void ActionButtonsComponent::resized()
{
} // resized

//////////////////////////////////////////////////////////////////////////////////////////////

void ActionButtonsComponent::setParent(TopiaryBeatsPatternComponent* p)
{
	parent = p;

} // setParent

//////////////////////////////////////////////////////////////////////////////////////////////

void ActionButtonsComponent::paint(Graphics& g)
{
	//int separator = 5;
	int labelOffset = 15;
	int lineWidth = 2;
	g.fillAll(TopiaryColour::background);
	g.setColour(TopiaryColour::foreground);
	g.setFont(12.0f);

	g.drawText("Actions", 0, 0, 200, (int)labelOffset, juce::Justification::centredLeft);
	auto area = getLocalBounds();
	auto recBounds = area.removeFromBottom(area.getHeight() - labelOffset);
	g.drawRoundedRectangle((float)recBounds.getX() + lineWidth, (float)recBounds.getY() + lineWidth, (float)recBounds.getWidth() - 2 * lineWidth, (float)recBounds.getHeight() - 2 * lineWidth, (float)lineWidth, (float)lineWidth);

	// turn it into inner rectangle
	auto inRecBounds = Rectangle<int>::Rectangle(recBounds.getX() + 3 * lineWidth, recBounds.getY() + 3 * lineWidth, recBounds.getWidth() - 6 * lineWidth, recBounds.getHeight() - 6 * lineWidth);

	auto columnBounds = inRecBounds.removeFromLeft(bW);
	auto bBounds = columnBounds.removeFromTop(bH);
	addButton.setBounds(bBounds);

	columnBounds.removeFromTop(2 * lineWidth);
	bBounds = columnBounds.removeFromTop(bH);
	deleteButton.setBounds(bBounds);

	columnBounds.removeFromTop(2 * lineWidth);
	bBounds = columnBounds.removeFromTop(bH);
	copyButton.setBounds(bBounds);

	columnBounds.removeFromTop(2 * lineWidth);
	bBounds = columnBounds.removeFromTop(bH);
	pasteButton.setBounds(bBounds);
	

} // paint