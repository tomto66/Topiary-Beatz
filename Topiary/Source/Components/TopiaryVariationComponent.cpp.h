/////////////////////////////////////////////////////////////////////////////
/*
This file is part of Topiary, Copyright Tom Tollenaere 2020-21.

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

#ifdef TOPIARYVARIATIONCOMPONENT


void TOPIARYVARIATIONCOMPONENT::getRandomizeNotes()
{
	// get note randomisation from model
	bool enable;
	int value;

	MODEL->getRandomizeNotes(variationDefinitionComponent.variationCombo.getSelectedId() - 1, enable, value);

	randomNoteComponent.onButton.setToggleState(enable, dontSendNotification);

	randomNoteComponent.percentSlider.setValue(value, dontSendNotification);

} // getRandomizeNotes

/////////////////////////////////////////////////////////////////////////

void TOPIARYVARIATIONCOMPONENT::setRandomizeNotes()
{
	// set note randomisation to model

	MODEL->setRandomizeNotes(variationDefinitionComponent.variationCombo.getSelectedId() - 1, randomNoteComponent.onButton.getToggleState(), (int)randomNoteComponent.percentSlider.getValue());

} // setRandomizeNotes

/////////////////////////////////////////////////////////////////////////

void TOPIARYVARIATIONCOMPONENT::setTiming()
{
	// set note randomisation to model

	MODEL->setRandomizeTiming(variationDefinitionComponent.variationCombo.getSelectedId() - 1, timingComponent.onButton.getToggleState(),
									(int)timingComponent.timingSlider.getValue(),
									timingComponent.plusButton.getToggleState(),
									timingComponent.minusButton.getToggleState());

} // setTiming

/////////////////////////////////////////////////////////////////////////

void TopiaryPadzVariationComponent::getTiming()
{
	// get note randomisation from model
	bool enable;
	bool plus, min;
	int value;

	MODEL->getRandomizeTiming(variationDefinitionComponent.variationCombo.getSelectedId() - 1, enable, value, plus, min);

	timingComponent.onButton.setToggleState(enable, dontSendNotification);

	timingComponent.timingSlider.setValue(value, dontSendNotification);
	timingComponent.plusButton.setToggleState(plus, dontSendNotification);
	timingComponent.minusButton.setToggleState(min, dontSendNotification);

} // getTiming


/////////////////////////////////////////////////////////////////////////

void TOPIARYVARIATIONCOMPONENT::setVelocity()
{
	// set note randomisation to model

	MODEL->setRandomizeVelocity(variationDefinitionComponent.variationCombo.getSelectedId() - 1, velocityComponent.onButton.getToggleState(),
									(int)velocityComponent.velocitySlider.getValue(),
									velocityComponent.plusButton.getToggleState(),
									velocityComponent.minusButton.getToggleState() 	);

} // setVelocity

/////////////////////////////////////////////////////////////////////////

void TOPIARYVARIATIONCOMPONENT::getVelocity()
{
	// get note randomisation from model
	bool enable;
	bool plus, min;
	int value;

	MODEL->getRandomizeVelocity (variationDefinitionComponent.variationCombo.getSelectedId() - 1, enable, value, plus, min);

	velocityComponent.onButton.setToggleState(enable, dontSendNotification);

	velocityComponent.velocitySlider.setValue(value, dontSendNotification);
	velocityComponent.plusButton.setToggleState(plus, dontSendNotification);
	velocityComponent.minusButton.setToggleState(min, dontSendNotification);

} // getVelocity

/////////////////////////////////////////////////////////////////////////


#endif