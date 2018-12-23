/////////////////////////////////////////////////////////////////////////////
/*
This file is part of Topiary, Copyright Tom Tollenaere 2018.

Topiary Beats is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

Topiary Beats is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Topiary. If not, see <https://www.gnu.org/licenses/>.
*/
/////////////////////////////////////////////////////////////////////////////

#pragma once
#include "../JuceLibraryCode/JuceHeader.h"
#include "Topiary.h"


class TopiaryTransportComponent : public Component
{
public:
	TopiaryTransportComponent();
	~TopiaryTransportComponent();
	void paint(Graphics& g) override;
	void resized() override;
	virtual void setOverride(bool override);
	void updateToggleState(TextButton*);
	void updateState(bool override, int bpm, int n, int d, int state);
	virtual void processUIChanges(bool override, int& n, int& d, int& bpm, int buttonEnabled);

private:
	TopiaryLookAndFeel topiaryLookAndFeel;

	int buttonW = 30;
	int buttonH = 15;
	int width = 350;
	int height = 45;

	TopiaryButton startButton;
	TopiaryButton stopButton;
	TopiaryButton recButton;

	ToggleButton overrideButton;
	int overrideW = 100;

	TextEditor bpmEditor;
	int bpmW = 30;
	TextEditor numeratorEditor, denominatorEditor;
	int sigW = 15;
	
	
	int signatureW = 55;
	
	void callProcessUIChanges()
	{
		// decode the transportstate from the buttons selected
		// buttonEnabled; 0=Stop; 1=start; 2=rec 
		int buttonEnabled = -1;
		
		if (stopButton.getToggleState())  buttonEnabled = 0;
		if (startButton.getToggleState()) buttonEnabled = 1;
		if (recButton.getToggleState())   buttonEnabled = 2;
		int n = numeratorEditor.getText().getIntValue();
		int d = denominatorEditor.getText().getIntValue();
		int b = bpmEditor.getText().getIntValue();
		if (buttonEnabled != -1)
			processUIChanges(overrideButton.getToggleState(), n, d, b, buttonEnabled); // virtual function because it needs the model!
		// no elses, screwed up when there was no button selected (yet) - can happen when toggling !!! spent hours figuring this out!

		numeratorEditor.setText(String(n));
		denominatorEditor.setText(String(d));
		bpmEditor.setText(String(b));

	};

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(TopiaryTransportComponent)
}; // end of Class