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
////////////////////////////////////////////////////////////////////////////

#pragma once
#include "TopiaryBeatsMasterChildren.h"
#include "TopiaryBeatsMasterComponent.h"

SettingComponent::SettingComponent()
{

	setSize(width, heigth);

	// WFFNButton

	WFFNButton.setClickingTogglesState(true);
	WFFNButton.setLookAndFeel(&topiaryLookAndFeel);
	WFFNButton.setSize(100, buttonH);
	addAndMakeVisible(WFFNButton);
	WFFNButton.setButtonText("WFFN");
	WFFNButton.onClick = [this]
	{
		parent->setSettings();
	};

	// notePassThoughButton

	notePassThroughButton.setClickingTogglesState(true);
	notePassThroughButton.setLookAndFeel(&topiaryLookAndFeel);
	notePassThroughButton.setSize(100, buttonH);
	addAndMakeVisible(notePassThroughButton);
	notePassThroughButton.setButtonText("Echo MIDI");
	notePassThroughButton.onClick = [this]
	{
		parent->setSettings();
	};

	// quantizeVariationStartCombo;

	quantizeVariationStartCombo.setSize(100, buttonH);
	addAndMakeVisible(quantizeVariationStartCombo);
	quantizeVariationStartCombo.setSize(100, buttonH);
	quantizeVariationStartCombo.addItem("Immediate", Topiary::Immediate);
	quantizeVariationStartCombo.addItem("Measure", Topiary::Measure);
	quantizeVariationStartCombo.addItem("Beat", Topiary::Quarter);

	quantizeVariationStartCombo.onChange = [this]
	{
		parent->setSettings();
	};

	// quantizeRunStop

	quantizeRunStopCombo.setSize(100, buttonH);
	addAndMakeVisible(quantizeRunStopCombo);
	quantizeRunStopCombo.setSize(100, buttonH);
	quantizeRunStopCombo.addItem("Immediate", 100);
	quantizeRunStopCombo.addItem("Whole variation", Topiary::WholePattern);
	quantizeRunStopCombo.addItem("Measure", 1);
	quantizeRunStopCombo.addItem("1/2", 3);
	quantizeRunStopCombo.addItem("1/4", 5);

	quantizeRunStopCombo.onChange = [this]
	{
		parent->setSettings();
	};

	addAndMakeVisible(saveButton);
	saveButton.setSize(100, buttonH);
	saveButton.setButtonText("Save");
	saveButton.onClick = [this]
	{
		parent->savePreset();
	};

	addAndMakeVisible(loadButton);
	loadButton.setSize(100, buttonH);
	loadButton.setButtonText("Load");
	loadButton.onClick = [this]
	{
		parent->loadPreset();
	};

	addAndMakeVisible(nameEditor);
	nameEditor.setSize(210, buttonH);

	nameEditor.onReturnKey = [this]
	{
		parent->setSettings();
	};
	nameEditor.onFocusLost = [this]
	{
		parent->setSettings();
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
		parent->setSettings();
	};

	addAndMakeVisible(forceOutputChannelButton);
	forceOutputChannelButton.setSize(100, buttonH);
	forceOutputChannelButton.setButtonText("Force identical output channels");
	forceOutputChannelButton.setClickingTogglesState(true);
	forceOutputChannelButton.setToggleState(false, dontSendNotification);
	forceOutputChannelButton.onClick = [this]
	{
		parent->setSettings();
	};

} // SettingComponent

//////////////////////////////////////////////////////////////////////////////////////////////

SettingComponent::~SettingComponent()
{
} // ~SettingComponent

//////////////////////////////////////////////////////////////////////////////////////////////

void SettingComponent::paint(Graphics& g)
{
	//// Settings
	int labelOffset = 15;
	int lineWidth = 2;
	auto area = getLocalBounds();

	g.fillAll(TopiaryColour::background);
	g.setColour(TopiaryColour::foreground);
	g.setFont(12.0f);
	g.drawText("Settings", lineWidth, lineWidth, 200, labelOffset, juce::Justification::centredLeft);
	
	auto recBounds = area.removeFromBottom(area.getHeight() - labelOffset);
	g.drawRoundedRectangle((float)recBounds.getX() + lineWidth, (float)recBounds.getY() + lineWidth, (float)recBounds.getWidth() - 2 * lineWidth, (float)recBounds.getHeight() - 2 * lineWidth, (float)lineWidth, (float)lineWidth);

	// turn it into inner rectangle
	auto inRecBounds = Rectangle<int>::Rectangle(recBounds.getX() + 3 * lineWidth, recBounds.getY() + 3 * lineWidth, recBounds.getWidth() - 6 * lineWidth, recBounds.getHeight() - 6 * lineWidth);

	int firstLine = buttonH + 7;

	WFFNButton.setBounds(20, firstLine, 100, buttonH);

	notePassThroughButton.setBounds(130, firstLine, 100, buttonH);

	g.drawText("Q Var Start:", 235, firstLine, 200, labelOffset + 3, juce::Justification::centredLeft);
	quantizeVariationStartCombo.setBounds(300, firstLine, 110, buttonH);

	g.drawText("Q Var End:", 415, firstLine, 200, labelOffset + 3, juce::Justification::centredLeft);
	quantizeRunStopCombo.setBounds(475, firstLine, 100, buttonH);

	g.drawText("Q Var Switch:", 580, firstLine, 200, labelOffset + 3, juce::Justification::centredLeft);
	switchVariationCombo.setBounds(650, firstLine, 150, buttonH);

	int secondLine = 2 * buttonH + 14;

	g.drawText("Preset name:", 20, secondLine, 200, labelOffset + 3, juce::Justification::centredLeft);

	nameEditor.setBounds(100, secondLine, 200, buttonH);
	saveButton.setBounds(320, secondLine, 100, buttonH);
	loadButton.setBounds(440, secondLine, 100, buttonH);
	forceOutputChannelButton.setBounds(580, secondLine, 300, buttonH);

} // paint

//////////////////////////////////////////////////////////////////////////////////////////////

void SettingComponent::resized()
{
} // resized

//////////////////////////////////////////////////////////////////////////////////////////////

void SettingComponent::setParent(TopiaryBeatsMasterComponent* p)
{
	parent = p;

} // setParent

//////////////////////////////////////////////////////////////////////////////////////////////