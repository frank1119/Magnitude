/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

#define UNREFERENCED_PARAMETER(P) (P)


bool test = false;

juce::AudioProcessor::BusesProperties MagnitudeAudioProcessor::getBusProps()
{
    auto _busesProperties = juce::AudioProcessor::BusesProperties();

    for (int i = 0; i < MAXBUSES; i++)
#if MAXCHANNELS == 32
        _busesProperties.addBus(true, std::string("Bus ").append(BUSCHARS.substr(i, 1)), juce::AudioChannelSet::discreteChannels(MAXCHANNELS), true);
#else
#if MAXCHANNELS == 16
        _busesProperties.addBus(true, std::string("Bus ").append(BUSCHARS.substr(i, 1)), juce::AudioChannelSet::create7point1(), true);
#else 
#if MAXCHANNELS == 2
        _busesProperties.addBus(true, std::string("Bus ").append(BUSCHARS.substr(i, 1)), juce::AudioChannelSet::stereo(), true);
#endif
#endif
#endif
    return _busesProperties;
}

MagnitudeAudioProcessor::MagnitudeAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
    : AudioProcessor(getBusProps())
#endif
{
    juce::AudioParameterIntAttributes _intAttribs = juce::AudioParameterIntAttributes().withAutomatable(true);

    juce::ParameterID _refreshRateParamID("Refresh rate", 0);
    addParameter(RefreshRateParameter = new juce::AudioParameterInt(_refreshRateParamID, "Refresh rate", 1, 30, 15, _intAttribs));

    RefreshRateParameter->addListener(this);

    juce::ParameterID _smoothingParamID("Smoothing", 0);
    addParameter(SmoothingParameter = new juce::AudioParameterInt(_smoothingParamID, "Smoothing", 0, 99, 0, _intAttribs));

    SmoothingParameter->addListener(this);

    for (int j = 0; j < MAXBUSES; j++)
        for (int i = 0; i < MAXCHANNELS; i++)
        {
            int _seqNum = i + (j * MAXCHANNELS);
            std::string _paramName = std::string("Bus ").append(BUSCHARS.substr(j, 1)).append(" Magn. Out ").append(std::to_string(i + 1));
            juce::ParameterID _dbParamID(_paramName.c_str(), 0);
            AudioParameterFloatAttributes fa = AudioParameterFloatAttributes().withCategory(AudioProcessorParameter::outputMeter).withAutomatable(true).withLabel(_paramName.c_str());
            addParameter(DbParameters[_seqNum] = new juce::AudioParameterFloat(_dbParamID, _paramName, juce::NormalisableRange(0.0f, 1.0f, 0.0f), 0.0f, fa));
            Dbs[_seqNum] = -1.0f;
        }

    auto dispDbFunc = [](int i, int len) {
        ignoreUnused(len);
        auto fi = static_cast<float>(i);
        return juce::Decibels::toString(fi/10.0, 1);
        };

    for (int j = 0; j < MAXBUSES; j++)
        for (int i = 0; i < MAXCHANNELS; i++)
        {
            int _seqNum = i + (j * MAXCHANNELS);
            std::string _paramName = std::string("Bus ").append(BUSCHARS.substr(j, 1)).append(" Magn dB. Out ").append(std::to_string(i + 1));
            juce::ParameterID _dbParamID(_paramName.c_str(), 0);
            AudioParameterIntAttributes ia = AudioParameterIntAttributes()
                .withCategory(AudioProcessorParameter::inputMeter)
                .withAutomatable(true)
                .withLabel(_paramName.c_str())
                .withStringFromValueFunction(dispDbFunc);
            addParameter(IDbParameters[_seqNum] = new juce::AudioParameterInt(_dbParamID, _paramName, -1000, 0, -1000, ia));
        }
    startTimerHz(15);
}

MagnitudeAudioProcessor::~MagnitudeAudioProcessor()
{
    stopTimer();
    SmoothingParameter->removeListener(this);
    RefreshRateParameter->removeListener(this);
}

juce::AudioProcessorParameter* MagnitudeAudioProcessor::getBypassParameter() const
{
    return nullptr;
}

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

void MagnitudeAudioProcessor::prepareToPlay(double sampleRate, int samplesPerBlock)
{
    UNREFERENCED_PARAMETER(sampleRate);
    UNREFERENCED_PARAMETER(samplesPerBlock);
    //AudioParameterInt* apiRefreshRate = (AudioParameterInt*)RefreshRateParameter;
    //startTimerHz(apiRefreshRate->convertFrom0to1( RefreshRateParameter->getValue()));
    burst = 1;
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
    {
        if (layouts.getNumChannels(true, j) > MAXCHANNELS
            || layouts.getNumChannels(true, j) < 1
            || layouts.getNumChannels(false, j) != 0
            )
            return false;

        //if (!channelToParamMappings.contains(layouts.getBuses(true)[j].getDescription().toStdString()))
        //{
        //	return false;
        //}
    }
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
        auto _sampleBusBuffer = getBusBuffer(buffer, true, j);
        for (int i = 0; i < getChannelCountOfBus(true, j); i++)
        {
            int _chanNum = i + (j * MAXCHANNELS);
            Dbs[_chanNum] = _sampleBusBuffer.getMagnitude(i, 0, _sampleBusBuffer.getNumSamples());
        }
    }

    ///for (auto i = 0; i < totalNumInputChannels; i++)
    //		dbs[i] = buffer.getMagnitude(i, 0, buffer.getNumSamples());


    //auto totalNumInputChannels = getTotalNumInputChannels();
    //auto totalNumOutputChannels = getTotalNumOutputChannels();

    // In case we have more outputs than inputs, this code clears any output
    // channels that didn't contain input data, (because these aren't
    // guaranteed to be empty - they may contain garbage).
    // This is here to avoid people getting screaming feedback
    // when they first compile a plugin, but obviously you don't need to keep
    // this code if your algorithm always overwrites all the output channels.

    //for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
    //	buffer.clear(i, 0, buffer.getNumSamples());

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

bool MagnitudeAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* MagnitudeAudioProcessor::createEditor()
{
    return new MagnitudeAudioProcessorEditor(*this);
    //return new juce::GenericAudioProcessorEditor(*this);
}

void MagnitudeAudioProcessor::getStateInformation(juce::MemoryBlock& destData)
{
    // This exports data from the plugin to the host
    MemoryOutputStream(destData, true).writeString("CFG"); // Force compatibility with the old config
    MemoryOutputStream(destData, true).writeInt64(configVersion);
    MemoryOutputStream(destData, true).writeFloat(RefreshRateParameter->getValue());
    MemoryOutputStream(destData, true).writeFloat(SmoothingParameter->getValue());
}

void MagnitudeAudioProcessor::setStateInformation(const void* data, int sizeInBytes)
{
    // This imports data from the host to the plugin
    int _refreshRate = 5;

    stopTimer();

    if (sizeInBytes > 0)
    {
        auto mem = MemoryInputStream(data, static_cast<size_t> (sizeInBytes), false);
        juce::int64 cfgVersion = 1;

        std::string FullState = mem.readString().toStdString();

        // configVersion 2 also saves a short string to maintain compatibility
        // with configVersion , but is is ignored for configVersion 2
        if (!mem.isExhausted())
            cfgVersion = mem.readInt64();

        AudioParameterInt* apiRefreshRate = (AudioParameterInt*)RefreshRateParameter;
        AudioParameterInt* apiSmooth = (AudioParameterInt*)SmoothingParameter;

        switch (cfgVersion)
        {
        case 1:
        {
            // Sorry. This version was rather lame. It did not feature a ConfigVersion and did not store the floats, but 
            // the display values (integers) as strings

            std::string _sRefreshRate;
            std::string _sSmoothing;
            std::size_t _found;

            _found = FullState.find("\n");
            if (_found != std::string::npos)
            {
                _sRefreshRate = FullState.substr(0, _found);
                FullState = FullState.substr(_found + 1);
            }

            _found = FullState.find("\n");
            if (_found != std::string::npos)
            {
                _sSmoothing = FullState.substr(0, _found);
                FullState = FullState.substr(_found + 1);
            }

            float _fRefreshRate = stof(_sRefreshRate);                                            // Display value
            float _fSmoothing = stof(_sSmoothing);                                                // Display value


            RefreshRateParameter->setValueNotifyingHost(apiRefreshRate->convertTo0to1(_fRefreshRate)); // Convert from display value
            SmoothingParameter->setValueNotifyingHost(apiSmooth->convertTo0to1(_fSmoothing));     // Convert from display value

            Smoothing = _fSmoothing / 100.0f;                                                     // Convert display value to internal factor

            _refreshRate = (int)_fRefreshRate;                                                    // Use display value. No storing needed.
        }
        break;
        case 2:
        {
            float _fRefreshRate = mem.readFloat();                                                // Raw parameter value
            float _fSmoothing = mem.readFloat();                                                  // Raw parameter value
            RefreshRateParameter->setValueNotifyingHost(_fRefreshRate);                           // Apply it
            SmoothingParameter->setValueNotifyingHost(_fSmoothing);                               // Apply it

            _refreshRate = (int)apiRefreshRate->convertFrom0to1(_fRefreshRate);                   // Convert to display value
            int _Smoothing = (int)apiSmooth->convertFrom0to1(_fSmoothing);                        // Convert to display value

            Smoothing = ((float)_Smoothing) / 100.0f;                                             // Convert display value to internal factor

        }
        break;
        default:
            break;
        }
    }

    for (int j = 0; j < MAXBUSES; j++)
        for (int i = 0; i < MAXCHANNELS; i++)
        {
            int chNum = i + (j * MAXCHANNELS);
            Dbs[chNum] = -1.0;
            PrevDbs[chNum] = 0.0;
        }

    startTimerHz((int)_refreshRate);                                                              // Use display value. No storing needed.
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new MagnitudeAudioProcessor();
}

void MagnitudeAudioProcessor::parameterValueChanged(int parameterIndex, float newValue)
{
    switch (parameterIndex)
    {
    case 0:
    {
        stopTimer();
        AudioParameterInt* apiRefreshRate = (AudioParameterInt*)RefreshRateParameter;
        int _refreshRate = (int)apiRefreshRate->convertFrom0to1(newValue);
        startTimerHz(_refreshRate);
        break;
    }
    case 1:
    {
        AudioParameterInt* apiSmooth = (AudioParameterInt*)SmoothingParameter;
        int _Smoothing = (int)apiSmooth->convertFrom0to1(newValue);
        Smoothing = ((float)_Smoothing) / 100.0f;
        break;
    }
    }
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

    juce::Array<juce::AudioChannelSet> _channelSets = getBusesLayout().getBuses(true);
    for (int j = 0; j < MAXBUSES; j++)
    {
        ChannelToParamMap _map = channelToParamMappings.at(_channelSets[j].getDescription().toStdString());
        int chCnt = getChannelCountOfBus(true, j);
        for (auto i = 0; i < chCnt; i++)
        {
            int _chanNum = (j * MAXCHANNELS) + i;
            int _paramNum = _map.cToPMap[i];

            Dbs[_chanNum] = (SmthDbs[_chanNum] * Smoothing) + ((1.0f - Smoothing) * Dbs[_chanNum]);
            SmthDbs[_chanNum] = Dbs[_chanNum];

            auto p1 = DbParameters[_paramNum + (j * MAXCHANNELS)];
            auto p2 = IDbParameters[_paramNum + (j * MAXCHANNELS)];
            if (PrevDbs[_chanNum] != Dbs[_chanNum] || p1->convertTo0to1( p1->get()) != Dbs[_chanNum] || p2->convertTo0to1(static_cast<float>(p2->get())) != Dbs[_chanNum])
            {
                p1->setValueNotifyingHost( Dbs[_chanNum]);

                auto db =  floor( juce::Decibels::gainToDecibels(Dbs[_chanNum])*10);
                p2->setValueNotifyingHost(p2->convertTo0to1(db));
            }
            PrevDbs[_chanNum] = Dbs[_chanNum];
        }
    }
}
