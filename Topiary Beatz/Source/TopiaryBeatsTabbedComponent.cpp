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
#include "TopiaryBeatsComponent.h"
#include "TopiaryBeatsTabbedComponent.h"

TopiaryBeatsTabbedComponent::TopiaryBeatsTabbedComponent() : beatsTabs(TabbedButtonBar::Orientation::TabsAtTop)   // above needed otherwise the header declares TabbedButtonBar beatsTabs (without orientation!!!)
	                                                         // see C++ member initialisation lists for explanation !
															
{
	tabHeight = TopiaryBeatsComponent::heigth - TopiaryBeatsComponent::headerHeigth - 30;
	height = TopiaryBeatsComponent::heigth - TopiaryBeatsComponent::headerHeigth;

	addAndMakeVisible(beatsTabs);
	// do NOT EVER make the tab components themselves visible - it'll screw up royally!!!!

	setSize(TopiaryBeatsComponent::width, height);

	masterComponent.setSize(TopiaryBeatsComponent::width, tabHeight);
	beatsTabs.addTab("MASTER", TopiaryColour::background, &masterComponent, true);
	
	//variationComponent.setSize(Topiary::WIDTH, tabHeight);
	//beatsTabs.addTab("VARIATIONS", Topiary::BACKGROUND_COLOUR, &variationComponent, true);
	
	realtimeComponent.setSize(TopiaryBeatsComponent::width, tabHeight);
	beatsTabs.addTab("REALTIME", TopiaryColour::background, &realtimeComponent, true);
	
	logComponent.setSize(TopiaryBeatsComponent::width, tabHeight);
	beatsTabs.addTab("LOG", TopiaryColour::background, &logComponent, true);
	
	setSize(TopiaryBeatsComponent::width, TopiaryBeatsComponent::heigth - TopiaryBeatsComponent::headerHeigth);

}

/////////////////////////////////////////////////////////////////////////

TopiaryBeatsTabbedComponent::~TopiaryBeatsTabbedComponent()
{
	beatsModel->removeListener((ActionListener*)this);
}

/////////////////////////////////////////////////////////////////////////

void TopiaryBeatsTabbedComponent::setModel(TopiaryBeatsModel* model) 
{
	beatsModel = model;
	masterComponent.setModel(beatsModel);
	logComponent.setModel(beatsModel);
	realtimeComponent.setModel(beatsModel);
	beatsModel->setListener((ActionListener*)this);
}

/////////////////////////////////////////////////////////////////////////

void TopiaryBeatsTabbedComponent::paint(Graphics& g)
{
	UNUSED(g)
	auto area = getLocalBounds();
	beatsTabs.setBounds(area);
}

/////////////////////////////////////////////////////////////////////////

void TopiaryBeatsTabbedComponent::resized()
{
}

/////////////////////////////////////////////////////////////////////////

void TopiaryBeatsTabbedComponent::actionListenerCallback(const String &message)
{
	if (message.compare("transport") == 0)
	{
		int b, n, d, runState;
		bool o, w;

		beatsModel->getTransportState(b, n, d, runState, o, w);
		if ((runState == Topiary::Running) || (runState == Topiary::Armed) || (runState == Topiary::Ending))
		{
			masterComponent.setEnabled(false);
		}
		else
		{
			masterComponent.setEnabled(true);
		}
	}
} // actionListenerCallback

/////////////////////////////////////////////////////////////////////////