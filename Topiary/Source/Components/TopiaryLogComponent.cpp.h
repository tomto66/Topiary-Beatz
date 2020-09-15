/////////////////////////////////////////////////////////////////////////////
/*
This file is part of Topiary, Copyright Tom Tollenaere 2018-2019.

Topiary is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

Topiary is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Topiary. If not, see <https://www.gnu.org/licenses/>.

/////////////////////////////////////////////////////////////////////////////

This code has a generic log component that can be included in every Topiary plugin.

CAREFUL: needs symbols:
- TOPIARYMODEL e.g. TopiaryPadzModel
- TOPIARYLOGCOMPONENT e.g. TopiaryPresetzLogComponent 

*/
/////////////////////////////////////////////////////////////////////////////

#ifdef TOPIARYLOGCOMPONENT

TOPIARYLOGCOMPONENT::TOPIARYLOGCOMPONENT()
{   // size set by TabbedComponent!

	addAndMakeVisible(logEditor);
	logEditor.setMultiLine(true);
	logEditor.setReadOnly(true);
	logEditor.setSize(w, h);

	addAndMakeVisible(midiInButton);
	midiInButton.setButtonText("MIDI In");
	midiInButton.setSize(bW, bH);
	midiInButton.setClickingTogglesState(true);
	midiInButton.onClick = [this] {
		processLogButtons();
	};

	addAndMakeVisible(midiOutButton);
	midiOutButton.setButtonText("MIDI Out");
	midiOutButton.setSize(bW, bH);
	midiOutButton.setClickingTogglesState(true);
	midiOutButton.onClick = [this] {
		processLogButtons();
	};

	addAndMakeVisible(infoButton);
	infoButton.setButtonText("Info");
	infoButton.setSize(bW, bH);
	infoButton.setClickingTogglesState(true);
	infoButton.onClick = [this] {
		processLogButtons();
	};

	addAndMakeVisible(transportButton);
	transportButton.setButtonText("Transport");
	transportButton.setSize(bW, bH);
	transportButton.setClickingTogglesState(true);
	transportButton.onClick = [this] {
		processLogButtons();
	};
	
	addAndMakeVisible(variationsButton);
	variationsButton.setButtonText("Variations");
	variationsButton.setSize(bW, bH);
	variationsButton.setClickingTogglesState(true);
	variationsButton.onClick = [this] {
		processLogButtons();
	};

	addAndMakeVisible(clearButton);
	clearButton.setButtonText("Clear");
	clearButton.setSize(bW, bH);
	clearButton.onClick = [this] {
		model->clearLog();
	};

} // TOPIARYLOGCOMPONENT

/////////////////////////////////////////////////////////////////////////


TOPIARYLOGCOMPONENT::~TOPIARYLOGCOMPONENT()
{
	model->removeListener((ActionListener*)this);
} //~TOPIARYLOGCOMPONENT

/////////////////////////////////////////////////////////////////////////

void TOPIARYLOGCOMPONENT::setModel(TOPIARYMODEL* m)
{
	model = m;
	model->setListener((ActionListener*)this);
	logEditor.setText(*(model->getLog())); // init the log window
	actionListenerCallback(MsgLog);

} // setModel

/////////////////////////////////////////////////////////////////////////

void TOPIARYLOGCOMPONENT::paint(Graphics& g)
{
	auto area = getLocalBounds();
	logEditor.setBounds(100, 10, w-90, h);
		
	g.fillAll(TopiaryColour::background);
	g.setColour(TopiaryColour::foreground);
	g.setFont(12.0f);

	clearButton.setBounds(w + 30, 10, bW, bH);
	midiInButton.setBounds(w + 30, 40, bW, bH);
	midiOutButton.setBounds(w + 30, 70, bW, bH);
	variationsButton.setBounds(w + 30, 100, bW, bH);
	transportButton.setBounds(w + 30, 130, bW, bH);
	infoButton.setBounds(w + 30, 160, bW, bH);
	
} // paint

/////////////////////////////////////////////////////////////////////////

void TOPIARYLOGCOMPONENT::resized()
{
} //resized

/////////////////////////////////////////////////////////////////////////

void TOPIARYLOGCOMPONENT::actionListenerCallback(const String &message)
{
	if (message.compare(MsgLog) == 0)
	{
		int a = model->getLog()->length();
		int b = logEditor.getText().length();

		if (abs(a - b) > 10)
		{
			logEditor.setText(*(model->getLog()));
			logEditor.moveCaretToEnd(); // keep scroll at bottom
			bool warning, midiIn, midiOut, transport, variations, info;
			model->getLogSettings(warning, midiIn, midiOut, transport, variations, info);
			
			midiInButton.setToggleState(midiIn, dontSendNotification);
			midiOutButton.setToggleState(midiOut, dontSendNotification);
			transportButton.setToggleState(transport, dontSendNotification);
			variationsButton.setToggleState(variations, dontSendNotification);
			infoButton.setToggleState(info, dontSendNotification);
			
		}
	}
} // actionListenerCallback

#endif