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
#include "TopiaryBeatsHeaderComponent.h"
#include "../../Topiary/Topiary.h"
//#include "../JuceLibraryCode/BinaryData.h"
#include "../../Topiary/TopiaryTransportComponent.h"
#include "../../Topiary/TopiaryVariationButtonsComponent.h"
#include "TopiaryBeatsComponent.h"

TopiaryBeatsHeaderComponent::TopiaryBeatsHeaderComponent()
{
	setSize(TopiaryBeatsComponent::width, TopiaryBeatsComponent::headerHeigth);
	addAndMakeVisible(transportComponent);
	addAndMakeVisible(variationButtonsComponent);
}

/////////////////////////////////////////////////////////////////////////

TopiaryBeatsHeaderComponent::~TopiaryBeatsHeaderComponent()
{ 	
	beatsModel->removeListener((ActionListener*)this);
}

void TopiaryBeatsHeaderComponent::setModel(TopiaryBeatsModel* m)
{
	beatsModel = m;
	variationButtonsComponent.setModel(m);
	transportComponent.setModel(m);
	beatsModel->setListener((ActionListener*)this);
	variationButtonsComponent.checkModel();
	transportComponent.checkModel();
	
}
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
}

void TopiaryBeatsHeaderComponent::actionListenerCallback(const String &message)
{
	if (message.compare("variations") == 0)
		variationButtonsComponent.checkModel();
	else if (message.compare("transport") == 0)
		transportComponent.checkModel();
}





