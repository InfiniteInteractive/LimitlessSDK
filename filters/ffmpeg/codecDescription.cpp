#include "codecDescription.h"

//extern "C"
//{
//	#include <libavutil/avutil.h>
//	#include <libavutil/opt.h>
//	#include <libavutil/imgutils.h>
//}

CodecOption::CodecOption(const AVOption *option):
option(option)
{
}

CodecDescription::CodecDescription(AVCodec *avCodec, std::string uiName):
avCodec(avCodec),
uiName(uiName)
{
	name=avCodec->name;
	fullName=avCodec->long_name;
	id=avCodec->id;
}

bool CodecDescription::hardwareAccel()
{
	return (avCodec->capabilities&CODEC_CAP_HWACCEL);
}

bool CodecDescription::hardwareAccelVDPAU()
{
	return (avCodec->capabilities&CODEC_CAP_HWACCEL_VDPAU);
}