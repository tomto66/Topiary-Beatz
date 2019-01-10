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
*/
/////////////////////////////////////////////////////////////////////////////

#pragma once
#include "../JuceLibraryCode/JuceHeader.h"
#include "Topiary.h"
#include "TopiaryModel.h"
#include "TopiaryThread.h"


/////////////////////////////////////////////////////////////////////////////

TopiaryThread::TopiaryThread() : Thread("Topiary", 0)
{

}

/////////////////////////////////////////////////////////////////////////////

TopiaryThread::~TopiaryThread()
{

}

/////////////////////////////////////////////////////////////////////////////

void TopiaryThread::setModel(TopiaryModel* m)
{
	model = m;
}

/////////////////////////////////////////////////////////////////////////////

void TopiaryThread::run()
{
	Logger::outputDebugString("Worker thread started.");
	wait(-1); // wait till model fully initialized - then it will get an notify
	//Logger::outputDebugString("Worker thread WAKES UP --------------------------------------------------");
	while (!threadShouldExit())
	{
		// main loop
		model->threadRunner();
		wait(-1); // wait until something notifies this thread
		Logger::outputDebugString("Worker thread WAKES UP --------------------------------------------------");
	}
	//Logger::outputDebugString("Worker thread stopped.");
	return;

} // run

/////////////////////////////////////////////////////////////////////////////