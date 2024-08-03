

#include "ChannelToParamMap.h"


// In some configurations the channels are not at the expected index
#if MAXCHANNELS == 32
const std::unordered_map<std::string, ChannelToParamMap> channelToParamMappings = [] {
	std::unordered_map<std::string, ChannelToParamMap> layouts;
	ChannelToParamMap Mono = ChannelToParamMap({ 0, 1, 2, 3, 4, 5, 6 ,7,8,9,10,11,12,13,14,15, 16, 17, 18, 19,20,21,22,23,24,25,26,27,28,29,30,31 });
	ChannelToParamMap Stereo = ChannelToParamMap({ 0, 1, 2, 3, 4, 5, 6 ,7,8,9,10,11,12,13,14,15, 16, 17, 18, 19,20,21,22,23,24,25,26,27,28,29,30,31 });
	ChannelToParamMap LCR = ChannelToParamMap({ 0, 1, 2, 3, 4, 5, 6 ,7,8,9,10,11,12,13,14,15, 16, 17, 18, 19,20,21,22,23,24,25,26,27,28,29,30,31 });
	ChannelToParamMap Quadro = ChannelToParamMap({ 0, 1, 2, 3, 4, 5, 6 ,7, 8,9,10,11,12,13,14,15 , 16, 17, 18, 19,20,21,22,23,24,25,26,27,28,29,30,31 });

	ChannelToParamMap Sur5_0 = ChannelToParamMap({ 0, 1, 2, 3, 4, 5, 6 ,7, 8,9,10,11,12,13,14,15 , 16, 17, 18, 19,20,21,22,23,24,25,26,27,28,29,30,31 });
	ChannelToParamMap Sur5_1 = ChannelToParamMap({ 0, 1, 2, 3, 4, 5, 6 ,7, 8,9,10,11,12,13,14,15, 16, 17, 18, 19,20,21,22,23,24,25,26,27,28,29,30,31 });
	ChannelToParamMap Sur7_0 = ChannelToParamMap({ 0, 1, 2, 5, 6, 3, 4 ,7 ,8,9,10,11,12,13,14,15 , 16, 17, 18, 19,20,21,22,23,24,25,26,27,28,29,30,31 });
	ChannelToParamMap Sur7_1 = ChannelToParamMap({ 0, 1, 2, 3, 6, 7, 4 ,5, 8,9,10,11,12,13,14,15 , 16, 17, 18, 19,20,21,22,23,24,25,26,27,28,29,30,31 });

	ChannelToParamMap Ambi2nd = ChannelToParamMap({ 0, 1, 2, 3, 4, 5, 6 ,7,8,9,10,11,12,13,14,15 , 16, 17, 18, 19,20,21,22,23,24,25,26,27,28,29,30,31 });
	ChannelToParamMap Ambi3rd = ChannelToParamMap({ 0, 1, 2, 3, 4, 5, 6 ,7,8,9,10,11,12,13,14,15 , 16, 17, 18, 19,20,21,22,23,24,25,26,27,28,29,30,31 });
	ChannelToParamMap Ambi4th = ChannelToParamMap({ 0, 1, 2, 3, 4, 5, 6 ,7,8,9,10,11,12,13,14,15 , 16, 17, 18, 19,20,21,22,23,24,25,26,27,28,29,30,31 });

	ChannelToParamMap Discr32 = ChannelToParamMap({ 0,11,12,13,14,15,16,17,18,19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31,  1, 2,   3,  4,  5,  6,  7,  8,  9, 10 });

	layouts = {
		{"Mono", Mono},
		{"Stereo", Stereo},
		{"LCR", LCR},
		{"Quadraphonic", Quadro},
		{"5.0 Surround", Sur5_0},
		{"5.1 Surround", Sur5_1},
		{"7.0 Surround", Sur7_0},
		{"7.1 Surround", Sur7_1},
		{"2nd Order Ambisonics", Ambi2nd},
		{"3rd Order Ambisonics", Ambi3rd},
		{"Unknown", Discr32}
	};
	return layouts;
	}();
#else
#if MAXCHANNELS == 16
const std::unordered_map<std::string, ChannelToParamMap> channelToParamMappings = [] {
	std::unordered_map<std::string, ChannelToParamMap> layouts;
	ChannelToParamMap Mono = ChannelToParamMap({ 0, 1, 2, 3, 4, 5, 6 ,7,8,9,10,11,12,13,14,15 });
	ChannelToParamMap Stereo = ChannelToParamMap({ 0, 1, 2, 3, 4, 5, 6 ,7,8,9,10,11,12,13,14,15 });
	ChannelToParamMap LCR = ChannelToParamMap({ 0, 1, 2, 3, 4, 5, 6 ,7,8,9,10,11,12,13,14,15 });
	ChannelToParamMap Quadro = ChannelToParamMap({ 0, 1, 2, 3, 4, 5, 6 ,7, 8,9,10,11,12,13,14,15 });

	ChannelToParamMap Sur5_0 = ChannelToParamMap({ 0, 1, 2, 3, 4, 5, 6 ,7, 8,9,10,11,12,13,14,15 });
	ChannelToParamMap Sur5_1 = ChannelToParamMap({ 0, 1, 2, 3, 4, 5, 6 ,7, 8,9,10,11,12,13,14,15 });
	ChannelToParamMap Sur7_0 = ChannelToParamMap({ 0, 1, 2, 5, 6, 3, 4 ,7 ,8,9,10,11,12,13,14,15 });
	ChannelToParamMap Sur7_1 = ChannelToParamMap({ 0, 1, 2, 3, 6, 7, 4 ,5, 8,9,10,11,12,13,14,15 });

	ChannelToParamMap Ambi2nd = ChannelToParamMap({ 0, 1, 2, 3, 4, 5, 6 ,7,8,9,10,11,12,13,14,15 });
	ChannelToParamMap Ambi3rd = ChannelToParamMap({ 0, 1, 2, 3, 4, 5, 6 ,7,8,9,10,11,12,13,14,15 });
	//ChannelToParamMap Ambi4th = ChannelToParamMap({ 0, 1, 2, 3, 4, 5, 6 ,7,8,9,10,11,12,13,14,15 });

	ChannelToParamMap Discr32 = ChannelToParamMap({ 0,1, 2, 3, 4, 5, 6, 7,8,9, 10, 11, 12, 13, 14, 15 });

	layouts = {
		{"Mono", Mono},
		{"Stereo", Stereo},
		{"LCR", LCR},
		{"Quadraphonic", Quadro},
		{"5.0 Surround", Sur5_0},
		{"5.1 Surround", Sur5_1},
		{"7.0 Surround", Sur7_0},
		{"7.1 Surround", Sur7_1},
		{"2nd Order Ambisonics", Ambi2nd},
		{"3rd Order Ambisonics", Ambi3rd},
		{"Unknown", Discr32}
	};
	return layouts;
	}();
#else
#if MAXCHANNELS == 2
const std::unordered_map<std::string, ChannelToParamMap> channelToParamMappings = [] {
	std::unordered_map<std::string, ChannelToParamMap> layouts;
	ChannelToParamMap Mono = ChannelToParamMap({ 0, 1});
	ChannelToParamMap Stereo = ChannelToParamMap({ 0, 1});
	ChannelToParamMap LCR = ChannelToParamMap({ 0, 1 });
	ChannelToParamMap Quadro = ChannelToParamMap({ 0, 1});

	ChannelToParamMap Sur5_0 = ChannelToParamMap({ 0, 1});
	ChannelToParamMap Sur5_1 = ChannelToParamMap({ 0, 1});
	ChannelToParamMap Sur7_0 = ChannelToParamMap({ 0, 1});
	ChannelToParamMap Sur7_1 = ChannelToParamMap({ 0, 1});

	ChannelToParamMap Ambi2nd = ChannelToParamMap({ 0, 1});
	ChannelToParamMap Ambi3rd = ChannelToParamMap({ 0, 1});
	ChannelToParamMap Ambi4th = ChannelToParamMap({ 0, 1});

	ChannelToParamMap Discr32 = ChannelToParamMap({ 0,1});

	layouts = {
		{"Mono", Mono},
		{"Stereo", Stereo},
	};
	return layouts;
	}();
#endif
#endif
#endif

