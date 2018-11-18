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
#include "TopiaryBeatsVariationComponent.h"


TopiaryBeatsVariationComponent::TopiaryBeatsVariationComponent()
{   // size set by paint!

	addAndMakeVisible(randomNoteComponent);
	addAndMakeVisible(variationDefinitionComponent);




} // TopiaryBeatsVariationComponent() 

/////////////////////////////////////////////////////////////////////////

TopiaryBeatsVariationComponent::~TopiaryBeatsVariationComponent()
{
} // ~TopiaryBeatsVariationComponent

/////////////////////////////////////////////////////////////////////////

void TopiaryBeatsVariationComponent::setModel(TopiaryBeatsModel* m)
{
	beatsModel = m;
	
	getSettings();

} //setModel

/////////////////////////////////////////////////////////////////////////

void TopiaryBeatsVariationComponent::paint(Graphics& g)
{
	UNUSED(g)
	randomNoteComponent.setBounds(100, 100, 20 + randomNoteComponent.width, 20 + randomNoteComponent.heigth);
	variationDefinitionComponent.setBounds(10, 10, 10 + variationDefinitionComponent.width, 10 + variationDefinitionComponent.heigth);
} // paint

void TopiaryBeatsVariationComponent::resized()
{
} // resized


//////////////////////////////////////////////////
// RandomNoteComponent
//////////////////////////////////////////////////

