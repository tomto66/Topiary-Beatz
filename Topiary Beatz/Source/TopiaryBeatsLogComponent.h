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
#define TOPIARYLOGCOMPONENT TopiaryBeatsLogComponent
#include "../../Topiary/Source/TopiaryLogComponent.h"
/*

#pragma once

#include"TopiaryBeatsModel.h"

class TopiaryBeatsLogComponent : public Component, ActionListener
{
public:
	TopiaryBeatsLogComponent();
	~TopiaryBeatsLogComponent();

	void paint(Graphics&) override;
	void resized() override;
	void setModel(TopiaryBeatsModel* m);
	void actionListenerCallback(const String &message) override;

private:
	TopiaryBeatsModel* beatsModel;
	TextEditor logEditor;
	int h = 360;
	int w = 750;

	TopiaryButton midiInButton;
	TopiaryButton midiOutButton;
	TopiaryButton infoButton;
	TopiaryButton transportButton;
	TopiaryButton variationsButton;
	TopiaryButton debugButton;
	TextButton clearButton;


	static const int bW = 120; //  button sizes
	static const int bH = 20;

	void processLogButtons()
	{
		
		beatsModel->setBeatsLogSettings( true, midiInButton.getToggleState(), midiOutButton.getToggleState(), 
								debugButton.getToggleState(), transportButton.getToggleState(), variationsButton.getToggleState(), infoButton.getToggleState());
	}

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(TopiaryBeatsLogComponent)
};

/////////////////////////////////////////////////////////////////////
*/