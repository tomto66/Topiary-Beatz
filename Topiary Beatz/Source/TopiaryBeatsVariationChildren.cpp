/////////////////////////////////////////////////////////////////////////////
/*
This file is part of Topiary Beats, Copyright Tom Tollenaere 2018.

Topiary Beats is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

Topiary Beats is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Topiary Beats. If not, see <https://www.gnu.org/licenses/>.
*/
/////////////////////////////////////////////////////////////////////////////

#pragma once
#include "../JuceLibraryCode/JuceHeader.h"
#include "../../Topiary/Topiary.h"
#include "TopiaryBeatsModel.h"
#include "TopiaryBeatsVariationChildren.h"

/////////////////////////////////////////////////////////////////////////

RandomNoteComponent::RandomNoteComponent()
	{
		pool1Button.setClickingTogglesState(true);
		pool1Button.setLookAndFeel(&rndLookAndFeel);
		//pool1Button.setSize(buttonH - 2, buttonH - 2);
		addAndMakeVisible(pool1Button);
		pool1Button.setButtonText("1");
		pool1Button.onClick = [this]
		{
		};

		pool2Button.setClickingTogglesState(true);
		pool2Button.setLookAndFeel(&rndLookAndFeel);
		//pool2Button.setSize(buttonH - 2, buttonH - 2);
		addAndMakeVisible(pool2Button);
		pool2Button.setButtonText("2");
		pool2Button.onClick = [this]
		{
		};

		pool3Button.setClickingTogglesState(true);
		pool3Button.setLookAndFeel(&rndLookAndFeel);
		//pool3Button.setSize(buttonH - 2, buttonH - 2);
		addAndMakeVisible(pool3Button);
		pool3Button.setButtonText("3");
		pool3Button.onClick = [this]
		{
		};

		pool4Button.setClickingTogglesState(true);
		pool4Button.setLookAndFeel(&rndLookAndFeel);
		//pool4Button.setSize(buttonH - 2, buttonH - 2);
		addAndMakeVisible(pool4Button);
		pool4Button.setButtonText("4");
		pool4Button.onClick = [this]
		{
		};

		onButton.setClickingTogglesState(true);
		onButton.setLookAndFeel(&rndLookAndFeel);
		onButton.setSize(100, buttonH);
		addAndMakeVisible(onButton);
		onButton.setButtonText("RND Notes");
		onButton.onClick = [this]
		{
		};

		addAndMakeVisible(percentSlider);
		percentSlider.setSliderStyle(Slider::RotaryHorizontalVerticalDrag);
		//percentSlider.setSize(sliderH, sliderV);
		percentSlider.setTextBoxStyle(Slider::TextBoxBelow, false, sliderTW, sliderTH);
		percentSlider.setTextBoxIsEditable(true);
		percentSlider.setTextBoxStyle(Slider::TextEntryBoxPosition::TextBoxBelow, false, sliderTW, sliderTH);
		percentSlider.setRange((double)0, (double)100, (double)1);
		percentSlider.setTextValueSuffix("%");

	} // RandomNoteComponent

	///////////////////////////////////////////////////////////////////////////////////

RandomNoteComponent::~RandomNoteComponent()
	{
	} // ~RandomNoteComponent

	///////////////////////////////////////////////////////////////////////////////////

	void  RandomNoteComponent::resized()
	{

	} // resized

	///////////////////////////////////////////////////////////////////////////////////

	void  RandomNoteComponent::paint(Graphics& g)
	{
		
		int labelOffset = 15;
		int lineWidth = 2;
		auto area = getLocalBounds();
		g.fillAll(TopiaryColour::background);
		g.setColour(TopiaryColour::foreground);
		g.setFont(12.0f);

		g.drawText("Random Notes", (int) lineWidth, (int) lineWidth, 200, (int) labelOffset, juce::Justification::centredLeft);
		auto recBounds = area.removeFromBottom(area.getHeight()-labelOffset);
		g.drawRoundedRectangle((float) recBounds.getX()+lineWidth, (float) recBounds.getY()+lineWidth, (float) recBounds.getWidth()-2*lineWidth, (float) recBounds.getHeight()-2*lineWidth, (float) lineWidth, (float) lineWidth);

		// turn it into inner rectangle
		auto inRecBounds = Rectangle<int>::Rectangle(recBounds.getX() + 3*lineWidth, recBounds.getY() + 3*lineWidth, recBounds.getWidth() - 6 * lineWidth, recBounds.getHeight() - 6 * lineWidth);
		
		// area for top button
		auto topBounds = inRecBounds.removeFromTop(buttonH);
		onButton.setBounds(topBounds);
		onButton.setSize(topBounds.getWidth(), topBounds.getHeight());


		inRecBounds.removeFromTop(2 * lineWidth);
		// inRecbounds is now the box for pool buttons + slider
		auto poolBounds = inRecBounds.removeFromLeft(pbuttonH + 2 * lineWidth);
		// inRecBounds is now box for slider
		
		auto pBounds = poolBounds.removeFromBottom(pbuttonH + 2 * lineWidth);
		pool4Button.setBounds(pBounds);
		pool4Button.setSize(pBounds.getWidth(), pBounds.getHeight());
		pBounds = poolBounds.removeFromBottom(pbuttonH + 2 * lineWidth);
		pool3Button.setSize(pBounds.getWidth(), pBounds.getHeight());
		pool3Button.setBounds(pBounds);
		pBounds = poolBounds.removeFromBottom(pbuttonH + 2 * lineWidth);
		pool2Button.setBounds(pBounds);
		pool2Button.setSize(pBounds.getWidth(), pBounds.getHeight());
		pBounds = poolBounds.removeFromBottom(pbuttonH + 2 * lineWidth);
		pool1Button.setBounds(pBounds);
		pool1Button.setSize(pBounds.getWidth(), pBounds.getHeight());

		inRecBounds.removeFromLeft(2 * lineWidth);
		percentSlider.setBounds(inRecBounds);
		percentSlider.setSize(inRecBounds.getWidth(), inRecBounds.getHeight());
		percentSlider.setLookAndFeel(&rndLookAndFeel);
		//getLookAndFeel().setColour(Slider::thumbColourId, Topiary::TOGGLE_COLOUR);
		//getLookAndFeel().setColour(Slider::rotarySliderFillColourId, Topiary::SLIDER_TRACE_COLOUR);
		//getLookAndFeel().setColour(Slider::rotarySliderOutlineColourId, Topiary::SLIDER_TRACE_COLOUR);
		

		
		/*
			thumbColourId
			The colour to draw the thumb with.

			It's up to the look and feel class how this is used.

			trackColourId
			The colour to draw the groove that the thumb moves along.

			rotarySliderFillColourId
			For rotary sliders, this colour fills the outer curve.

			rotarySliderOutlineColourId
			For rotary sliders, this colour is used to draw the outer curve's outline.
			*/
	} // paint

	// End RandomNOteComponent

	/////////////////////////////////////////////////////////////////////////
	// VariationDefinitionComponent
	/////////////////////////////////////////////////////////////////////////

	VariationDefinitionComponent::VariationDefinitionComponent()
	{

		addAndMakeVisible(variationCombo);

		addAndMakeVisible(nameEditor);

		addAndMakeVisible(fromBeatEditor);
		addAndMakeVisible(toBeatEditor);
		addAndMakeVisible(fromMeasureEditor);
		addAndMakeVisible(toMeasureEditor);
		addAndMakeVisible(fromTickEditor);
		addAndMakeVisible(toTickEditor);

	} // RandomNoteComponent

	///////////////////////////////////////////////////////////////////////////////////

	VariationDefinitionComponent::~VariationDefinitionComponent()
	{
	} // ~RandomNoteComponent

	///////////////////////////////////////////////////////////////////////////////////

	void  VariationDefinitionComponent::resized()
	{

	} // resized

	///////////////////////////////////////////////////////////////////////////////////

	void  VariationDefinitionComponent::paint(Graphics& g)
	{
		int labelOffset = 15;
		int lineWidth = 2;
		auto area = getLocalBounds();
		g.fillAll(TopiaryColour::background);
		g.setColour(TopiaryColour::foreground);
		g.setFont(12.0f);
		g.drawText("Variation Definition", lineWidth, lineWidth, 200, labelOffset, juce::Justification::centredLeft);

		auto recBounds = area.removeFromBottom(area.getHeight() - labelOffset);
		g.drawRoundedRectangle((float) recBounds.getX() + lineWidth, (float) recBounds.getY() + lineWidth, (float) recBounds.getWidth() - 2 * lineWidth, (float) recBounds.getHeight() - 2 * lineWidth, (float) lineWidth, (float) lineWidth);

		// turn it into inner rectangle
		auto inRecBounds = Rectangle<int>::Rectangle(recBounds.getX() + 3 * lineWidth, recBounds.getY() + 3 * lineWidth, recBounds.getWidth() - 6 * lineWidth, recBounds.getHeight() - 6 * lineWidth);

		// area for name and from editors
		auto topBounds = inRecBounds.removeFromTop(eH);
		
		// topBounds is first line of objects
		auto hBounds = topBounds.removeFromLeft(eW);
		nameEditor.setBounds(hBounds);

		int fromSpacer = 80; // from label X offset
		
		topBounds.removeFromLeft(fromSpacer);
		hBounds = topBounds.removeFromLeft(meW);
		fromMeasureEditor.setBounds(hBounds);

		topBounds.removeFromLeft(4);
		hBounds = topBounds.removeFromLeft(beW);
		fromBeatEditor.setBounds(hBounds);

		topBounds.removeFromLeft(4);
		hBounds = topBounds.removeFromLeft(teW);
		fromTickEditor.setBounds(hBounds);

		inRecBounds.removeFromTop(4 * lineWidth);
		topBounds = inRecBounds.removeFromTop(eH);

		// topBounds is second line of objects
		g.setColour(Colours::antiquewhite);
		

		hBounds = topBounds.removeFromLeft(eW);
		variationCombo.setBounds(hBounds);

		topBounds.removeFromLeft(fromSpacer);
		hBounds = topBounds.removeFromLeft(meW);
		toMeasureEditor.setBounds(hBounds);

		topBounds.removeFromLeft(4);
		hBounds = topBounds.removeFromLeft(beW);
		toBeatEditor.setBounds(hBounds);

		topBounds.removeFromLeft(4);
		hBounds = topBounds.removeFromLeft(teW);
		toTickEditor.setBounds(hBounds);

		//variationCombo.setBounds(topBounds);

		/*
		onButton.setSize(topBounds.getWidth(), topBounds.getHeight());


		inRecBounds.removeFromTop(2 * lineWidth);
		// inRecbounds is now the box for pool buttons + slider
		auto poolBounds = inRecBounds.removeFromLeft(pbuttonH + 2 * lineWidth);
		// inRecBounds is now box for slider

		auto pBounds = poolBounds.removeFromBottom(pbuttonH + 2 * lineWidth);
		pool4Button.setBounds(pBounds);
		pool4Button.setSize(pBounds.getWidth(), pBounds.getHeight());
		pBounds = poolBounds.removeFromBottom(pbuttonH + 2 * lineWidth);
		pool3Button.setSize(pBounds.getWidth(), pBounds.getHeight());
		pool3Button.setBounds(pBounds);
		pBounds = poolBounds.removeFromBottom(pbuttonH + 2 * lineWidth);
		pool2Button.setBounds(pBounds);
		pool2Button.setSize(pBounds.getWidth(), pBounds.getHeight());
		pBounds = poolBounds.removeFromBottom(pbuttonH + 2 * lineWidth);
		pool1Button.setBounds(pBounds);
		pool1Button.setSize(pBounds.getWidth(), pBounds.getHeight());

		inRecBounds.removeFromLeft(2 * lineWidth);
		percentSlider.setBounds(inRecBounds);
		percentSlider.setSize(inRecBounds.getWidth(), inRecBounds.getHeight());
		percentSlider.setLookAndFeel(&rndLookAndFeel);
		//getLookAndFeel().setColour(Slider::thumbColourId, Topiary::TOGGLE_COLOUR);
		//getLookAndFeel().setColour(Slider::rotarySliderFillColourId, Topiary::SLIDER_TRACE_COLOUR);
		//getLookAndFeel().setColour(Slider::rotarySliderOutlineColourId, Topiary::SLIDER_TRACE_COLOUR);
		*/


		/*
			thumbColourId
			The colour to draw the thumb with.

			It's up to the look and feel class how this is used.

			trackColourId
			The colour to draw the groove that the thumb moves along.

			rotarySliderFillColourId
			For rotary sliders, this colour fills the outer curve.

			rotarySliderOutlineColourId
			For rotary sliders, this colour is used to draw the outer curve's outline.
			*/
	} // paint