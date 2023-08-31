/*
  ==============================================================================

	This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

#define UNREFERENCED_PARAMETER(P) (P)


juce::AudioProcessor::BusesProperties MagnitudeAudioProcessor::getBusProps()
{
	auto z = juce::AudioProcessor::BusesProperties();

	for (int i = 0; i < MAXBUSES; i++)
		z.addBus(true, std::string("Bus ").append(BUSCHARS.substr(i, 1)), juce::AudioChannelSet::discreteChannels(MAXCHANNELS), true);
	return z;
}

//==============================================================================
MagnitudeAudioProcessor::MagnitudeAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
	: AudioProcessor(getBusProps())
#endif
{
	juce::AudioParameterIntAttributes ia = juce::AudioParameterIntAttributes().withAutomatable(true);

	juce::ParameterID rrid("Refresh rate", 0);
	addParameter(refreshRate = new juce::AudioParameterInt(rrid, "Refresh rate", 10, 30, 30, ia));

	refreshRate->addListener(this);

	juce::ParameterID smid("Smoothing", 0);
	addParameter(smoothing = new juce::AudioParameterInt(smid, "Smoothing", 0, 99, 0, ia));

	smoothing->addListener(this);

	for (int j = 0; j < MAXBUSES; j++)
		for (int i = 0; i < MAXCHANNELS; i++)
		{
			int contiguousNumber = i + (j * MAXCHANNELS);

			std::string pname = std::string("Bus ").append(BUSCHARS.substr(j, 1)).append(" Magn. Out ").append(std::to_string(i + 1));
			juce::ParameterID dbid(pname.c_str(), 0);
			addParameter(db[contiguousNumber] = new juce::AudioParameterFloat(dbid, pname, 0.0f, 1.0f, 0.0f));
			dbs[contiguousNumber] = -1.0f;
		}
	startTimerHz(30);
}

MagnitudeAudioProcessor::~MagnitudeAudioProcessor()
{
	stopTimer();
}

juce::AudioProcessorParameter* MagnitudeAudioProcessor::getBypassParameter() const
{
	return refreshRate;
}

//==============================================================================

const juce::String MagnitudeAudioProcessor::getName() const
{
	return JucePlugin_Name;
}

bool MagnitudeAudioProcessor::acceptsMidi() const
{
#if JucePlugin_WantsMidiInput
	return true;
#else
	return false;
#endif
}

bool MagnitudeAudioProcessor::producesMidi() const
{
#if JucePlugin_ProducesMidiOutput
	return true;
#else
	return false;
#endif
}

bool MagnitudeAudioProcessor::isMidiEffect() const
{
#if JucePlugin_IsMidiEffect
	return true;
#else
	return false;
#endif
}

double MagnitudeAudioProcessor::getTailLengthSeconds() const
{
	return 0.0;
}

int MagnitudeAudioProcessor::getNumPrograms()
{
	return 1;
	// NB: some hosts don't cope very well if you tell them there are 0 programs,
	// so this should be at least 1, even if you're not really implementing programs.
}

int MagnitudeAudioProcessor::getCurrentProgram()
{
	return 0;
}

void MagnitudeAudioProcessor::setCurrentProgram(int index)
{
	UNREFERENCED_PARAMETER(index);
}

const juce::String MagnitudeAudioProcessor::getProgramName(int index)
{
	UNREFERENCED_PARAMETER(index);
	return "Default";
}

void MagnitudeAudioProcessor::changeProgramName(int index, const juce::String& newName)
{
	UNREFERENCED_PARAMETER(index);
	UNREFERENCED_PARAMETER(newName);
}

//==============================================================================
void MagnitudeAudioProcessor::prepareToPlay(double sampleRate, int samplesPerBlock)
{
	UNREFERENCED_PARAMETER(sampleRate);
	UNREFERENCED_PARAMETER(samplesPerBlock);

	// Reset the parameters
	for (int j = 0; j < MAXBUSES; j++)
		for (int i = 0; i < MAXCHANNELS; i++)
		{
			int chNum = i + (j * MAXCHANNELS);
			db[chNum]->sendValueChangedMessageToListeners(0.0);
		}
}

void MagnitudeAudioProcessor::releaseResources()
{
	// When playback stops, you can use this as an opportunity to free up any
	// spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool MagnitudeAudioProcessor::isBusesLayoutSupported(const BusesLayout& layouts) const
{
#if JucePlugin_IsMidiEffect
	juce::ignoreUnused(layouts);
	return true;
#else
	// This is the place where you check if the layout is supported.
	// In this template code we only support mono or stereo.
	// Some plugin hosts, such as certain GarageBand versions, will only
	// load plugins that support stereo bus layouts.


	for (int j = 0; j < MAXBUSES; j++)
		if (layouts.getNumChannels(true, j) > MAXCHANNELS 
			|| layouts.getNumChannels(true, j) <= 0
			|| layouts.getNumChannels(false, j) != 0
			)
			return false;
	return true;
#endif
}
#endif

void MagnitudeAudioProcessor::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
	UNREFERENCED_PARAMETER(midiMessages);

	juce::ScopedNoDenormals noDenormals;

	for (int j = 0; j < MAXBUSES; j++)
	{
		auto _input = getBusBuffer(buffer, true, j);
		for (int i = 0; i < getChannelCountOfBus(true, j); i++)
		{
			int chNum = i + (j * MAXCHANNELS);

			dbs[chNum] = _input.getMagnitude(i, 0, _input.getNumSamples());

			//Smooth
			//dbs[chNum] = (smthDbs[chNum] * fSmooth) + ((1.0f - fSmooth) * dbs[chNum]);
			//smthDbs[chNum] = dbs[chNum];
		}
	}

	//for (auto i = 0; i < totalNumInputChannels; i++)
	//		dbs[i] = buffer.getMagnitude(i, 0, buffer.getNumSamples());
	/*

	auto totalNumInputChannels = getTotalNumInputChannels();
	auto totalNumOutputChannels = getTotalNumOutputChannels();
	*/
	// In case we have more outputs than inputs, this code clears any output
	// channels that didn't contain input data, (because these aren't
	// guaranteed to be empty - they may contain garbage).
	// This is here to avoid people getting screaming feedback
	// when they first compile a plugin, but obviously you don't need to keep
	// this code if your algorithm always overwrites all the output channels.
	/*
	for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
		buffer.clear(i, 0, buffer.getNumSamples());
	*/
	// This is the place where you'd normally do the guts of your plugin's
	// audio processing...
	// Make sure to reset the state if your inner loop is processing
	// the samples and the outer loop is handling the channels.
	// Alternatively, you can process the samples with the channels
	// interleaved by keeping the same state.
	/*
	for (int channel = 0; channel < totalNumInputChannels; ++channel)
	{
		auto* channelData = buffer.getWritePointer(channel);

		// ..do something to the data...
	}
	*/
}

//==============================================================================
bool MagnitudeAudioProcessor::hasEditor() const
{
	return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* MagnitudeAudioProcessor::createEditor()
{
	return new MagnitudeAudioProcessorEditor(*this);
	//return new juce::GenericAudioProcessorEditor(this);
}

//==============================================================================
void MagnitudeAudioProcessor::getStateInformation(juce::MemoryBlock& destData)
{
	// This exports data from the plugin to the host

	std::string sRefresh = refreshRate->getCurrentValueAsText().toStdString();
	std::string sSmoothing = smoothing->getCurrentValueAsText().toStdString();

	std::string data = sRefresh.append("\n").append(sSmoothing).append("\n");

	destData.replaceAll(data.c_str(), data.size());
}

void MagnitudeAudioProcessor::setStateInformation(const void* data, int sizeInBytes)
{
	// This imports data from the host to the plugin

	if (sizeInBytes > 0)
	{
		std::string FullState = std::string((const char*)data);
		std::string sRefresh;
		std::string sSmoothing;
		std::size_t found;

		found = FullState.find("\n");
		if (found != std::string::npos)
		{
			sRefresh = FullState.substr(0, found);
			FullState = FullState.substr(found + 1);
		}

		found = FullState.find("\n");
		if (found != std::string::npos)
		{
			sSmoothing = FullState.substr(0, found);
			FullState = FullState.substr(found + 1);
		}

		float fRefresh = stof(sRefresh);
		refreshRate->setValueNotifyingHost(refreshRate->convertTo0to1(fRefresh));

		float fSmoothing = stof(sSmoothing);
		smoothing->setValueNotifyingHost(smoothing->convertTo0to1(fSmoothing));

		stopTimer();
		startTimerHz((int)fRefresh);
	}
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
	return new MagnitudeAudioProcessor();
}

void MagnitudeAudioProcessor::parameterValueChanged(int parameterIndex, float newValue)
{
	if (parameterIndex == 0)
	{
		stopTimer();
		int rr = (int)refreshRate->convertFrom0to1(newValue);
		startTimerHz(rr);

		//juce::AudioProcessorEditor* ae = getActiveEditor();
		//if (ae != nullptr)
		//	ae->repaint();
	}

	if (parameterIndex == 1)
	{
		int sm = (int)smoothing->convertFrom0to1(newValue);
		fSmooth = ((float)sm) / 100.0f;
	}

	//adjustSmoothing();
}

void MagnitudeAudioProcessor::parameterGestureChanged(int parameterIndex, bool gestureIsStarting)
{
	UNREFERENCED_PARAMETER(parameterIndex);
	UNREFERENCED_PARAMETER(gestureIsStarting);
}

/// <summary>
/// Send the magnitudes to the parameters
/// </summary>
void MagnitudeAudioProcessor::timerCallback()
{
	for (int j = 0; j < MAXBUSES; j++)
	{
		int chCnt = getChannelCountOfBus(true, j);
		for (auto i = 0; i < chCnt; i++)
		{
			int chNum = (j * MAXCHANNELS) + i;

			dbs[chNum] = (smthDbs[chNum] * fSmooth) + ((1.0f - fSmooth) * dbs[chNum]);
			smthDbs[chNum] = dbs[chNum];

			if (prevDbs[chNum] != dbs[chNum])
				db[MAPPING[chCnt-1][i] +(j * MAXCHANNELS)]->sendValueChangedMessageToListeners(dbs[chNum]);
			prevDbs[chNum] = dbs[chNum];
		}
	}
}
