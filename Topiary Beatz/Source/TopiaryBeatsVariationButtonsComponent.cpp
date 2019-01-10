/////////////////////////////////////////////////////////////////////////////
/*
This file is part of Topiary Beats, Copyright Tom Tollenaere 2018-19.

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
#include "TopiaryBeatsVariationButtonsComponent.h"
#include "../../Topiary/Source/TopiaryVariationButtonsComponent.cpp"
									
/*
TopiaryBeatsVariationButtonsComponent::TopiaryBeatsVariationButtonsComponent()
{
}

/////////////////////////////////////////////////////////////////////////////

TopiaryBeatsVariationButtonsComponent::~TopiaryBeatsVariationButtonsComponent()
{
}

/////////////////////////////////////////////////////////////////////////////

void TopiaryBeatsVariationButtonsComponent::setModel(TopiaryBeatsModel *m)
{
	beatsModel = m;
	getEnabled();  // see if there are disabled variationbuttons
} // setModel

/////////////////////////////////////////////////////////////////////////////

void TopiaryBeatsVariationButtonsComponent::updateToggleState(TextButton* button)
{
	String buttonText = button->getButtonText();
	beatsModel->setVariation(buttonText.getIntValue()-1);
} // updateToggleState

/////////////////////////////////////////////////////////////////////////////

void TopiaryBeatsVariationButtonsComponent::checkModel()
{  // Called by the HeaderComponent timer to check whether the model has changed; if so adapt accordingly
	
		int running;
		int selected;
		beatsModel->getVariation(running, selected);
		if (selected == running)
			selectButton(running,running);
		else
			selectButton(selected,running);
	
} // checkModel

/////////////////////////////////////////////////////////////////////////////

void TopiaryBeatsVariationButtonsComponent::getEnabled()
{
	bool enables[8];
	beatsModel->getVariationEnables(enables);
		for (int i = 0; i < 8; i++)
			enableButton(i, enables[i]);

} // getEnabled

/////////////////////////////////////////////////////////////////////////////
*/