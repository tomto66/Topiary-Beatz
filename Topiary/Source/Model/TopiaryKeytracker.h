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

#pragma once
#include "../../Topiary/Source/Topiary.h"


#define KEYTRACKERBUFFER 512

class TopiaryKeytracker
{ 

public:
	TopiaryKeytracker();
	~TopiaryKeytracker();

	enum NoteOrder : int
	{
		First = 3604,
		Last = 3605,
		Lowest = 3607,
		Highest = 3608
	};

	int noteOrder= First;
	int notePlaying = -1;		// note that is generating output
	int nextNotePlaying = -1;	// if <> notePlaying, next one to use to generate output
	int lastNotePlaying = -1;
	int bufferSize = 0;

	void push(int note);
	void pop(int note);
	void init();

private:

	int keys[KEYTRACKERBUFFER];

	void calcNextnote()
	{
		// depending on noteOrder, decide what the next note to play is or would be
		// called by push & pop

		if (bufferSize == 0)
		{
			nextNotePlaying = -1;
			return;  // do nothing; if latch is on it will keep generating as it was left
		}
		int lowest = 1000;
		int highest = -1;

		switch (noteOrder)
		{
		case First:
			if (keys[0] != notePlaying)
				nextNotePlaying = keys[0];
			break;
			 
		case Last: 
			if (keys[bufferSize-1] != notePlaying)
				nextNotePlaying = keys[bufferSize-1];

			break;

		case Lowest:
			// find lowest note
		
			for (int i = 0; i < bufferSize; i++)
				if (keys[i] < lowest)
					lowest = keys[i];

			if (lowest != notePlaying)
				nextNotePlaying = lowest; 

			break;

		case Highest:
			// find highest note
			
			for (int i = 0; i < bufferSize; i++)
				if (keys[i] > highest)
					highest = keys[i];

			if (highest != notePlaying)
				nextNotePlaying = highest;

			break;
		default: 
			jassert(false);
		} // switch

		
	}

}; // TopiaryKeytracker
