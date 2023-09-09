#pragma once

#include <unordered_map>
#include <string>


#define use32 0
#if use32 ==1
#define MAXCHANNELS 32
#else
#define MAXCHANNELS 16
#endif
#define MAXBUSES 2
const std::string BUSCHARS("ABCD");


struct ChannelToParamMap
{
public:
	int cToPMap[MAXCHANNELS];
};


extern const std::unordered_map<std::string, ChannelToParamMap> channelToParamMappings;