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

/////////////////////////////////////////////////////////////////////////////

This code has a generic editor component that can be included in every Topiary plugin.

CAREFUL: needs symbols:
- TOPIARYMODEL e.g. TOPIARYMODEL
- TOPIARYEDITORCOMPONENT - e.g. TopiaryPresetsComponent

*/
/////////////////////////////////////////////////////////////////////////////

#include "PluginProcessor.h"
#include "PluginEditor.h"

#ifdef TOPIARYMODEL


TopiaryAudioProcessorEditor::TopiaryAudioProcessorEditor (TopiaryAudioProcessor& p)
    : AudioProcessorEditor (&p), processor (p)
{
	setSize(TOPIARYEDITORCOMPONENT::width, TOPIARYEDITORCOMPONENT::heigth);
	model = processor.getModel();
	topiaryEditorComponent.setModel(model);
	addAndMakeVisible(topiaryEditorComponent);
}

TopiaryAudioProcessorEditor::~TopiaryAudioProcessorEditor()
{
}

void TopiaryAudioProcessorEditor::paint (Graphics& g)
{
	UNUSED(g)
	topiaryEditorComponent.setBounds(getLocalBounds());
}

void TopiaryAudioProcessorEditor::resized()
{
    
}
#endif