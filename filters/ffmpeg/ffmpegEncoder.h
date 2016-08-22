#ifndef _FFMPEG_ENCODER_H
#define _FFMPEG_ENCODER_H

#include "Media/MediaPluginFactory.h"
#include "Media/IMediaFilter.h"
#include "ffmpegMediaFilter.h"
#include "ffmpegPacketSample.h"
#include "codecDescription.h"
#include <unordered_map>

extern "C"
{
	#include <libavcodec/avcodec.h>
	#include <libswscale/swscale.h>
	#include <libswresample/swresample.h>
}

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

class FfmpegEncoder:public Limitless::MediaAutoRegister<FfmpegEncoder, FfmpegMediaFilter>
{
public:
	FfmpegEncoder(std::string name, Limitless::SharedMediaFilter parent);
	~FfmpegEncoder();

	virtual bool initialize(const Limitless::Attributes &attributes);
	virtual bool shutdown(){return true;}

	virtual Limitless::SharedPluginView getView();

	virtual bool processSample(Limitless::SharedMediaPad sinkPad, Limitless::SharedMediaSample sample);

	void emptyEncoders();

protected:
	void initSwsContext();
	void initAudioSwsContext();

	//IMediaFilter
	virtual StateChange onReady();
	virtual StateChange onPaused();
	virtual StateChange onPlaying();

	virtual bool FfmpegEncoder::onAcceptMediaFormat(Limitless::SharedMediaPad pad, Limitless::SharedMediaFormat format);
	virtual void onLinkFormatChanged(Limitless::SharedMediaPad mediaPad, Limitless::SharedMediaFormat format);
	//PluginObject
	virtual void onAttributeChanged(std::string name, Limitless::SharedAttribute attribute);

private:
	void queryCodecs();
//	void queryFormats();
//	void setupFormat();

	int getVideoEncoderIndex(std::string name);
	int getVideoEncoderIndexUi(std::string name);
	int getVideoEncoderIndexFromId(AVCodecID id);
	int getAudioEncoderIndex(std::string name);
	int getAudioEncoderIndexUi(std::string name);
	
	void updateVideoEncoderAttributes();
	void updateAudioEncoderAttributes();

	Limitless::SharedPluginView m_view;
	size_t m_imageSampleId;
	size_t m_bufferSampleId;
	size_t m_ffmpegPacketSampleId;
	size_t m_iaudioSampleId;
	size_t m_eventSampleId;

	unsigned int m_videoEncoderId;
	unsigned int m_audioEncoderId;

//	typedef std::vector<CodecDetail> CodecDetails;
//	CodecDetails m_accessibleCodecs;
//	typedef std::vector<AVCodecID> AVCodecIDs;
//	AVCodecIDs m_accessibleCodecs;

	typedef std::unordered_map<std::string, std::string> CodecNameMap;

	CodecNameMap m_codecNameMap;

	typedef std::vector<CodecDescription> CodecDescriptions;
	CodecDescriptions m_videoCodecs;
	CodecDescriptions m_audioCodecs;

	bool m_isVideoEncoder;
	int m_currentVideoEncoder;
	AVCodecContext *m_videoEncoder;
	CodecOptions m_videoOptions;
	int m_currentAudioEncoder;
	AVCodecContext *m_audioEncoder;

	SharedFfmpegPacketSample m_currentBufferSample;
	AVFrame *m_frame;
	int m_avFrameSize;
	AVPicture m_picture;
	AVPacket m_pkt;
	SwsContext *m_swsContext;
	bool m_initSwsContext;
	int m_swsContextWidth;
	int m_swsContextHeight;
	bool m_enabled;
	bool m_wasEnabled;

	bool m_initAudioSwsContext;
	SwrContext *m_swrAudioContext;
	uint8_t *m_audioFormatBuffer[2];
	size_t m_audioFormatBufferSize;

	unsigned int m_samplesInAudioFormatBuffer;
	
	unsigned int m_frameIndex;
};

namespace Limitless{namespace traits
{
	template<> struct type<FfmpegEncoder>
	{
		static FilterType get()
		{return Filter;}
	};
	template<> struct category<FfmpegEncoder>
	{
		static std::string get()
		{return "encoder";}
	};
}}//Limitless::traits

#endif // _FFMPEG_ENCODER_H
