#ifndef _FFMPEG_ENCODER_H
#define _FFMPEG_ENCODER_H

#include "Media/MediaPluginFactory.h"
#include "Media/IMediaFilter.h"
#include "codecDescription.h"
#include "ffmpegMediaFilter.h"
#include "ffmpegFrameSample.h"
#include "ffmpegAudioSample.h"

extern "C"
{
	#include <libavcodec/avcodec.h>
	#include <libswscale/swscale.h>
}

#include <mutex>
#include <condition_variable>

//class CodecOption
//{
//public:
//	CodecOption(const AVOption *option);
//	~CodecOption(){};
//
//	const AVOption *option;
//};
//typedef std::vector<CodecOption> CodecOptions;
//
//class CodecDescription
//{
//public:
//	CodecDescription(AVCodec *avCodec);
//	~CodecDescription(){}
//
//	std::string name;
//	std::string fullName;
//	AVCodecID id;
//
//	AVCodec *avCodec;
//};
class FfmpegFrameSample;

class FfmpegDecoder:public Limitless::MediaAutoRegister<FfmpegDecoder, FfmpegMediaFilter>
{
public:
	FfmpegDecoder(std::string name, Limitless::SharedMediaFilter parent);
	~FfmpegDecoder();

	virtual bool initialize(const Limitless::Attributes &attributes);
	virtual bool shutdown(){return true;}

	virtual Limitless::SharedPluginView getView();

	virtual bool processSample(Limitless::SharedMediaPad sinkPad, Limitless::SharedMediaSample sample);

protected:
	//IMediaFilter
	virtual StateChange onReady();
	virtual StateChange onPaused();
	virtual StateChange onPlaying();

	virtual void onLinked(Limitless::SharedMediaPad pad, Limitless::SharedMediaPad filterPad);

	virtual bool FfmpegDecoder::onAcceptMediaFormat(Limitless::SharedMediaPad pad, Limitless::SharedMediaFormat format);
	virtual void onLinkFormatChanged(Limitless::SharedMediaPad mediaPad, Limitless::SharedMediaFormat format);
	//PluginObject
	virtual void onAttributeChanged(std::string name, Limitless::SharedAttribute attribute);

private:
	void queryCodecs();
//	void queryFormats();
//	void setupFormat();
	typedef std::unordered_map<std::string, std::string> CodecNameMap;

	CodecNameMap m_codecNameMap;

	int getVideoCodecIndex(std::string name);
	int getVideoCodecIndexFromId(AVCodecID id);
	int getAudioCodecIndex(std::string name);
	void updateVideoCodecAttributes();

	Limitless::SharedPluginView m_view;
	size_t m_ffmpegPacketSampleId;
	size_t m_ffmpegFrameSampleId;
	size_t m_ffmpegAudioSampleId;
	size_t m_imageSampleId;
	size_t m_bufferSampleId;

//	typedef std::vector<CodecDetail> CodecDetails;
//	CodecDetails m_accessibleCodecs;
//	typedef std::vector<AVCodecID> AVCodecIDs;
//	AVCodecIDs m_accessibleCodecs;

	CodecDescriptions m_videoCodecs;
	CodecDescriptions m_audioCodecs;

	bool m_videoCodecInit;
	int m_currentVideoCodec;
	AVCodecContext *m_videoCodec;
	CodecOptions m_videoOptions;
	int m_currentAudioCodec;
	AVCodecContext *m_audioCodec;

	std::mutex m_codecMutex;
	std::condition_variable m_codecEvent;

//	AVFrame *m_frame;
//	int m_avFrameSize;
//	AVPicture m_picture;
//	AVPacket m_pkt;
//	SwsContext *m_swsContext;
	SharedFfmpegFrameSample m_frameSample;
	SharedFfmpegAudioSample m_audioSample;

    //unsigned int m_sequenceNumber;
};

namespace Limitless{namespace traits
{
	template<> struct type<FfmpegDecoder>
	{
		static FilterType get()
		{return Filter;}
	};
	template<> struct category<FfmpegDecoder>
	{
		static std::string get()
		{return "decoder";}
	};
}}//Limitless::traits

#endif // _FFMPEG_ENCODER_H
