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
#include "../JuceLibraryCode/JuceHeader.h"
#include "../../Topiary/Source/Topiary.h"
#include "TopiaryBeatsModel.h"
#include "TopiaryBeatsComponent.h"
#include "TopiaryBeatsHeaderComponent.h"
#include "TopiaryBeatsTabbedComponent.h"

class TopiaryBeatsComponent : public Component
{
public:
	TopiaryBeatsComponent();
	~TopiaryBeatsComponent();
	void setModel(TopiaryBeatsModel* beatsmodel);
	void paint(Graphics& g) override;
	void resized() override;
	static const int headerHeigth;
	static const int width;
	static const int heigth;

private:
	TopiaryBeatsModel* beatsModel;  
	
	TopiaryBeatsHeaderComponent topiaryBeatsHeaderComponent;
	TopiaryBeatsTabbedComponent topiaryBeatsTabbedComponent;

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(TopiaryBeatsComponent)

};



