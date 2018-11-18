/////////////////////////////////////////////////////////////////////////////
/*
This file is part of Topiary, Copyright Tom Tollenaere 2018.

Topiary Beats is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

Topiary Beats is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Topiary. If not, see <https://www.gnu.org/licenses/>.
*/
/////////////////////////////////////////////////////////////////////////////

#pragma once
#include "../JuceLibraryCode/JuceHeader.h"
#include "TopiaryTransportComponent.h"

TopiaryTransportComponent::TopiaryTransportComponent()
{
	setSize(width, height);
	startButton.setLookAndFeel(&topiaryLookAndFeel);
	startButton.setButtonText(">");
	startButton.setSize(buttonW, buttonH);
	startButton.setEnabled(true);
	startButton.setClickingTogglesState(true);
	startButton.setRadioGroupId(Topiary::TransportButtonIds::TransportButtons);
	startButton.onClick = [this] {
		updateToggleState(&startButton);
		callProcessUIChanges();
	};
	addAndMakeVisible(startButton);


	stopButton.setLookAndFeel(&topiaryLookAndFeel);
	stopButton.setButtonText("||");
	stopButton.setSize(buttonW, buttonH);
	stopButton.setClickingTogglesState(true);
	stopButton.setRadioGroupId(Topiary::TransportButtonIds::TransportButtons);
	stopButton.onClick = [this] { 
		updateToggleState(&stopButton);
		callProcessUIChanges();
	};
	stopButton.setToggleState(true,dontSendNotification);
	addAndMakeVisible(stopButton);
	
	recButton.setLookAndFeel(&topiaryLookAndFeel);
	recButton.setButtonText("O");
	recButton.setSize(buttonW, buttonH);
	recButton.setClickingTogglesState(true);
	recButton.setRadioGroupId(Topiary::TransportButtonIds::TransportButtons);
	recButton.onClick = [this] 
	{ 
		updateToggleState(&recButton);
		callProcessUIChanges();
	};
	addAndMakeVisible(recButton);

	overrideButton.setSize(overrideW, buttonH);
	overrideButton.setButtonText("Override host");
	overrideButton.setClickingTogglesState(true);
	overrideButton.setToggleState(false, dontSendNotification);
	overrideButton.onClick = [this] 
	{ 
		callProcessUIChanges();
	};
	addAndMakeVisible(overrideButton);

	numeratorEditor.setSize(sigW, buttonH);
	addAndMakeVisible(numeratorEditor); 
	numeratorEditor.onFocusLost =[this] 
	{ 
		callProcessUIChanges(); 
	};
	numeratorEditor.onReturnKey = [this]
	{
		callProcessUIChanges();
	};

	denominatorEditor.setSize(sigW, buttonH);
	addAndMakeVisible(denominatorEditor);
	denominatorEditor.onFocusLost = [this] 
	{
		callProcessUIChanges();
	};
	denominatorEditor.onReturnKey = [this]
	{
		callProcessUIChanges();
	};

	bpmEditor.setSize(bpmW, buttonH);
	addAndMakeVisible(bpmEditor);
	bpmEditor.onReturnKey =[this]
	{
		callProcessUIChanges();
	};
	bpmEditor.onFocusLost = [this]
	{
		callProcessUIChanges();
	};


}

/////////////////////////////////////////////////////////////////////////

void TopiaryTransportComponent::updateToggleState(TextButton* button)
{
	auto state = button->getToggleState();
	String stateString = state ? "ON" : "OFF";
	//String selectedString = state ? " (selected)" : "";

	Logger::outputDebugString(" Button " + button->getButtonText() + "  changed to " + stateString);

}

/////////////////////////////////////////////////////////////////////////

TopiaryTransportComponent::~TopiaryTransportComponent()
{
}

/////////////////////////////////////////////////////////////////////////

void TopiaryTransportComponent::resized()
{
}

/////////////////////////////////////////////////////////////////////////

void TopiaryTransportComponent::paint(Graphics& g) {
	
	int lineSize = 2;
	int labelOffset = 15;
	int buttonOffsetY = labelOffset + 5;
	//int buttonOffsetX = lineSize + 5;

	int buttonSeparator = 4;

	auto area = getLocalBounds();
	
	g.fillAll(TopiaryColour::background);
	//g.fillAll(Colours::darkblue);
	g.setColour(TopiaryColour::foreground);
	g.setFont(12.0f);
	g.drawText("Transport", 0, 0, 200, labelOffset, juce::Justification::centredLeft);
	g.drawRoundedRectangle((float) lineSize, (float) labelOffset, (float) width - 2 * lineSize - 10, (float)height - labelOffset - 2 * lineSize, (float) 4, (float) lineSize);

	overrideButton.setBounds(7, buttonOffsetY, 100, buttonH);

	stopButton.setBounds(107, buttonOffsetY, buttonW, buttonH);
	startButton.setBounds(107 + buttonW + buttonSeparator, buttonOffsetY, buttonW, buttonH);
	recButton.setBounds(107 + 2*(buttonW + buttonSeparator), buttonOffsetY, buttonW, buttonH);

	bpmEditor.setBounds(220, buttonOffsetY, bpmW, buttonH);
	g.drawText("BPM", 220 + bpmW + 5, buttonOffsetY+1, 30, labelOffset, juce::Justification::centredLeft);

	numeratorEditor.setBounds(280, buttonOffsetY, sigW, buttonH);
	g.drawText(" / ", 297 , buttonOffsetY + 1, 15, labelOffset, juce::Justification::centredLeft);
	denominatorEditor.setBounds(310, buttonOffsetY , sigW, buttonH);

	
} // paint

/////////////////////////////////////////////////////////////////////////

void TopiaryTransportComponent:: setOverride(bool override)

{ // disable components when host is overridden and vice versa
  // does not call the model; is called from the derived class instead! - do not call directly!! (should be private, really ?) 
	if (!override) {
		startButton.setEnabled(false);
		stopButton.setEnabled(false);
		recButton.setEnabled(false);
		bpmEditor.setEnabled(false);
		numeratorEditor.setEnabled(false);	
		denominatorEditor.setEnabled(false);
		overrideButton.setToggleState(false, dontSendNotification);
	}
	else {
		startButton.setEnabled(true);
		stopButton.setEnabled(true);
		recButton.setEnabled(true);
		bpmEditor.setEnabled(true);
		numeratorEditor.setEnabled(true);
		denominatorEditor.setEnabled(true);
		overrideButton.setToggleState(true, dontSendNotification);
	}
} // setOverride

/////////////////////////////////////////////////////////////////////////

void TopiaryTransportComponent::updateState(bool override, int bpm, int n, int d, int state)
// updates the whole state: value of override will enabele/disable buttons
// values of fields are simply set
{
	setOverride(override);
	if (!bpmEditor.hasKeyboardFocus(true)) bpmEditor.setText(String(bpm)); // if in focus we may still be editing - don't overwrite own edit
	numeratorEditor.setText(String(n),dontSendNotification);
	denominatorEditor.setText(String(d),dontSendNotification);
	switch (state)
	{
	case 5: // ending 
		stopButton.specialState = Topiary::TopiaryButtonSpecialState::waitToEnd;
		startButton.specialState = 0;
		stopButton.setToggleState(true, dontSendNotification);
		break;

	//case 4:
	case 0:  // Stopped
		stopButton.specialState = 0;
		startButton.specialState = 0;
		stopButton.setToggleState(false, dontSendNotification); // dirty trick to force repaint
		stopButton.setToggleState(true, dontSendNotification);
		break;

	case 1: // Running
		startButton.specialState = 0;
		stopButton.specialState = 0;
		startButton.setToggleState(false, dontSendNotification); // dirty trick to force repaint
		startButton.setToggleState(true, dontSendNotification);
		break;

	case 3: // Armed
		startButton.specialState = Topiary::TopiaryButtonSpecialState::waitToApply;
		stopButton.specialState = 0;
		startButton.setToggleState(true, dontSendNotification);
		break;

	case 2: 
		recButton.setToggleState(true, sendNotification);
		break;
	default: break;
	}



	
}  // updateState

/////////////////////////////////////////////////////////////////////////

void TopiaryTransportComponent::processUIChanges(bool override, int& n, int& d, int& bpm, int buttonEnabled)
{
	UNUSED(override)
	UNUSED(n)
	UNUSED(d)
	UNUSED(bpm)
	UNUSED(buttonEnabled)
	// virtual function!
} // processUIChanges

