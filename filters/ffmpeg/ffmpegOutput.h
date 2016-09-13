#ifndef _FFMPEG_OUTPUT_H
#define _FFMPEG_OUTPUT_H

#include "Media/MediaPluginFactory.h"
#include "Media/IMediaFilter.h"
#include "Utilities/eventQueueThread.h"

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

typedef std::pair<Limitless::SharedMediaPad, Limitless::SharedMediaSample> PadSample;

class FfmpegOutput:public Limitless::MediaAutoRegister<FfmpegOutput, FfmpegMediaFilter>
{
public:
	FfmpegOutput(std::string name, Limitless::SharedMediaFilter parent);
	~FfmpegOutput();

	virtual bool initialize(const Limitless::Attributes &attributes);
	virtual bool shutdown(){return true;}

	virtual Limitless::SharedPluginView getView();

	virtual bool processSample(Limitless::SharedMediaPad sinkPad, Limitless::SharedMediaSample sample);
	void processSampleThread(PadSample sample);

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
	void writeAudioSample(Limitless::SharedMediaSample sample);
	void writeAudioSample(FfmpegPacketSample *ffmpegPacketSample);

	void setupFormat();
	void queryFormats();
	void setupAudioFormat();

	Limitless::SharedMediaPad m_videoSinkPad;
	Limitless::SharedMediaPad m_audioSinkPad;

	bool m_firstSample;
	int64_t m_startPts;

	bool m_audioConnected;
	bool m_firstAudioSample;
	int64_t m_startAudioPts;

	bool m_enabled;
	bool m_recording;
	bool m_audioRecording;

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
	AVStream *m_audioStream;
	AVPacket m_audioPkt;

	Limitless::EventQueueThread<PadSample> m_outputQueue;

	AVCodecID m_codecId;
	int m_bitrate;
	AVRational m_timeBase;
	int m_keyframeRate;
	AVPixelFormat m_pixelFormat;
	int m_width;
	int m_height;
	unsigned int m_codecContextId;

	AVCodecID m_audioCodecId;
	int m_audioBitrate;
	int m_audioSampleRate;
	AVRational m_audioTimeBase;
	int m_audioChannels;
	AVSampleFormat m_audioSampleFormat;
	int m_audioFrameSize;
	unsigned int m_audioCodecContextId;
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
