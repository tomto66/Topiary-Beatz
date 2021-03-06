/////////////////////////////////////////////////////////////////////////////
/*
This file is part of Topiary Beatz, Copyright Tom Tollenaere 2018-21.

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
#include "TopiaryBeatsModel.h"
#include "TopiaryBeatsTransportComponent.h"
#include "TopiaryBeatsVariationButtonsComponent.h"

class TopiaryBeatsHeaderComponent : public Component,  ActionListener, Timer
{
public:

	TopiaryBeatsHeaderComponent();
	~TopiaryBeatsHeaderComponent();
	void setModel(TopiaryBeatsModel* m);
	void paint(Graphics& g) override;
	void resized() override;
	void actionListenerCallback(const String &message) override;
	void timerCallback();

private:
	TopiaryBeatsTransportComponent transportComponent;
	TopiaryBeatsVariationButtonsComponent variationButtonsComponent;
	TopiaryBeatsModel* beatsModel;
	TextEditor warningEditor;
	TextEditor lockedEditor;
	TextEditor timeEditor;

	//////////////////////////////////////////////////////

	void getTime()
	{
		int m, b;
		String timing;
		
		beatsModel->getTime(m, b);
		timing = String(m) + " : " + String(b);

		if (timeEditor.getText().compare(timing) !=0)
			timeEditor.setText(timing);

	} // getTime

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(TopiaryBeatsHeaderComponent)
};



