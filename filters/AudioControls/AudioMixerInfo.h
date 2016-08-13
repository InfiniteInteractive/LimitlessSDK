#ifndef _AudioMixerInfo_h_
#define _AudioMixerInfo_h_

#include "Media/MediaPluginFactory.h"
#include "Media/IMediaFilter.h"
#include "audioControls_global.h"

struct OutputInfo
{
	Limitless::SharedMediaPad pad;

    bool mute;
    float level;
};

struct InputInfo
{
	std::string name;
	Limitless::SharedMediaPad intputPad;

	unsigned int sampleFrequency;

    bool mute;
	std::vector<OutputInfo> outputs;
};
typedef std::vector<InputInfo> MixInfo;

#endif // _AudioMixerInfo_h_
