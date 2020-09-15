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
/////////////////////////////////////////////////////////////////////////////
/*

This code has generic stuff that can be included in every Topiary model code.
It includes:
- Variation logic (that depends on variation variables defined in the actual
  models (not in the generic TopiaryModel))

CAREFUL: needs a symbol TOPIARYMODEL to actually build!

*/
/////////////////////////////////////////////////////////////////////////////

#ifdef TOPIARYMODEL
#ifndef FILE_TOPIARYMIDILEARNEDITOR_SEEN
#define FILE_TOPIARYMIDILEARNEDITOR_SEEN
class TOPIARYMODEL;

class TopiaryMidiLearnEditor : public TextEditor
{
public:
	TopiaryMidiLearnEditor();
	~TopiaryMidiLearnEditor();
	void addPopupMenuItems(PopupMenu &menuToAddTo, const MouseEvent *mouseClickEvent) override;
	void performPopupMenuAction(int menuItemID) override;
	void setModel(TOPIARYMODEL* m, int ID);

private:
	TOPIARYMODEL *model;
	int Id; // unique identifier for the model
	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(TopiaryMidiLearnEditor)
};

#endif
#endif