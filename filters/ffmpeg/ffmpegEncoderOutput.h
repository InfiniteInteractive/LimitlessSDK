#ifndef _FFMPEG_ENCODEROUTPUT_H
#define _FFMPEG_ENCODEROUTPUT_H

#include "Media/MediaPluginFactory.h"
#include "Media/IMediaFilter.h"
#include "Media/IImageSample.h"
#include "Media/IAudioSample.h"
#include "Utilities/eventQueueThread.h"

#include "ffmpegMediaFilter.h"
#include "formatDescription.h"
#include "ffmpegPacketSample.h"
#include "codecDescription.h"

extern "C"
{
    #include <libavcodec/avcodec.h>
	#include <libavformat/avformat.h>
	#include <libswscale/swscale.h>
	#include <libswresample/swresample.h>
}

typedef std::pair<Limitless::SharedMediaPad, Limitless::SharedMediaSample> PadSample;

class FfmpegEncoderOutput:public Limitless::MediaAutoRegister<FfmpegEncoderOutput, FfmpegMediaFilter>
{
public:
	FfmpegEncoderOutput(std::string name, Limitless::SharedMediaFilter parent);
	~FfmpegEncoderOutput();

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
	int writeFrame(const AVRational *time_base, AVStream *st, AVPacket *pkt);
	AVFrame *allocVideoFrame(enum AVPixelFormat pix_fmt, int width, int height);

	void openVideo(AVCodec *codec/*, AVDictionary *opt_arg*/);
	void addVideoStream();
	void closeVideoStream();
	int writeVideoFrame(Limitless::SharedIImageSample imageSample);

	AVFrame *allocAudioFrame(enum AVSampleFormat sample_fmt, uint64_t channel_layout, int sample_rate, int nb_samples);
	void openAudio(AVCodec *codec/*, AVDictionary *opt_arg*/);
	void addAudioStream();
	void closeAudioStream();
	int writeAudioFrame(Limitless::SharedIAudioSample audioSample);

	bool openOutput();
	void closeOutput();

	void emptyEncoders();

	void queryCodecs();
	int getVideoEncoderIndexFromId(AVCodecID id);
	int getVideoEncoderIndexFromUiName(std::string name);
	int getVideoProfileIndex(std::string name);
	int getAudioEncoderIndexFromUiName(std::string name);

	Limitless::EventQueueThread<PadSample> m_outputQueue;

	Limitless::SharedMediaPad m_videoSinkPad;
	Limitless::SharedMediaPad m_audioSinkPad;

//Sample Ids
	size_t m_imageSampleId;
	size_t m_bufferSampleId;
	size_t m_iaudioSampleId;
	size_t m_eventSampleId;

//Codec Info
	typedef std::unordered_map<std::string, std::string> CodecNameMap;

	CodecNameMap m_codecNameMap;

	typedef std::vector<CodecDescription> CodecDescriptions;
	CodecDescriptions m_videoCodecs;
	std::vector<std::string> m_videoProfiles;
	CodecDescriptions m_audioCodecs;

//Settings
	bool m_enabled;
	bool m_outputOpen;

//video encoder
	AVCodecID m_videoCodecId;
	AVCodec *m_videoCodec;
	AVStream *m_videoStream;
	AVFrame *m_videoFrame;
	AVFrame *m_videoTempFrame;
	SwsContext *m_swsContext;

	bool m_videoStartFrame;
	__int64 m_videoStartTime;

	AVPixelFormat m_videoInFormat;
	AVRational m_videoInTimeBase;

	int m_videoOutputEncoder;
	int m_videoOutputProfile;
	int m_videoOutputBitrate;
	int m_videoOutputKeyFrameInterval;

//audio encoder
	AVCodecID m_audioCodecId;
	AVCodec *m_audioCodec;
	AVStream *m_audioStream;
	AVFrame *m_audioFrame;
	AVFrame *m_audioTempFrame;
	SwrContext *m_swrContext;

	bool m_audioStartFrame;
	__int64 m_audioStartTime;

	int m_audioInChannels;
	int64_t m_audioInSampleRate;
	AVSampleFormat m_audioInFormat;

	int m_audioOutputEncoder;
	int m_audioOutputChannels;
	int m_audioOutputSampleRate;
	int m_audioOutputBitrate;
	AVSampleFormat m_audioOutputSampleFormat;

	size_t m_samplesInAudioFrame;

//output
	std::string m_outputLocation;

	AVFormatContext *m_formatContext;
};

namespace Limitless{namespace traits
{
	template<> struct type<FfmpegEncoderOutput>
	{
		static FilterType get()
		{return Sink;}
	};
	template<> struct category<FfmpegEncoderOutput>
	{
		static std::string get()
		{return "sink";}
	};
}}//Limitless::traits

#endif // _FFMPEG_ENCODEROUTPUT_H
