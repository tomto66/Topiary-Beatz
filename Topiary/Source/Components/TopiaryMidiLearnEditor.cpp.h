/////////////////////////////////////////////////////////////////////////////
/*
This file is part of Topiary, Copyright Tom Tollenaere 2018-2021.

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

#ifdef TOPIARYMODEL

TopiaryMidiLearnEditor::TopiaryMidiLearnEditor()
{
}


TopiaryMidiLearnEditor::~TopiaryMidiLearnEditor()
{
}

void TopiaryMidiLearnEditor::setModel(TOPIARYMODEL* m, int ID)  // ID is the unique ID of the editor, as known by the model (otherwise the model would never know what to learn MIDI for
{
	model = m;
	Id = ID;
}

void TopiaryMidiLearnEditor::addPopupMenuItems(PopupMenu &menuToAddTo, const MouseEvent *mouseClickEvent)
{
	UNUSED(mouseClickEvent)
	menuToAddTo.addItem(19660426, TRANS("MIDI Learn"), true);
}

void TopiaryMidiLearnEditor::performPopupMenuAction(int menuItemID) 
{
	// only possibility is 19660426
	if (menuItemID == 19660426)
	{
		model->learnMidi(Id); // let the model know which MidiLearnEditor is asking to learn
	}
}

#endif
