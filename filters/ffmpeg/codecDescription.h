#ifndef _FFMPEG_CODECDESCRIPTION_H
#define _FFMPEG_CODECDESCRIPTION_H

#include "Media/MediaPluginFactory.h"
#include "Media/IMediaFilter.h"
#include "ffmpegMediaFilter.h"

extern "C"
{
	#include <libavcodec/avcodec.h>
	#include <libswscale/swscale.h>
}

class CodecOption
{
public:
	CodecOption(const AVOption *option);
	~CodecOption(){};

	const AVOption *option;
};
typedef std::vector<CodecOption> CodecOptions;

class CodecDescription
{
public:
	CodecDescription(AVCodec *avCodec, std::string uiName);
	~CodecDescription(){}

	bool hardwareAccel();
	bool hardwareAccelVDPAU();

	std::string name;
	std::string fullName;
	AVCodecID id;
	std::string uiName;

	AVCodec *avCodec;
};
typedef std::vector<CodecDescription> CodecDescriptions;

#endif //_FFMPEG_CODECDESCRIPTION_H