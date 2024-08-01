#pragma once

#include <unordered_map>
#include <string>


#define MAXCHANNELS 16
#define MAXBUSES 2
const std::string BUSCHARS("ABCD");


struct ChannelToParamMap
{
public:
	int cToPMap[MAXCHANNELS];
};


extern const std::unordered_map<std::string, ChannelToParamMap> channelToParamMappings;