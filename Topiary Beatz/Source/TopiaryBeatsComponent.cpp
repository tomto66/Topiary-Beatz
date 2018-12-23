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
#include "TopiaryBeatsComponent.h"

const int TopiaryBeatsComponent::headerHeigth = 75;
const int TopiaryBeatsComponent::width = 1000;
const int TopiaryBeatsComponent::heigth = 500;

TopiaryBeatsComponent::TopiaryBeatsComponent()
{   // don't do too much - model needs to be set first !!
	//getLookAndFeel().setColour(Component:: , Topiary::SLIDER_TRACE_COLOUR);
	//jassert(false);
}

void TopiaryBeatsComponent::setModel(TopiaryBeatsModel* model)
{
	beatsModel = model;
	topiaryBeatsHeaderComponent.setModel(beatsModel); // size set in headerComponent
	addAndMakeVisible(topiaryBeatsHeaderComponent);
	topiaryBeatsTabbedComponent.setModel(beatsModel);
	addAndMakeVisible(topiaryBeatsTabbedComponent); // size set in tabbedComponent 	
}

/////////////////////////////////////////////////////////////////////////

TopiaryBeatsComponent::~TopiaryBeatsComponent()
{	
}



/////////////////////////////////////////////////////////////////////////

void TopiaryBeatsComponent::resized()
{ 
}

/////////////////////////////////////////////////////////////////////////

void TopiaryBeatsComponent::paint(Graphics& g) {
	auto area = getLocalBounds();
	
	g.fillAll(TopiaryColour::background);
	topiaryBeatsTabbedComponent.setBounds(area.removeFromBottom(TopiaryBeatsComponent::heigth-TopiaryBeatsComponent::headerHeigth-10));
	topiaryBeatsHeaderComponent.setBounds(area.removeFromTop(TopiaryBeatsComponent::headerHeigth));
	
}









