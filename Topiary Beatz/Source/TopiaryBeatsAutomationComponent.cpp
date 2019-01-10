//////////////////////////////////////////////////////////////////////////////
/*
This file is part of Topiary Beats, Copyright Tom Tollenaere 2018-19.

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
#include "TopiaryBeatsAutomationComponent.h"
#include "../../Topiary/Source/TopiaryAutomationComponent.cpp"

/*


TopiaryBeatsAutomationComponent::TopiaryBeatsAutomationComponent()
{   // size set by TabbedComponent!

	addAndMakeVisible(notesButton);
	notesButton.setButtonText("Notes");
	notesButton.setClickingTogglesState(true);
	notesButton.setRadioGroupId(Topiary::VariationButtonIds::VariationSwitchButtons);
	notesButton.onClick = [this]
	{
		setVariationControl();
	};

	addAndMakeVisible(ccButton);
	ccButton.setButtonText("CC");
	ccButton.setClickingTogglesState(true);
	ccButton.setRadioGroupId(Topiary::VariationButtonIds::VariationSwitchButtons);
	ccButton.onClick = [this]
	{
		setVariationControl();
	};


	addAndMakeVisible(variationChannelEditor);
	variationChannelEditor.setText("16");
	variationChannelEditor.onFocusLost = [this]
	{
		setVariationControl();
	};
	variationChannelEditor.onReturnKey = [this]
	{
		setVariationControl();
	};

	for (int i = 0; i < 8; i++)
	{
		addAndMakeVisible(variationControlEditor[i]);
		variationControlEditor[i].onFocusLost = [this]
		{
			setVariationControl();
		};
		variationControlEditor[i].onReturnKey = [this]
		{
			setVariationControl();
		};
	}

} // TopiaryBeatsAutomationComponent

/////////////////////////////////////////////////////////////////////////


TopiaryBeatsAutomationComponent::~TopiaryBeatsAutomationComponent()
{
	beatsModel->removeListener((ActionListener*)this);

} //~TopiaryBeatsautomationComponent

/////////////////////////////////////////////////////////////////////////

void TopiaryBeatsAutomationComponent::setModel(TopiaryBeatsModel* m)
{
	beatsModel = m;
	beatsModel->setListener((ActionListener*)this);
	actionListenerCallback(MsgVariationAutomation);  // find variation automation values

} // setModel

/////////////////////////////////////////////////////////////////////////

void TopiaryBeatsAutomationComponent::paint(Graphics& g)
{
	auto area = getLocalBounds();

	g.fillAll(TopiaryColour::background);
	g.setColour(TopiaryColour::foreground);
	g.setFont(12.0f);

	int labelOffset = 15;
	int lineWidth = 2;

	g.fillAll(TopiaryColour::background);

	// Variation automation settings
	int posX = 250;
	int posY = 100;
	int w = 500;
	int h = 48;
	int buttonH = 40;
	int buttonW = 60;
	int editorW = 30;
	int spacer = 10;
	auto variationSwitchArea = Rectangle<int>(posX, posY, w, h);

	g.setColour(TopiaryColour::foreground);
	g.setFont(12.0f);
	g.drawText("Variation switch automation   CH        V1       V2        V3        V4        V5        V6        V7       V8", posX + lineWidth, posY+lineWidth, 500, labelOffset, juce::Justification::centredLeft);

	auto recBounds = variationSwitchArea.removeFromBottom(variationSwitchArea.getHeight() - labelOffset);
	g.drawRoundedRectangle((float)recBounds.getX() + lineWidth, (float)recBounds.getY() + lineWidth, (float)recBounds.getWidth() - 2 * lineWidth, (float)recBounds.getHeight() - 2 * lineWidth, (float)lineWidth, (float)lineWidth);

	// turn it into inner rectangle
	auto inRecBounds = Rectangle<int>::Rectangle(recBounds.getX() + 3 * lineWidth, recBounds.getY() + 3 * lineWidth, recBounds.getWidth() - 6 * lineWidth, recBounds.getHeight() - 6 * lineWidth);

	// area for buttons & editors
	//auto buttonBounds = inRecBounds.removeFromBottom(buttonH);
	auto buttonBounds = inRecBounds.removeFromLeft(buttonW);
	notesButton.setBounds(buttonBounds);
	notesButton.setSize(buttonBounds.getWidth(), buttonBounds.getHeight());

	buttonBounds = inRecBounds.removeFromLeft(spacer);
	buttonBounds = inRecBounds.removeFromLeft(buttonW);
	ccButton.setBounds(buttonBounds);
	ccButton.setSize(buttonBounds.getWidth(), buttonBounds.getHeight());



	buttonBounds = inRecBounds.removeFromLeft(spacer);
	buttonBounds = inRecBounds.removeFromLeft(editorW);
	variationChannelEditor.setBounds(buttonBounds);
	variationChannelEditor.setSize(buttonBounds.getWidth(), buttonBounds.getHeight());

	

	for (int i = 0; i < 8; i++)
	{
		buttonBounds = inRecBounds.removeFromLeft(spacer);
		buttonBounds = inRecBounds.removeFromLeft(editorW);
		variationControlEditor[i].setBounds(buttonBounds);
		variationControlEditor[i].setSize(buttonBounds.getWidth(), buttonBounds.getHeight());
	}



} // paint

/////////////////////////////////////////////////////////////////////////

void TopiaryBeatsAutomationComponent::resized()
{
} //resized

/////////////////////////////////////////////////////////////////////////

void TopiaryBeatsAutomationComponent::actionListenerCallback(const String &message)
{
	if (message.compare(MsgVariationAutomation) == 0)
	{
		getVariationControl();
	}
} // actionListenerCallback

/////////////////////////////////////////////////////////////////////////

void TopiaryBeatsAutomationComponent::getVariationControl()
{
	// get data from model
	bool ccSwitching;
	int channel;
	int switches[8];

	beatsModel->getVariationControl(ccSwitching, channel, switches);

	if (ccSwitching)
		ccButton.setToggleState(true, dontSendNotification);
	else
		notesButton.setToggleState(true, dontSendNotification);

	variationChannelEditor.setText(String(channel));

	for (int i = 0; i < 8; i++)
	{
		if (ccButton.getToggleState())
		{
			variationControlEditor[i].setText(String(switches[i]), dontSendNotification);
		}
		else
		{
			variationControlEditor[i].setText(MidiMessage::getMidiNoteName(switches[i], true, true, 5), dontSendNotification);
			//Logger::outputDebugString(String("input ") + String(switches[i]));
			//Logger::outputDebugString(String("output ") + MidiMessage::getMidiNoteName(switches[i], true, true, 5));

		}

	}

} // getVariationControl

/////////////////////////////////////////////////////////////////////////

void TopiaryBeatsAutomationComponent::setVariationControl()
{
	// write data to model

	// first validate the input
	int switches[8];
	bool ccSwitching = ccButton.getToggleState();
	int channel = variationChannelEditor.getText().getIntValue();
	int cc;
	String note;

	if (channel < 0) channel = 0;
	if (channel > 16) channel = 16;
	variationChannelEditor.setText(String(channel), dontSendNotification);

	for (int i=0; i<8; i++)
	{
		if (ccSwitching)
		{
			cc = variationControlEditor[i].getText().getIntValue();
			if (cc > 127) cc = 127;
			if (cc < 0) cc = 0;
			switches[i] = cc;
			variationControlEditor[i].setText(String(cc), dontSendNotification);
		}
		else
		{
			// we use notes; validate the nodes and translate to note numbers
			note = validateNote(variationControlEditor[i].getText());
			switches[i] = validNoteNumber(note);
			//Logger::outputDebugString(String("input ") + note);
			//Logger::outputDebugString(String("output ") + String(switches[i]));
			variationControlEditor[i].setText(note, dontSendNotification);

		}
	}

	beatsModel->setVariationControl(ccSwitching, channel, switches);

} // setVariationControl

/////////////////////////////////////////////////////////////////////////
*/