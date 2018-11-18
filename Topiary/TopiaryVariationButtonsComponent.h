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
#include "Topiary.h"

class TopiaryVariationButtonsComponent : public Component 
{
public:
	TopiaryVariationButtonsComponent();
	~TopiaryVariationButtonsComponent();
	void paint(Graphics& g) override;
	void resized() override;
	virtual void updateToggleState(TextButton*);
	void selectButton(int running, int selected);

private:
	TopiaryLookAndFeel topiaryLookAndFeel;

	TopiaryButton	button0, button1, button2, button3, button4, button5, button6, button7, button8;

	TopiaryButton* variationButton[8];
	int buttonWidth = 37;
	int buttonHeight = 15;
	int width = 340;
	int height = 45;

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(TopiaryVariationButtonsComponent)
  
	
};