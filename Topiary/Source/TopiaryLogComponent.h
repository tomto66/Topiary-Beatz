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

class TOPIARYLOGCOMPONENT : public Component, ActionListener
{
public:
	TOPIARYLOGCOMPONENT();
	~TOPIARYLOGCOMPONENT();

	void paint(Graphics&) override;
	void resized() override;
	void setModel(TOPIARYMODEL* m);
	void actionListenerCallback(const String &message) override;

private:
	TOPIARYMODEL* model;
	TextEditor logEditor;
	int h = 360;
	int w = 750;

	TopiaryButton midiInButton;
	TopiaryButton midiOutButton;
	TopiaryButton infoButton;
	TopiaryButton transportButton;
	TopiaryButton variationsButton;
	TextButton clearButton;


	static const int bW = 120; //  button sizes
	static const int bH = 20;

	void processLogButtons()
	{
		
		model->setLogSettings( true, midiInButton.getToggleState(), midiOutButton.getToggleState(), 
								transportButton.getToggleState(), variationsButton.getToggleState(), infoButton.getToggleState());
	}

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(TOPIARYLOGCOMPONENT)
};

#endif


/////////////////////////////////////////////////////////////////////