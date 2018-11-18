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
//////////////////////////////////////////////////////////////////////////////

#pragma once
#include "../JuceLibraryCode/JuceHeader.h"
#include "TopiaryBeatsTransportComponent.h"


TopiaryBeatsTransportComponent::TopiaryBeatsTransportComponent()
{
}

/////////////////////////////////////////////////////////////////////////

TopiaryBeatsTransportComponent::~TopiaryBeatsTransportComponent()
{
}

/////////////////////////////////////////////////////////////////////////

void TopiaryBeatsTransportComponent::setModel(TopiaryBeatsModel * m)
{
	beatsModel = m;
}

/////////////////////////////////////////////////////////////////////////

void TopiaryBeatsTransportComponent::processUIChanges(bool override, int& n, int& d, int& bpm, int buttonEnabled) // buttonEnabled; 0=Stop; 1=start; 2=rec
// override of virtual function; we get the (new?) values from the component and update the model
{
	if (!bpm) bpm = 120;
	if (!n) n = 4;
	if(!d) d = 4;

	beatsModel->setBPM(bpm);
	beatsModel->setNumeratorDenominator(n, d);
	beatsModel->setOverrideHostTransport(override);
	if (override) {
		beatsModel->processTransportControls(buttonEnabled);
	}
	//checkModel(); // to catch e.g. bad tempo input - will be done by means of messagelistener!
	
} // processUIChanges

/////////////////////////////////////////////////////////////////////////

void TopiaryBeatsTransportComponent::checkModel()
{
	int BPM;
	int n, d;
	int transportState;
	bool override;
	bool waitFFN;
	beatsModel->getTransportState(BPM, n, d, transportState, override, waitFFN, true);
	updateState(override, BPM, n, d, transportState);
		
	
} // checkModel

/////////////////////////////////////////////////////////////////////////

