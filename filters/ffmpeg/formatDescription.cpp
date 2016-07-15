#include "FfmpegInput.h"
#include "ffmpegResources.h"

FormatDescription::FormatDescription(AVOutputFormat *avFormat):
avFormat(avFormat)
{
	name=avFormat->name;
	fullName=avFormat->long_name;
}