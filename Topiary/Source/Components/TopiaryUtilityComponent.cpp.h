/////////////////////////////////////////////////////////////////////////////
/*
This file is part of Topiary, Copyright Tom Tollenaere 2018-2020.

Topiary is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

Topiary is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Topiary. If not, see <https://www.gnu.org/licenses/>.

/////////////////////////////////////////////////////////////////////////////

This code has a generic automation component that can be included in every Topiary plugin.

CAREFUL: needs symbols:
- TOPIARYMODEL e.g. TOPIARYMODEL
- TOPIARYUTILITYCOMPONENT e.g. TOPIARYUTILITYCOMPONENT 

*/
/////////////////////////////////////////////////////////////////////////////

#ifdef TOPIARYMODEL


TOPIARYUTILITYCOMPONENT::TOPIARYUTILITYCOMPONENT()
{   // size set by TabbedComponent!

	//////////////////////////////////////
	// Automation
	//////////////////////////////////////

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
	variationChannelEditor.setSelectAllWhenFocused(true);
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
		variationControlEditor[i].setSelectAllWhenFocused(true);
		variationControlEditor[i].onFocusLost = [this]
		{
			setVariationControl();
		};
		variationControlEditor[i].onReturnKey = [this]
		{
			setVariationControl();
		};
	}

	////////////////////////////////
	// Copy Variations
	////////////////////////////////

	addAndMakeVisible(vCopyGoButton);
	vCopyGoButton.setButtonText("Go");
	vCopyGoButton.onClick = [this]
	{
		model->copyVariation(vCopyFromCombo.getSelectedId()-1, vCopyToCombo.getSelectedId()-1);
	};

	addAndMakeVisible(vCopyFromCombo);
	addAndMakeVisible(vCopyToCombo);
	for (int i=1; i<9; i++)
	{
		vCopyFromCombo.addItem(String(i), i);
		vCopyToCombo.addItem(String(i), i);
	}
	vCopyFromCombo.setSelectedId(1);
	vCopyToCombo.setSelectedId(2);

	/////////////////////////////////////
	// Swap Variations
	/////////////////////////////////////

	addAndMakeVisible(vSwapGoButton);
	vSwapGoButton.setButtonText("Go");
	vSwapGoButton.onClick = [this]
	{
		model->swapVariation(vSwapFromCombo.getSelectedId()-1, vSwapToCombo.getSelectedId()-1);
	};

	addAndMakeVisible(vSwapFromCombo);
	addAndMakeVisible(vSwapToCombo);
	for (int i = 1; i < 9; i++)
	{
		vSwapFromCombo.addItem(String(i), i);
		vSwapToCombo.addItem(String(i), i);
	}
	vSwapFromCombo.setSelectedId(1);
	vSwapToCombo.setSelectedId(2);

#ifdef PRESETZ
	///////////////////////////////////////
	// Copy Presets
	///////////////////////////////////////

	addAndMakeVisible(pCopyGoButton);
	pCopyGoButton.setButtonText("Go");
	pCopyGoButton.onClick = [this]
	{
		model->copyPreset(pCopyFromCombo.getSelectedId() - 1, pCopyToCombo.getSelectedId() - 1);
	};

	addAndMakeVisible(pCopyFromCombo);
	addAndMakeVisible(pCopyToCombo);
	for (int i = 1; i <= PRESETELEMENTS; i++)
	{
		pCopyFromCombo.addItem(String(i), i);
		pCopyToCombo.addItem(String(i), i);
	}
	pCopyFromCombo.setSelectedId(1);
	pCopyToCombo.setSelectedId(2);


	////////////////////////////////
	// Swap Presets
	////////////////////////////////

	addAndMakeVisible(pSwapGoButton);
	pSwapGoButton.setButtonText("Go");
	pSwapGoButton.onClick = [this]
	{
		model->swapPreset(pSwapFromCombo.getSelectedId() - 1, pSwapToCombo.getSelectedId() - 1);
	};

	addAndMakeVisible(pSwapFromCombo);
	addAndMakeVisible(pSwapToCombo);
	for (int i = 1; i <= PRESETELEMENTS; i++)
	{
		pSwapFromCombo.addItem(String(i), i);
		pSwapToCombo.addItem(String(i), i);
	}
	pSwapFromCombo.setSelectedId(1);
	pSwapToCombo.setSelectedId(2);
#endif  // PRESETZ



} // TOPIARYUTILITYCOMPONENT

/////////////////////////////////////////////////////////////////////////


TOPIARYUTILITYCOMPONENT::~TOPIARYUTILITYCOMPONENT()
{
	model->removeListener((ActionListener*)this);

} //~TOPIARYUTILITYCOMPONENT

/////////////////////////////////////////////////////////////////////////

void TOPIARYUTILITYCOMPONENT::setModel(TOPIARYMODEL* m)
{
	model = m;
	model->setListener((ActionListener*)this);
	actionListenerCallback(MsgVariationAutomation);  // find variation automation values
	for (int i = 0; i < 8; i++)
		variationControlEditor[i].setModel(m, Topiary::LearnMidiId::variationSwitch+i);

} // setModel

/////////////////////////////////////////////////////////////////////////

void TOPIARYUTILITYCOMPONENT::paint(Graphics& g)
{
	auto area = getLocalBounds();

	g.fillAll(TopiaryColour::background);
	g.setColour(TopiaryColour::foreground);
	g.setFont(12.0f);

	int labelOffset = 15;
	int lineWidth = 2;

	g.fillAll(TopiaryColour::background);

	////////////////////////////////////////
	// Variation automation settings
	////////////////////////////////////////

	int posX = 250;
	int posY = 100;
	int w = 500;
	int h = 48;
	//int buttonH = 40;
	int buttonW = 60;
	int editorW = 30;
	int spacer = 10;
	auto variationSwitchArea = Rectangle<int>(posX, posY, w, h);
	
	g.setColour(TopiaryColour::foreground);
	g.setFont(12.0f);
	
	g.drawText("Variation Switch Automation   CH        V1       V2        V3        V4        V5        V6        V7       V8", posX + lineWidth, posY+lineWidth, 500, labelOffset, juce::Justification::centredLeft);

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

	//////////////////////////////////////////
	// Copy Variations
	//////////////////////////////////////////

	posX = 250;
	posY = 150;
	w = 240;
	h = 48;
	int comboW = 60;

	auto copyVariationsArea = Rectangle<int>(posX, posY, w, h);
	g.drawText("Copy Variation", posX + lineWidth, posY + lineWidth, 500, labelOffset, juce::Justification::centredLeft);

	recBounds = copyVariationsArea.removeFromBottom(copyVariationsArea.getHeight() - labelOffset);
	g.drawRoundedRectangle((float)recBounds.getX() + lineWidth, (float)recBounds.getY() + lineWidth, (float)recBounds.getWidth() - 2 * lineWidth, (float)recBounds.getHeight() - 2 * lineWidth, (float)lineWidth, (float)lineWidth);

	// turn it into inner rectangle
	inRecBounds = Rectangle<int>::Rectangle(recBounds.getX() + 3 * lineWidth, recBounds.getY() + 3 * lineWidth, recBounds.getWidth() - 6 * lineWidth, recBounds.getHeight() - 6 * lineWidth);

	g.drawText("From                    To", posX + spacer, posY + lineWidth + 20, 500, labelOffset, juce::Justification::centredLeft);

	buttonBounds = inRecBounds.removeFromLeft(spacer + 25);
	buttonBounds = inRecBounds.removeFromLeft(comboW);
	vCopyFromCombo.setBounds(buttonBounds);

	buttonBounds = inRecBounds.removeFromLeft(spacer + 15);
	buttonBounds = inRecBounds.removeFromLeft(comboW);
	vCopyToCombo.setBounds(buttonBounds);

	buttonBounds = inRecBounds.removeFromLeft(spacer-2);
	buttonBounds = inRecBounds.removeFromLeft(buttonW);
	vCopyGoButton.setBounds(buttonBounds);

	///////////////////////////////////////////
	// Swap Variations
	///////////////////////////////////////////

	posX = 510;
	posY = 150;
	w = 240;
	h = 48;
	
	auto swapVariationsArea = Rectangle<int>(posX, posY, w, h);
	g.drawText("Swap Variation", posX + lineWidth, posY + lineWidth, 500, labelOffset, juce::Justification::centredLeft);

	recBounds = swapVariationsArea.removeFromBottom(swapVariationsArea.getHeight() - labelOffset);
	g.drawRoundedRectangle((float)recBounds.getX() + lineWidth, (float)recBounds.getY() + lineWidth, (float)recBounds.getWidth() - 2 * lineWidth, (float)recBounds.getHeight() - 2 * lineWidth, (float)lineWidth, (float)lineWidth);

	// turn it into inner rectangle
	inRecBounds = Rectangle<int>::Rectangle(recBounds.getX() + 3 * lineWidth, recBounds.getY() + 3 * lineWidth, recBounds.getWidth() - 6 * lineWidth, recBounds.getHeight() - 6 * lineWidth);

	g.drawText("From                    To", posX + spacer, posY + lineWidth + 20, 500, labelOffset, juce::Justification::centredLeft);

	buttonBounds = inRecBounds.removeFromLeft(spacer + 25);
	buttonBounds = inRecBounds.removeFromLeft(comboW);
	vSwapFromCombo.setBounds(buttonBounds);

	buttonBounds = inRecBounds.removeFromLeft(spacer + 15);
	buttonBounds = inRecBounds.removeFromLeft(comboW);
	vSwapToCombo.setBounds(buttonBounds);

	buttonBounds = inRecBounds.removeFromLeft(spacer - 2);
	buttonBounds = inRecBounds.removeFromLeft(buttonW);
	vSwapGoButton.setBounds(buttonBounds);

#ifdef PRESETZ
	//////////////////////////////////////////
	// Copy Presets
	//////////////////////////////////////////

	posX = 250;
	posY = 200;
	w = 240;
	h = 48;
	
	auto copyPresetsArea = Rectangle<int>(posX, posY, w, h);
	g.drawText("Copy Preset Definition", posX + lineWidth, posY + lineWidth, 500, labelOffset, juce::Justification::centredLeft);

	recBounds = copyPresetsArea.removeFromBottom(copyPresetsArea.getHeight() - labelOffset);
	g.drawRoundedRectangle((float)recBounds.getX() + lineWidth, (float)recBounds.getY() + lineWidth, (float)recBounds.getWidth() - 2 * lineWidth, (float)recBounds.getHeight() - 2 * lineWidth, (float)lineWidth, (float)lineWidth);

	// turn it into inner rectangle
	inRecBounds = Rectangle<int>::Rectangle(recBounds.getX() + 3 * lineWidth, recBounds.getY() + 3 * lineWidth, recBounds.getWidth() - 6 * lineWidth, recBounds.getHeight() - 6 * lineWidth);

	g.drawText("From                    To", posX + spacer, posY + lineWidth + 20, 500, labelOffset, juce::Justification::centredLeft);

	buttonBounds = inRecBounds.removeFromLeft(spacer + 25);
	buttonBounds = inRecBounds.removeFromLeft(comboW);
	pCopyFromCombo.setBounds(buttonBounds);

	buttonBounds = inRecBounds.removeFromLeft(spacer + 15);
	buttonBounds = inRecBounds.removeFromLeft(comboW);
	pCopyToCombo.setBounds(buttonBounds);

	buttonBounds = inRecBounds.removeFromLeft(spacer - 2);
	buttonBounds = inRecBounds.removeFromLeft(buttonW);
	pCopyGoButton.setBounds(buttonBounds);

	///////////////////////////////////////////
	// Swap Presets
	///////////////////////////////////////////

	posX = 510;
	posY = 200;
	w = 240;
	h = 48;

	auto swapPresetsArea = Rectangle<int>(posX, posY, w, h);
	g.drawText("Swap Preset Definition", posX + lineWidth, posY + lineWidth, 500, labelOffset, juce::Justification::centredLeft);

	recBounds = swapPresetsArea.removeFromBottom(swapPresetsArea.getHeight() - labelOffset);
	g.drawRoundedRectangle((float)recBounds.getX() + lineWidth, (float)recBounds.getY() + lineWidth, (float)recBounds.getWidth() - 2 * lineWidth, (float)recBounds.getHeight() - 2 * lineWidth, (float)lineWidth, (float)lineWidth);

	// turn it into inner rectangle
	inRecBounds = Rectangle<int>::Rectangle(recBounds.getX() + 3 * lineWidth, recBounds.getY() + 3 * lineWidth, recBounds.getWidth() - 6 * lineWidth, recBounds.getHeight() - 6 * lineWidth);

	g.drawText("From                    To", posX + spacer, posY + lineWidth + 20, 500, labelOffset, juce::Justification::centredLeft);

	buttonBounds = inRecBounds.removeFromLeft(spacer + 25);
	buttonBounds = inRecBounds.removeFromLeft(comboW);
	pSwapFromCombo.setBounds(buttonBounds);

	buttonBounds = inRecBounds.removeFromLeft(spacer + 15);
	buttonBounds = inRecBounds.removeFromLeft(comboW);
	pSwapToCombo.setBounds(buttonBounds);

	buttonBounds = inRecBounds.removeFromLeft(spacer - 2);
	buttonBounds = inRecBounds.removeFromLeft(buttonW);
	pSwapGoButton.setBounds(buttonBounds);
#endif


} // paint

/////////////////////////////////////////////////////////////////////////

void TOPIARYUTILITYCOMPONENT::resized()
{
} //resized

/////////////////////////////////////////////////////////////////////////

void TOPIARYUTILITYCOMPONENT::actionListenerCallback(const String &message)
{
	if (message.compare(MsgVariationAutomation) == 0)
	{
		getVariationControl();
	}
} // actionListenerCallback

/////////////////////////////////////////////////////////////////////////

void TOPIARYUTILITYCOMPONENT::getVariationControl()
{
	// get data from model
	bool ccSwitching;
	int channel;
	int switches[8];

	model->getVariationControl(ccSwitching, channel, switches);

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

void TOPIARYUTILITYCOMPONENT::setVariationControl()
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

	model->setVariationControl(ccSwitching, channel, switches);

} // setVariationControl

#endif
/////////////////////////////////////////////////////////////////////////