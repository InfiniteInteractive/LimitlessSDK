#ifndef _FFMPEG_OUTPUT_H
#define _FFMPEG_OUTPUT_H

#include "Media/MediaPluginFactory.h"
#include "Media/IMediaFilter.h"
#include "ffmpegMediaFilter.h"
#include "formatDescription.h"

#include "QtComponents/QtPluginView.h"
#include "Base/common.h"

#include "Utilities/eventQueue.h"

#include <thread>

extern "C"
{
	#include <libavformat/avformat.h>
}

struct StreamPad
{
	Limitless::SharedMediaPad pad;
	Limitless::EventQueue<Limitless::SharedMediaSample, 10> m_queue;
};
typedef std::vector<StreamPad> StreamPads;

class FfmpegInput:public Limitless::MediaAutoRegister<FfmpegInput, FfmpegMediaFilter>
{
public:
	struct Format
	{
		Format(std::string name, std::string shortName, std::string extension):name(name), shortName(shortName), extension(extension) {}
		std::string name;
		std::string shortName;
		std::string extension;
	};

	FfmpegInput(std::string name, Limitless::SharedMediaFilter parent);
	~FfmpegInput();

	virtual bool initialize(const Limitless::Attributes &attributes);
	virtual bool shutdown(){return true;}

	//PluginObject
	virtual Limitless::SharedPluginView getView();
	virtual Limitless::SharedPluginView getAttributeView();

	virtual bool processSample(Limitless::SharedMediaPad sinkPad, Limitless::SharedMediaSample sample);

	std::vector<Format> getFormats();

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
	void startReadThread();
	void stopReadThread();
	void readThread();
	void setupFormat();
	void queryFormats();

	std::thread m_readThread;
	std::mutex m_readThreadMutex;
	std::condition_variable m_readThreadEvent;
	bool m_readThreadRunning;
	bool m_stopReadThread;

	bool m_firstSample;

//	Limitless::SharedPluginView m_view;
	Limitless::WeakQtPluginView m_attributeView;
//	Limitless::SharedMediaPads m_streamPads;
	StreamPads m_streamPads;
//	size_t m_imageSampleId;
//	size_t m_bufferSampleId;
	size_t m_ffmpegPacketSampleId;
    size_t m_eventSampleId;

	Strings m_accessibleFormats;
	std::vector<Format> m_formatDetails;

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

	Limitless::MediaTime m_playStreamTime;
	Limitless::MediaTime m_loopTime;
	Limitless::MediaTime m_lastFrameTime;

	bool m_loop;
};

namespace Limitless{namespace traits
{
	template<> struct type<FfmpegInput>
	{
		static FilterType get()
		{return Source;}
	};
	template<> struct category<FfmpegInput>
	{
		static std::string get()
		{return "source";}
	};

}}//Limitless::traits
#endif // _FFMPEG_OUTPUT_H
