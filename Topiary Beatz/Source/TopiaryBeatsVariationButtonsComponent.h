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
#include"TopiaryBeatsModel.h"
#define TOPIARYMODEL TopiaryBeatsModel
#define TOPIARYVARIATIONBUTTONSCOMPONENT TopiaryBeatsVariationButtonsComponent
#include "../../Topiary/Source/TopiaryVariationButtonsComponent.h"

/*
class TopiaryBeatsVariationButtonsComponent : public TopiaryVariationButtonsComponent
{
public:
	TopiaryBeatsVariationButtonsComponent();
	~TopiaryBeatsVariationButtonsComponent();
	void setModel(TopiaryBeatsModel *m);
	void checkModel();
	void updateToggleState(TextButton*) override;
	void getEnabled();     // get enablement data from model and enable/disable buttons

private:
	TopiaryBeatsModel* beatsModel;
	
	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(TopiaryBeatsVariationButtonsComponent)

};
*/