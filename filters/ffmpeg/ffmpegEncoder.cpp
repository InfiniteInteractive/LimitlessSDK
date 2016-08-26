#include "ffmpegEncoder.h"

#include "QtComponents/QtPluginView.h"
#include "Media/MediaPad.h"
#include "Media/MediaSampleFactory.h"
#include "Media/ImageSample.h"
#include "Media/BufferSample.h"
#include "Media/EventSample.h"

//#include "medialib/audioConversion.h"

#include "ffmpegResources.h"
#include "ffmpegControls.h"
#include "ffmpegPacketSample.h"

#include <boost/foreach.hpp>

extern "C"
{
	#include <libavutil/avutil.h>
	#include <libavutil/opt.h>
	#include <libavutil/imgutils.h>
}

using namespace Limitless;

//void savePPM(const char* const strFile, unsigned char *data, int width, int height)
//{
//	FILE *file;
//	char str[64];
//
//	file=fopen(strFile, "wb");
//	if(file != NULL)
//	{
//		fputs("P6\n", file);
//		sprintf(str, "%d %d\n255\n", width, height);
//		fputs(str, file);
//		fwrite(data, sizeof(unsigned char), width*height*3, file);
//		fclose(file);
//	}
//}
//
//CodecOption::CodecOption(const AVOption *option):
//option(option)
//{
//}
//
//CodecDescription::CodecDescription(AVCodec *avCodec):
//avCodec(avCodec)
//{
//	name=avCodec->name;
//	fullName=avCodec->long_name;
//	id=avCodec->id;
//}

FfmpegEncoder::FfmpegEncoder(std::string name, SharedMediaFilter parent):
MediaAutoRegister(name, parent),
m_currentVideoEncoder(0),
m_currentAudioEncoder(0),
m_videoEncoder(NULL),
m_audioEncoder(NULL),
m_frame(NULL),
m_initSwsContext(false),
m_initAudioSwsContext(false),
m_enabled(false),
m_frameIndex(0),
//m_audioFormatBuffer(NULL),
m_audioFormatBufferSize(0),
m_wasEnabled(false),
m_isVideoEncoder(true)
{
	m_audioFormatBuffer[0]=NULL;
	m_audioFormatBuffer[1]=NULL;

//	m_accessibleCodecs.push_back(CodecDetail(AV_CODEC_ID_MJPEG, "image/mjpeg"));
//	m_accessibleCodecs.push_back(CodecDetail(AV_CODEC_ID_MPEG2VIDEO, "video/mpeg"));
//	m_accessibleCodecs.push_back(CodecDetail(AV_CODEC_ID_MPEG4, "video/mp4"));
//	m_accessibleCodecs.push_back(CodecDetail(AV_CODEC_ID_H264, "video/h264"));
//	m_accessibleCodecs.push_back(CodecDetail(AV_CODEC_ID_THEORA, "video/theora"));
//
//	m_accessibleCodecs.push_back(CodecDetail(AV_CODEC_ID_MP3, "audio/mp3"));
//	m_accessibleCodecs.push_back(CodecDetail(AV_CODEC_ID_AAC, "audio/aav"));
//	m_accessibleCodecs.push_back(CodecDetail(AV_CODEC_ID_AC3, "audio/ac3"));
//	m_accessibleCodecs.push_back(CodecDetail(AV_CODEC_ID_VORBIS, "image/vorbis"));

	//video
	m_codecNameMap.insert(CodecNameMap::value_type("mpeg4", "Mpeg4"));
	m_codecNameMap.insert(CodecNameMap::value_type("libtheora", "Theora"));
	m_codecNameMap.insert(CodecNameMap::value_type("libx264", "h264"));
	//audio
	m_codecNameMap.insert(CodecNameMap::value_type("aac", "AAC"));
	m_codecNameMap.insert(CodecNameMap::value_type("ac3", "AC3"));
	m_codecNameMap.insert(CodecNameMap::value_type("libmp3lame", "Mp3"));
	m_codecNameMap.insert(CodecNameMap::value_type("libvorbis", "Vorbis"));

	av_log_set_level(AV_LOG_VERBOSE);
}

FfmpegEncoder::~FfmpegEncoder()
{

}

bool FfmpegEncoder::initialize(const Attributes &attributes)
{
	FfmpegResources::instance().registerAll();

	m_imageSampleId=MediaSampleFactory::getTypeId("ImageSample");
	m_bufferSampleId=MediaSampleFactory::getTypeId("BufferSample");
	m_ffmpegPacketSampleId=MediaSampleFactory::getTypeId("FfmpegPacketSample");
	m_iaudioSampleId=MediaSampleFactory::getTypeId("IAudioSample");
	m_eventSampleId=Limitless::MediaSampleFactory::getTypeId("EventSample");

	queryCodecs();

	addAttribute("enable", false);
	//Use h264 by default
	int index=getVideoEncoderIndexFromId(AV_CODEC_ID_H264);
//	int index=getVideoEncoderIndexFromId(AV_CODEC_ID_MPEG2VIDEO);
	
	if(index != -1)
		m_currentVideoEncoder=index;
	else
		m_currentVideoEncoder=0;

	CodecDescription &codecDescription=m_videoCodecs[m_currentVideoEncoder];
	Strings videoEncoders;

	for(int i=0; i<m_videoCodecs.size(); ++i)
	{
		videoEncoders.push_back(m_videoCodecs[i].uiName);
	}
	addAttribute("videoEncoder", codecDescription.uiName, videoEncoders);

//	if(index!=-1)
//		m_currentAudioEncoder=index;
//	else
		m_currentAudioEncoder=0;

	CodecDescription &audioCodecDescription=m_audioCodecs[m_currentAudioEncoder];
	Strings audioEncoders;
	for(int i=0; i<m_audioCodecs.size(); ++i)
	{
		audioEncoders.push_back(m_audioCodecs[i].uiName);
	}
	addAttribute("audioEncoder", audioCodecDescription.uiName, audioEncoders);

	addAttribute("width", 1920);
	addAttribute("height", 1080);
	addAttribute("bitrate", 1024*1024);
	
	Strings profiles;

	profiles.push_back("Baseline");
	profiles.push_back("Main");
	profiles.push_back("High");

	addAttribute("profile", profiles[0], profiles);
    addAttribute("gop", 30);

//	queryFormats();
//
//	Strings avFormats;
//	for(int i=0; i<m_avFormats.size(); ++i)
//	{
//		avFormats.push_back(m_avFormats[i].name);
//	}
//
//	//default mp4
//	std::string avFormatName="mp4";
//	Strings::iterator iter=std::find(avFormats.begin(), avFormats.end(), "mp4");
//
//	if(iter == avFormats.end())
//		avFormatName=avFormats[0];
//
//	addAttribute("outputFormat", avFormatName, avFormats);
//	addAttribute("outputLocation", "test.mpg");

	addSinkPad("Sink", "[{\"mime\":\"video/raw\"}, {\"mime\":\"image/raw\"}, {\"mime\":\"audio/raw\"}]");
	addSourcePad("Source", "[{\"mime\":\"video/raw\"}, {\"mime\":\"image/raw\"}, {\"mime\":\"audio/raw\"}]");

	updateVideoEncoderAttributes();

	return true;
}

SharedPluginView FfmpegEncoder::getView()
{
	if(m_view == SharedPluginView())
	{
		FfmpegControls *controls=new FfmpegControls(this);
		m_view.reset(new QtPluginView(controls));
	}
	return m_view;
}

bool FfmpegEncoder::processSample(SharedMediaPad sinkPad, SharedMediaSample sample)
{
	if(!m_enabled)
	{
		if((m_wasEnabled) && m_isVideoEncoder)
		{
			emptyEncoders();

			SharedEventSample eventSample=newSampleType<EventSample>(m_eventSampleId);

			eventSample->setEvent(Limitless::Event::EndOf);
			m_wasEnabled=false;

			pushSample(eventSample);

            closeEncoder();
		}
		return true;
	}

	m_wasEnabled=true;

	if(sample->type() == m_imageSampleId)
	{
		SharedImageSample imageSample=boost::dynamic_pointer_cast<ImageSample>(sample);

		if(imageSample == SharedImageSample())
			return false;

        if(m_videoEncoder==NULL)
            openEncoder();

//		imageSample->save("encoderImg.ppm");

		int output;
		int avError;
	//	pushSample(sample);
//		SharedMediaSample sourceSample=newSample(m_bufferSampleId);
//		SharedBufferSample bufferSample=boost::dynamic_pointer_cast<BufferSample>(sourceSample);
		if(!m_currentBufferSample)
		{
			SharedMediaSample sourceSample=newSample(m_ffmpegPacketSampleId);
//			SharedFfmpegPacketSample bufferSample=boost::dynamic_pointer_cast<FfmpegPacketSample>(sourceSample);
			m_currentBufferSample=boost::dynamic_pointer_cast<FfmpegPacketSample>(sourceSample);
		}

		m_currentBufferSample->allocate(imageSample->width()*imageSample->height()*3);

		int imageSize=(imageSample->size() < m_avFrameSize)?imageSample->size():m_avFrameSize;
		
		if(imageSize <= 0)
			return false;

//		savePPM("test.ppm", imageSample->buffer(), imageSample->width(), imageSample->height());
//		memcpy(m_frame->data[0], imageSample->buffer(), imageSize);
//		m_frame->data[0]=imageSample->buffer();
		uint8_t *bufferArray[1];
		int bufferSizeArray[1];

		bufferArray[0]=imageSample->buffer();
		bufferSizeArray[0]=imageSample->width()*4;
		
		if(!m_initSwsContext)
			initSwsContext();
		else
		{
			if((imageSample->width() != m_swsContextWidth) || (imageSample->height() != m_swsContextHeight))
				initSwsContext();
		}

		//m_frame->pts=imageSample->sequenceNumber()*1001;

//		AVRational rational={1, 48000};//time stamp comming from audio clock at 48khz
//        m_frame->pts=av_rescale_q(imageSample->sourceTimestamp(), rational, m_videoEncoder->time_base);

        AVRational rational={1, 1000000};//media pipeline in useconds
		m_frame->pts=av_rescale_q(imageSample->timestamp(), rational, m_videoEncoder->time_base);

		OutputDebugStringA((boost::format("encode frame: seq:%d, pts time:%d\n")%imageSample->sequenceNumber()%m_frame->pts).str().c_str());

//		AVRational rational={1, 48000};//comming from audio clock at 48khz
//		m_frame->pts=av_rescale_q(imageSample->sourceTimestamp(), rational,m_videoEncoder->time_base);
//		m_frame->pts=m_frameIndex++;

//		sws_scale(m_swsContext, bufferArray, bufferSizeArray, imageSample->width(), imageSample->height(), m_frame->data, m_frame->linesize);
		sws_scale(m_swsContext, bufferArray, bufferSizeArray, 0, imageSample->height(), m_frame->data, m_frame->linesize);

//		m_pkt.data=bufferSample->buffer();
//		m_pkt.size=bufferSample->actualSize();
//		m_currentBufferSample->setSize(m_currentBufferSample->actualSize()); //set back to full allocation
		m_currentBufferSample->resetPacket();

//		m_currentBufferSample->initPacket();
		AVPacket *packet=m_currentBufferSample->getPacket();

//		OutputDebugStringA((boost::format("encode frame: pkt.data=0x%08x, pkt.size=%d\n")%(void *)packet->data%packet->size).str().c_str());
//		avError=avcodec_encode_video2(m_videoEncoder, &m_pkt, m_frame, &output);
		avError=avcodec_encode_video2(m_videoEncoder, packet, m_frame, &output);
		
//		OutputDebugStringA((boost::format("frame encoded: pkt.data=0x%08x, pkt.size=%d\n")%(void *)packet->data%packet->size).str().c_str());

//		if(m_videoEncoder->coded_frame->pts != AV_NOPTS_VALUE)
//			packet->pts=//av_rescale_q(m_videoEncoder->coded_frame->pts, m_videoEncoder->time_base, );
		if(m_videoEncoder->coded_frame->key_frame)
			packet->flags|=AV_PKT_FLAG_KEY;

		if(output == 1)
		{
			m_currentBufferSample->copyHeader(sample, instance());

//			m_currentBufferSample->setSize(m_pkt.size); //reduce back to only what is used
			pushSample(m_currentBufferSample);
			m_currentBufferSample.reset();
		}

//		deleteSample(sample);
	}
	else if(sample->isType(m_iaudioSampleId))
	{
		SharedIAudioSample audioSample=boost::dynamic_pointer_cast<IAudioSample>(sample);

		if(!audioSample)
			return false;

		if(m_frame==NULL)
		{
			m_frame=av_frame_alloc();

			m_frame->nb_samples=m_audioEncoder->frame_size;
			m_frame->format=m_audioEncoder->sample_fmt;
			m_frame->channel_layout=m_audioEncoder->channel_layout;
		}

		int output;
		int avError;

		if(!m_initAudioSwsContext)
			initAudioSwsContext();

		if(m_frame->nb_samples > m_audioFormatBufferSize)
		{
			if(m_audioFormatBuffer[0]!=NULL)
				av_freep(m_audioFormatBuffer[0]);

			av_samples_alloc(m_audioFormatBuffer, NULL, m_audioEncoder->channels, m_frame->nb_samples, m_audioEncoder->sample_fmt, 0);
			m_samplesInAudioFormatBuffer=0;
			m_audioFormatBufferSize=m_frame->nb_samples;
		}

//		uint8_t *dstData[1];
//		uint8_t *dstData[1];
		int samplesUsed=0;

		while(samplesUsed<audioSample->samples())
		{
//			if(audioSample->samples()>m_audioFormatBufferSize)
			

			const uint8_t *audioSampleBuffer=audioSample->buffer()+(samplesUsed*audioSample->channels()*Limitless::sampleSize(audioSample->format()));
			int samplesToConvert=audioSample->samples()-samplesUsed;

			if(m_samplesInAudioFormatBuffer+samplesToConvert > m_frame->nb_samples)
				samplesToConvert=m_frame->nb_samples-m_samplesInAudioFormatBuffer;

			uint8_t *bufferPos[2];

			bufferPos[0]=m_audioFormatBuffer[0]+m_samplesInAudioFormatBuffer*av_get_bytes_per_sample(m_audioEncoder->sample_fmt);
			bufferPos[1]=m_audioFormatBuffer[1]+m_samplesInAudioFormatBuffer*av_get_bytes_per_sample(m_audioEncoder->sample_fmt);

//			const uint8_t *audioSampleBuffer=(uint8_t *)audioSample->buffer();
//			int samples=swr_convert(m_swrAudioContext, m_audioFormatBuffer, m_audioFormatBufferSize, &audioSampleBuffer, audioSample->samples());
			int samples=swr_convert(m_swrAudioContext, bufferPos, m_audioFormatBufferSize, &audioSampleBuffer, samplesToConvert);

			if(m_samplesInAudioFormatBuffer == 0)
				m_frame->pts=audioSample->sourceTimestamp()+samplesUsed;

			samplesUsed+=samples;
			m_samplesInAudioFormatBuffer+=samples;

			if(m_samplesInAudioFormatBuffer >=m_frame->nb_samples)
			{
				int sampleBufferSize=av_samples_get_buffer_size(NULL, m_audioEncoder->channels, m_samplesInAudioFormatBuffer, m_audioEncoder->sample_fmt, 0);

				avcodec_fill_audio_frame(m_frame, m_audioEncoder->channels, m_audioEncoder->sample_fmt, m_audioFormatBuffer[0], sampleBufferSize, 0);
//				m_frame->pts=audioSample->sourceTimestamp();

				if(!m_currentBufferSample)
				{
					SharedMediaSample sourceSample=newSample(m_ffmpegPacketSampleId);
					//			SharedFfmpegPacketSample bufferSample=boost::dynamic_pointer_cast<FfmpegPacketSample>(sourceSample);
					m_currentBufferSample=boost::dynamic_pointer_cast<FfmpegPacketSample>(sourceSample);
				}

				int bufferSize=av_samples_get_buffer_size(NULL, m_audioEncoder->channels, m_audioEncoder->frame_size, m_audioEncoder->sample_fmt, 0);
				m_currentBufferSample->allocate(bufferSize);

				m_currentBufferSample->resetPacket();
				avError=avcodec_encode_audio2(m_audioEncoder, m_currentBufferSample->getPacket(), m_frame, &output);

				if(output==1)
				{
					m_currentBufferSample->copyHeader(sample, instance());

					pushSample(m_currentBufferSample);
					m_currentBufferSample.reset();
				}

				m_samplesInAudioFormatBuffer=0;
			}
		}
	}
    else
        pushSample(sample);

	return true;
}

void FfmpegEncoder::emptyEncoders()
{
	int output=1;
	int avError;

	while(output)
	{
		if(!m_currentBufferSample)
		{
			SharedMediaSample sourceSample=newSample(m_ffmpegPacketSampleId);
			//			SharedFfmpegPacketSample bufferSample=boost::dynamic_pointer_cast<FfmpegPacketSample>(sourceSample);
			m_currentBufferSample=boost::dynamic_pointer_cast<FfmpegPacketSample>(sourceSample);
		}

		m_currentBufferSample->allocate(m_avFrameSize);
		m_currentBufferSample->resetPacket();

		//		m_currentBufferSample->initPacket();
		AVPacket *packet=m_currentBufferSample->getPacket();

		avError=avcodec_encode_video2(m_videoEncoder, packet, NULL, &output);

		if(output)
		{
			pushSample(m_currentBufferSample);
			m_currentBufferSample.reset();
		}
	}

//	output=1;
//	while(output)
//	{
//		if(!m_currentBufferSample)
//		{
//			SharedMediaSample sourceSample=newSample(m_ffmpegPacketSampleId);
//			//			SharedFfmpegPacketSample bufferSample=boost::dynamic_pointer_cast<FfmpegPacketSample>(sourceSample);
//			m_currentBufferSample=boost::dynamic_pointer_cast<FfmpegPacketSample>(sourceSample);
//		}
//
//		m_currentBufferSample->allocate(m_avFrameSize);
//		m_currentBufferSample->resetPacket();
//
//		//		m_currentBufferSample->initPacket();
//		AVPacket *packet=m_currentBufferSample->getPacket();
//
//		avError=avcodec_encode_video2(m_videoEncoder, packet, NULL, &output);
//
//		if(output)
//		{
//			pushSample(m_currentBufferSample);
//			m_currentBufferSample.reset();
//		}
//	}
}

IMediaFilter::StateChange FfmpegEncoder::onReady()
{
//	if((m_currentVideoEncoder < 0) || (m_currentVideoEncoder >= m_videoCodecs.size()))
//		return;
//	std::string videoEncoder=attribute("videoEncoder")->toString();
//	std::string audioEncoder=attribute("audioEncoder")->toString();
//
//	if(m_videoCodecs[m_currentVideoEncoder].name != videoEncoder)
//		m_currentVideoEncoder=getVideoEncoderIndex(videoEncoder);
//
	initSwsContext();

//	SharedMediaPads sinkMediaPads=getSinkPads();
//
//	if(sinkMediaPads.size() <= 0)
//		return FAILED;
//
//	SharedMediaPad sinkMediaPad=sinkMediaPads[0];
//
//	if(!sinkMediaPad->linked())
//		return FAILED;
//
//	SharedMediaPads sourceMediaPads=getSourcePads();
//
//	if(sourceMediaPads.size() <= 0)
//		return FAILED;
//
//	if((m_currentVideoEncoder >= 0) && (m_currentVideoEncoder < m_videoCodecs.size()))
//	{
//		SharedMediaFormat format=sinkMediaPad->format();
//		int width=0, height=0;
//		int avError;
//
//		if(format->exists("width"))
//			width=format->attribute("width")->toInt();
//		if(format->exists("height"))
//			height=format->attribute("height")->toInt();
//
//		if((width != 0) && (height != 0))
//		{
//			if(m_frame == NULL)
//			{
//				m_frame=avcodec_alloc_frame();
//				
////				m_frame->format=AV_PIX_FMT_BGR24;
//				m_frame->format=m_videoEncoder->pix_fmt;
//				m_frame->width=m_videoEncoder->width;
//				m_frame->height=m_videoEncoder->height;
//			}
//
////			avpicture_alloc(&m_picture, m_videoEncoder->pix_fmt, width, height);
////			*((AVPicture *)m_frame)=m_picture;
//			m_avFrameSize=av_image_alloc(m_frame->data, m_frame->linesize, m_frame->width, m_frame->height, (AVPixelFormat)m_frame->format, 32);		
//
//			av_init_packet(&m_pkt);
//
//			m_swsContext=sws_getContext(width, height, AV_PIX_FMT_BGR24, width, height, m_videoEncoder->pix_fmt, SWS_BILINEAR, NULL, NULL, NULL);
//		}
//	}
	return SUCCESS;
}

void FfmpegEncoder::initSwsContext()
{
	SharedMediaPads sinkMediaPads=getSinkPads();

	if(sinkMediaPads.size() <= 0)
		return;

	SharedMediaPad sinkMediaPad=sinkMediaPads[0];

	if(!sinkMediaPad->linked())
		return;

	SharedMediaPads sourceMediaPads=getSourcePads();

	if(sourceMediaPads.size() <= 0)
		return;

	if((m_currentVideoEncoder >= 0) && (m_currentVideoEncoder < m_videoCodecs.size()))
	{
		SharedMediaFormat format=sinkMediaPad->format();
		int width=0, height=0;
		int avError;

		if(format->exists("width"))
			width=format->attribute("width")->toInt();
		if(format->exists("height"))
			height=format->attribute("height")->toInt();

		if((width != 0) && (height != 0))
		{
			if(m_frame == NULL)
			{
//				m_frame=avcodec_alloc_frame();
				m_frame=av_frame_alloc();
				
				m_frame->format=m_videoEncoder->pix_fmt;
				m_frame->width=m_videoEncoder->width;
				m_frame->height=m_videoEncoder->height;
			}

			m_avFrameSize=av_image_alloc(m_frame->data, m_frame->linesize, m_frame->width, m_frame->height, (AVPixelFormat)m_frame->format, 32);		
			av_init_packet(&m_pkt);

			m_swsContextWidth=width;
			m_swsContextHeight=height;
			m_swsContext=sws_getContext(width, height, AV_PIX_FMT_RGBA, width, height, m_videoEncoder->pix_fmt, SWS_BILINEAR, NULL, NULL, NULL);

			m_initSwsContext=true;
		}
	}
}

void FfmpegEncoder::initAudioSwsContext()
{
	SharedMediaPads sinkMediaPads=getSinkPads();

	if(sinkMediaPads.size()<=0)
		return;

	SharedMediaPad sinkMediaPad=sinkMediaPads[0];

	if(!sinkMediaPad->linked())
		return;

	SharedMediaPads sourceMediaPads=getSourcePads();

	if(sourceMediaPads.size()<=0)
		return;

	if((m_currentAudioEncoder>=0)&&(m_currentAudioEncoder < m_audioCodecs.size()))
	{
			av_init_packet(&m_pkt);

		m_swrAudioContext=swr_alloc_set_opts(NULL,  // we're allocating a new context
			AV_CH_LAYOUT_STEREO,  // out_ch_layout
			AV_SAMPLE_FMT_FLTP,    // out_sample_fmt
			48000,                // out_sample_rate
			AV_CH_LAYOUT_STEREO, // in_ch_layout
			AV_SAMPLE_FMT_S32,   // in_sample_fmt
			48000,                // in_sample_rate
			0,                    // log_offset
			NULL);                // log_ctx

		swr_init(m_swrAudioContext);

		m_initAudioSwsContext=true;
	}
}
IMediaFilter::StateChange FfmpegEncoder::onPaused()
{
	return SUCCESS;
}

IMediaFilter::StateChange FfmpegEncoder::onPlaying()
{
	return SUCCESS;
}

bool FfmpegEncoder::onAcceptMediaFormat(SharedMediaPad pad, SharedMediaFormat format)
{
	if(pad->type() == MediaPad::SINK)
	{
		return IMediaFilter::onAcceptMediaFormat(pad, format);
	}
	else if(pad->type() == MediaPad::SOURCE)
		return true;
	return false;
}

void FfmpegEncoder::onLinkFormatChanged(SharedMediaPad pad, SharedMediaFormat format)
{
	if(pad->type() == MediaPad::SINK)
	{
		if(!format->exists("mime"))
			return;
		if((format->attribute("mime")->toString() == "video/raw") ||
			(format->attribute("mime")->toString() == "image/raw"))
		{
			m_isVideoEncoder=true;

            if(format->exists("width"))
            {
                m_width=format->attribute("width")->toInt();
                attribute("width")->fromInt(m_width);
            }
            if(format->exists("height"))
            {
                m_height=format->attribute("height")->toInt();
                attribute("height")->fromInt(m_height);
            }
            updateVideoEncoderAttributes();

//	//		CodecDetails::iterator iter=std::find(m_accessibleCodecs.begin(), m_accessibleCodecs.end(), m_videoCodecs[m_currentVideoEncoder].id);
//			FfmpegCodecs codecs=FfmpegResources::instance().codecs();
//
//			FfmpegCodecs::iterator iter=std::find(codecs.begin(), codecs.end(), m_videoCodecs[m_currentVideoEncoder].id);
//
//	//		if(iter != m_accessibleCodecs.end())
//			if(iter!=codecs.end())
//			{
//				MediaFormat sourceFormat;
//
//				sourceFormat.addAttribute("mime", iter->mime);
//				if(format->exists("width"))
//				{
//					sourceFormat.addAttribute("width", format->attribute("width")->toString());
//					m_videoEncoder->width=format->attribute("width")->toInt();
//					attribute("width")->fromInt(format->attribute("width")->toInt());
//				}
//				if(format->exists("height"))
//				{
//					sourceFormat.addAttribute("height", format->attribute("height")->toString());
//					m_videoEncoder->height=format->attribute("height")->toInt();
//					attribute("height")->fromInt(format->attribute("height")->toInt());
//				}
//
//				updateVideoEncoderAttributes();
//				//			sourceFormat.addAttribute("bitrate", 3000000);
//				//			sourceFormat.addAttribute("timeBaseNum", 1);
//				//			sourceFormat.addAttribute("timeBaseDen", 15);
//				//			sourceFormat.addAttribute("keyframeRate", 10);
//				//			sourceFormat.addAttribute("format", "YUV420P");
//				sourceFormat.addAttribute("bitrate", m_videoEncoder->bit_rate);
//				sourceFormat.addAttribute("timeBaseNum", m_videoEncoder->time_base.num);
//				sourceFormat.addAttribute("timeBaseDen", m_videoEncoder->time_base.den);
//				sourceFormat.addAttribute("keyframeRate", m_videoEncoder->gop_size);
//				sourceFormat.addAttribute("format", "YUV420P");
//				sourceFormat.addAttribute("ffmpegCodecContext", m_videoEncoderId);
//
//				//			//default settings
//				//			m_videoEncoder->bit_rate = 400000;
//				//			//frames per second
//				//			AVRational rational={1, 15};
//				//			m_videoEncoder->time_base=rational;
//				////			m_videoEncoder->time_base= (AVRational){1,15};
//				//			m_videoEncoder->gop_size=10; /* emit one intra frame every ten frames */
//				//			m_videoEncoder->max_b_frames=1;
//				//			m_videoEncoder->pix_fmt=AV_PIX_FMT_YUV420P;
//
//				SharedMediaPads sourcePads=getSourcePads();
//
//				BOOST_FOREACH(SharedMediaPad &sourcePad, sourcePads)
//				{
//					sourcePad->setFormat(sourceFormat);
//				}
//			}
		}
		else if(format->attribute("mime")->toString()=="audio/raw")
		{
			m_isVideoEncoder=false;
            updateAudioEncoderAttributes();
//			FfmpegCodecs codecs=FfmpegResources::instance().codecs();
//			FfmpegCodecs::iterator iter=std::find(codecs.begin(), codecs.end(), m_audioCodecs[m_currentAudioEncoder].id);
//
//			if(iter!=codecs.end())
//			{
//				MediaFormat sourceFormat;
//
//				updateAudioEncoderAttributes();
//
//				sourceFormat.addAttribute("mime", iter->mime);
//
//				sourceFormat.addAttribute("bitrate", m_audioEncoder->bit_rate);
//				sourceFormat.addAttribute("sampleRate", m_audioEncoder->sample_rate);
//				sourceFormat.addAttribute("timeBaseNum", m_audioEncoder->time_base.num);
//				sourceFormat.addAttribute("timeBaseDen", m_audioEncoder->time_base.den);
//				sourceFormat.addAttribute("channels", m_audioEncoder->channels);
//
//				std::string sampleFormat;
//
//				if(m_audioEncoder->sample_fmt==AV_SAMPLE_FMT_FLTP)
//					sampleFormat="FloatP";
//
//				sourceFormat.addAttribute("sampleFormat", sampleFormat);
//				sourceFormat.addAttribute("frameSize", m_audioEncoder->frame_size);
//
//				sourceFormat.addAttribute("ffmpegCodecContext", m_audioEncoderId);
//
//				SharedMediaPads sourcePads=getSourcePads();
//
//				BOOST_FOREACH(SharedMediaPad &sourcePad, sourcePads)
//				{
//					sourcePad->setFormat(sourceFormat);
//				}
//			}
		}

        
        updateLink();
	}
}

void FfmpegEncoder::updateLink()
{
    if(m_isVideoEncoder)
    {
        FfmpegCodecs codecs=FfmpegResources::instance().codecs();
        FfmpegCodecs::iterator iter=std::find(codecs.begin(), codecs.end(), m_videoCodecs[m_currentVideoEncoder].id);

        if(iter!=codecs.end())
        {
            MediaFormat sourceFormat;

            sourceFormat.addAttribute("mime", iter->mime);

            sourceFormat.addAttribute("width", m_width);
            sourceFormat.addAttribute("height", m_height);
            sourceFormat.addAttribute("bitrate", m_videoEncoder->bit_rate);
            sourceFormat.addAttribute("timeBaseNum", m_videoEncoder->time_base.num);
            sourceFormat.addAttribute("timeBaseDen", m_videoEncoder->time_base.den);
            sourceFormat.addAttribute("keyframeRate", m_videoEncoder->gop_size);
            sourceFormat.addAttribute("format", "YUV420P");
            sourceFormat.addAttribute("ffmpegCodecContext", m_videoEncoderId);

            SharedMediaPads sourcePads=getSourcePads();

            BOOST_FOREACH(SharedMediaPad &sourcePad, sourcePads)
            {
                sourcePad->setFormat(sourceFormat);
            }
        }
    }
    else
    {
        FfmpegCodecs codecs=FfmpegResources::instance().codecs();
        FfmpegCodecs::iterator iter=std::find(codecs.begin(), codecs.end(), m_audioCodecs[m_currentAudioEncoder].id);

        if(iter!=codecs.end())
        {
            MediaFormat sourceFormat;

            sourceFormat.addAttribute("mime", iter->mime);

            sourceFormat.addAttribute("bitrate", m_audioEncoder->bit_rate);
            sourceFormat.addAttribute("sampleRate", m_audioEncoder->sample_rate);
            sourceFormat.addAttribute("timeBaseNum", m_audioEncoder->time_base.num);
            sourceFormat.addAttribute("timeBaseDen", m_audioEncoder->time_base.den);
            sourceFormat.addAttribute("channels", m_audioEncoder->channels);

            std::string sampleFormat;

            if(m_audioEncoder->sample_fmt==AV_SAMPLE_FMT_FLTP)
                sampleFormat="FloatP";

            sourceFormat.addAttribute("sampleFormat", sampleFormat);
            sourceFormat.addAttribute("frameSize", m_audioEncoder->frame_size);

            sourceFormat.addAttribute("ffmpegCodecContext", m_audioEncoderId);

            SharedMediaPads sourcePads=getSourcePads();

            BOOST_FOREACH(SharedMediaPad &sourcePad, sourcePads)
            {
                sourcePad->setFormat(sourceFormat);
            }
        }
    }
}

void FfmpegEncoder::onAttributeChanged(std::string name, SharedAttribute attribute)
{
	if(name == "enable")
	{
		m_enabled=attribute->toBool();
	}
	else if(name == "videoEncoder")
	{
		std::string videoEncoder=attribute->toString();

		if(m_videoCodecs[m_currentVideoEncoder].name != videoEncoder)
			m_currentVideoEncoder=getVideoEncoderIndexUi(videoEncoder);
		updateVideoEncoderAttributes();
	}
	else if(name=="audioEncoder")
	{
		std::string encoder=attribute->toString();

		if(m_videoCodecs[m_currentAudioEncoder].name!=encoder)
			m_currentAudioEncoder=getAudioEncoderIndexUi(encoder);
		updateAudioEncoderAttributes();
	}
}

void FfmpegEncoder::queryCodecs()
{	
	AVCodec *avCodec=NULL;
	int index=0;

	while((avCodec=av_codec_next(avCodec)) != NULL)
	{
//		CodecDetails::iterator iter=std::find(m_accessibleCodecs.begin(), m_accessibleCodecs.end(), avCodec->id);
//
//		if(iter != m_accessibleCodecs.end())
		std::string mime=FfmpegResources::instance().getAvCodecMime(avCodec->id);

		if(mime != "None")
		{
			if((avCodec->encode_sub == NULL) && (avCodec->encode2 == NULL))
				continue;

			CodecNameMap::iterator iter=m_codecNameMap.find(avCodec->name);

			if(iter == m_codecNameMap.end())
				continue;

			if(avCodec->type == AVMEDIA_TYPE_VIDEO)
			{
				m_videoCodecs.push_back(CodecDescription(avCodec, iter->second));
				index++;
			}
			else if(avCodec->type == AVMEDIA_TYPE_AUDIO)
				m_audioCodecs.push_back(CodecDescription(avCodec, iter->second));
		}
	}

//	updateVideoEncoderAttributes();
}

int FfmpegEncoder::getVideoEncoderIndex(std::string name)
{
	for(size_t i=0; i<m_videoCodecs.size(); ++i)
	{
		if(m_videoCodecs[i].name == name)
			return i;
	}
	return -1;
}

int FfmpegEncoder::getVideoEncoderIndexUi(std::string name)
{
	for(size_t i=0; i<m_videoCodecs.size(); ++i)
	{
		if(m_videoCodecs[i].uiName == name)
			return i;
	}
	return 0;
}

int FfmpegEncoder::getVideoEncoderIndexFromId(AVCodecID id)
{
	for(size_t i=0; i<m_videoCodecs.size(); ++i)
	{
		if(m_videoCodecs[i].id == id)
			return i;
	}
	return -1;
}

int FfmpegEncoder::getAudioEncoderIndex(std::string name)
{
	for(size_t i=0; i<m_audioCodecs.size(); ++i)
	{
		if(m_audioCodecs[i].name == name)
			return i;
	}
	return -1;
}

int FfmpegEncoder::getAudioEncoderIndexUi(std::string name)
{
	for(size_t i=0; i<m_audioCodecs.size(); ++i)
	{
		if(m_audioCodecs[i].uiName==name)
			return i;
	}
	return 0;
}

void FfmpegEncoder::openEncoder()
{
    if(m_isVideoEncoder)
    {
        if((m_currentVideoEncoder>=0)&&(m_currentVideoEncoder<m_videoCodecs.size()))
        {
            AVCodec *avCodec=m_videoCodecs[m_currentVideoEncoder].avCodec;
            int avError;

            m_videoEncoder=avcodec_alloc_context3(avCodec);

            AVDictionary *avDictionary=NULL;

            int optError;

            if(avCodec->id==AV_CODEC_ID_H264)
            {
                optError=av_opt_set(m_videoEncoder->priv_data, "preset", "ultrafast", 0);
                optError=av_opt_set(m_videoEncoder->priv_data, "profile", "baseline", 0);
                optError=av_opt_set_double(m_videoEncoder->priv_data, "crf", 18, 0);
            }

            m_videoEncoder->bit_rate=attribute("bitrate")->toInt();
            m_videoEncoder->width=attribute("width")->toInt();
            m_videoEncoder->height=attribute("height")->toInt();

            AVRational rational={1001, 30000};

            m_videoEncoder->time_base=rational;
            m_videoEncoder->gop_size=10; // emit one intra frame every ten frames
            m_videoEncoder->max_b_frames=1;
            m_videoEncoder->pix_fmt=AV_PIX_FMT_YUV420P;

            m_videoEncoder->thread_count=6;

            if((avError=avcodec_open2(m_videoEncoder, avCodec, NULL))<0)
            {
                char errorStr[64];

                av_strerror(avError, errorStr, 64);
            }

            m_videoEncoderId=FfmpegResources::pushCodecContext(this, m_videoEncoder);

            updateLink();
        }
    }
    else
    {
        if((m_currentAudioEncoder>=0)&&(m_currentAudioEncoder<m_audioCodecs.size()))
        {
            AVCodec *avCodec=m_audioCodecs[m_currentAudioEncoder].avCodec;
            int avError;

            m_audioEncoder=avcodec_alloc_context3(avCodec);

            //default settings
            m_audioEncoder->bit_rate=attribute("bitrate")->toInt();
            m_audioEncoder->sample_fmt=AV_SAMPLE_FMT_FLTP;
            m_audioEncoder->sample_rate=48000;
            m_audioEncoder->channel_layout=AV_CH_LAYOUT_STEREO;
            m_audioEncoder->channels=2;

            if((avError=avcodec_open2(m_audioEncoder, avCodec, NULL))<0)//&avDictionary)) < 0)
            {
                char errorStr[64];

                av_strerror(avError, errorStr, 64);
            }

            m_audioEncoderId=FfmpegResources::pushCodecContext(this, m_audioEncoder);

            updateLink();
        }
    }
}

void FfmpegEncoder::closeEncoder()
{
    if(m_videoEncoder!=NULL)
    {
        FfmpegResources::popCodecContext(m_videoEncoderId);

        avcodec_close(m_videoEncoder);
        av_free(m_videoEncoder);
        m_videoEncoder=NULL;
    }

    if(m_audioEncoder!=NULL)
    {
        FfmpegResources::popCodecContext(m_audioEncoderId);

        avcodec_close(m_audioEncoder);
        av_free(m_audioEncoder);
        m_audioEncoder=NULL;
    }
}

void FfmpegEncoder::updateVideoEncoderAttributes()
{
//	if(m_videoEncoder != NULL)
//	{
//		avcodec_close(m_videoEncoder);
//		av_free(m_videoEncoder);
//		m_videoEncoder=NULL;
//	}
    closeEncoder();

	//Backup current attributes to set new
	Attributes attributesBackup=attributes();
	Attributes &localAttributes=attributes();

	for(Attributes::iterator iter=localAttributes.begin(); iter!=localAttributes.end(); )
	{
		if(iter->first.compare(0, 6, "codec_") == 0)
			iter=localAttributes.erase(iter);
		else
			++iter;
	}

	if((m_currentVideoEncoder >= 0) && (m_currentVideoEncoder < m_videoCodecs.size()))
	{
//		AVCodec *avCodec=m_videoCodecs[m_currentVideoEncoder].avCodec;
//		int avError;
//
//		m_videoEncoder=avcodec_alloc_context3(avCodec);
//
//		AVDictionary *avDictionary=NULL;
//
//		int optError;
//
//		if(avCodec->id == AV_CODEC_ID_H264)
//		{
////			optError=av_opt_set(m_videoEncoder->priv_data, "preset", "medium", 0);
//            optError=av_opt_set(m_videoEncoder->priv_data, "preset", "ultrafast", 0);
//			optError=av_opt_set(m_videoEncoder->priv_data, "profile", "baseline", 0);
//			optError=av_opt_set_double(m_videoEncoder->priv_data, "crf", 18, 0);
//
////            optError=av_opt_set(m_videoEncoder->priv_data, "tune", "zerolatency", 0);
////            optError=av_opt_set(m_videoEncoder->priv_data, "x264opts", "no-mbtree:sliced-threads:sync-lookahead=0", 0);
//		}
//
//		//default settings
//		m_videoEncoder->bit_rate = 13000000;
//
//		m_videoEncoder->width=attribute("width")->toInt();
//		m_videoEncoder->height=attribute("height")->toInt();
//		//frames per second
////		AVRational rational={1, 15};
////		AVRational rational={1, 24};
//        AVRational rational={1001, 30000};
//
//		m_videoEncoder->time_base=rational;
//		//			m_videoEncoder->time_base= (AVRational){1,15};
//		m_videoEncoder->gop_size=10; /* emit one intra frame every ten frames */
//		m_videoEncoder->max_b_frames=1;
//		m_videoEncoder->pix_fmt=AV_PIX_FMT_YUV420P;
//
//		attribute("width")->fromInt(m_videoEncoder->width);
//		attribute("height")->fromInt(m_videoEncoder->height);
//		attribute("bitrate")->fromInt(m_videoEncoder->bit_rate);
//
//		m_videoEncoder->thread_count=6;
/////		ctx->bit_rate = 500*1000;
////		m_videoEncoder->bit_rate_tolerance = 0;
////		m_videoEncoder->rc_max_rate = 0;
////		m_videoEncoder->rc_buffer_size = 0;
/////		ctx->gop_size = 40;
////		m_videoEncoder->max_b_frames = 3;
////		m_videoEncoder->b_frame_strategy = 1;
////		m_videoEncoder->coder_type = 1;
////		m_videoEncoder->me_cmp = 1;
////		m_videoEncoder->me_range = 16;
////		m_videoEncoder->qmin = 10;
////		m_videoEncoder->qmax = 51;
////		m_videoEncoder->scenechange_threshold = 40;
////		m_videoEncoder->flags |= CODEC_FLAG_LOOP_FILTER;
////		m_videoEncoder->me_method = ME_HEX;
////		m_videoEncoder->me_subpel_quality = 5;
////		m_videoEncoder->i_quant_factor = 0.71;
////		m_videoEncoder->qcompress = 0.6;
////		m_videoEncoder->max_qdiff = 4;
//
////		m_videoEncoder->profile=FF_PROFILE_H264_BASELINE;
//
////		av_dict_set((AVDictionary **)m_videoEncoder->priv_data, "vprofile", "baseline", 0);
//
////		av_dict_set(&avDictionary, "vprofile", "baseline", 0);
//
////		av_dict_set(&avDictionary, "preset", "ultrafast", 0);
//
//		char errorStr[64];
//
//		if((avError=avcodec_open2(m_videoEncoder, avCodec, NULL)) < 0)//&avDictionary)) < 0)
//		{
//			av_strerror(avError, errorStr, 64);
//		}
//
//		m_videoEncoderId=FfmpegResources::pushCodecContext(this, m_videoEncoder);
        openEncoder();

		const AVOption *option=NULL;

        m_videoOptions.clear();
		while((option=av_opt_next(m_videoEncoder, option)) != NULL)
		{
			m_videoOptions.push_back(CodecOption(option));

			buildAttribute(m_videoEncoder, option, "codec_");
		}
	}
}

void FfmpegEncoder::updateAudioEncoderAttributes()
{
//	if(m_audioEncoder!=NULL)
//	{
//		avcodec_close(m_audioEncoder);
//		av_free(m_audioEncoder);
//		m_audioEncoder=NULL;
//	}
    closeEncoder();
	//Backup current attributes to set new
	Attributes attributesBackup=attributes();
	Attributes &localAttributes=attributes();

	for(Attributes::iterator iter=localAttributes.begin(); iter!=localAttributes.end(); )
	{
		if(iter->first.compare(0, 6, "codec_")==0)
			iter=localAttributes.erase(iter);
		else
			++iter;
	}

	if((m_currentAudioEncoder>=0)&&(m_currentAudioEncoder < m_audioCodecs.size()))
	{
//		AVCodec *avCodec=m_audioCodecs[m_currentAudioEncoder].avCodec;
//		int avError;
//
//		m_audioEncoder=avcodec_alloc_context3(avCodec);
//
//		//default settings
//		m_audioEncoder->bit_rate=128000;
//
//		m_audioEncoder->sample_fmt=AV_SAMPLE_FMT_FLTP;
//		m_audioEncoder->sample_rate=48000;
//		m_audioEncoder->channel_layout=AV_CH_LAYOUT_STEREO;
//		m_audioEncoder->channels=2;
//
//		attribute("bitrate")->fromInt(m_audioEncoder->bit_rate);
//
//		char errorStr[64];
//
//		if((avError=avcodec_open2(m_audioEncoder, avCodec, NULL)) < 0)//&avDictionary)) < 0)
//		{
//			av_strerror(avError, errorStr, 64);
//		}
//
//		m_audioEncoderId=FfmpegResources::pushCodecContext(this, m_audioEncoder);
        openEncoder();

		const AVOption *option=NULL;

        m_audioOptions.clear();
		while((option=av_opt_next(m_audioEncoder, option))!=NULL)
		{
			m_audioOptions.push_back(CodecOption(option));

			buildAttribute(m_audioEncoder, option, "codec_");
		}
	}
}

//void FfmpegEncoder::queryFormats()
//{
//	AVOutputFormat *avFormat=NULL;
//
//	while((avFormat=av_oformat_next(avFormat)) != NULL)
//	{
//		Strings::iterator iter=std::find(m_accessibleFormats.begin(), m_accessibleFormats.end(), avFormat->name);
//
//		if(iter != m_accessibleFormats.end())
//			m_avFormats.push_back(FormatDescription(avFormat));
//	}
//}
//
//void FfmpegEncoder::setupFormat()
//{
//	//	if((m_currentFormat < 0) || (m_currentFormat >= m_avFormats.size()))
//	//		return;
//	//
//	//	FormatDescription &format=m_avFormats[m_currentFormat];
//	if(!exists("outputFormat"))
//		return;
//	if(!exists("outputLocation"))
//		return;
//
//	std::string formatName=attribute("outputFormat")->toString();
//	std::string location=attribute("outputLocation")->toString();
//
//	FormatDescriptions::iterator iter=std::find(m_avFormats.begin(), m_avFormats.end(), formatName);
//
//	if(iter == m_avFormats.end())
//		return;
//
//	avformat_alloc_output_context2(&m_avFormatContext, NULL, formatName.c_str(), location.c_str());
//
//	m_avOutputFormat=m_avFormatContext->oformat;
//
//	AVCodec *codec;
//
//	codec=avcodec_find_encoder(m_avOutputFormat->video_codec);
//
//	if(codec == NULL)
//		return;
//
//	m_videoStream=avformat_new_stream(m_avFormatContext, codec);
//
//	av_init_packet(&m_pkt);
//}