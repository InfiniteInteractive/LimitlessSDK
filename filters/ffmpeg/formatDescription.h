#ifndef FFMPEG_FORMAT_DESCRIPTION_H
#define FFMPEG_FORMAT_DESCRIPTION_H

extern "C"
{
	#include <libavformat/avformat.h>
	#include <libswscale/swscale.h>
}

class FormatDescription
{
public:
	FormatDescription(AVOutputFormat *avFormat);
	~FormatDescription(){}

	std::string name;
	std::string fullName;

	AVOutputFormat *avFormat;

	bool operator==(const std::string &thatName){return name == thatName;}
};
typedef std::vector<FormatDescription> FormatDescriptions;

#endif // FFMPEG_FORMAT_DESCRIPTION_H
