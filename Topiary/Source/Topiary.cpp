/////////////////////////////////////////////////////////////////////////////
/*
This file is part of Topiary, Copyright Tom Tollenaere 2018-19.

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
*/
/////////////////////////////////////////////////////////////////////////////

#include "Topiary.h"

const Colour TopiaryColour::background = Colour(47, 50, 52);
const Colour TopiaryColour::foreground = Colour(115, 115, 115);
const Colour TopiaryColour::warning = Colour(127, 10, 10);
const Colour TopiaryColour::rec = Colour(127, 10, 10); // for record button
const Colour TopiaryColour::green = Colour(4, 99, 10);  //green
const Colour TopiaryColour::brightBlue = Colour(3, 100, 165); // blue, also used for slider trace
const Colour TopiaryColour::lightBlue = Colour(0, 150, 250); // blue (thumb)
const Colour TopiaryColour::darkBlue = Colour(0, 60, 110); // blue (dark trace)
const Colour TopiaryColour::orange = Colour(255, 138, 0);
const Colour TopiaryColour::yellow = Colour(255, 255, 0);
const Colour TopiaryColour::purple = Colour(172, 0, 172);
const Colour TopiaryColour::paleBlue = Colour(178, 227, 237);

TopiaryLookAndFeel::TopiaryLookAndFeel()
{
	setColour(ScrollBar::thumbColourId, TopiaryColour::brightBlue);
}

/////////////////////////////////////////////////////////////////////////////

TopiaryLookAndFeel::~TopiaryLookAndFeel()
{
}

/////////////////////////////////////////////////////////////////////////////

void TopiaryLookAndFeel::drawTopiaryButtonBackground(Graphics& g,
												TopiaryButton& button,
												const Colour& backgroundColour,
												bool isMouseOverButton,
												bool isButtonDown)
{
	auto cornerSize = 2.0f;
	auto bounds = button.getLocalBounds().toFloat().reduced(0.5f, 0.5f);

	Colour baseColour = backgroundColour;

	if (isButtonDown) {
		baseColour = TopiaryColour::brightBlue;
		
		// if specialState == armed (3) or ending(5) or recording (7) we do something special
		if (button.specialState == 3) 		baseColour = TopiaryColour::orange;
		else if (button.specialState == 5)  baseColour = TopiaryColour::purple;
		else if (button.specialState == 7)  baseColour = TopiaryColour::rec; // recording
		
	}
	if (isButtonDown || isMouseOverButton)
		baseColour = baseColour.contrasting(isButtonDown ? 0.2f : 0.05f);
	 
	
	if (!isButtonDown && (button.getToggleState()))
	{
		// if specialState == armed (3) or ending(5) we do something special
		if (button.specialState == Topiary::TopiaryButtonSpecialState::waitToApply) 		
			baseColour = TopiaryColour::orange;
		else if (button.specialState == 7)
			baseColour = TopiaryColour::rec;
		else
		{
			if (button.specialState == Topiary::TopiaryButtonSpecialState::waitToEnd)		
				baseColour = TopiaryColour::purple;
			else baseColour = TopiaryColour::brightBlue;
		}	
	}
	

	baseColour = baseColour.withMultipliedSaturation(button.hasKeyboardFocus(true) ? 1.3f : 0.9f)
		.withMultipliedAlpha(button.isEnabled() ? 1.0f : 0.5f);

	g.setColour(baseColour);

	if (button.isConnectedOnLeft() || button.isConnectedOnRight())
	{
		Path path;
		path.addRoundedRectangle(bounds.getX(), bounds.getY(),
			bounds.getWidth(), bounds.getHeight(),
			cornerSize, cornerSize,
			!button.isConnectedOnLeft(),
			!button.isConnectedOnRight(),
			!button.isConnectedOnLeft(),
			!button.isConnectedOnRight());

		g.fillPath(path);

		g.setColour(button.findColour(ComboBox::outlineColourId));
		g.strokePath(path, PathStrokeType(1.0f));
	}
	else
	{
		g.fillRoundedRectangle(bounds, cornerSize);

		g.setColour(button.findColour(ComboBox::outlineColourId));
		g.drawRoundedRectangle(bounds, cornerSize, 1.0f);
	}
} // drawButtonBackground

///////////////////////////////////////////////////////////////////////

void TopiaryLookAndFeel::drawRotarySlider(Graphics& g, int x, int y, int width, int height, float sliderPos,
	const float rotaryStartAngle, const float rotaryEndAngle, Slider& slider)
{
	auto fill = TopiaryColour::orange;
	auto outline = TopiaryColour::brightBlue;

	auto bounds = Rectangle<int>(x, y, width, height).toFloat().reduced(10);

	auto radius = jmin(bounds.getWidth(), bounds.getHeight()) / 2.0f;
	auto toAngle = rotaryStartAngle + sliderPos * (rotaryEndAngle - rotaryStartAngle);
	//auto lineW = jmin(8.0f, radius * 0.5f);
	auto lineW = 3.0f;
	auto arcRadius = radius - lineW * 0.5f ;

	Path backgroundArc;
	backgroundArc.addCentredArc(bounds.getCentreX(),
		bounds.getCentreY(),
		arcRadius,
		arcRadius,
		0.0f,
		rotaryStartAngle,
		rotaryEndAngle,
		true);

	g.setColour(outline);
	g.strokePath(backgroundArc, PathStrokeType(lineW, PathStrokeType::curved, PathStrokeType::rounded));

	if (slider.isEnabled())
	{
		Path valueArc;
		valueArc.addCentredArc(bounds.getCentreX(),
			bounds.getCentreY(),
			arcRadius,
			arcRadius,
			0.0f,
			rotaryStartAngle,
			toAngle,
			true);

		g.setColour(fill);
		g.strokePath(valueArc, PathStrokeType(lineW, PathStrokeType::curved, PathStrokeType::rounded));
	}

	//auto thumbWidth = lineW * 2.0f;
	auto thumbWidth = 8.0f;
	Point<float> thumbPoint(bounds.getCentreX() + arcRadius * std::cos(toAngle - MathConstants<float>::halfPi),
		bounds.getCentreY() + arcRadius * std::sin(toAngle - MathConstants<float>::halfPi));

	g.setColour(TopiaryColour::paleBlue);
	
	g.fillEllipse(Rectangle<float>(thumbWidth, thumbWidth).withCentre(thumbPoint));
} // drawRotarySlider


//////////////////////////////////////////////////////////////////////////

String validateNote(String newNote)
{
	// see if this a valid note; if not return "C2"
	// this DOES allow things like E# (which is F) and B# (which is C) -- but B#4 == C4, not C5 !!!

	String validatedNote;
	auto c = static_cast<const char*> (newNote.toUTF8());
	char note = ' ';
	int number = -1;
	char sharp = '!';

	if (newNote.length() == 2)
	{
		sscanf((char*)c, "%1c%1d", &note, &number);
	}
	else
	{
		sscanf((char*)c, "%1c%1c%d", &note, &sharp, &number);
	}
	
	if ((number < 0) || (number > 8))
		return "C2";

	note = (char) toupper(note);
	if (((int)note < (int)'A') || ((int)note > (int)'G'))
		return "C2";

	if (sharp == '!')
		validatedNote = String(&note,1).toUpperCase() + String(number);
	else 
		validatedNote = String(&note,1).toUpperCase() + String("#") + String(number);

	//Logger::outputDebugString(String("input ") + String(newNote));
	//Logger::outputDebugString(String("output ") + String(validatedNote));

	return validatedNote;
} // validateNote

//////////////////////////////////////////////////////////////////////////

int validNoteNumber(String inNote)
{
	// returns the midi note number of a formatted note name; assume sharps and octaves 1-8
	// assumes the note string has been validated !!!
	auto c = static_cast<const char*> (inNote.toUTF8());
	char note = ' ';
	int octave = -1;
	char sharp = '!';
	int number=0;

	if (inNote.length() == 2)
	{
		sscanf((char*)c, "%1c%1d", &note, &octave);
	}
	else
	{
		sscanf((char*)c, "%1c%1c%d", &note, &sharp, &octave);
	}

	switch ((int)note)
	{
	case (int)'C': number = 0;
		break;
	case (int)'D': number = 2;
		break;
	case (int)'E': number = 4;
		break;
	case (int)'F': number = 5;
		break;
	case (int)'G': number = 7;
		break;
	case (int)'A': number = 9;
		break;
	case (int)'B': number = 11; 
		break;
		default: jassert(0);

	}

	if (sharp == '#') number++;
	number = number + (12 * octave);

	//Logger::outputDebugString(String("input ") + String(inNote));
	//Logger::outputDebugString(String("output ") + String(number));

	return number;

}  // validNoteNumber

//////////////////////////////////////////////////////////////////////////

void TopiaryButton::paintButton(Graphics& g, bool isMouseOverButton, bool isButtonDown)
{
	LookAndFeel& lf = getLookAndFeel();

	TopiaryLookAndFeel::drawTopiaryButtonBackground(g, *this,
		findColour(getToggleState() ? buttonOnColourId : buttonColourId),
		isMouseOverButton, isButtonDown);

	lf.drawButtonText(g, *this, isMouseOverButton, isButtonDown);
}

//////////////////////////////////////////////////////////////////////////

TopiaryButton::TopiaryButton()
{
}

//////////////////////////////////////////////////////////////////////////

TopiaryButton::~TopiaryButton()
{
}

//////////////////////////////////////////////////////////////////////////

void boolSwap(bool &a, bool &b)
{
	bool r = a;
	a = b;
	b = r;
}

//////////////////////////////////////////////////////////////////////////

void intSwap(int &a, int &b)
{
	int r = a;
	a = b;
	b = r;
}

//////////////////////////////////////////////////////////////////////////

void stringSwap(String &a, String &b)
{
	String r = a;
	a = b;
	b = r;
}
