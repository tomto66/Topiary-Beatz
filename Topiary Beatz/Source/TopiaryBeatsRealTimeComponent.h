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
#include"../../Topiary/TopiaryTable.h"  // temp for testing
#include"TopiaryBeatsModel.h"

class TopiaryBeatsRealtimeComponent : public Component, ActionListener
{
public:
	TopiaryBeatsRealtimeComponent();
	~TopiaryBeatsRealtimeComponent();

	void paint(Graphics&) override;
	void resized() override;
	void setModel(TopiaryBeatsModel* m);
	void actionListenerCallback(const String &message) override;

private:

	TopiaryBeatsModel* beatsModel;
	TextEditor measuresEditor;
	TextEditor beatsEditor;
	

	//////////////////////////////////////////////////////

	void getTime()
	{
		int m, b;
		
		beatsModel->getTime(m, b);
		if (m != topiaryStoi(measuresEditor.getText()))
			measuresEditor.setText(String(m));
		if (b != topiaryStoi(beatsEditor.getText()))
			beatsEditor.setText(String(b));

	} // getTime
	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(TopiaryBeatsRealtimeComponent)
};

