#include "FfmpegEncoderOutput.h"

#include "QtComponents/QtPluginView.h"
#include "Media/MediaPad.h"
#include "Media/MediaSampleFactory.h"
#include "Media/ImageSample.h"
#include "Media/BufferSample.h"
#include "Media/EventSample.h"
#include "Base/Log.h"

#include "ffmpegResources.h"
#include "ffmpegControls.h"

#include <boost/foreach.hpp>

extern "C"
{
	#include <libavutil/avutil.h>
	#include <libavutil/opt.h>
	#include <libavutil/imgutils.h>
}

using namespace Limitless;

FfmpegEncoderOutput::FfmpegEncoderOutput(std::string name, SharedMediaFilter parent):
MediaAutoRegister(name, parent)
{
	m_videoCodecId=AV_CODEC_ID_H264;
	m_audioCodecId=AV_CODEC_ID_AAC;

	m_videoInFormat=AV_PIX_FMT_RGBA;
	m_videoInTimeBase={1001,30000};

	m_audioInFormat=AV_SAMPLE_FMT_S32;
	m_audioInChannels=2;
	m_audioInSampleRate=48000;

//video
	m_codecNameMap.insert(CodecNameMap::value_type("mpeg4", "Mpeg4"));
	m_codecNameMap.insert(CodecNameMap::value_type("libtheora", "Theora"));
	m_codecNameMap.insert(CodecNameMap::value_type("libx264", "h264"));

	m_videoProfiles.push_back("Baseline");
	m_videoProfiles.push_back("Main");
	m_videoProfiles.push_back("High");

//audio
	m_codecNameMap.insert(CodecNameMap::value_type("aac", "AAC"));
	m_codecNameMap.insert(CodecNameMap::value_type("ac3", "AC3"));
	m_codecNameMap.insert(CodecNameMap::value_type("libmp3lame", "Mp3"));
	m_codecNameMap.insert(CodecNameMap::value_type("libvorbis", "Vorbis"));
}

FfmpegEncoderOutput::~FfmpegEncoderOutput()
{

}

bool FfmpegEncoderOutput::initialize(const Attributes &attributes)
{
	FfmpegResources::instance().registerAll();

	m_imageSampleId=MediaSampleFactory::getTypeId("IImageSample");
	m_bufferSampleId=MediaSampleFactory::getTypeId("BufferSample");
	m_iaudioSampleId=MediaSampleFactory::getTypeId("IAudioSample");
	m_eventSampleId=Limitless::MediaSampleFactory::getTypeId("EventSample");

	queryCodecs();

	m_outputOpen=false;
	m_swsContext=NULL;
	m_swrContext=NULL;

	m_enabled=false;
	addAttribute("enable", m_enabled);

////video////////////////////////
	int index=getVideoEncoderIndexFromId(AV_CODEC_ID_H264);

	if(index!=-1)
		m_videoOutputEncoder=index;
	else
		m_videoOutputEncoder=0;

	CodecDescription &codecDescription=m_videoCodecs[m_videoOutputEncoder];
	Strings videoEncoders;

	for(int i=0; i<m_videoCodecs.size(); ++i)
	{
		videoEncoders.push_back(m_videoCodecs[i].uiName);
	}
	addAttribute("videoEncoder", codecDescription.uiName, videoEncoders);
	addAttribute("width", 1920);
	addAttribute("height", 1080);

	m_videoOutputBitrate=1024*1024;
	addAttribute("videoBitrate", m_videoOutputBitrate);

	m_videoOutputProfile=0;
	addAttribute("videoProfile", m_videoProfiles[m_videoOutputProfile], m_videoProfiles);
	
	m_videoOutputKeyFrameInterval=30;
	addAttribute("videoFrameRateInterval", m_videoOutputKeyFrameInterval);

////audio////////////////////////
	m_audioOutputEncoder=0;

	CodecDescription &audioCodecDescription=m_audioCodecs[m_audioOutputEncoder];
	Strings audioEncoders;
	for(int i=0; i<m_audioCodecs.size(); ++i)
	{
		audioEncoders.push_back(m_audioCodecs[i].uiName);
	}
	addAttribute("audioEncoder", audioCodecDescription.uiName, audioEncoders);

	m_audioOutputChannels=2;
	addAttribute("audioChannels", m_audioOutputChannels);
	m_audioOutputBitrate=1280000;
	addAttribute("audioBitRate", m_audioOutputBitrate);
	m_audioOutputSampleRate=48000;
	addAttribute("audioSampleRate", m_audioOutputSampleRate);

	Strings sampleFormats;

	sampleFormats.push_back("Int8");
	sampleFormats.push_back("Int16");
	sampleFormats.push_back("Int32");
	sampleFormats.push_back("Float");
	sampleFormats.push_back("Double");
	sampleFormats.push_back("Int8P");
	sampleFormats.push_back("Int16P");
	sampleFormats.push_back("Int32P");
	sampleFormats.push_back("FloatP");
	sampleFormats.push_back("DoubleP");

	m_audioOutputSampleFormat=AV_SAMPLE_FMT_S16P;
	addAttribute("audioSampleFormat", sampleFormats[6], sampleFormats);

////output////////////////////////
	m_outputLocation="default.mp4";
	addAttribute("outputLocation", m_outputLocation);

	m_videoSinkPad=addSinkPad("Sink", "[{\"mime\":\"video/*\"}, {\"mime\":\"image/*\"}]");
	m_audioSinkPad=addSinkPad("AudioSink", "[{\"mime\":\"audio/raw\"}]");

	return true;
}

SharedPluginView FfmpegEncoderOutput::getView()
{
	return SharedPluginView();
}

bool FfmpegEncoderOutput::processSample(SharedMediaPad sinkPad, SharedMediaSample sample)
{
	m_outputQueue.push_back(PadSample(sinkPad, sample));
	return true;
}

void FfmpegEncoderOutput::processSampleThread(PadSample padSample)
{
	SharedMediaPad sinkPad=padSample.first;
	SharedMediaSample sample=padSample.second;

	if(!m_enabled)
	{
		if(m_outputOpen)
		{
        	emptyEncoders();
			closeOutput();
			m_outputOpen=false;
        }
		return;
    }

	if(!m_outputOpen)
	{ 
		m_outputOpen=openOutput();

		if(!m_outputOpen)
			return;
	}

	if(sample->isType(m_imageSampleId))
	{
		SharedIImageSample imageSample=boost::dynamic_pointer_cast<IImageSample>(sample);

		writeVideoFrame(imageSample);
	}
	else if(sample->isType(m_iaudioSampleId))
	{
		SharedIAudioSample audioSample=boost::dynamic_pointer_cast<IAudioSample>(sample);

		writeAudioFrame(audioSample);
	}
	return;
}

IMediaFilter::StateChange FfmpegEncoderOutput::onReady()
{
	m_outputQueue.start(std::bind(&FfmpegEncoderOutput::processSampleThread, this, std::placeholders::_1));
	return SUCCESS;
}

IMediaFilter::StateChange FfmpegEncoderOutput::onPaused()
{
	return SUCCESS;
}

IMediaFilter::StateChange FfmpegEncoderOutput::onPlaying()
{
	return SUCCESS;
}

void FfmpegEncoderOutput::onLinkFormatChanged(SharedMediaPad pad, SharedMediaFormat format)
{
}

bool FfmpegEncoderOutput::onAcceptMediaFormat(SharedMediaPad pad, SharedMediaFormat format)
{
	if(pad->type() == MediaPad::SINK)
	{
		SharedMediaPads sinkPads=getSinkPads();
		SharedMediaPads::iterator iter=std::find(sinkPads.begin(), sinkPads.end(), pad);

		if(iter != sinkPads.end())
		{
			if(pad==m_videoSinkPad)
			{
				if(format->exists("mime"))
				{
					std::string mime=format->attribute("mime")->toString();

					if(mime.compare(0, 5, "video")==0)
						return true;
					if(mime.compare(0, 5, "image")==0)
						return true;
				}
			}
			else if(pad==m_audioSinkPad)
			{
				if(format->exists("mime"))
				{
					std::string mime=format->attribute("mime")->toString();

					if(mime.compare(0, 5, "audio")==0)
						return true;
				}
			}
		}
	}
	return false;
}

void FfmpegEncoderOutput::onAttributeChanged(std::string name, SharedAttribute attribute)
{
	if(name == "enable")
		m_enabled=attribute->toBool();
	else if(name=="videoEncoder")
	{
		std::string encoderName=attribute->toString();
		m_videoOutputEncoder=getVideoEncoderIndexFromUiName(encoderName);
	}
	else if(name=="videoProfile")
	{
		std::string profile=attribute->toString();
		m_videoOutputProfile=getVideoProfileIndex(profile);
	}
	else if(name=="videoBitrate")
		m_videoOutputBitrate=attribute->toInt();
	else if(name=="videoKeyframeInterval")
		m_videoOutputKeyFrameInterval=attribute->toInt();
	else if(name=="audioEncoder")
	{
		std::string encoderName=attribute->toString();
		m_audioOutputEncoder=getAudioEncoderIndexFromUiName(encoderName);
	}
	else if(name=="audioBitrate")
		m_audioOutputBitrate=attribute->toInt();
	else if(name=="audioSampleRate")
		m_audioOutputSampleRate=attribute->toInt();
	else if(name == "outputLocation")
		m_outputLocation=attribute->toString();
}

int FfmpegEncoderOutput::writeFrame(const AVRational *time_base, AVStream *st, AVPacket *pkt)
{
	/* rescale output packet timestamp values from codec to stream timebase */
	av_packet_rescale_ts(pkt, *time_base, st->time_base);
	pkt->stream_index=st->index;

	/* Write the compressed frame to the media file. */
//	log_packet(fmt_ctx, pkt);
	int ret;

	if(st == m_videoStream)
		Limitless::Log::message("FfmpegEncoderOutput", (boost::format("write video: pts time:%d\n")%pkt->pts).str().c_str());
	else
		Limitless::Log::message("FfmpegEncoderOutput", (boost::format("write audio: pts time:%d\n")%pkt->pts).str().c_str());

	ret=av_interleaved_write_frame(m_formatContext, pkt);
//	if(ret<0)
//		assert(false);
	return ret;
//	return av_interleaved_write_frame(m_formatContext, pkt);
}

AVFrame *FfmpegEncoderOutput::allocVideoFrame(enum AVPixelFormat pix_fmt, int width, int height)
{
	AVFrame *picture;
	int ret;

	picture=av_frame_alloc();
	if(!picture)
		return NULL;

	picture->format=pix_fmt;
	picture->width=width;
	picture->height=height;

	/* allocate the buffers for the frame data */
	ret=av_frame_get_buffer(picture, 32);
	if(ret < 0)
	{
		fprintf(stderr, "Could not allocate frame data.\n");
		exit(1);
	}

	return picture;
}

void FfmpegEncoderOutput::openVideo(AVCodec *codec/*, AVDictionary *opt_arg*/)
{
	int ret;
	AVCodecContext *codecContext=m_videoStream->codec;
	AVDictionary *opt=NULL;

//	av_dict_copy(&opt, opt_arg, 0);

	m_videoStartFrame=true;

	/* open the codec */
	ret=avcodec_open2(codecContext, codec, &opt);
	av_dict_free(&opt);

	if(ret < 0)
	{
		assert(false);
		return;
	}

	/* allocate and init a re-usable frame */
	m_videoFrame=allocVideoFrame(codecContext->pix_fmt, codecContext->width, codecContext->height);
	if(!m_videoFrame)
	{
		assert(false);
		return;
	}

	/* If the output format is not YUV420P, then a temporary YUV420P
	* picture is needed too. It is then converted to the required
	* output format. */
	m_videoTempFrame=NULL;
	if(codecContext->pix_fmt!=m_videoInFormat)
	{
		m_videoTempFrame=allocVideoFrame(m_videoInFormat, codecContext->width, codecContext->height);
		if(!m_videoTempFrame)
		{
			assert(false);
			return;
		}

		if(!m_swsContext)
		{
			m_swsContext=sws_getContext(codecContext->width, codecContext->height, m_videoInFormat,
				codecContext->width, codecContext->height, codecContext->pix_fmt, SWS_BICUBIC, NULL, NULL, NULL);
		}
	}
}

void FfmpegEncoderOutput::addVideoStream()
{
	/* find the encoder */
	m_videoCodec=avcodec_find_encoder(m_videoCodecId);

	if(!(m_videoCodec))
	{
		assert(false);
		return;
	}

	m_videoStream=avformat_new_stream(m_formatContext, m_videoCodec);
	
	if(!m_videoStream)
	{
		assert(false);
		return;
	}

	m_videoStream->id=m_formatContext->nb_streams-1;
	AVCodecContext *codecContext=m_videoStream->codec;

	codecContext->codec_id=m_videoCodecId;

	if(codecContext->codec_id==AV_CODEC_ID_H264)
	{
		int optError;

		optError=av_opt_set(codecContext->priv_data, "preset", "ultrafast", 0);
		optError=av_opt_set(codecContext->priv_data, "profile", "baseline", 0);
//		optError=av_opt_set_double(codecContext->priv_data, "crf", 18, 0);
	}

	codecContext->bit_rate=10000000;
		/* Resolution must be a multiple of two. */
	codecContext->width=2560;
	codecContext->height=1440;
	/* timebase: This is the fundamental unit of time (in seconds) in terms
	* of which frame timestamps are represented. For fixed-fps content,
	* timebase should be 1/framerate and timestamp increments should be
	* identical to 1. */
	m_videoStream->time_base=m_videoInTimeBase;
	codecContext->time_base=m_videoStream->time_base;

	codecContext->gop_size=12; /* emit one intra frame every twelve frames at most */
	codecContext->pix_fmt=AV_PIX_FMT_YUV420P;
	if((codecContext->codec_id==AV_CODEC_ID_MPEG2VIDEO) ||(codecContext->codec_id==AV_CODEC_ID_H264))
	{
		/* just for testing, we also add B frames */
		codecContext->max_b_frames=2;
	}
	if(codecContext->codec_id==AV_CODEC_ID_MPEG1VIDEO)
	{
		/* Needed to avoid using macroblocks in which some coeffs overflow.
		* This does not happen with normal video, it just happens here as
		* the motion of the chroma plane does not match the luma plane. */
		codecContext->mb_decision=2;
	}

	if(m_formatContext->flags & AVFMT_GLOBALHEADER)
		codecContext->flags|=CODEC_FLAG_GLOBAL_HEADER;
}

void FfmpegEncoderOutput::closeVideoStream()
{
	avcodec_close(m_videoStream->codec);
	av_frame_free(&m_videoFrame);
	av_frame_free(&m_videoTempFrame);
	sws_freeContext(m_swsContext);
	m_swsContext=NULL;
}

int FfmpegEncoderOutput::writeVideoFrame(SharedIImageSample imageSample)
{
	int ret;
//	AVFrame *frame;
	int got_packet=0;
	AVPacket pkt={0};

	AVCodecContext *codecContext=m_videoStream->codec;

	uint8_t *bufferArray[1];
	int bufferSizeArray[1];

	bufferArray[0]=imageSample->buffer();
	bufferSizeArray[0]=imageSample->width()*4;

	ret=av_frame_make_writable(m_videoFrame);
//	frame=get_video_frame(ost);
	sws_scale(m_swsContext, bufferArray, bufferSizeArray, 0, imageSample->height(), m_videoFrame->data, m_videoFrame->linesize);

	av_init_packet(&pkt);

	if(m_videoStartFrame)
	{
		m_videoStartTime=imageSample->timestamp();
		m_videoStartFrame=false;
	}

	AVRational rational={1, 1000000};//media pipeline in useconds
	m_videoFrame->pts=av_rescale_q(imageSample->timestamp()-m_videoStartTime, rational, codecContext->time_base);

	Limitless::Log::message("FfmpegEncoderOutput", (boost::format("encode video: pts time:%d\n")%m_videoFrame->pts).str().c_str());
	/* encode the image */
	ret=avcodec_encode_video2(codecContext, &pkt, m_videoFrame, &got_packet);
	
	if(ret < 0)
	{
		assert(false);
		return 0;
	}

	if(got_packet)
	{
		ret=writeFrame(&codecContext->time_base, m_videoStream, &pkt);

		if(ret<0)
		{
//			assert(false);
			return 0;
		}
	}

	return 1;
}


//////////////////////////////////////////////////////////////////////////////////////////////////////
// audio
//////////////////////////////////////////////////////////////////////////////////////////////////////
AVFrame *FfmpegEncoderOutput::allocAudioFrame(enum AVSampleFormat sample_fmt,	uint64_t channel_layout, int sample_rate, int nb_samples)
{
	AVFrame *frame=av_frame_alloc();
	int ret;

	if(!frame)
	{
		assert(false);
		return NULL;
	}

	frame->format=sample_fmt;
	frame->channel_layout=channel_layout;
	frame->sample_rate=sample_rate;
	frame->nb_samples=nb_samples;

	if(nb_samples)
	{
		ret=av_frame_get_buffer(frame, 0);
		if(ret < 0)
		{
			assert(false);
			return NULL;
		}
	}

	return frame;
}

void FfmpegEncoderOutput::openAudio(AVCodec *codec/*, AVDictionary *opt_arg*/)
{
	int nb_samples;
	int ret;
	AVDictionary *opt=NULL;
	AVCodecContext *codecContext=m_audioStream->codec;

	/* open it */
//	av_dict_copy(&opt, opt_arg, 0);
	m_audioStartFrame=true;

	ret=avcodec_open2(codecContext, codec, &opt);
	av_dict_free(&opt);
	if(ret < 0)
	{
		assert(false);
		return;
	}

	if(codecContext->codec->capabilities & CODEC_CAP_VARIABLE_FRAME_SIZE)
		nb_samples=10000;
	else
		nb_samples=codecContext->frame_size;

	m_samplesInAudioFrame=0;
	m_audioFrame=allocAudioFrame(codecContext->sample_fmt, codecContext->channel_layout,
		codecContext->sample_rate, nb_samples);
	m_audioTempFrame=allocAudioFrame(AV_SAMPLE_FMT_FLTP, codecContext->channel_layout,
		codecContext->sample_rate, nb_samples);

	/* create resampler context */
	m_swrContext=swr_alloc();
	if(!m_swrContext)
	{
		assert(false);
		return;
	}

	/* set options */
	av_opt_set_int(m_swrContext, "in_channel_count", m_audioInChannels, 0);
	av_opt_set_int(m_swrContext, "in_sample_rate", m_audioInSampleRate, 0);
	av_opt_set_sample_fmt(m_swrContext, "in_sample_fmt", m_audioInFormat, 0);
	av_opt_set_int(m_swrContext, "out_channel_count", codecContext->channels, 0);
	av_opt_set_int(m_swrContext, "out_sample_rate", codecContext->sample_rate, 0);
	av_opt_set_sample_fmt(m_swrContext, "out_sample_fmt", codecContext->sample_fmt, 0);

	/* initialize the resampling context */
	if((ret=swr_init(m_swrContext)) < 0)
	{
		assert(false);
		return;
	}
}

void FfmpegEncoderOutput::addAudioStream()
{
	/* find the encoder */
	m_audioCodec=avcodec_find_encoder(m_audioCodecId);

	if(!(m_audioCodec))
	{
		assert(false);
		return;
	}

	m_audioStream=avformat_new_stream(m_formatContext, m_audioCodec);

	if(!m_audioStream)
	{
		assert(false);
		return;
	}

	m_audioStream->id=m_formatContext->nb_streams-1;
	AVCodecContext *codecContext=m_audioStream->codec;

	codecContext->sample_fmt=m_audioCodec->sample_fmts?
		m_audioCodec->sample_fmts[0]:AV_SAMPLE_FMT_FLTP;
	codecContext->bit_rate=128000;
	codecContext->sample_rate=48000;
	if(m_audioCodec->supported_samplerates)
	{
		codecContext->sample_rate=m_audioCodec->supported_samplerates[0];
		for(size_t i=0; m_audioCodec->supported_samplerates[i]; i++)
		{
			if(m_audioCodec->supported_samplerates[i]==48000)
				codecContext->sample_rate=48000;
		}
	}
	codecContext->channels=av_get_channel_layout_nb_channels(codecContext->channel_layout);
	codecContext->channel_layout=AV_CH_LAYOUT_STEREO;
	if(m_audioCodec->channel_layouts)
	{
		codecContext->channel_layout=m_audioCodec->channel_layouts[0];
		for(size_t i=0; m_audioCodec->channel_layouts[i]; i++)
		{
			if(m_audioCodec->channel_layouts[i]==AV_CH_LAYOUT_STEREO)
				codecContext->channel_layout=AV_CH_LAYOUT_STEREO;
		}
	}
	codecContext->channels=av_get_channel_layout_nb_channels(codecContext->channel_layout);
	m_audioStream->time_base={ 1, codecContext->sample_rate };
}

void FfmpegEncoderOutput::closeAudioStream()
{
	avcodec_close(m_audioStream->codec);
	av_frame_free(&m_audioFrame);
	av_frame_free(&m_audioTempFrame);
	swr_free(&m_swrContext);
	m_swrContext=NULL;
}

int FfmpegEncoderOutput::writeAudioFrame(SharedIAudioSample audioSample)
{
	AVPacket pkt={0}; // data and size must be 0;
//	AVFrame *frame;
	int ret;
	int got_packet;
	int dstSamples;

	av_init_packet(&pkt);
	AVCodecContext *codecContext=m_audioStream->codec;

	if(m_audioStartFrame)
	{
		m_audioStartTime=audioSample->timestamp();
		m_audioStartFrame=false;
	}

	size_t samplesUsed=0;
	while(samplesUsed < audioSample->samples())
	{
		const uint8_t *audioSampleBuffer=audioSample->buffer()+(samplesUsed*audioSample->channels()*Limitless::sampleSize(audioSample->format()));
		int samplesToConvert=audioSample->samples()-samplesUsed;

		if(m_samplesInAudioFrame+samplesToConvert > m_audioFrame->nb_samples)
			samplesToConvert=m_audioFrame->nb_samples-m_samplesInAudioFrame;

		/* convert samples from native format to destination codec format, using the resampler */
		/* compute destination number of samples */
		dstSamples=av_rescale_rnd(swr_get_delay(m_swrContext, m_audioInSampleRate)+samplesToConvert,
			m_audioInSampleRate, codecContext->sample_rate, AV_ROUND_UP);
//		assert(dstSamples==m_audioFrame->nb_samples);

		/* when we pass a frame to the encoder, it may keep a reference to it
		* internally;
		* make sure we do not overwrite it here
		*/
		ret=av_frame_make_writable(m_audioFrame);
		if(ret < 0)
			exit(1);

		uint8_t *bufferPos[2];

		bufferPos[0]=m_audioFrame->data[0]+m_samplesInAudioFrame*av_get_bytes_per_sample(codecContext->sample_fmt);
		bufferPos[1]=m_audioFrame->data[1]+m_samplesInAudioFrame*av_get_bytes_per_sample(codecContext->sample_fmt);

		/* convert to destination format */
		int samplesConverted=swr_convert(m_swrContext, bufferPos, dstSamples, (const uint8_t **)&audioSampleBuffer, samplesToConvert);
		
		if(samplesConverted < 0)
		{
			assert(false);
			return 0;
		}

		if(m_samplesInAudioFrame == 0)
		{
			m_audioFrame->pts=audioSample->timestamp()-m_audioStartTime;

			AVRational rational={1, 1000000};//media pipeline in useconds

			m_audioFrame->pts=av_rescale_q(m_audioFrame->pts, rational, codecContext->time_base);
			m_audioFrame->pts+=samplesUsed;
		}

		samplesUsed+=samplesToConvert;
		m_samplesInAudioFrame+=samplesConverted;

		if(m_samplesInAudioFrame>=m_audioFrame->nb_samples)
		{
			Limitless::Log::message("FfmpegEncoderOutput", (boost::format("encode audio: pts time:%d\n")%m_audioFrame->pts).str().c_str());

			ret=avcodec_encode_audio2(codecContext, &pkt, m_audioFrame, &got_packet);
			if(ret < 0)
			{
				assert(false);
				return 0;
			}
			m_samplesInAudioFrame=0;

			if(got_packet)
			{
				ret=writeFrame(&codecContext->time_base, m_audioStream, &pkt);

				if(ret < 0)
				{
//					assert(false);
					return 0;
				}
			}
		}
	}
	return 1;
}

bool FfmpegEncoderOutput::openOutput()
{
	int ret;

	/* allocate the output media context */
	avformat_alloc_output_context2(&m_formatContext, NULL, "FLV", m_outputLocation.c_str());
	if(!m_formatContext)
	{
//		printf("Could not deduce output format from file extension: using MPEG.\n");
		avformat_alloc_output_context2(&m_formatContext, NULL, "mpeg", m_outputLocation.c_str());
	}
	if(!m_formatContext)
	{
		assert(false);
		return false;
	}

	AVOutputFormat *fmt=m_formatContext->oformat;

	addVideoStream();
	addAudioStream();

	openVideo(m_videoCodec);
	openAudio(m_audioCodec);

//	av_dump_format(m_formatContext, 0, m_outputLocation, 1);

	/* open the output file, if needed */
	if(!(fmt->flags & AVFMT_NOFILE))
	{
		ret=avio_open(&m_formatContext->pb, m_outputLocation.c_str(), AVIO_FLAG_WRITE);

		if(ret < 0)
		{
			return false;
		}
	}

	AVDictionary *opt=NULL;

	/* Write the stream header, if any. */
	ret=avformat_write_header(m_formatContext, &opt);
	if(ret < 0)
	{
		assert(false);
		return false;
	}

	return true;
}

void FfmpegEncoderOutput::closeOutput()
{
	av_write_trailer(m_formatContext);

	/* Close each codec. */
	closeVideoStream();
	closeAudioStream();

	AVOutputFormat *fmt=m_formatContext->oformat;

	if(!(fmt->flags & AVFMT_NOFILE))
		/* Close the output file. */
		avio_closep(&m_formatContext->pb);

	/* free the stream */
	avformat_free_context(m_formatContext);
}

void FfmpegEncoderOutput::emptyEncoders()
{
	int got_packet=1;
	int ret;

	while(got_packet)
	{
		AVPacket pkt={0};
		AVCodecContext *codecContext=m_videoStream->codec;

		av_init_packet(&pkt);
		ret=avcodec_encode_video2(codecContext, &pkt, NULL, &got_packet);

		if(ret<0)
			break;

		if(got_packet)
		{
			ret=writeFrame(&codecContext->time_base, m_videoStream, &pkt);

			if(ret<0)
				break;
		}
	}

	got_packet=1;
	while(got_packet)
	{
		AVPacket pkt={0};
		AVCodecContext *codecContext=m_audioStream->codec;

		av_init_packet(&pkt);
		ret=avcodec_encode_audio2(codecContext, &pkt, NULL, &got_packet);
		
		if(ret<0)
			break;
		
		if(got_packet)
		{
			ret=writeFrame(&codecContext->time_base, m_audioStream, &pkt);

			if(ret<0)
				break;
		}
	}
}

void FfmpegEncoderOutput::queryCodecs()
{
	AVCodec *avCodec=NULL;
	int index=0;

	while((avCodec=av_codec_next(avCodec))!=NULL)
	{
		std::string mime=FfmpegResources::instance().getAvCodecMime(avCodec->id);

		if(mime!="None")
		{
			if((avCodec->encode_sub==NULL)&&(avCodec->encode2==NULL))
				continue;

			CodecNameMap::iterator iter=m_codecNameMap.find(avCodec->name);

			if(iter==m_codecNameMap.end())
				continue;

			if(avCodec->type==AVMEDIA_TYPE_VIDEO)
			{
				m_videoCodecs.push_back(CodecDescription(avCodec, iter->second));
				index++;
			}
			else if(avCodec->type==AVMEDIA_TYPE_AUDIO)
				m_audioCodecs.push_back(CodecDescription(avCodec, iter->second));
		}
	}
}

int FfmpegEncoderOutput::getVideoEncoderIndexFromId(AVCodecID id)
{
	for(size_t i=0; i<m_videoCodecs.size(); ++i)
	{
		if(m_videoCodecs[i].id==id)
			return i;
	}
	return -1;
}

int FfmpegEncoderOutput::getVideoEncoderIndexFromUiName(std::string name)
{
	for(size_t i=0; i<m_videoCodecs.size(); ++i)
	{
		if(m_videoCodecs[i].uiName==name)
			return i;
	}
	return -1;
}

int FfmpegEncoderOutput::getVideoProfileIndex(std::string name)
{
	for(size_t i=0; i<m_videoProfiles.size(); ++i)
	{
		if(m_videoProfiles[i]==name)
			return i;
	}
	return -1;
}

int FfmpegEncoderOutput::getAudioEncoderIndexFromUiName(std::string name)
{
	for(size_t i=0; i<m_audioCodecs.size(); ++i)
	{
		if(m_audioCodecs[i].uiName==name)
			return i;
	}
	return -1;
}