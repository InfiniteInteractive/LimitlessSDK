#ifndef _AudioMixerInfo_h_
#define _AudioMixerInfo_h_

#include "Media/MediaPluginFactory.h"
#include "Media/IMediaFilter.h"
#include "audioControls_global.h"


struct InputInfo
{
	std::string name;
	Limitless::SharedMediaPad pad;

	unsigned int sampleFrequency;

    bool mute;
	
//	std::vector<OutputInfo> outputs;
};
typedef std::vector<InputInfo> InputInfoVector;

struct ChannelMix
{
	ChannelMix():mute(false), level(0.0f) {}

	bool mute;
	float level;
};

struct InputOutputMix
{
	InputOutputMix():mute(false){}

	InputInfo *inputInfo;

	bool mute;
	std::vector<ChannelMix> channelMixes;
//	float level;
};

struct OutputInfo
{
	OutputInfo():mute(false), level(1.0f) {}

	std::string name;
	Limitless::SharedMediaPad pad;
	unsigned int channel;

	bool mute;
	float level;

	std::vector<InputOutputMix> inputs;
};

typedef std::vector<OutputInfo> MixInfo;

//struct OutputInfo
//{
//	Limitless::SharedMediaPad pad;
//
//	bool mute;
//	float level;
//};
//
//struct InputInfo
//{
//	std::string name;
//	Limitless::SharedMediaPad intputPad;
//
//	unsigned int sampleFrequency;
//
//	bool mute;
//	std::vector<OutputInfo> outputs;
//};
//typedef std::vector<InputInfo> MixInfo;

#endif // _AudioMixerInfo_h_
