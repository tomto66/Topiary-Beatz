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

This code has a generic RT component that can be included in every Topiary plugin.

CAREFUL: needs symbols:
- TOPIARYMODEL e.g. TopiaryBeatsModel
- TOPIARYREALTIMECOMPONENT e.g. TopiaryPresetzRealTimeComponent 

*/
/////////////////////////////////////////////////////////////////////////////


#ifdef TOPIARYREALTIMECOMPONENT

class TOPIARYREALTIMECOMPONENT : public Component, ActionListener
{
public:
	TOPIARYREALTIMECOMPONENT();
	~TOPIARYREALTIMECOMPONENT();

	void paint(Graphics&) override;
	void resized() override;
	void setModel(TOPIARYMODEL* m);
	void actionListenerCallback(const String &message) override;

private:

	TOPIARYMODEL* model;
	TextEditor measuresEditor;
	TextEditor beatsEditor;
	

	//////////////////////////////////////////////////////

	void getTime()
	{
		int m, b;
		
		model->getTime(m, b);
		if (m != measuresEditor.getText().getIntValue())
			measuresEditor.setText(String(m));
		if (b != beatsEditor.getText().getIntValue())
			beatsEditor.setText(String(b));

	} // getTime
	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(TOPIARYREALTIMECOMPONENT)
};

#endif