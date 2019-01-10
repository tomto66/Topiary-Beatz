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

This code has a generic automation component that can be included in every Topiary plugin.

CAREFUL: needs symbols:
- TOPIARYMODEL e.g. TOPIARYMODEL
- TOPIARYAUTOMATIONCOMPONENT e.g. TopiaryPresetzAutomationComponent (a class definition)

*/
/////////////////////////////////////////////////////////////////////////////

#ifdef TOPIARYAUTOMATIONCOMPONENT

class TOPIARYAUTOMATIONCOMPONENT : public Component, ActionListener
{
public:
	TOPIARYAUTOMATIONCOMPONENT();
	~TOPIARYAUTOMATIONCOMPONENT();

	void paint(Graphics&) override;
	void resized() override;
	void setModel(TOPIARYMODEL* m);
	void actionListenerCallback(const String &message) override;
	void getVariationControl();
	void setVariationControl();

private:
	TOPIARYMODEL* model;

	TopiaryButton notesButton;
	TopiaryButton ccButton;
	TextEditor variationChannelEditor;
	TextEditor variationControlEditor[8];

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(TOPIARYAUTOMATIONCOMPONENT)
};

#endif
/////////////////////////////////////////////////////////////////////