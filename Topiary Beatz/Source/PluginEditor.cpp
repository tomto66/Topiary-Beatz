/////////////////////////////////////////////////////////////////////////////
/*
This file is part of Topiary Beatz, Copyright Tom Tollenaere 2018-19.

Topiary Beatz is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

Topiary Beatz is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Topiary Beats. If not, see <https://www.gnu.org/licenses/>.
*/
/////////////////////////////////////////////////////////////////////////////

#include "PluginProcessor.h"
#include "PluginEditor.h"
#include "TopiaryBeatsComponent.h"

//==============================================================================
TopiaryBeatsAudioProcessorEditor::TopiaryBeatsAudioProcessorEditor (TopiaryBeatsAudioProcessor& p)
    : AudioProcessorEditor (&p), processor (p)
{
	setSize(TopiaryBeatsComponent::width, TopiaryBeatsComponent::heigth);
	beatsModel = processor.getModel();
	topiaryBeatsComponent.setModel(beatsModel);
	addAndMakeVisible(topiaryBeatsComponent);
}

TopiaryBeatsAudioProcessorEditor::~TopiaryBeatsAudioProcessorEditor()
{
}

//==============================================================================
void TopiaryBeatsAudioProcessorEditor::paint (Graphics& g)
{
	UNUSED(g)
	topiaryBeatsComponent.setBounds(getLocalBounds());
}

void TopiaryBeatsAudioProcessorEditor::resized()
{
    
}
