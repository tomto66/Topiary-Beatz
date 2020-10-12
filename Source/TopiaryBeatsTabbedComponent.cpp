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
	beatsTabs.addTab("Master", TopiaryColour::background, &masterComponent, true);

	patternComponent.setSize(TopiaryBeatsComponent::width, tabHeight);
	beatsTabs.addTab("Patterns", TopiaryColour::background, &patternComponent, true);

	variationComponent.setSize(TopiaryBeatsComponent::width, tabHeight);
	beatsTabs.addTab("Variations", TopiaryColour::background, &variationComponent, true);
	
	utilityComponent.setSize(TopiaryBeatsComponent::width, tabHeight);
	beatsTabs.addTab("Utility", TopiaryColour::background, &utilityComponent, true);

	logComponent.setSize(TopiaryBeatsComponent::width, tabHeight);
	beatsTabs.addTab("Log", TopiaryColour::background, &logComponent, true);
	
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
	variationComponent.setModel(beatsModel);
	utilityComponent.setModel(beatsModel);
	patternComponent.setModel(beatsModel);
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
	if (message.compare(MsgTransport) == 0)
	{
		int b, n, d, runState;
		bool o, w;

		beatsModel->getTransportState(b, n, d, runState, o, w);
		if ((runState == Topiary::Running) || (runState == Topiary::Armed) || (runState == Topiary::Ending))
		{
			masterComponent.setEnabled(false);
			variationComponent.setEnabled(false);
			utilityComponent.setEnabled(false);
			patternComponent.setEnabled(false);
		}
		else
		{
			masterComponent.setEnabled(true);
			variationComponent.setEnabled(true);
			utilityComponent.setEnabled(true);
			patternComponent.setEnabled(true);
		}
	}
} // actionListenerCallback

/////////////////////////////////////////////////////////////////////////