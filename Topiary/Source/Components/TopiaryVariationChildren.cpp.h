/////////////////////////////////////////////////////////////////////////////
/*
This file is part of Topiary, Copyright Tom Tollenaere 2018-20.

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


#ifdef BEATZ

//////////////////////////////////////////////////////////////////////////////////////////////
// EnablePoolComponent
//////////////////////////////////////////////////////////////////////////////////////////////

EnablePoolComponent::EnablePoolComponent() 
{
	setSize(width, heigth);

	addAndMakeVisible(enableButton1);
	enableButton1.setButtonText("Pool 1");
	enableButton1.setClickingTogglesState(true);
	enableButton1.onClick = [this]
	{
		parent->setVariationDefinition();
	};

	addAndMakeVisible(enableButton2);
	enableButton2.setButtonText("Pool 2");
	enableButton2.setClickingTogglesState(true);
	enableButton2.onClick = [this]
	{
		parent->setVariationDefinition();
	};

	addAndMakeVisible(enableButton3);
	enableButton3.setButtonText("Pool 3");
	enableButton3.setClickingTogglesState(true);
	enableButton3.onClick = [this]
	{
		parent->setVariationDefinition();
	};

	addAndMakeVisible(enableButton4);
	enableButton4.setButtonText("Pool 4");
	enableButton4.setClickingTogglesState(true);
	enableButton4.onClick = [this]
	{
		parent->setVariationDefinition();
	};

} // EnablePoolComponent

//////////////////////////////////////////////////////////////////////////////////////////////

EnablePoolComponent::~EnablePoolComponent() 
{
} // ~EnablePoolComponent

//////////////////////////////////////////////////////////////////////////////////////////////

void EnablePoolComponent::resized()
{
} // resized

//////////////////////////////////////////////////////////////////////////////////////////////

void EnablePoolComponent::setParent(TOPIARYVARIATIONCOMPONENT* p)
{
	parent = p;
} // setparent

//////////////////////////////////////////////////////////////////////////////////////////////

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
#endif

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
			parent->setRandomizeNotes();
		};

		pool2Button.setClickingTogglesState(true);
		pool2Button.setLookAndFeel(&rndLookAndFeel);
		
		addAndMakeVisible(pool2Button);
		pool2Button.setButtonText("2");
		pool2Button.onClick = [this]
		{
			parent->setRandomizeNotes();
		};

		pool3Button.setClickingTogglesState(true);
		pool3Button.setLookAndFeel(&rndLookAndFeel);
		addAndMakeVisible(pool3Button);
		pool3Button.setButtonText("3");
		pool3Button.onClick = [this]
		{
			parent->setRandomizeNotes();
		};

		pool4Button.setClickingTogglesState(true);
		pool4Button.setLookAndFeel(&rndLookAndFeel);
		//pool4Button.setSize(buttonH - 2, buttonH - 2);
		addAndMakeVisible(pool4Button);
		pool4Button.setButtonText("4");
		pool4Button.onClick = [this]
		{
			parent->setRandomizeNotes();
		};

#ifndef BEATZ
		pool1Button.setEnabled(false);
		pool2Button.setEnabled(false);
		pool3Button.setEnabled(false);
		pool4Button.setEnabled(false);
#endif

		onButton.setClickingTogglesState(true);
		onButton.setLookAndFeel(&rndLookAndFeel);
		onButton.setSize(100, buttonH);
		addAndMakeVisible(onButton);
		onButton.setButtonText("RND Notes");
		onButton.onClick = [this]
		{
			parent->setRandomizeNotes();
		};

		addAndMakeVisible(percentSlider);
		percentSlider.setSliderStyle(Slider::RotaryHorizontalVerticalDrag);
		percentSlider.setTextBoxStyle(Slider::TextBoxBelow, false, sliderTW, sliderTH);
		percentSlider.setTextBoxIsEditable(true);
		percentSlider.setTextBoxStyle(Slider::TextEntryBoxPosition::TextBoxBelow, false, sliderTW, sliderTH);
		percentSlider.setRange((double)0, (double)100, (double)1);
		percentSlider.setTextValueSuffix("%");

		percentSlider.onValueChange = [this]
		{
			parent->setRandomizeNotes();
		};

	} // RandomNoteComponent

	///////////////////////////////////////////////////////////////////////////////////

RandomNoteComponent::~RandomNoteComponent()
	{
	} // ~RandomNoteComponent

void RandomNoteComponent::setParent(TOPIARYVARIATIONCOMPONENT* p)
{
	parent = p;
} // setParent

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

/////////////////////////////////////////////////////////////////////////
// SwingComponent
/////////////////////////////////////////////////////////////////////////

SwingComponent::SwingComponent()
{
	pool1Button.setClickingTogglesState(true);
	pool1Button.setLookAndFeel(&rndLookAndFeel);

	addAndMakeVisible(pool1Button);
	pool1Button.setButtonText("1");
	pool1Button.onClick = [this]
	{
		parent->setSwing();
	};

	pool2Button.setClickingTogglesState(true);
	pool2Button.setLookAndFeel(&rndLookAndFeel);

	addAndMakeVisible(pool2Button);
	pool2Button.setButtonText("2");
	pool2Button.onClick = [this]
	{
		parent->setSwing();
	};

	pool3Button.setClickingTogglesState(true);
	pool3Button.setLookAndFeel(&rndLookAndFeel);
	//pool3Button.setSize(buttonH - 2, buttonH - 2);
	addAndMakeVisible(pool3Button);
	pool3Button.setButtonText("3");
	pool3Button.onClick = [this]
	{
		parent->setSwing();
	};

	pool4Button.setClickingTogglesState(true);
	pool4Button.setLookAndFeel(&rndLookAndFeel);
	//pool4Button.setSize(buttonH - 2, buttonH - 2);
	addAndMakeVisible(pool4Button);
	pool4Button.setButtonText("4");
	pool4Button.onClick = [this]
	{
		parent->setSwing();
	};

#ifndef BEATZ
		pool1Button.setEnabled(false);
		pool2Button.setEnabled(false);
		pool3Button.setEnabled(false);
		pool4Button.setEnabled(false);
#endif

	onButton.setClickingTogglesState(true);
	onButton.setLookAndFeel(&rndLookAndFeel);
	onButton.setSize(100, buttonH);
	addAndMakeVisible(onButton);
	onButton.setButtonText("Swing");
	onButton.onClick = [this]
	{
		parent->setSwing();
	};

	addAndMakeVisible(swingSlider);
	swingSlider.setSliderStyle(Slider::RotaryHorizontalVerticalDrag);
	swingSlider.setTextBoxStyle(Slider::TextBoxBelow, false, sliderTW, sliderTH);
	swingSlider.setTextBoxIsEditable(true);
	swingSlider.setTextBoxStyle(Slider::TextEntryBoxPosition::TextBoxBelow, false, sliderTW, sliderTH);
	swingSlider.setRange((double) -100, (double) 100, (double) 1);
	swingSlider.setTextValueSuffix("%");

	swingSlider.onValueChange = [this]
	{
		parent->setSwing();
	};

} // SwingComponent

///////////////////////////////////////////////////////////////////////////////////

SwingComponent::~SwingComponent()
{
} // ~SwingComponent

///////////////////////////////////////////////////////////////////////////////////

void SwingComponent::setParent(TOPIARYVARIATIONCOMPONENT* p)
{
	parent = p;
} // setParent

///////////////////////////////////////////////////////////////////////////////////

void  SwingComponent::resized()
{

} // resized

///////////////////////////////////////////////////////////////////////////////////

void  SwingComponent::paint(Graphics& g)
{

	int labelOffset = 15;
	int lineWidth = 2;
	auto area = getLocalBounds();
	g.fillAll(TopiaryColour::background);
	g.setColour(TopiaryColour::foreground);
	g.setFont(12.0f);

	g.drawText("Swing", (int)lineWidth, (int)lineWidth, 200, (int)labelOffset, juce::Justification::centredLeft);
	auto recBounds = area.removeFromBottom(area.getHeight() - labelOffset);
	g.drawRoundedRectangle((float)recBounds.getX() + lineWidth, (float)recBounds.getY() + lineWidth, (float)recBounds.getWidth() - 2 * lineWidth, (float)recBounds.getHeight() - 2 * lineWidth, (float)lineWidth, (float)lineWidth);

	// turn it into inner rectangle
	auto inRecBounds = Rectangle<int>::Rectangle(recBounds.getX() + 3 * lineWidth, recBounds.getY() + 3 * lineWidth, recBounds.getWidth() - 6 * lineWidth, recBounds.getHeight() - 6 * lineWidth);

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
	swingSlider.setBounds(inRecBounds);
	swingSlider.setSize(inRecBounds.getWidth(), inRecBounds.getHeight());
	swingSlider.setLookAndFeel(&rndLookAndFeel);

} // paint

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
		nameEditor.setSelectAllWhenFocused(true);
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

#ifdef BEATZ
		addAndMakeVisible(patternCombo);
		
		patternCombo.onChange = [this]
		{
			parent->setVariationDefinition();
		};
#endif
		
		
	} // VariationDefinitionComponent

	///////////////////////////////////////////////////////////////////////////////////

	void VariationDefinitionComponent::setParent(TOPIARYVARIATIONCOMPONENT* p)
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

		inRecBounds.removeFromTop(2 * lineWidth);
		topBounds = inRecBounds.removeFromTop(eH);

		// topBounds is second line of objects
		g.setColour(Colours::antiquewhite);
		
		variationCombo.setBounds(topBounds);
		
		inRecBounds.removeFromTop(2 * lineWidth);
		topBounds = inRecBounds.removeFromTop(eH);
		
		// topBounds is now third row which only has the enablebutton
		//hBounds = topBounds.removeFromLeft(eW);
		enableButton.setBounds(topBounds);
		enableButton.setSize(topBounds.getWidth(), topBounds.getHeight());

		inRecBounds.removeFromTop(2 * lineWidth);
		topBounds = inRecBounds.removeFromTop(eH);
#ifdef BEATZ
		patternCombo.setBounds(topBounds);
		patternCombo.setSize(topBounds.getWidth(), topBounds.getHeight());
#endif

	} // paint

#ifdef BEATZ
/////////////////////////////////////////////////////////////////////////
// PoolChannelComponent
/////////////////////////////////////////////////////////////////////////

	PoolChannelComponent::PoolChannelComponent()
	{
		setSize(width, heigth);
		addAndMakeVisible(poolChannelEditor1);
		poolChannelEditor1.setSize(eW, eH);
		poolChannelEditor1.setSelectAllWhenFocused(true);
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
		poolChannelEditor2.setSelectAllWhenFocused(true);
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
		poolChannelEditor3.setSelectAllWhenFocused(true);
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
		poolChannelEditor4.setSelectAllWhenFocused(true);
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

	void PoolChannelComponent ::setParent(TOPIARYVARIATIONCOMPONENT* p)
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

	// end poolChannelComponent
#endif

/////////////////////////////////////////////////////////////////////////
//VelocityComponent
/////////////////////////////////////////////////////////////////////////

	VelocityComponent::VelocityComponent()
	{
		pool1Button.setClickingTogglesState(true);
		pool1Button.setLookAndFeel(&rndLookAndFeel);

		addAndMakeVisible(pool1Button);
		pool1Button.setButtonText("1");
		pool1Button.onClick = [this]
		{
			parent->setVelocity();
		};

		pool2Button.setClickingTogglesState(true);
		pool2Button.setLookAndFeel(&rndLookAndFeel);

		addAndMakeVisible(pool2Button);
		pool2Button.setButtonText("2");
		pool2Button.onClick = [this]
		{
			parent->setVelocity();
		};

		pool3Button.setClickingTogglesState(true);
		pool3Button.setLookAndFeel(&rndLookAndFeel);
		//pool3Button.setSize(buttonH - 2, buttonH - 2);
		addAndMakeVisible(pool3Button);
		pool3Button.setButtonText("3");
		pool3Button.onClick = [this]
		{
			parent->setVelocity();
		};

		pool4Button.setClickingTogglesState(true);
		pool4Button.setLookAndFeel(&rndLookAndFeel);
		//pool4Button.setSize(buttonH - 2, buttonH - 2);
		addAndMakeVisible(pool4Button);
		pool4Button.setButtonText("4");
		pool4Button.onClick = [this]
		{
			parent->setVelocity();
		};

#ifndef BEATZ
		pool1Button.setEnabled(false);
		pool2Button.setEnabled(false);
		pool3Button.setEnabled(false);
		pool4Button.setEnabled(false);
#endif


		onButton.setClickingTogglesState(true);
		onButton.setLookAndFeel(&rndLookAndFeel);
		onButton.setSize(100, buttonH);
		addAndMakeVisible(onButton);
		onButton.setButtonText("RND Vel");
		onButton.onClick = [this]
		{
			parent->setVelocity();
		};

		addAndMakeVisible(velocitySlider);
		velocitySlider.setSliderStyle(Slider::RotaryHorizontalVerticalDrag);
		velocitySlider.setTextBoxStyle(Slider::TextBoxBelow, false, sliderTW, sliderTH);
		velocitySlider.setTextBoxIsEditable(true);
		velocitySlider.setTextBoxStyle(Slider::TextEntryBoxPosition::TextBoxBelow, false, sliderTW, sliderTH);
		velocitySlider.setRange((double)0, (double)100, (double)1);
		velocitySlider.setTextValueSuffix("%");

		velocitySlider.onValueChange = [this]
		{
			parent->setVelocity();
		};

		plusButton.setClickingTogglesState(true);
		plusButton.setLookAndFeel(&rndLookAndFeel);
		plusButton.setSize(plusminusSize, plusminusSize);
		addAndMakeVisible(plusButton);
		plusButton.setButtonText("+");
		plusButton.onClick = [this]
		{
			// do not allow both + and - buttons to be off at the same time
			if ((plusButton.getToggleState() == false) && (minusButton.getToggleState() == false))
				minusButton.setToggleState(true, dontSendNotification);
			
			parent->setVelocity();
		};

		minusButton.setClickingTogglesState(true);
		minusButton.setLookAndFeel(&rndLookAndFeel);
		minusButton.setSize(plusminusSize, plusminusSize);
		addAndMakeVisible(minusButton);
		minusButton.setButtonText("-");
		minusButton.onClick = [this]
		{
			// do not allow both + and - buttons to be off at the same time
			if ((plusButton.getToggleState() == false) && (minusButton.getToggleState() == false))
				plusButton.setToggleState(true, dontSendNotification);
			
			parent->setVelocity();
		};

	} // VelocityComponent

	///////////////////////////////////////////////////////////////////////////////////

	VelocityComponent::~VelocityComponent()
	{
	} // ~VelocityComponent

	///////////////////////////////////////////////////////////////////////////////////

	void VelocityComponent::setParent(TOPIARYVARIATIONCOMPONENT* p)
	{
		parent = p;
	} // setParent

	///////////////////////////////////////////////////////////////////////////////////

	void  VelocityComponent::resized()
	{

	} // resized

	///////////////////////////////////////////////////////////////////////////////////

	void  VelocityComponent::paint(Graphics& g)
	{

		int labelOffset = 15;
		int lineWidth = 2;
		auto area = getLocalBounds();
		g.fillAll(TopiaryColour::background);
		g.setColour(TopiaryColour::foreground);
		g.setFont(12.0f);

		g.drawText("Velocity", (int)lineWidth, (int)lineWidth, 200, (int)labelOffset, juce::Justification::centredLeft);
		auto recBounds = area.removeFromBottom(area.getHeight() - labelOffset);
		g.drawRoundedRectangle((float)recBounds.getX() + lineWidth, (float)recBounds.getY() + lineWidth, (float)recBounds.getWidth() - 2 * lineWidth, (float)recBounds.getHeight() - 2 * lineWidth, (float)lineWidth, (float)lineWidth);

		// turn it into inner rectangle
		auto inRecBounds = Rectangle<int>::Rectangle(recBounds.getX() + 3 * lineWidth, recBounds.getY() + 3 * lineWidth, recBounds.getWidth() - 6 * lineWidth, recBounds.getHeight() - 6 * lineWidth);

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
		velocitySlider.setBounds(inRecBounds);
		velocitySlider.setSize(inRecBounds.getWidth(), inRecBounds.getHeight());
		velocitySlider.setLookAndFeel(&rndLookAndFeel);

		// plusbutton top right
		pBounds = inRecBounds.removeFromRight(plusminusSize);
		pBounds.removeFromBottom(pBounds.getHeight() - plusminusSize);
		plusButton.setBounds(pBounds);

		// minus bottom top left
		pBounds = inRecBounds.removeFromLeft(plusminusSize);
		pBounds.removeFromBottom(pBounds.getHeight() - plusminusSize);
		minusButton.setBounds(pBounds);

	} // paint

// End VelocityComponent

/////////////////////////////////////////////////////////////////////////
// TimingComponent
/////////////////////////////////////////////////////////////////////////

	TimingComponent::TimingComponent()
	{
		pool1Button.setClickingTogglesState(true);
		pool1Button.setLookAndFeel(&rndLookAndFeel);
		addAndMakeVisible(pool1Button);
		pool1Button.setButtonText("1");
		pool1Button.onClick = [this]
		{
			parent->setTiming();
		};

		pool2Button.setClickingTogglesState(true);
		pool2Button.setLookAndFeel(&rndLookAndFeel);

		addAndMakeVisible(pool2Button);
		pool2Button.setButtonText("2");
		pool2Button.onClick = [this]
		{
			parent->setTiming();
		};

		pool3Button.setClickingTogglesState(true);
		pool3Button.setLookAndFeel(&rndLookAndFeel);
		//pool3Button.setSize(buttonH - 2, buttonH - 2);
		addAndMakeVisible(pool3Button);
		pool3Button.setButtonText("3");
		pool3Button.onClick = [this]
		{
			parent->setTiming();
		};

		pool4Button.setClickingTogglesState(true);
		pool4Button.setLookAndFeel(&rndLookAndFeel);
		//pool4Button.setSize(buttonH - 2, buttonH - 2);
		addAndMakeVisible(pool4Button);
		pool4Button.setButtonText("4");
		pool4Button.onClick = [this]
		{
			parent->setTiming();
		};
		
#ifndef BEATZ
		pool1Button.setEnabled(false);
		pool2Button.setEnabled(false);
		pool3Button.setEnabled(false);
		pool4Button.setEnabled(false);
#endif

		onButton.setClickingTogglesState(true);
		onButton.setLookAndFeel(&rndLookAndFeel);
		onButton.setSize(100, buttonH);
		addAndMakeVisible(onButton);
		onButton.setButtonText("RND Timing");
		onButton.onClick = [this]
		{
			parent->setTiming();
		};

		addAndMakeVisible(timingSlider);
		timingSlider.setSliderStyle(Slider::RotaryHorizontalVerticalDrag);
		timingSlider.setTextBoxStyle(Slider::TextBoxBelow, false, sliderTW, sliderTH);
		timingSlider.setTextBoxIsEditable(true);
		timingSlider.setTextBoxStyle(Slider::TextEntryBoxPosition::TextBoxBelow, false, sliderTW, sliderTH);
		timingSlider.setRange((double)0, (double)100, (double)1);
		timingSlider.setTextValueSuffix("%");

		timingSlider.onValueChange = [this]
		{
			parent->setTiming();
		};

		plusButton.setClickingTogglesState(true);
		plusButton.setLookAndFeel(&rndLookAndFeel);
		plusButton.setSize(plusminusSize, plusminusSize);
		addAndMakeVisible(plusButton);
		plusButton.setButtonText("+");
		plusButton.onClick = [this]
		{
			if ((plusButton.getToggleState() == false) && (minusButton.getToggleState() == false))
				minusButton.setToggleState(true, dontSendNotification);

			parent->setTiming();
		};

		minusButton.setClickingTogglesState(true);
		minusButton.setLookAndFeel(&rndLookAndFeel);
		minusButton.setSize(plusminusSize, plusminusSize);
		addAndMakeVisible(minusButton);
		minusButton.setButtonText("-");
		minusButton.onClick = [this]
		{
			if ((plusButton.getToggleState() == false) && (minusButton.getToggleState() == false))
				plusButton.setToggleState(true, dontSendNotification);

			parent->setTiming();
		};

	} // TimingComponent

	///////////////////////////////////////////////////////////////////////////////////

	TimingComponent::~TimingComponent()
	{
	} // ~TimingComponent

	///////////////////////////////////////////////////////////////////////////////////

	void TimingComponent::setParent(TOPIARYVARIATIONCOMPONENT* p)
	{
		parent = p;
	} // setParent

	///////////////////////////////////////////////////////////////////////////////////

	void  TimingComponent::resized()
	{

	} // resized

	///////////////////////////////////////////////////////////////////////////////////

	void  TimingComponent::paint(Graphics& g)
	{

		int labelOffset = 15;
		int lineWidth = 2;
		auto area = getLocalBounds();
		g.fillAll(TopiaryColour::background);
		g.setColour(TopiaryColour::foreground);
		g.setFont(12.0f);

		g.drawText("Timing", (int)lineWidth, (int)lineWidth, 200, (int)labelOffset, juce::Justification::centredLeft);
		auto recBounds = area.removeFromBottom(area.getHeight() - labelOffset);
		g.drawRoundedRectangle((float)recBounds.getX() + lineWidth, (float)recBounds.getY() + lineWidth, (float)recBounds.getWidth() - 2 * lineWidth, (float)recBounds.getHeight() - 2 * lineWidth, (float)lineWidth, (float)lineWidth);

		// turn it into inner rectangle
		auto inRecBounds = Rectangle<int>::Rectangle(recBounds.getX() + 3 * lineWidth, recBounds.getY() + 3 * lineWidth, recBounds.getWidth() - 6 * lineWidth, recBounds.getHeight() - 6 * lineWidth);

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

		timingSlider.setBounds(inRecBounds);
		timingSlider.setSize(inRecBounds.getWidth(), inRecBounds.getHeight());
		timingSlider.setLookAndFeel(&rndLookAndFeel);

		// plusbutton top right
		pBounds = inRecBounds.removeFromRight(plusminusSize);
		pBounds.removeFromBottom(pBounds.getHeight() - plusminusSize);
		plusButton.setBounds(pBounds);

		// minus bottom top left
		pBounds = inRecBounds.removeFromLeft(plusminusSize);
		pBounds.removeFromBottom(pBounds.getHeight() - plusminusSize);
		minusButton.setBounds(pBounds);

	} // paint

// End TimingComponent

/////////////////////////////////////////////////////////////////////////
// VariationTypeComponent
/////////////////////////////////////////////////////////////////////////

	VariationTypeComponent::VariationTypeComponent()
	{
		setSize(width, heigth);
		
		addAndMakeVisible(introButton);
		introButton.setClickingTogglesState(true);
		introButton.setButtonText("Intro");
		introButton.setRadioGroupId(Topiary::VariationTypeRadioID);
		introButton.onClick = [this]
		{
			type = Topiary::VariationTypeIntro;
			parent->setVariationDefinition();
		};

		addAndMakeVisible(fillButton);
		fillButton.setClickingTogglesState(true);
		fillButton.setButtonText("Fill");
		fillButton.setRadioGroupId(Topiary::VariationTypeRadioID);
		fillButton.onClick = [this]
		{
			type = Topiary::VariationTypeFill;
			parent->setVariationDefinition();
		};

		addAndMakeVisible(endButton);
		endButton.setClickingTogglesState(true);
		endButton.setButtonText("Ending");
		endButton.setRadioGroupId(Topiary::VariationTypeRadioID);
		endButton.onClick = [this]
		{
			type = Topiary::VariationTypeEnd;
			parent->setVariationDefinition();
		};

		addAndMakeVisible(steadyButton);
		steadyButton.setClickingTogglesState(true);
		steadyButton.setButtonText("Steady");
		steadyButton.setRadioGroupId(Topiary::VariationTypeRadioID);
		steadyButton.onClick = [this]
		{
			type = Topiary::VariationTypeSteady;
			parent->setVariationDefinition();
		};

	} // VariationTypeComponent

	///////////////////////////////////////////////////////////////////////////////////

	void VariationTypeComponent::setParent(TOPIARYVARIATIONCOMPONENT* p)
	{
		parent = p;
	} // setparent

	///////////////////////////////////////////////////////////////////////////////////

	VariationTypeComponent::~VariationTypeComponent()
	{
	} // ~VariationTypeComponent

	///////////////////////////////////////////////////////////////////////////////////

	void  VariationTypeComponent::resized()
	{

	} // resized

	///////////////////////////////////////////////////////////////////////////////////

	void  VariationTypeComponent::paint(Graphics& g)
	{
		int labelOffset = 15;
		int lineWidth = 2;
		auto area = getLocalBounds();

		g.fillAll(TopiaryColour::background);
		g.setColour(TopiaryColour::foreground);
		g.setFont(12.0f);
		g.drawText("Variation type", lineWidth, lineWidth, 200, labelOffset, juce::Justification::centredLeft);

		auto recBounds = area.removeFromBottom(area.getHeight() - labelOffset);
		g.drawRoundedRectangle((float)recBounds.getX() + lineWidth, (float)recBounds.getY() + lineWidth, (float)recBounds.getWidth() - 2 * lineWidth, (float)recBounds.getHeight() - 2 * lineWidth, (float)lineWidth, (float)lineWidth);

		// turn it into inner rectangle
		auto inRecBounds = Rectangle<int>::Rectangle(recBounds.getX() + 3 * lineWidth, recBounds.getY() + 3 * lineWidth, recBounds.getWidth() - 6 * lineWidth, recBounds.getHeight() - 6 * lineWidth);

		// area for name and from editors
		auto topBounds = inRecBounds.removeFromTop(eH);

		// topBounds is first line of objects
		//auto hBounds = topBounds.removeFromLeft(eW);
		introButton.setBounds(topBounds);

		inRecBounds.removeFromTop(2 * lineWidth);
		topBounds = inRecBounds.removeFromTop(eH);

		// topBounds is second line of objects
		steadyButton.setBounds(topBounds);
		steadyButton.setSize(topBounds.getWidth(), topBounds.getHeight());

		inRecBounds.removeFromTop(2 * lineWidth);
		topBounds = inRecBounds.removeFromTop(eH);

		// topBounds is now third row which only has the enablebutton
		fillButton.setBounds(topBounds);
		fillButton.setSize(topBounds.getWidth(), topBounds.getHeight());

		inRecBounds.removeFromTop(2 * lineWidth);
		topBounds = inRecBounds.removeFromTop(eH);

		// topBounds is now fourth row which only has the patternCombo
		// hBounds = topBounds.removeFromLeft(eW);
		endButton.setBounds(topBounds);
		endButton.setSize(topBounds.getWidth(), topBounds.getHeight());

	} // paint

#ifdef BEATZ
/////////////////////////////////////////////////////////////////////////
// VelocityOffsetComponent
/////////////////////////////////////////////////////////////////////////

	VelocityOffsetComponent::VelocityOffsetComponent()
	{
		setSize(width, heigth);

		addAndMakeVisible(velocityOffsetButton);
		velocityOffsetButton.setClickingTogglesState(true);
		velocityOffsetButton.setButtonText("Velocity Offset");
		velocityOffsetButton.onClick = [this]
		{
			parent->setVelocityOffset();
		};

		addAndMakeVisible(velocityOffsetSlider0);
		velocityOffsetSlider0.setLookAndFeel(&lookAndFeel);
		velocityOffsetSlider0.setSize(sliderW, sliderH);
		velocityOffsetSlider0.setSliderStyle(Slider::RotaryHorizontalVerticalDrag);
		velocityOffsetSlider0.setTextBoxStyle(Slider::TextBoxBelow, false, sliderTW, sliderTH);
		velocityOffsetSlider0.setTextBoxIsEditable(true);
		velocityOffsetSlider0.setTextBoxStyle(Slider::TextEntryBoxPosition::TextBoxBelow, false, sliderTW, sliderTH);
		velocityOffsetSlider0.setRange((double)-127, (double)127, (double)1);
		velocityOffsetSlider0.setTextValueSuffix("");
		velocityOffsetSlider0.onValueChange = [this]
		{
			parent->setVelocityOffset();
		};

		addAndMakeVisible(velocityOffsetSlider1);
		velocityOffsetSlider1.setLookAndFeel(&lookAndFeel);
		velocityOffsetSlider1.setSize(sliderW, sliderH);
		velocityOffsetSlider1.setSliderStyle(Slider::RotaryHorizontalVerticalDrag);
		velocityOffsetSlider1.setTextBoxStyle(Slider::TextBoxBelow, false, sliderTW, sliderTH);
		velocityOffsetSlider1.setTextBoxIsEditable(true);
		velocityOffsetSlider1.setTextBoxStyle(Slider::TextEntryBoxPosition::TextBoxBelow, false, sliderTW, sliderTH);
		velocityOffsetSlider1.setRange((double)-127, (double)127, (double)1);
		velocityOffsetSlider1.setTextValueSuffix("");
		velocityOffsetSlider1.onValueChange = [this]
		{
			parent->setVelocityOffset();
		};

		addAndMakeVisible(velocityOffsetSlider2);
		velocityOffsetSlider2.setLookAndFeel(&lookAndFeel);
		velocityOffsetSlider2.setSize(sliderW, sliderH);
		velocityOffsetSlider2.setSliderStyle(Slider::RotaryHorizontalVerticalDrag);
		velocityOffsetSlider2.setTextBoxStyle(Slider::TextBoxBelow, false, sliderTW, sliderTH);
		velocityOffsetSlider2.setTextBoxIsEditable(true);
		velocityOffsetSlider2.setTextBoxStyle(Slider::TextEntryBoxPosition::TextBoxBelow, false, sliderTW, sliderTH);
		velocityOffsetSlider2.setRange((double)-127, (double)127, (double)1);
		velocityOffsetSlider2.setTextValueSuffix("");
		velocityOffsetSlider2.onValueChange = [this]
		{
			parent->setVelocityOffset();
		};

		addAndMakeVisible(velocityOffsetSlider3);
		velocityOffsetSlider3.setLookAndFeel(&lookAndFeel);
		velocityOffsetSlider3.setSize(sliderW, sliderH);
		velocityOffsetSlider3.setSliderStyle(Slider::RotaryHorizontalVerticalDrag);
		velocityOffsetSlider3.setTextBoxStyle(Slider::TextBoxBelow, false, sliderTW, sliderTH);
		velocityOffsetSlider3.setTextBoxIsEditable(true);
		velocityOffsetSlider3.setTextBoxStyle(Slider::TextEntryBoxPosition::TextBoxBelow, false, sliderTW, sliderTH);
		velocityOffsetSlider3.setRange((double)-127, (double)127, (double)1);
		velocityOffsetSlider3.setTextValueSuffix("");
		velocityOffsetSlider3.onValueChange = [this]
		{
			parent->setVelocityOffset();
		};


	} // VariationOffsetComponent

	///////////////////////////////////////////////////////////////////////////////////

	void VelocityOffsetComponent::setParent(TOPIARYVARIATIONCOMPONENT* p)
	{
		parent = p;
	} // setparent

	///////////////////////////////////////////////////////////////////////////////////

	VelocityOffsetComponent::~VelocityOffsetComponent()
	{
	} // ~VariationTypeComponent

	///////////////////////////////////////////////////////////////////////////////////

	void  VelocityOffsetComponent::resized()
	{

	} // resized

	///////////////////////////////////////////////////////////////////////////////////

	void  VelocityOffsetComponent::paint(Graphics& g)
	{
		int labelOffset = 15;
		int lineWidth = 2;
		auto area = getLocalBounds();

		g.fillAll(TopiaryColour::background);
		g.setColour(TopiaryColour::foreground);
		g.setFont(12.0f);
		g.drawText("Velocity Offset", lineWidth, lineWidth, 200, labelOffset, juce::Justification::centredLeft);

		auto recBounds = area.removeFromBottom(area.getHeight() - labelOffset);
		g.drawRoundedRectangle((float)recBounds.getX() + lineWidth, (float)recBounds.getY() + lineWidth, (float)recBounds.getWidth() - 2 * lineWidth, (float)recBounds.getHeight() - 2 * lineWidth, (float)lineWidth, (float)lineWidth);

		// turn it into inner rectangle
		auto inRecBounds = Rectangle<int>::Rectangle(recBounds.getX() + 3 * lineWidth, recBounds.getY() + 3 * lineWidth, recBounds.getWidth() - 6 * lineWidth, recBounds.getHeight() - 6 * lineWidth);

		// area for name and from editors
		auto topBounds = inRecBounds.removeFromTop(eH);

		// topBounds is first line of objects
		//auto hBounds = topBounds.removeFromLeft(eW);
		velocityOffsetButton.setBounds(topBounds);
		velocityOffsetButton.setSize(topBounds.getWidth(), topBounds.getHeight());

		//inRecBounds.removeFromTop(4 * lineWidth);
		inRecBounds = inRecBounds.removeFromBottom(sliderH);
		topBounds = inRecBounds.removeFromLeft(sliderW);
		velocityOffsetSlider0.setSize(topBounds.getWidth(), topBounds.getHeight());
		velocityOffsetSlider0.setBounds(topBounds);

		inRecBounds.removeFromLeft(4 * lineWidth);
		topBounds = inRecBounds.removeFromLeft(sliderW);
		velocityOffsetSlider1.setSize(topBounds.getWidth(), topBounds.getHeight());
		velocityOffsetSlider1.setBounds(topBounds);

		inRecBounds.removeFromLeft(4 * lineWidth);
		topBounds = inRecBounds.removeFromLeft(sliderW);
		velocityOffsetSlider2.setSize(topBounds.getWidth(), topBounds.getHeight());
		velocityOffsetSlider2.setBounds(topBounds);

		inRecBounds.removeFromLeft(4 * lineWidth);
		topBounds = inRecBounds.removeFromLeft(sliderW);
		velocityOffsetSlider3.setSize(topBounds.getWidth(), topBounds.getHeight());
		velocityOffsetSlider3.setBounds(topBounds);

	} // paint
#endif

/////////////////////////////////////////////////////////////////////////
// SwingQComponent
/////////////////////////////////////////////////////////////////////////

	SwingQComponent::SwingQComponent()
	{
		setSize(width, heigth);
		
		addAndMakeVisible(fourButton);
		fourButton.setClickingTogglesState(true);
		fourButton.setButtonText("1/4");
		fourButton.setRadioGroupId(Topiary::SwingQButtonIds::SwingQRadioID);
		fourButton.onClick = [this]
		{
			swingQ = Topiary::SwingQButtonIds::SwingQ4;
			parent->setSwingQ();
		};

		addAndMakeVisible(eightButton);
		eightButton.setClickingTogglesState(true);
		eightButton.setButtonText("1/8");
		eightButton.setRadioGroupId(Topiary::SwingQButtonIds::SwingQRadioID);
		eightButton.onClick = [this]
		{
			swingQ = Topiary::SwingQButtonIds::SwingQ8;
			parent->setSwingQ();
		};


	} // SwingQComponent

	///////////////////////////////////////////////////////////////////////////////////

	void SwingQComponent::setParent(TOPIARYVARIATIONCOMPONENT* p)
	{
		parent = p;
	} // setparent

	///////////////////////////////////////////////////////////////////////////////////

	SwingQComponent::~SwingQComponent()
	{
	} // ~PoolChannelComponent

	///////////////////////////////////////////////////////////////////////////////////

	void  SwingQComponent::resized()
	{

	} // resized

	///////////////////////////////////////////////////////////////////////////////////

	void  SwingQComponent::paint(Graphics& g)
	{
		int labelOffset = 15;
		int lineWidth = 2;
		auto area = getLocalBounds();

		g.fillAll(TopiaryColour::background);
		g.setColour(TopiaryColour::foreground);
		g.setFont(12.0f);
		g.drawText("Swing resolution", lineWidth, lineWidth, 200, labelOffset, juce::Justification::centredLeft);

		auto recBounds = area.removeFromBottom(area.getHeight() - labelOffset);
		g.drawRoundedRectangle((float)recBounds.getX() + lineWidth, (float)recBounds.getY() + lineWidth, (float)recBounds.getWidth() - 2 * lineWidth, (float)recBounds.getHeight() - 2 * lineWidth, (float)lineWidth, (float)lineWidth);

		// turn it into inner rectangle
		auto inRecBounds = Rectangle<int>::Rectangle(recBounds.getX() + 3 * lineWidth, recBounds.getY() + 3 * lineWidth, recBounds.getWidth() - 6 * lineWidth, recBounds.getHeight() - 6 * lineWidth);

		// area for name and from editors
		auto topBounds = inRecBounds.removeFromTop(bH);

		// topBounds is first line of objects
		auto eBounds = topBounds.removeFromLeft(bW);
		fourButton.setBounds(eBounds);

		topBounds.removeFromLeft(4 * lineWidth);
		eBounds = topBounds.removeFromLeft(bW);
		eightButton.setBounds(eBounds);

	} // paint

#ifndef BEATZ

	/////////////////////////////////////////////////////////////////////////
	// NoteLengthComponent
	/////////////////////////////////////////////////////////////////////////

	NoteLengthComponent::NoteLengthComponent()
	{

		onButton.setClickingTogglesState(true);
		onButton.setLookAndFeel(&rndLookAndFeel);
		onButton.setSize(100, buttonH);
		addAndMakeVisible(onButton);
		onButton.setButtonText("RND Length");
		onButton.onClick = [this]
		{
			parent->setNoteLength();
		};

		addAndMakeVisible(percentSlider);
		percentSlider.setSliderStyle(Slider::RotaryHorizontalVerticalDrag);
		percentSlider.setTextBoxStyle(Slider::TextBoxBelow, false, sliderTW, sliderTH);
		percentSlider.setTextBoxIsEditable(true);
		percentSlider.setTextBoxStyle(Slider::TextEntryBoxPosition::TextBoxBelow, false, sliderTW, sliderTH);
		percentSlider.setRange((double)0, (double)100, (double)1);
		percentSlider.setTextValueSuffix("%");

		percentSlider.onValueChange = [this]
		{
			parent->setNoteLength();
		};

		plusButton.setClickingTogglesState(true);
		plusButton.setLookAndFeel(&rndLookAndFeel);
		plusButton.setSize(plusminusSize, plusminusSize);
		addAndMakeVisible(plusButton);
		plusButton.setButtonText("+");
		plusButton.onClick = [this]
		{
			// do not allow both + and - buttons to be off at the same time
			if ((plusButton.getToggleState() == false) && (minusButton.getToggleState() == false))
				minusButton.setToggleState(true, dontSendNotification);

			parent->setNoteLength();
		};

		minusButton.setClickingTogglesState(true);
		minusButton.setLookAndFeel(&rndLookAndFeel);
		minusButton.setSize(plusminusSize, plusminusSize);
		addAndMakeVisible(minusButton);
		minusButton.setButtonText("-");
		minusButton.onClick = [this]
		{
			// do not allow both + and - buttons to be off at the same time
			if ((plusButton.getToggleState() == false) && (minusButton.getToggleState() == false))
				plusButton.setToggleState(true, dontSendNotification);
			parent->setNoteLength();
		};

	} // NoteLenghtComponent

	///////////////////////////////////////////////////////////////////////////////////

    NoteLengthComponent::~NoteLengthComponent()
	{
	} // ~NoteLengthComponent

	void NoteLengthComponent::setParent(TOPIARYVARIATIONCOMPONENT* p)
	{
		parent = p;
	} // setParent

  ///////////////////////////////////////////////////////////////////////////////////

	void  NoteLengthComponent::resized()
	{

	} // resized 

	///////////////////////////////////////////////////////////////////////////////////

	void  NoteLengthComponent::paint(Graphics& g)
	{
		
		int labelOffset = 15;
		int lineWidth = 2;
		auto area = getLocalBounds();
		g.fillAll(TopiaryColour::background);
		g.setColour(TopiaryColour::foreground);
		g.setFont(12.0f);

		g.drawText("Note Length", (int) lineWidth, (int) lineWidth, 200, (int) labelOffset, juce::Justification::centredLeft);
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
		
		inRecBounds.removeFromLeft(2 * lineWidth);
		percentSlider.setBounds(inRecBounds);
		percentSlider.setSize(inRecBounds.getWidth(), inRecBounds.getHeight());
		percentSlider.setLookAndFeel(&rndLookAndFeel);

		// plusbutton top right
		auto pBounds = inRecBounds.removeFromRight(plusminusSize);
		pBounds.removeFromBottom(pBounds.getHeight() - plusminusSize);
		plusButton.setBounds(pBounds);

		// minus bottom top left
		pBounds = inRecBounds.removeFromLeft(plusminusSize);
		pBounds.removeFromBottom(pBounds.getHeight() - plusminusSize);
		minusButton.setBounds(pBounds);

			
	} // paint
#endif  // ifndef BEATZ