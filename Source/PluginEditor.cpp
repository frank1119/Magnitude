/*
  ==============================================================================

	This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

using namespace juce;

//==============================================================================
MagnitudeAudioProcessorEditor::MagnitudeAudioProcessorEditor(MagnitudeAudioProcessor& p)
	: AudioProcessorEditor(&p), audioProcessor(p)

{
	setSize(p.Bounds.getWidth(),p.Bounds.getHeight());

	sldRefreshRate.reset(new AttachedSlider(*(juce::RangedAudioParameter*)p.RefreshRateParameter));
	sldSmoothing.reset(new AttachedSlider(*(juce::RangedAudioParameter*)p.SmoothingParameter));

	addAllAndMakeVisible(*this, *sldRefreshRate, *sldSmoothing);
	performLayout(getLocalBounds(), *sldRefreshRate, *sldSmoothing);

	this->setResizeLimits(350, 150, 700, 300);
	this->setResizable(true, true);
}

MagnitudeAudioProcessorEditor::~MagnitudeAudioProcessorEditor()
{
}

//==============================================================================
void MagnitudeAudioProcessorEditor::paint(juce::Graphics& g)
{
	// (Our component is opaque, so we must completely fill the background with a solid colour)
	//g.fillAll(getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId));
	g.fillAll(juce::Colours::slategrey);
	
	//if (resizing)
//		performLayout(getLocalBounds(), *this->sldRefreshRate.get(), *this->sldSmoothing);
}

void MagnitudeAudioProcessorEditor::resized()
{
	audioProcessor.Bounds = getLocalBounds();
	performLayout(getLocalBounds(), *sldRefreshRate, *sldSmoothing);
}
