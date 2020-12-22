/////////////////////////////////////////////////////////////////////////////
/*
This file is part of Topiary, Copyright Tom Tollenaere 2020.

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

#include "TopiaryKeytracker.h"

TopiaryKeytracker::TopiaryKeytracker() {
	
} // TopiaryKeytracker

/////////////////////////////////////////////////////////////////////////////

TopiaryKeytracker::~TopiaryKeytracker() {
	 
} // ~TopiaryKeytracker
 
///////////////////////////////////////////////////////////////////////////// 

void TopiaryKeytracker::push(int note)
{
	pop(note); // make sure that if it was already inthere, it's gone now
	
	keys[bufferSize] = note;
	bufferSize++;
	calcNextnote();

	if (bufferSize == KEYTRACKERBUFFER)
	{
		jassert(false);
		pop(keys[0]); // make space
		calcNextnote();
	}
	/*
	if (DEBUG)
	{
		Logger::outputDebugString("------------------------");
		for (int i = 0; i<bufferSize; i++)
			Logger::outputDebugString("<" + String(keys[i]) + ">" + MidiMessage::getMidiNoteName(keys[i], true, true, 5) );
		Logger::outputDebugString("------------------------");
	}
	*/
} // push

/////////////////////////////////////////////////////////////////////////////

void TopiaryKeytracker::pop(int note)
{
	// find this note and remove it from the buffer
	// note may NOT be inthere multiple times!
	// that's why there is a pop in push()

	for (int i = 0; i < bufferSize; i++)
	{
		// find the note
		if (keys[i] == note)
		{
			// shift everything up
			bufferSize--;

			for (int j = i; j < bufferSize; j++)
				keys[j] = keys[j + 1];

			i = bufferSize;
		}
	}
	calcNextnote();

	/*
	if (DEBUG)
	{
		Logger::outputDebugString("------------------------");
		for (int i = 0; i<bufferSize; i++)
			Logger::outputDebugString("<" + String(keys[i]) + ">" + MidiMessage::getMidiNoteName(keys[i], true, true, 5));
		Logger::outputDebugString("------------------------");
	}
	*/
} // pop

/////////////////////////////////////////////////////////////////////////////

void TopiaryKeytracker::init()
{
	bufferSize = 0;
	notePlaying = -1;		// note that is generating output
	nextNotePlaying = -1;
} // init