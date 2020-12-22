/*
This file is part of Topiary, Copyright Tom Tollenaere 2018-2020.

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

This code has a generic transport component that can be included in every Topiary plugin.

CAREFUL: needs symbols:
- TOPIARYMODEL e.g. TOPIARYMODEL
- TOPIARYVARIATIONBUTTONSCOMPONENT e.g. TopiaryPresetzVariationButtonsComponent (a class definition)

*/
/////////////////////////////////////////////////////////////////////////////

#ifdef TOPIARYVARIATIONBUTTONSCOMPONENT

class TOPIARYVARIATIONBUTTONSCOMPONENT : public Component 
{
public:
	TOPIARYVARIATIONBUTTONSCOMPONENT();
	~TOPIARYVARIATIONBUTTONSCOMPONENT();
	void paint(Graphics& g) override;
	void resized() override;
	void selectButton(int running, int selected);
	void enableButton(int i, bool enable);
	void setModel(TOPIARYMODEL *m);
	void checkModel();
	void updateToggleState(TextButton*);
	void getEnabled();     // get enablement data from model and enable/disable buttons

private:
	TopiaryLookAndFeel topiaryLookAndFeel;
	TOPIARYMODEL *model;
	TopiaryButton	button0, button1, button2, button3, button4, button5, button6, button7, button8;

	TopiaryButton* variationButton[8];
	int buttonWidth = 37;
	int buttonHeight = 15;
	int width = 340;
	int height = 45;

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(TOPIARYVARIATIONBUTTONSCOMPONENT)
  	
};

#endif