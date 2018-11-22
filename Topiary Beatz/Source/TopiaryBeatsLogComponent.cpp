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
#include "../JuceLibraryCode/JuceHeader.h"
#include "../../Topiary/Topiary.h"
#include "TopiaryBeatsModel.h"
#include "TopiaryBeatsLogComponent.h"



TopiaryBeatsLogComponent::TopiaryBeatsLogComponent()
{   // size set by TabbedComponent!

	addAndMakeVisible(logEditor);
	logEditor.setMultiLine(true);
	logEditor.setReadOnly(true);
	logEditor.setSize(w, h);

} // TopiaryBeatsLogComponent

/////////////////////////////////////////////////////////////////////////


TopiaryBeatsLogComponent::~TopiaryBeatsLogComponent()
{
	beatsModel->removeListener((ActionListener*)this);
} //~TopiaryBeatsLogComponent

/////////////////////////////////////////////////////////////////////////

void TopiaryBeatsLogComponent::setModel(TopiaryBeatsModel* m)
{
	beatsModel = m;
	beatsModel->setListener((ActionListener*)this);
	logEditor.setText(*(beatsModel->getBeatsLog())); // init the log window

} // setMdel

/////////////////////////////////////////////////////////////////////////

void TopiaryBeatsLogComponent::paint(Graphics& g)
{
	auto area = getLocalBounds();
	logEditor.setBounds(10, 10, w, h);
		
	g.fillAll(TopiaryColour::background);
	g.setColour(TopiaryColour::foreground);
	g.setFont(12.0f);

	
} // paint

/////////////////////////////////////////////////////////////////////////

void TopiaryBeatsLogComponent::resized()
{
} //resized

/////////////////////////////////////////////////////////////////////////

void TopiaryBeatsLogComponent::actionListenerCallback(const String &message)
{
	if (message.compare("log") == 0)
	{
		int a = beatsModel->getBeatsLog()->length();
		int b = logEditor.getText().length();

		if (abs(a - b) > 10)
		{
			logEditor.setText(*(beatsModel->getBeatsLog()));
			logEditor.moveCaretToEnd(); // keep scroll at bottom
		}
	}
} // actionListenerCallback

