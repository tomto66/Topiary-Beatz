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
#include "TopiaryBeatsComponent.h"

TopiaryBeatsHeaderComponent::TopiaryBeatsHeaderComponent()
{
	setSize(TopiaryBeatsComponent::width, TopiaryBeatsComponent::headerHeigth);
	addAndMakeVisible(transportComponent);
	addAndMakeVisible(variationButtonsComponent);
	addAndMakeVisible(warningEditor);
	warningEditor.setReadOnly(true);
	warningEditor.setColour(TextEditor::backgroundColourId, TopiaryColour::orange);
	warningEditor.setColour(TextEditor::textColourId, Colours::lightyellow);
	warningEditor.setColour(TextEditor::outlineColourId, TopiaryColour::orange);
	addAndMakeVisible(timeEditor);
	timeEditor.setVisible(false);
	timeEditor.setEnabled(false);

}

/////////////////////////////////////////////////////////////////////////

TopiaryBeatsHeaderComponent::~TopiaryBeatsHeaderComponent()
{ 	
	beatsModel->removeListener((ActionListener*)this);
}

/////////////////////////////////////////////////////////////////////////

void TopiaryBeatsHeaderComponent::setModel(TopiaryBeatsModel* m)
{
	beatsModel = m;
	variationButtonsComponent.setModel(m);
	transportComponent.setModel(m);
	beatsModel->setListener((ActionListener*)this);
	variationButtonsComponent.checkModel();
	transportComponent.checkModel();
	warningEditor.setVisible(false);
} // setModel

/////////////////////////////////////////////////////////////////////////

void TopiaryBeatsHeaderComponent::resized()
{ // bounds are handled by viewer
}

/////////////////////////////////////////////////////////////////////////

void TopiaryBeatsHeaderComponent::paint(Graphics& g) {
	// load background
	g.fillAll(TopiaryColour::background );
	Image background = ImageCache::getFromMemory(BinaryData::topiaryBeatsLogo__75_png, BinaryData::topiaryBeatsLogo__75_pngSize);
	g.drawImageAt(background, 0, 0);
	auto area = getLocalBounds();
	
	variationButtonsComponent.setBounds(640,30 ,350,45);
	transportComponent.setBounds(295, 30, 350,45);

	warningEditor.setBounds(645, 5, 330, 18);
	timeEditor.setBounds(402, 17, 70, 18);

}

/////////////////////////////////////////////////////////////////////////

void TopiaryBeatsHeaderComponent::actionListenerCallback(const String &message)
{
	
	if (message.compare(MsgWarning) == 0)
	{
		warningEditor.setText(beatsModel->getLastWarning());
		warningEditor.setVisible(true);
		startTimer(3000);
	}
	else if (message.compare(MsgTiming) == 0) 
	{
		timeEditor.setVisible(true);
		getTime();
		startTimer(5000);
	}
	else
	{
		if (message.compare(MsgVariationSelected) == 0)
			variationButtonsComponent.checkModel();
		else if (message.compare(MsgTransport) == 0)
			transportComponent.checkModel();
		else if (message.compare(MsgVariationEnables) == 0)
			variationButtonsComponent.getEnabled();
		//if (message.compare(MsgLog) == 0)
		//	warningEditor.setVisible(false);
	}
}

/////////////////////////////////////////////////////////////////////////

void TopiaryBeatsHeaderComponent::timerCallback()
{
	if (warningEditor.isVisible())
		warningEditor.setVisible(false);
	else if (timeEditor.isVisible())
		timeEditor.setVisible(false);
	stopTimer();
}



