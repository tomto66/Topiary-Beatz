/////////////////////////////////////////////////////////////////////////////
/*
This file is part of Topiary Beats, Copyright Tom Tollenaere 2018-2019.

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
#define TOPIARYTRANSPORTCOMPONENT TopiaryBeatsTransportComponent
#include "../../Topiary/Source/TopiaryTransportComponent.h"

/*
class TopiaryBeatsTransportComponent : public TopiaryTransportComponent
{
public:
	TopiaryBeatsTransportComponent();
	~TopiaryBeatsTransportComponent();
	void setModel(TopiaryBeatsModel *m);
	void checkModel();
	void processUIChanges(bool override, int& n, int& d, int& bpm, int buttonEnabled) override;

private:
	TopiaryBeatsModel* beatsModel;
	
	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(TopiaryBeatsTransportComponent)

};
*/