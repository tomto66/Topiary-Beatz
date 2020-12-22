/////////////////////////////////////////////////////////////////////////////
/*
This file is part of Topiary Beatz, Copyright Tom Tollenaere 2018-20

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
#include "TopiaryBeatsMasterComponent.h"
#include "TopiaryBeatsLogComponent.h"
#include "TopiaryBeatsVariationComponent.h"
#include "TopiaryBeatsComponent.h"
#include "TopiaryBeatsUtilityComponent.h"
#include "TopiaryBeatsPatternComponent.h"

class TopiaryBeatsTabbedComponent:  public Component, ActionListener
{
public:
	TopiaryBeatsTabbedComponent();
    ~TopiaryBeatsTabbedComponent();
	void setModel(TopiaryBeatsModel* model);
    void paint (Graphics&) override;
    void resized() override;  
	void actionListenerCallback(const String &message) override;

private:
	TopiaryBeatsModel* beatsModel;
	TabbedComponent beatsTabs;
	TopiaryBeatsMasterComponent masterComponent;
	TopiaryBeatsVariationComponent variationComponent;
	TopiaryBeatsLogComponent logComponent;
	TopiaryBeatsUtilityComponent utilityComponent;
	TopiaryBeatsPatternComponent patternComponent;

	int tabHeight;
	int height;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (TopiaryBeatsTabbedComponent)
};
