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

#pragma once
#include "../JuceLibraryCode/JuceHeader.h"

#define TMIDIBUFFERSIZE 512

class TopiaryNoteOffBuffer
{ 
public:
	TopiaryNoteOffBuffer();
	~TopiaryNoteOffBuffer();
	void push(int c, int n, int64 ts);
	void pop(int &c, int &n, int64 &ts);
	int64 firstTimeStamp();
	int bufferSize = 0;

private:
	int note[TMIDIBUFFERSIZE];
	int64 timestamp[TMIDIBUFFERSIZE];
	int channel[TMIDIBUFFERSIZE];
};
