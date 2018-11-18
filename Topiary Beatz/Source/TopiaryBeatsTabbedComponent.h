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
#include "TopiaryBeatsMasterComponent.h"
#include "TopiaryBeatsLogComponent.h"
#include "TopiaryBeatsVariationComponent.h"
#include "TopiaryBeatsRealTimeComponent.h"
#include "TopiaryBeatsComponent.h"

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
	TopiaryBeatsRealtimeComponent realtimeComponent;

	int tabHeight;
	int height;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (TopiaryBeatsTabbedComponent)
};
