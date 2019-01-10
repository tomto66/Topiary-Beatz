/////////////////////////////////////////////////////////////////////////////
/*
This file is part of Topiary Beats, Copyright Tom Tollenaere 2018-19.

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
#include "../../Topiary/Source/Topiary.h"
#include "TopiaryBeatsModel.h"
#include "TopiaryBeatsVariationChildren.h"
#include "TopiaryBeatsVariationComponent.h"

/////////////////////////////////////////////////////////////////////////////
// PoolTimingComponent
/////////////////////////////////////////////////////////////////////////////

PoolTimingComponent::PoolTimingComponent()
{
	addAndMakeVisible(measureEditor);
	measureEditor.setSelectAllWhenFocused(true);
	measureEditor.setEnabled(false);

	addAndMakeVisible(beatEditor);
	beatEditor.setSelectAllWhenFocused(true);
	beatEditor.setEnabled(false);

	addAndMakeVisible(tickEditor);
	tickEditor.setSelectAllWhenFocused(true);
	tickEditor.setEnabled(false);

	measureEditor.onFocusLost = [this]
	{
		parent->setVariationDefinition();
	};

	measureEditor.onReturnKey = [this]
	{
		parent->setVariationDefinition();
	};
	measureEditor.onFocusLost = [this]
	{
		parent->setVariationDefinition();
	};

	beatEditor.onReturnKey = [this]
	{
		parent->setVariationDefinition();
	};
	beatEditor.onFocusLost = [this]
	{
		parent->setVariationDefinition();
	};

	tickEditor.onReturnKey = [this]
	{
		parent->setVariationDefinition();
	};
	   

	setSize(width, heigth);

} // PoolTimingComponent

PoolTimingComponent::~PoolTimingComponent()
{

} // ~PoolTimingComponent

void PoolTimingComponent::resized()
{

} // resized

void PoolTimingComponent::setParent(TopiaryBeatsVariationComponent* p)
{
	parent = p;
} // s

void PoolTimingComponent::paint(Graphics& g)
{
	int separator = 5;
	int labelOffset = 15;
	g.fillAll(TopiaryColour::background);
	g.setColour(TopiaryColour::foreground);
	g.setFont(12.0f);

	g.drawText(label, 0, 0, 200, (int)labelOffset, juce::Justification::centredLeft);

	auto area = getLocalBounds();
	auto left = area.removeFromLeft(65); //space for label
	left = area.removeFromLeft(mw);
	measureEditor.setBounds(left);
	left = area.removeFromLeft(separator);
	left = area.removeFromLeft(bw);
	beatEditor.setBounds(left);
	left = area.removeFromLeft(separator);
	left = area.removeFromLeft(tw);
	tickEditor.setBounds(left);

} // paint

//////////////////////////////////////////////////////////////////////////////////////////////
// PoolLengthComponent
//////////////////////////////////////////////////////////////////////////////////////////////

PoolLengthComponent:: PoolLengthComponent()
{
	for (int i = 0; i < 4; i++)
	{
		addAndMakeVisible(poolStart[i]);  // sizes set by constructor
		poolStart[i].label = "Pool " + String(i + 1) + " Start";
		addAndMakeVisible(poolStop[i]);
		poolStop[i].label = "Pool " + String(i + 1) + " Stop";
	}
}

PoolLengthComponent::~PoolLengthComponent()
{
}

void PoolLengthComponent::setParent(TopiaryBeatsVariationComponent* p)
{
	parent = p;
	for (int i = 0; i < 4; i++)
	{
		poolStart[i].setParent(parent);
		poolStop[i].setParent(parent);
	}
} // setParent

void PoolLengthComponent::paint(Graphics& g)
{
	//int separator = 5;
	int labelOffset = 15;
	g.fillAll(TopiaryColour::background);
	g.setColour(TopiaryColour::foreground);
	g.setFont(12.0f);
	auto area = getLocalBounds();
	int lineWidth = 2;

	g.drawText("Pool Length", 0, 0, 200, (int)labelOffset, juce::Justification::centredLeft);
	auto recBounds = area.removeFromBottom(area.getHeight() - labelOffset);
	g.drawRoundedRectangle((float)recBounds.getX() + lineWidth, (float)recBounds.getY() + lineWidth, (float)recBounds.getWidth() - 2 * lineWidth, (float)recBounds.getHeight() - 2 * lineWidth, (float)lineWidth, (float)lineWidth);

	// turn it into inner rectangle
	auto inRecBounds = Rectangle<int>::Rectangle(recBounds.getX() + 3 * lineWidth, recBounds.getY() + 3 * lineWidth, recBounds.getWidth() - 6 * lineWidth, recBounds.getHeight() - 6 * lineWidth);

	auto bBounds = inRecBounds.removeFromTop(poolStart[0].heigth);
	for (int i = 0; i < 4; i++)
	{
		poolStart[i].setBounds(bBounds);
		inRecBounds.removeFromTop(2 * lineWidth);
		bBounds = inRecBounds.removeFromTop(poolStart[0].heigth);
		poolStop[i].setBounds(bBounds);
		inRecBounds.removeFromTop(2 * lineWidth);
		bBounds = inRecBounds.removeFromTop(poolStart[0].heigth);
	}

} // paint

void PoolLengthComponent::resized()
{

} // resized

//////////////////////////////////////////////////////////////////////////////////////////////
// EnablePoolComponent
//////////////////////////////////////////////////////////////////////////////////////////////

EnablePoolComponent::EnablePoolComponent() 
{
	setSize(width, heigth);

	addAndMakeVisible(enableButton1);
	enableButton1.setButtonText("Pool 1");
	//enableButton1.setSize(eW, eH);
	enableButton1.setClickingTogglesState(true);
	enableButton1.onClick = [this]
	{
		parent->setVariationDefinition();
	};

	addAndMakeVisible(enableButton2);
	enableButton2.setButtonText("Pool 2");
	//enableButton2.setSize(eW, eH);
	enableButton2.setClickingTogglesState(true);
	enableButton2.onClick = [this]
	{
		parent->setVariationDefinition();
	};

	addAndMakeVisible(enableButton3);
	enableButton3.setButtonText("Pool 3");
	//enableButton3.setSize(eW, eH);
	enableButton3.setClickingTogglesState(true);
	enableButton3.onClick = [this]
	{
		parent->setVariationDefinition();
	};

	addAndMakeVisible(enableButton4);
	enableButton4.setButtonText("Pool 4");
	//enableButton4.setSize(eW, eH);
	enableButton4.setClickingTogglesState(true);
	enableButton4.onClick = [this]
	{
		parent->setVariationDefinition();
	};

} // EnablePoolComponent

EnablePoolComponent::~EnablePoolComponent() 
{

} // ~EnablePoolComponent

void EnablePoolComponent::resized()
{

} // resized

void EnablePoolComponent::setParent(TopiaryBeatsVariationComponent* p)
{
	parent = p;
} // setparent

void  EnablePoolComponent::paint(Graphics& g)
{
	int labelOffset = 15;
	int lineWidth = 2;
	auto area = getLocalBounds();

	g.fillAll(TopiaryColour::background);
	g.setColour(TopiaryColour::foreground);
	g.setFont(12.0f);
	g.drawText("Enable Pools", lineWidth, lineWidth, 200, labelOffset, juce::Justification::centredLeft);

	auto recBounds = area.removeFromBottom(area.getHeight() - labelOffset);
	g.drawRoundedRectangle((float)recBounds.getX() + lineWidth, (float)recBounds.getY() + lineWidth, (float)recBounds.getWidth() - 2 * lineWidth, (float)recBounds.getHeight() - 2 * lineWidth, (float)lineWidth, (float)lineWidth);

	// turn it into inner rectangle
	auto inRecBounds = Rectangle<int>::Rectangle(recBounds.getX() + 3 * lineWidth, recBounds.getY() + 3 * lineWidth, recBounds.getWidth() - 6 * lineWidth, recBounds.getHeight() - 6 * lineWidth);

	auto bBounds = inRecBounds.removeFromTop(eH);
	enableButton1.setBounds(bBounds);

	inRecBounds.removeFromTop(2 * lineWidth);
	bBounds = inRecBounds.removeFromTop(eH);
	enableButton2.setBounds(bBounds);

	inRecBounds.removeFromTop(2 * lineWidth);
	bBounds = inRecBounds.removeFromTop(eH);
	enableButton3.setBounds(bBounds);

	inRecBounds.removeFromTop(2 * lineWidth);
	bBounds = inRecBounds.removeFromTop(eH);
	enableButton4.setBounds(bBounds);

} // paint

/////////////////////////////////////////////////////////////////////////
// RandomNoteComponent
/////////////////////////////////////////////////////////////////////////

RandomNoteComponent::RandomNoteComponent()
	{
		pool1Button.setClickingTogglesState(true);
		pool1Button.setLookAndFeel(&rndLookAndFeel);
		
		addAndMakeVisible(pool1Button);
		pool1Button.setButtonText("1");
		pool1Button.onClick = [this]
		{
		};

		pool2Button.setClickingTogglesState(true);
		pool2Button.setLookAndFeel(&rndLookAndFeel);
		
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

void RandomNoteComponent::setParent(TopiaryBeatsVariationComponent* p)
{
	parent = p;
} // s

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
		
} // paint

// End RandomNOteComponent

/////////////////////////////////////////////////////////////////////////
// VariationDefinitionComponent
/////////////////////////////////////////////////////////////////////////

	VariationDefinitionComponent::VariationDefinitionComponent()
	{
		setSize(width, heigth);
		addAndMakeVisible(variationCombo);
		variationCombo.addItem("Variation 1", 1);
		variationCombo.addItem("Variation 2", 2);
		variationCombo.addItem("Variation 3", 3);
		variationCombo.addItem("Variation 4", 4);
		variationCombo.addItem("Variation 5", 5);
		variationCombo.addItem("Variation 6", 6);
		variationCombo.addItem("Variation 7", 7);
		variationCombo.addItem("Variation 8", 8);
		
		variationCombo.onChange = [this]
		{
			parent->getVariationDefinition();
		};

		addAndMakeVisible(nameEditor);
		nameEditor.onFocusLost = [this]
		{
			parent->setVariationDefinition();
		};

		nameEditor.onReturnKey = [this]
		{
			parent->setVariationDefinition();
		};

		addAndMakeVisible(enableButton);
		enableButton.setClickingTogglesState(true);
		enableButton.setButtonText("Enabled");
		enableButton.onClick = [this]
		{
			parent->setVariationDefinition();
		};

		addAndMakeVisible(patternCombo);
		
		patternCombo.onChange = [this]
		{
			parent->setVariationDefinition();
		};
		
		
	} // VariationDefinitionComponent

	///////////////////////////////////////////////////////////////////////////////////

	void VariationDefinitionComponent::setParent(TopiaryBeatsVariationComponent* p)
	{
		parent = p;
	} // setparent

	///////////////////////////////////////////////////////////////////////////////////

	VariationDefinitionComponent::~VariationDefinitionComponent()
	{
	} // ~VariationDefinitionComponent

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
		//auto hBounds = topBounds.removeFromLeft(eW);
		nameEditor.setBounds(topBounds);

		inRecBounds.removeFromTop(4 * lineWidth);
		topBounds = inRecBounds.removeFromTop(eH);

		// topBounds is second line of objects
		g.setColour(Colours::antiquewhite);
		
		variationCombo.setBounds(topBounds);
		
		inRecBounds.removeFromTop(4 * lineWidth);
		topBounds = inRecBounds.removeFromTop(eH);
		
		// topBounds is now third row which only has the enablebutton
		//hBounds = topBounds.removeFromLeft(eW);
		enableButton.setBounds(topBounds);
		enableButton.setSize(topBounds.getWidth(), topBounds.getHeight());

		inRecBounds.removeFromTop(4 * lineWidth);
		topBounds = inRecBounds.removeFromTop(eH);

		// topBounds is now fourth row which only has the patternCombo
		// hBounds = topBounds.removeFromLeft(eW);
		patternCombo.setBounds(topBounds);
		patternCombo.setSize(topBounds.getWidth(), topBounds.getHeight());

		int debug = patternCombo.getSelectedId();

		debug = debug + 0;

	} // paint

/////////////////////////////////////////////////////////////////////////
// PoolChannelComponent
/////////////////////////////////////////////////////////////////////////

	PoolChannelComponent::PoolChannelComponent()
	{
		setSize(width, heigth);
		addAndMakeVisible(poolChannelEditor1);
		poolChannelEditor1.setSize(eW, eH);
		poolChannelEditor1.onFocusLost = [this]
		{
			parent->setVariationDefinition();
		};
		poolChannelEditor1.onReturnKey = [this]
		{
			parent->setVariationDefinition();
		};
		
		addAndMakeVisible(poolChannelEditor2);
		poolChannelEditor2.setSize(eW, eH);
		poolChannelEditor2.onFocusLost = [this]
		{
			parent->setVariationDefinition();
		};
		poolChannelEditor2.onReturnKey = [this]
		{
			parent->setVariationDefinition();
		};

		addAndMakeVisible(poolChannelEditor3);
		poolChannelEditor3.setSize(eW, eH);
		poolChannelEditor3.onFocusLost = [this]
		{
			parent->setVariationDefinition();
		};
		poolChannelEditor3.onReturnKey = [this]
		{
			parent->setVariationDefinition();
		};

		addAndMakeVisible(poolChannelEditor4);
		poolChannelEditor4.setSize(eW, eH);
		poolChannelEditor4.onFocusLost = [this]
		{
			parent->setVariationDefinition();
		};
		poolChannelEditor4.onReturnKey = [this]
		{
			parent->setVariationDefinition();
		};

	} // PoolChannelComponent

	///////////////////////////////////////////////////////////////////////////////////

	void PoolChannelComponent ::setParent(TopiaryBeatsVariationComponent* p)
	{
		parent = p;
	} // setparent

	///////////////////////////////////////////////////////////////////////////////////

	PoolChannelComponent::~PoolChannelComponent()
	{
	} // ~PoolChannelComponent

	///////////////////////////////////////////////////////////////////////////////////

	void  PoolChannelComponent::resized()
	{

	} // resized

	///////////////////////////////////////////////////////////////////////////////////

	void  PoolChannelComponent::paint(Graphics& g)
	{
		int labelOffset = 15;
		int lineWidth = 2;
		auto area = getLocalBounds();

		g.fillAll(TopiaryColour::background);
		g.setColour(TopiaryColour::foreground);
		g.setFont(12.0f);
		g.drawText("Pool Output Channels", lineWidth, lineWidth, 200, labelOffset, juce::Justification::centredLeft);

		auto recBounds = area.removeFromBottom(area.getHeight() - labelOffset);
		g.drawRoundedRectangle((float)recBounds.getX() + lineWidth, (float)recBounds.getY() + lineWidth, (float)recBounds.getWidth() - 2 * lineWidth, (float)recBounds.getHeight() - 2 * lineWidth, (float)lineWidth, (float)lineWidth);

		// turn it into inner rectangle
		auto inRecBounds = Rectangle<int>::Rectangle(recBounds.getX() + 3 * lineWidth, recBounds.getY() + 3 * lineWidth, recBounds.getWidth() - 6 * lineWidth, recBounds.getHeight() - 6 * lineWidth);

		// area for name and from editors
		auto topBounds = inRecBounds.removeFromTop(eH);
		
		// topBounds is first line of objects
		auto eBounds = topBounds.removeFromLeft(eW);
		poolChannelEditor1.setBounds(eBounds);

		topBounds.removeFromLeft(4 * lineWidth);
		eBounds = topBounds.removeFromLeft(eW);
		poolChannelEditor2.setBounds(eBounds);

		topBounds.removeFromLeft(4 * lineWidth);
		eBounds = topBounds.removeFromLeft(eW);
		poolChannelEditor3.setBounds(eBounds);

		topBounds.removeFromLeft(4 * lineWidth);
		eBounds = topBounds.removeFromLeft(eW);
		poolChannelEditor4.setBounds(eBounds);


	} // paint