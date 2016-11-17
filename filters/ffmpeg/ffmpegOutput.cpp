#include "FfmpegOutput.h"

#include "QtComponents/QtPluginView.h"
#include "Media/MediaPad.h"
#include "Media/MediaSampleFactory.h"
#include "Media/ImageSample.h"
#include "Media/BufferSample.h"
#include "Media/EventSample.h"

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
//FormatDescription::FormatDescription(AVOutputFormat *avFormat):
//avFormat(avFormat)
//{
//	name=avFormat->name;
//	fullName=avFormat->long_name;
//}

FfmpegOutput::FfmpegOutput(std::string name, SharedMediaFilter parent):
MediaAutoRegister(name, parent),
m_firstSample(true),
m_firstAudioSample(true),
m_avOutputFormat(nullptr),
m_codecContextId(0),
m_audioCodecContextId(0),
m_audioConnected(false)
{
	m_accessibleFormats.push_back("f4v");
	m_accessibleFormats.push_back("flv");
	m_accessibleFormats.push_back("h264");
	m_accessibleFormats.push_back("hls");
	m_accessibleFormats.push_back("mp4");
	m_accessibleFormats.push_back("mpeg");
	m_accessibleFormats.push_back("mpeg2video");
	m_accessibleFormats.push_back("ogg");
	m_accessibleFormats.push_back("webm");
	m_accessibleFormats.push_back("mpegts");
	m_accessibleFormats.push_back("yuv4mpegpipe");
	m_accessibleFormats.push_back("rtp");
	m_accessibleFormats.push_back("rtsp");
}

FfmpegOutput::~FfmpegOutput()
{

}

bool FfmpegOutput::initialize(const Attributes &attributes)
{
	FfmpegResources::instance().registerAll();

	queryFormats();

	Strings avFormats;
	for(int i=0; i<m_avFormats.size(); ++i)
	{
		avFormats.push_back(m_avFormats[i].name);
	}

	//default mp4
//	std::string avFormatName="mp4";
//	std::string avFormatName="h264";
	std::string avFormatName="f4v";
//	std::string avFormatName="flv";
	Strings::iterator iter=std::find(avFormats.begin(), avFormats.end(), avFormatName);

	if(iter == avFormats.end())
		avFormatName=avFormats[0];
	
	m_enabled=false;
	m_recording=false;
	m_audioRecording=false;

	addAttribute("enable", false);
	addAttribute("outputFormat", avFormatName, avFormats);
	addAttribute("outputLocation", "untitled.mp4");
//	addAttribute("outputLocation", "test.mp4");
//	addAttribute("outputLocation", "test.h264");
//	addAttribute("outputLocation", "rtmp://localhost/live/test");
//	addAttribute("outputLocation", "rtmp://localhost:1935/live/stream");

	m_bufferSampleId=MediaSampleFactory::getTypeId("BufferSample");
	m_ffmpegPacketSampleId=MediaSampleFactory::getTypeId("FfmpegPacketSample");
    m_eventSampleId=Limitless::MediaSampleFactory::getTypeId("EventSample");

	m_videoSinkPad=addSinkPad("Sink", "[{\"mime\":\"video/*\"}, {\"mime\":\"image/*\"}]");
	m_audioSinkPad=addSinkPad("AudioSink", "[{\"mime\":\"audio/raw\"}]");
//	addSinkPad("[{\"mime\":\"audio/*\"}]");

	return true;
}

SharedPluginView FfmpegOutput::getView()
{
//	if(m_view == SharedPluginView())
//	{
//		FfmpegControls *controls=new FfmpegControls(this);
//		m_view.reset(new QtPluginView(controls));
//	}
//	return m_view;
	return SharedPluginView();
}

bool FfmpegOutput::processSample(SharedMediaPad sinkPad, SharedMediaSample sample)
{
	m_outputQueue.push_back(PadSample(sinkPad, sample));
	return true;
}

void FfmpegOutput::processSampleThread(PadSample padSample)
{
	SharedMediaPad sinkPad=padSample.first;
	SharedMediaSample sample=padSample.second;

    if(sample->isType(m_eventSampleId))
    {
        SharedEventSample eventSample=boost::dynamic_pointer_cast<EventSample>(sample);

        if(eventSample->getEvent()==Limitless::Event::EndOf)
        {
			bool stoppedRecording=false;

			if(sinkPad==m_videoSinkPad)
			{
				if(m_recording)
				{
					m_recording=false;
					m_firstSample=true;
					stoppedRecording=true;
				}
			}
			else if(sinkPad==m_audioSinkPad)
			{
				if(m_audioRecording)
				{
					m_audioRecording=false;
					m_firstAudioSample=true;
					stoppedRecording=true;
				}
			}

			if(stoppedRecording && !m_recording && !m_audioRecording)
			{
				av_write_trailer(m_avFormatContext);
				avio_close(m_avFormatContext->pb);

				std::string location=attribute("outputLocation")->toString();
				std::string message="Closed "+location;
				Log::message("FfmpegOutput", message);
			}
        }
    }

	if(!sample->isType(m_ffmpegPacketSampleId))
		return;

	int avError;

	if(m_enabled || m_recording)
	{
		if(sinkPad==m_videoSinkPad)
		{
			if(m_firstSample)
			{
				SharedFfmpegPacketSample ffmpegPacketSample=boost::dynamic_pointer_cast<FfmpegPacketSample>(sample);
				AVPacket *packet=ffmpegPacketSample->getPacket();

				//wait for key frame to start
				if(!(packet->flags&AV_PKT_FLAG_KEY))
					return;

				std::string location=attribute("outputLocation")->toString();

				setupFormat();
				if(m_audioConnected)
					setupAudioFormat();

				if(!(m_avOutputFormat->flags & AVFMT_NOFILE))
				{
					std::string message="Opening "+location;

					Log::message("FfmpegOutput", message);
					avError=avio_open(&m_avFormatContext->pb, location.c_str(), AVIO_FLAG_WRITE);

					if(avError!=0)
					{
						message="Failed to open "+location;
						Log::error("FfmpegOutput", message);

						m_enabled=false;
						return;
					}
				}

				avError=avformat_write_header(m_avFormatContext, NULL);
				m_firstSample=false;
				m_recording=true;
				m_audioRecording=true;
				m_startPts=packet->pts;

				writeSample(sample);
			}
			else
			{
				writeSample(sample);
			}
		}
		else if(sinkPad==m_audioSinkPad)
		{ 
			if(!m_audioRecording)
				return;

			if(m_firstAudioSample)
			{
				SharedFfmpegPacketSample ffmpegPacketSample=boost::dynamic_pointer_cast<FfmpegPacketSample>(sample);
				AVPacket *packet=ffmpegPacketSample->getPacket();

				m_startAudioPts=packet->pts;
				m_firstAudioSample=false;
			}
			writeAudioSample(sample);
		}
	}
//	else
//	{
//		//check if we were recording and stop
//		if(m_recording)
//		{
//            //keep pumping till keyframe
//			SharedFfmpegPacketSample ffmpegPacketSample=boost::dynamic_pointer_cast<FfmpegPacketSample>(sample);
//			AVPacket *packet=ffmpegPacketSample->getPacket();
//
//			writeSample(ffmpegPacketSample.get());
//			if(packet->flags&AV_PKT_FLAG_KEY)
//			{
//				m_recording=false;
//
//				av_write_trailer(m_avFormatContext);
//				avio_close(m_avFormatContext->pb);
//				m_firstSample=true;
//				m_firstAudioSample=true;
//			}
//		}
//	}
//	deleteSample(sample);
	return;
}

void FfmpegOutput::writeSample(Limitless::SharedMediaSample sample)
{
	SharedFfmpegPacketSample ffmpegPacketSample=boost::dynamic_pointer_cast<FfmpegPacketSample>(sample);

	writeSample(ffmpegPacketSample.get());
}

void FfmpegOutput::writeSample(FfmpegPacketSample *ffmpegPacketSample)
{
	AVPacket *packet=ffmpegPacketSample->getPacket();
	AVPacket localPacket;

//	av_init_packet(&localPacket);
	int copyError=av_copy_packet(&localPacket, packet);
	av_copy_packet_side_data(&localPacket, packet);

//	AVRational rational={1, 24};
    AVRational rational={1001, 30000};
		
	localPacket.pts-=m_startPts;
	localPacket.dts-=m_startPts;
	localPacket.duration=1;
//	localPacket.duration=1001;

	av_packet_rescale_ts(&localPacket, rational, m_videoStream->time_base);

	localPacket.stream_index=m_videoStream->index;

	int avError;

	Limitless::Log::message("FfmpegOutput", (boost::format("write frame: idx:%d size:%d pts:%d dts:%d\n")%localPacket.stream_index%localPacket.size%localPacket.pts%localPacket.dts).str().c_str());
	avError=av_interleaved_write_frame(m_avFormatContext, &localPacket);

	
}

void FfmpegOutput::writeAudioSample(Limitless::SharedMediaSample sample)
{
	SharedFfmpegPacketSample ffmpegPacketSample=boost::dynamic_pointer_cast<FfmpegPacketSample>(sample);

	writeAudioSample(ffmpegPacketSample.get());
}

void FfmpegOutput::writeAudioSample(FfmpegPacketSample *ffmpegPacketSample)
{
	int avError;

	AVPacket *packet=ffmpegPacketSample->getPacket();
	AVPacket localPacket;

	av_init_packet(&localPacket);
	av_copy_packet(&localPacket, packet);

	//	AVRational rational={1, 24};
	AVRational rational={1, 48000};

	localPacket.pts-=m_startAudioPts;
	localPacket.dts-=m_startAudioPts;
//	localPacket.duration=1;
	av_packet_rescale_ts(&localPacket, rational, m_audioStream->time_base);
	localPacket.stream_index=m_audioStream->index;

	Limitless::Log::message("FfmpegOutput", (boost::format("write audio: idx:%d size:%d pts:%d dts:%d\n")%localPacket.stream_index%localPacket.size%localPacket.pts%localPacket.dts).str().c_str());

	avError=av_interleaved_write_frame(m_avFormatContext, &localPacket);
}

IMediaFilter::StateChange FfmpegOutput::onReady()
{
//	if((m_currentVideoEncoder < 0) || (m_currentVideoEncoder >= m_videoCodecs.size()))
//		return;
//	std::string videoEncoder=attribute("videoEncoder")->toString();
//	std::string audioEncoder=attribute("audioEncoder")->toString();
//
//	if(m_videoCodecs[m_currentVideoEncoder].name != videoEncoder)
//		m_currentVideoEncoder=getVideoEncoderIndex(videoEncoder);
//

	m_outputQueue.start(std::bind(&FfmpegOutput::processSampleThread, this, std::placeholders::_1));

	SharedMediaPads sinkMediaPads=getSinkPads();

	if(sinkMediaPads.size() <= 0)
		return FAILED;

	SharedMediaPad sinkMediaPad=sinkMediaPads[0];

	if(!sinkMediaPad->linked())
		return FAILED;

	if(!exists("outputLocation"))
		return FAILED;

	int avError;
	std::string location=attribute("outputLocation")->toString();

	setupFormat();

//	if(!(m_avOutputFormat->flags & AVFMT_NOFILE))
//	{
//		avError=avio_open(&m_avFormatContext->pb, location.c_str(), AVIO_FLAG_WRITE);
//	}
//	avError=avformat_write_header(m_avFormatContext, NULL);

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

IMediaFilter::StateChange FfmpegOutput::onPaused()
{
	return SUCCESS;
}

IMediaFilter::StateChange FfmpegOutput::onPlaying()
{
	return SUCCESS;
}

void FfmpegOutput::onLinkFormatChanged(SharedMediaPad pad, SharedMediaFormat format)
{
	if(pad->type() == MediaPad::SINK)
	{
		if(pad==m_videoSinkPad)
		{
			if(format->exists("mime"))
				m_codecId=FfmpegResources::instance().getAvCodecID(format->attribute("mime")->toString());
			if(format->exists("bitrate"))
				m_bitrate=format->attribute("bitrate")->toInt();
			if(format->exists("timeBaseNum"))
				m_timeBase.num=format->attribute("timeBaseNum")->toInt();
			if(format->exists("timeBaseDen"))
				m_timeBase.den=format->attribute("timeBaseDen")->toInt();
			if(format->exists("keyframeRate"))
				m_keyframeRate=format->attribute("keyframeRate")->toInt();
			if(format->exists("format"))
				m_pixelFormat=FfmpegResources::instance().getAvPixelFormat(format->attribute("format")->toString());

			if(format->exists("width"))
				m_width=format->attribute("width")->toInt();
			if(format->exists("height"))
				m_height=format->attribute("height")->toInt();

			if(format->exists("ffmpegCodecContext"))
				m_codecContextId=format->attribute("ffmpegCodecContext")->toInt();
		}
		else if(pad==m_audioSinkPad)
		{ 
			if(format->exists("mime"))
				m_audioCodecId=FfmpegResources::instance().getAvCodecID(format->attribute("mime")->toString());
			if(format->exists("bitrate"))
				m_audioBitrate=format->attribute("bitrate")->toInt();
			if(format->exists("sampleRate"))
			{
				m_audioSampleRate=format->attribute("sampleRate")->toInt();
				m_audioTimeBase.num=1;
				m_audioTimeBase.den=m_audioSampleRate;
			}
			if(format->exists("channels"))
				m_audioChannels=format->attribute("channels")->toInt();
			if(format->exists("sampleFormat"))
			{
				std::string sampleFormat=format->attribute("sampleFormat")->toString();

				m_audioSampleFormat=FfmpegResources::getAudioFormatFromName(sampleFormat);
			}
			if(format->exists("frameSize"))
				m_audioFrameSize=format->attribute("frameSize")->toInt();
			if(format->exists("ffmpegCodecContext"))
				m_audioCodecContextId=format->attribute("ffmpegCodecContext")->toInt();

			m_audioConnected=true;
		}
	}
}

bool FfmpegOutput::onAcceptMediaFormat(SharedMediaPad pad, SharedMediaFormat format)
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

void FfmpegOutput::onAttributeChanged(std::string name, SharedAttribute attribute)
{
	if(name == "enable")
	{
		m_enabled=attribute->toBool();
	}
	else if(name == "outputFormat")
	{
		std::string outputFormat=attribute->toString();

		for(int i=0; i<m_avFormats.size(); ++i)
		{
			if(m_avFormats[i].name == outputFormat)
			{
//				m_currentFormat=i;

				
			}
		}
	}
	else if(name == "outputLocation")
	{
	}
}

void FfmpegOutput::setupFormat()
{
	//	if((m_currentFormat < 0) || (m_currentFormat >= m_avFormats.size()))
	//		return;
	//
	//	FormatDescription &format=m_avFormats[m_currentFormat];
	if(!exists("outputFormat"))
		return;
	if(!exists("outputLocation"))
		return;

	std::string formatName=attribute("outputFormat")->toString();
	std::string location=attribute("outputLocation")->toString();

	if(location.compare(0, 7, "rtmp://")==0)
	{
		avformat_alloc_output_context2(&m_avFormatContext, NULL, "FLV", location.c_str());

		m_avOutputFormat=m_avFormatContext->oformat;
	}
	else
	{
		AVOutputFormat *guessedFormat=av_guess_format(NULL, location.c_str(), NULL);

		FormatDescriptions::iterator iter=std::find(m_avFormats.begin(), m_avFormats.end(), formatName);

		if(iter==m_avFormats.end()&&guessedFormat==NULL)
			return;

		if(guessedFormat!=NULL)
		{
			//		m_avFormatContext=avformat_alloc_context();
			avformat_alloc_output_context2(&m_avFormatContext, guessedFormat, NULL, location.c_str());

			m_avOutputFormat=m_avFormatContext->oformat;
		}
		else
		{
			avformat_alloc_output_context2(&m_avFormatContext, NULL, formatName.c_str(), location.c_str());

			m_avOutputFormat=m_avFormatContext->oformat;
		}
	}

	//	AVCodec *codec;
	//
	//	codec=avcodec_find_encoder(m_codecId);
	//
	//	if(codec == NULL)
	//		return;
	AVCodecContext *codecContext=FfmpegResources::getCodecContext(m_codecContextId);

    if(codecContext == nullptr)
        return;

	m_videoStream=avformat_new_stream(m_avFormatContext, codecContext->codec);
	//	m_videoStream=avformat_new_stream(m_avFormatContext, NULL);

	AVCodecContext *streamCodec=m_avFormatContext->streams[m_videoStream->index]->codec;

	streamCodec->codec=codecContext->codec;
	streamCodec->codec_id=m_codecId;
	streamCodec->bit_rate=m_bitrate;
	streamCodec->time_base=m_timeBase;
	streamCodec->width=m_width;
	streamCodec->height=m_height;
	streamCodec->gop_size=m_keyframeRate;
	streamCodec->pix_fmt=m_pixelFormat;
	
	if(m_avFormatContext->oformat->flags & AVFMT_GLOBALHEADER)
		streamCodec->flags!=CODEC_FLAG_GLOBAL_HEADER;

	m_videoStream->id=m_avFormatContext->nb_streams-1;
	m_videoStream->time_base=m_timeBase;

	av_init_packet(&m_pkt);

	m_pkt.stream_index=m_videoStream->index;
}

void FfmpegOutput::setupAudioFormat()
{
//audio
	AVCodecContext *audioCodecContext=FfmpegResources::getCodecContext(m_audioCodecContextId);

	m_audioStream=avformat_new_stream(m_avFormatContext, audioCodecContext->codec);

	AVCodecContext *audioStreamCodec=m_avFormatContext->streams[m_audioStream->index]->codec;

//	audioStreamCodec->codec=audioCodecContext->codec;
	audioStreamCodec->codec_id=m_audioCodecId;
	audioStreamCodec->bit_rate=m_audioBitrate;
	audioStreamCodec->sample_rate=m_audioSampleRate;
	audioStreamCodec->time_base=m_audioTimeBase;
	audioStreamCodec->channels=m_audioChannels;
	audioStreamCodec->sample_fmt=m_audioSampleFormat;
	audioStreamCodec->frame_size=m_audioFrameSize;

//	if(m_avFormatContext->oformat->flags & AVFMT_GLOBALHEADER)
//		audioStreamCodec->flags!=CODEC_FLAG_GLOBAL_HEADER;

	m_audioStream->time_base=m_audioTimeBase;
	m_audioStream->id=m_avFormatContext->nb_streams-1;

	av_init_packet(&m_audioPkt);

	m_audioPkt.stream_index=m_audioStream->index;
}

void FfmpegOutput::queryFormats()
{
	AVOutputFormat *avFormat=NULL;

	while((avFormat=av_oformat_next(avFormat)) != NULL)
	{
		Strings::iterator iter=std::find(m_accessibleFormats.begin(), m_accessibleFormats.end(), avFormat->name);

		if(iter != m_accessibleFormats.end())
			m_avFormats.push_back(FormatDescription(avFormat));
	}
}