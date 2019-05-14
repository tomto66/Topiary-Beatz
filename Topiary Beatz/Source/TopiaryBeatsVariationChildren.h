/////////////////////////////////////////////////////////////////////////////
/*
This file is part of Topiary Beatz, Copyright Tom Tollenaere 2018-19.

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
#include"TopiaryBeatsModel.h"

class TopiaryBeatsVariationComponent;

////////////////////////////////////////////////////////////////////////////////
// EnablePoolComponent
////////////////////////////////////////////////////////////////////////////////

class EnablePoolComponent : public Component
{

public:
	TopiaryButton enableButton1, enableButton2, enableButton3, enableButton4;
	EnablePoolComponent();
	~EnablePoolComponent();
	void resized();
	void paint(Graphics& g) override;
	void setParent(TopiaryBeatsVariationComponent* p);
	int width = 85;
	int heigth = 120;

private:
	TopiaryBeatsVariationComponent* parent;
	static const int eW = 120; //  enable button
	static const int eH = 20;
	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(EnablePoolComponent)
};

////////////////////////////////////////////////////////////////////////////////
// PoolLengthComponent
////////////////////////////////////////////////////////////////////////////////

class PoolLengthComponent : public Component
{
public:
	PoolLengthComponent();
	~PoolLengthComponent();
	void paint(Graphics&) override;
	void resized() override;
	void setParent(TopiaryBeatsVariationComponent* p);

	
	int width = 168;
	int heigth = 215;

private:
	TopiaryBeatsVariationComponent* parent;
	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PoolLengthComponent)
};

////////////////////////////////////////////////////////////////////////////////
// RandomNoteComponent
////////////////////////////////////////////////////////////////////////////////

class RandomNoteComponent : public Component
{
public:

	TopiaryLookAndFeel rndLookAndFeel;
	TopiaryButton pool1Button;
	TopiaryButton pool2Button;
	TopiaryButton pool3Button;
	TopiaryButton pool4Button;

	TopiaryButton onButton;
	Slider	percentSlider;

	int width = 85;
	int heigth = 120;

	RandomNoteComponent();
	~RandomNoteComponent();
	void resized();
	void paint(Graphics& g) override;
	void setParent(TopiaryBeatsVariationComponent* p);

private:

	TopiaryBeatsVariationComponent* parent;
	
	static const int buttonH = 20;
	static const int pbuttonH = 12;
	static const int sliderH = 100;
	static const int sliderV = 65;
	static const int sliderTW = 60;
	static const int sliderTH = 15;

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(RandomNoteComponent)

}; // RandomNoteComponent

////////////////////////////////////////////////////////////////////////////////
// VariationDefinitionComponent
////////////////////////////////////////////////////////////////////////////////

class VariationDefinitionComponent : public Component
{
public:

	ComboBox variationCombo;
	TextEditor nameEditor;
	TopiaryButton enableButton;
	ComboBox patternCombo;

	int width = 140;
	int heigth = 120;

	VariationDefinitionComponent();
	~VariationDefinitionComponent();
	void setParent(TopiaryBeatsVariationComponent* p);

	void resized();
	void paint(Graphics& g) override;

private:

	TopiaryBeatsVariationComponent* parent;

	static const int eW = 120; // editor & combo & enable button
	static const int eH = 20; 

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(VariationDefinitionComponent)

}; // VariationDefinitionComponent

////////////////////////////////////////////////////////////////////////////////
// SwingComponent
////////////////////////////////////////////////////////////////////////////////

class SwingComponent : public Component
{
public:

	TopiaryLookAndFeel rndLookAndFeel;
	TopiaryButton pool1Button;
	TopiaryButton pool2Button;
	TopiaryButton pool3Button;
	TopiaryButton pool4Button;

	TopiaryButton onButton;
	Slider	swingSlider;

	int width = 85;
	int heigth = 120;

	SwingComponent();
	~SwingComponent();
	void resized();
	void paint(Graphics& g) override;
	void setParent(TopiaryBeatsVariationComponent* p);

private:

	TopiaryBeatsVariationComponent* parent;

	static const int buttonH = 20;
	static const int pbuttonH = 12;
	static const int sliderH = 100;
	static const int sliderV = 65;
	static const int sliderTW = 60;
	static const int sliderTH = 15;

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SwingComponent)

}; // SwingComponent

////////////////////////////////////////////////////////////////////////////////
// VelocityComponent
////////////////////////////////////////////////////////////////////////////////

class VelocityComponent : public Component
{
public:

	TopiaryLookAndFeel rndLookAndFeel;
	TopiaryButton pool1Button;
	TopiaryButton pool2Button;
	TopiaryButton pool3Button;
	TopiaryButton pool4Button;

	TopiaryButton plusButton;
	TopiaryButton minusButton;

	TopiaryButton onButton;
	Slider	velocitySlider;

	int width = 85;
	int heigth = 120;

	VelocityComponent();
	~VelocityComponent();
	void resized();
	void paint(Graphics& g) override;
	void setParent(TopiaryBeatsVariationComponent* p);

private:

	TopiaryBeatsVariationComponent* parent;

	static const int buttonH = 20;
	static const int pbuttonH = 12;
	static const int sliderH = 100;
	static const int sliderV = 65;
	static const int sliderTW = 60;
	static const int sliderTH = 15;
	static const int plusminusSize = 12;

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(VelocityComponent)

}; // VelocityComponent

////////////////////////////////////////////////////////////////////////////////
// PoolChannelComponent
////////////////////////////////////////////////////////////////////////////////

class PoolChannelComponent : public Component
{
public:

	TextEditor poolChannelEditor1, poolChannelEditor2, poolChannelEditor3, poolChannelEditor4;
	
	int width = 140;
	int heigth = 48;

	PoolChannelComponent();
	~PoolChannelComponent();
	void setParent(TopiaryBeatsVariationComponent* p);

	void resized();
	void paint(Graphics& g) override;

private:

	TopiaryBeatsVariationComponent* parent;

	static const int eW = 26; // editor & combo & enable button
	static const int eH = 20;

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PoolChannelComponent)

}; // PoolChannelComponent


////////////////////////////////////////////////////////////////////////////////
// TimingComponent
////////////////////////////////////////////////////////////////////////////////

class TimingComponent : public Component
{
public:

	TopiaryLookAndFeel rndLookAndFeel;
	TopiaryButton pool1Button;
	TopiaryButton pool2Button;
	TopiaryButton pool3Button;
	TopiaryButton pool4Button;

	TopiaryButton plusButton;
	TopiaryButton minusButton;

	TopiaryButton onButton;
	Slider	timingSlider;

	int width = 85;
	int heigth = 120;

	TimingComponent();
	~TimingComponent();
	void resized();
	void paint(Graphics& g) override;
	void setParent(TopiaryBeatsVariationComponent* p);

private:

	TopiaryBeatsVariationComponent* parent;

	static const int plusminusSize = 12;
	static const int buttonH = 20;
	static const int pbuttonH = 12;
	static const int sliderH = 100;
	static const int sliderV = 65;
	static const int sliderTW = 60;
	static const int sliderTH = 15;

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(TimingComponent)

}; // TimingComponent

////////////////////////////////////////////////////////////////////////////////
// VariationTypeComponent
////////////////////////////////////////////////////////////////////////////////

class VariationTypeComponent : public Component
{
public:

	TopiaryButton introButton;
	TopiaryButton steadyButton;
	TopiaryButton fillButton;
	TopiaryButton endButton;

	int width = 85;
	int heigth = 120; 
	int type = Topiary::VariationTypeSteady;
	VariationTypeComponent();
	~VariationTypeComponent();
	void setParent(TopiaryBeatsVariationComponent* p);

	void resized();
	void paint(Graphics& g) override;

private:
	TopiaryBeatsVariationComponent* parent;

	static const int eW = 120; // editor & combo & enable button
	static const int eH = 20;
	
	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(VariationTypeComponent)

}; // VariationTypeComponent

////////////////////////////////////////////////////////////////////////////////
// VelocityOffsetComponent
////////////////////////////////////////////////////////////////////////////////

class VelocityOffsetComponent : public Component
{
public:
	TopiaryLookAndFeel lookAndFeel;
	TopiaryButton velocityOffsetButton;
	Slider	velocityOffsetSlider0;
	Slider	velocityOffsetSlider1;
	Slider	velocityOffsetSlider2;
	Slider	velocityOffsetSlider3;

	int width = 250;
	int heigth = 120;

	VelocityOffsetComponent();
	~VelocityOffsetComponent();
	void setParent(TopiaryBeatsVariationComponent* p);

	void resized();
	void paint(Graphics& g) override;

private:
	TopiaryBeatsVariationComponent* parent;

	static const int eW = 120; // editor & combo & enable button
	static const int eH = 20;
	static const int sliderH = 69;
	static const int sliderW = 53;
	static const int sliderTW = 60;
	static const int sliderTH = 15;
	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(VelocityOffsetComponent)

}; // VariationOffsetComponent

////////////////////////////////////////////////////////////////////////////////
// SwingQComponent
////////////////////////////////////////////////////////////////////////////////

class SwingQComponent : public Component
{
public:

	TopiaryButton fourButton, eightButton;

	int width = 140;
	int heigth = 48;
	int swingQ;

	SwingQComponent();
	~SwingQComponent();
	void setParent(TopiaryBeatsVariationComponent* p);

	void resized();
	void paint(Graphics& g) override;

private:

	TopiaryBeatsVariationComponent* parent;

	static const int bW = 60; // button size
	static const int bH = 20;

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SwingQComponent)

}; // PoolChannelComponent