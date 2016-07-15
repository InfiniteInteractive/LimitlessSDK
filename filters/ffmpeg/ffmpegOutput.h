#ifndef _FFMPEG_OUTPUT_H
#define _FFMPEG_OUTPUT_H

#include "Media/MediaPluginFactory.h"
#include "Media/IMediaFilter.h"
#include "ffmpegMediaFilter.h"
#include "formatDescription.h"
#include "ffmpegPacketSample.h"

extern "C"
{
    #include <libavcodec/avcodec.h>
	#include <libavformat/avformat.h>
	#include <libswscale/swscale.h>
}

//class FormatDescription
//{
//public:
//	FormatDescription(AVOutputFormat *avFormat);
//	~FormatDescription(){}
//
//	std::string name;
//	std::string fullName;
//
//	AVOutputFormat *avFormat;
//
//	bool operator==(const std::string &thatName){return name == thatName;}
//};
//typedef std::vector<FormatDescription> FormatDescriptions;

class FfmpegOutput:public Limitless::MediaAutoRegister<FfmpegOutput, FfmpegMediaFilter>
{
public:
	FfmpegOutput(std::string name, Limitless::SharedMediaFilter parent);
	~FfmpegOutput();

	virtual bool initialize(const Limitless::Attributes &attributes);
	virtual bool shutdown(){return true;}

	virtual Limitless::SharedPluginView getView();

	virtual bool processSample(Limitless::SharedMediaPad sinkPad, Limitless::SharedMediaSample sample);

protected:
	//IMediaFilter
	virtual StateChange onReady();
	virtual StateChange onPaused();
	virtual StateChange onPlaying();

	virtual void onLinkFormatChanged(Limitless::SharedMediaPad mediaPad, Limitless::SharedMediaFormat format);
	virtual bool onAcceptMediaFormat(Limitless::SharedMediaPad pad, Limitless::SharedMediaFormat format);
	//PluginObject
	virtual void onAttributeChanged(std::string name, Limitless::SharedAttribute attribute);

private:
	void writeSample(Limitless::SharedMediaSample sample);
	void writeSample(FfmpegPacketSample *ffmpegPacketSample);

	void setupFormat();
	void queryFormats();

	bool m_firstSample;
	int64_t m_startPts;

	bool m_enabled;
	bool m_recording;

	Limitless::SharedPluginView m_view;
	size_t m_imageSampleId;
	size_t m_bufferSampleId;
	size_t m_ffmpegPacketSampleId;
    size_t m_eventSampleId;

	Strings m_accessibleFormats;
	FormatDescriptions m_avFormats;
	int m_currentFormat;

	AVFormatContext *m_avFormatContext;
	AVOutputFormat *m_avOutputFormat;
	AVStream *m_videoStream;
	AVPacket m_pkt;

	AVCodecID m_codecId;
	int m_bitrate;
	AVRational m_timeBase;
	int m_keyframeRate;
	AVPixelFormat m_pixelFormat;
	int m_width;
	int m_height;
	unsigned int m_codecContextId;
};

namespace Limitless{namespace traits
{
	template<> struct type<FfmpegOutput>
	{
		static FilterType get()
		{return Sink;}
	};
	template<> struct category<FfmpegOutput>
	{
		static std::string get()
		{return "sink";}
	};
}}//Limitless::traits

#endif // _FFMPEG_OUTPUT_H
