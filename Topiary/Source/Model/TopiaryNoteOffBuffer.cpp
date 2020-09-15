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

#include "TopiaryNoteOffBuffer.h"

TopiaryNoteOffBuffer::TopiaryNoteOffBuffer()
{
	bufferSize = 0;
}

/////////////////////////////////////////////////////////////////////////////

TopiaryNoteOffBuffer::~TopiaryNoteOffBuffer()
{
}

/////////////////////////////////////////////////////////////////////////////

void TopiaryNoteOffBuffer::push(int c, int n, int64 ts)
{
	int index=0;
	
	while ((ts > timestamp[index]) && (index < bufferSize))
	{
		index++;
	}

	// make space
	if (index < TMIDIBUFFERSIZE)
	{
		// need to make space to insert
		for (int i = index; i < bufferSize; i++)
		{
				note[i + 1] = note[i];
				timestamp[i + 1] = timestamp[i];
				channel[i + 1] = channel[i];
		}

		// insert
		timestamp[index] = ts;
		note[index] = n;
		channel[index] = c;
		bufferSize++;
	}
	else
		jassert(false);
	
	////////// debug code
	////////// see if we get duplicates
	if (bufferSize>=2)
		jassert( (timestamp[0] != timestamp[1]) || (note[0] != note[1]) );

} // push

/////////////////////////////////////////////////////////////////////////////

void TopiaryNoteOffBuffer::pop(int &c, int &n, int64 &ts)
{
	jassert(bufferSize > 0);

	n = note[0];
	ts = timestamp[0];
	c = channel[0];
	// shift everything down
	for (int i = 1; i < bufferSize; i++)
	{
		note[i - 1] = note[i];
		timestamp[i - 1] = timestamp[i];
		channel[i - 1] = channel[i];
	}

	bufferSize--;

} // pop

/////////////////////////////////////////////////////////////////////////////

int64 TopiaryNoteOffBuffer::firstTimeStamp()
{
	if (bufferSize > 0)
		return timestamp[0];
	else 
		return 9999999999999999;

} //firstTimeStamp

/////////////////////////////////////////////////////////////////////////////

