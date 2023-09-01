/*
  ==============================================================================

	This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "ChannelToParamMap.h"


//==============================================================================
/**
*/
class MagnitudeAudioProcessor : public juce::AudioProcessor, public juce::AudioProcessorParameter::Listener, public juce::Timer
#if JucePlugin_Enable_ARA
	, public juce::AudioProcessorARAExtension
#endif
{

public:
	//==============================================================================
	MagnitudeAudioProcessor();
	~MagnitudeAudioProcessor() override;

	virtual void parameterValueChanged(int parameterIndex, float newValue) override;
	void parameterGestureChanged(int parameterIndex, bool gestureIsStarting) override;

	virtual juce::AudioProcessorParameter* getBypassParameter() const override;

	// Parameters reporting to the host about the magnitude
	juce::AudioParameterFloat* db[MAXCHANNELS * MAXBUSES] = { nullptr };

	// Frequency to report
	juce::AudioParameterInt* refreshRate = { nullptr };

	// Smoothing percentage
	juce::AudioParameterInt* smoothing = { nullptr };

	// The magnitudes found during processing
	float dbs[MAXCHANNELS * MAXBUSES] = { 0.0f };
	// Smoothed magnitudes
	float smthDbs[MAXCHANNELS * MAXBUSES] = { 0.0f };

	// Previous smoothed magnitudes (makes sure that no unnecessary updates are boadcasted 
	// to the host
	float prevDbs[MAXCHANNELS * MAXBUSES] = { 0.0f };

	// Smoothing factor
	float fSmooth = 0.0f;
	// For restoring the editor-size
	juce::Rectangle<int> bounds = juce::Rectangle(0, 0, 350, 150);

	//==============================================================================
	void prepareToPlay(double sampleRate, int samplesPerBlock) override;
	void releaseResources() override;

	double processSample(float newSample, double curEnvVal);

#ifndef JucePlugin_PreferredChannelConfigurations
	bool isBusesLayoutSupported(const BusesLayout& layouts) const override;
#endif

	void processBlock(juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

	//==============================================================================
	juce::AudioProcessorEditor* createEditor() override;
	bool hasEditor() const override;

	//==============================================================================
	const juce::String getName() const override;

	bool acceptsMidi() const override;
	bool producesMidi() const override;
	bool isMidiEffect() const override;
	double getTailLengthSeconds() const override;

	//==============================================================================
	int getNumPrograms() override;
	int getCurrentProgram() override;
	void setCurrentProgram(int index) override;
	const juce::String getProgramName(int index) override;
	void changeProgramName(int index, const juce::String& newName) override;

	//==============================================================================
	void getStateInformation(juce::MemoryBlock& destData) override;
	void setStateInformation(const void* data, int sizeInBytes) override;

	static juce::AudioProcessor::BusesProperties getBusProps();

private:
	void timerCallback();
	//==============================================================================
	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MagnitudeAudioProcessor)
};
