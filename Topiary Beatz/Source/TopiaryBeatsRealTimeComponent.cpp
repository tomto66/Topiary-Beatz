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
#include "TopiaryBeatsRealtimeComponent.h"


TopiaryBeatsRealtimeComponent::TopiaryBeatsRealtimeComponent()
{   
	addAndMakeVisible(measuresEditor);
	measuresEditor.setEnabled(false);
	addAndMakeVisible(beatsEditor);
	beatsEditor.setEnabled(false);
	
} // TopiaryBeatsRealTimeComponent() 

/////////////////////////////////////////////////////////////////////////

TopiaryBeatsRealtimeComponent::~TopiaryBeatsRealtimeComponent()
{
	beatsModel->removeListener((ActionListener*)this);
} // ~TopiaryBeatsRealtimeComponent

/////////////////////////////////////////////////////////////////////////

void TopiaryBeatsRealtimeComponent::setModel(TopiaryBeatsModel* m)
{
	beatsModel = m;
	beatsModel->setListener((ActionListener*)this);
} //setModel

/////////////////////////////////////////////////////////////////////////

void TopiaryBeatsRealtimeComponent::paint(Graphics& g)
{  
	int labelOffset = 15;
	int lineWidth = 2;
	auto area = getLocalBounds();
	g.fillAll(TopiaryColour::background);
	g.setColour(TopiaryColour::foreground);
	g.setFont(12.0f);

	int x = 400;
	int y = 150;
	int w = 200;
	int h = 50;

	g.drawText("Real time information", (int)lineWidth + x, (int)lineWidth + y - labelOffset, w, (int)labelOffset, juce::Justification::centredLeft);

	g.drawRoundedRectangle((float)x + lineWidth, (float) y + lineWidth, (float) w - 2 * lineWidth, (float) h - 2 * lineWidth, (float) lineWidth, (float) lineWidth);

	g.setColour(Colours::white);
	g.drawText("Measure:", x + 20, y + 20, 200, labelOffset, juce::Justification::centredLeft);
	measuresEditor.setBounds(x +80, y+20, 30, 14);

	g.drawText("Beat:", x+120, y+20, 200, labelOffset, juce::Justification::centredLeft);
	beatsEditor.setBounds(x + 155, y+20, 30, 14);
} // paint

void TopiaryBeatsRealtimeComponent::resized()
{
} // resized

/////////////////////////////////////////////////////////////////////////

void TopiaryBeatsRealtimeComponent::actionListenerCallback(const String &message)
{
	if (message.compare(MsgTiming)==0) {
		getTime();
	}

} // actionListenerCallback

/////////////////////////////////////////////////////////////////////////