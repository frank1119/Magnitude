/*
  ==============================================================================

	This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "ChannelToParamMap.h"

#define configVersion 2

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

	// Parameters reporting to the host about the magnitude
	juce::AudioParameterFloat* DbParameters[MAXCHANNELS * MAXBUSES] = { nullptr };

	// Frequency to report
	juce::AudioProcessorParameter* RefreshRateParameter = { nullptr };

	// Smoothing percentage
	juce::AudioProcessorParameter* SmoothingParameter = { nullptr };

	// The magnitudes found during processing
	float Dbs[MAXCHANNELS * MAXBUSES] = { 0.0f };
	// Smoothed magnitudes
	float SmthDbs[MAXCHANNELS * MAXBUSES] = { 0.0f };
	// Previous smoothed magnitudes (makes sure that no unnecessary updates are boadcasted 
	// to the host
	float PrevDbs[MAXCHANNELS * MAXBUSES] = { 0.0f };

	// Smoothing factor
	float Smoothing = 0.0f;
	// For restoring the editor-size
	juce::Rectangle<int> Bounds = juce::Rectangle(0, 0, 350, 150);

	//double processSample(float newSample, double curEnvVal);

	virtual juce::AudioProcessorParameter* getBypassParameter() const override;

	virtual void parameterValueChanged(int parameterIndex, float newValue) override;

	void parameterGestureChanged(int parameterIndex, bool gestureIsStarting) override;

	void prepareToPlay(double sampleRate, int samplesPerBlock) override;

	void releaseResources() override;

#ifndef JucePlugin_PreferredChannelConfigurations
	bool isBusesLayoutSupported(const BusesLayout& layouts) const override;
#endif

	void processBlock(juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

	juce::AudioProcessorEditor* createEditor() override;

	bool hasEditor() const override;

	const juce::String getName() const override;

	bool acceptsMidi() const override;

	bool producesMidi() const override;

	bool isMidiEffect() const override;

	double getTailLengthSeconds() const override;

	int getNumPrograms() override;

	int getCurrentProgram() override;

	void setCurrentProgram(int index) override;

	const juce::String getProgramName(int index) override;

	void changeProgramName(int index, const juce::String& newName) override;

	void getStateInformation(juce::MemoryBlock& destData) override;

	void setStateInformation(const void* data, int sizeInBytes) override;

	static juce::AudioProcessor::BusesProperties getBusProps();

private:
	virtual void timerCallback() override;
	
	//==============================================================================
	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MagnitudeAudioProcessor)
};
