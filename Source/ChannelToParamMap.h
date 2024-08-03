#pragma once

#include <unordered_map>
#include <string>


// Using multiple buses with 2 channels is the simplest solution to have a lot of inputs
// Downside is that the user cannot optimize the number of inputs that is to be processed:
// Even if there's only 2 inputs used, all inputs are evaluated.
// The 16 channels/2 buses solution is more fficient, but the user has to
// perform some extra steps to get more than the default 4 inputs.
#define MAXCHANNELS 16
#define MAXBUSES 2
const std::string BUSCHARS("ABCD");


struct ChannelToParamMap
{
public:
	int cToPMap[MAXCHANNELS];
};


extern const std::unordered_map<std::string, ChannelToParamMap> channelToParamMappings;