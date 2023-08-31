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
	setSize(p.bounds.getWidth(),p.bounds.getHeight());

	sldRefreshRate.reset(new AttachedSlider(*p.refreshRate));
	sldSmoothing.reset(new AttachedSlider(*p.smoothing));

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
	//resizing = false;
}

void MagnitudeAudioProcessorEditor::resized()
{
	audioProcessor.bounds = getLocalBounds();
	performLayout(getLocalBounds(), *sldRefreshRate, *sldSmoothing);
	//resizing = true;
}
