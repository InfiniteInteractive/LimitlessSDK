#include "FfmpegInput.h"

#include "QtComponents/QtPluginView.h"
#include "Base/Log.h"
#include "Media/MediaPad.h"
#include "Media/MediaSampleFactory.h"
#include "Media/ImageSample.h"
#include "Media/BufferSample.h"
#include "Media/EventSample.h"

#include "ffmpegResources.h"
#include "ffmpegInputAttributeView.h"
#include "ffmpegPacketSample.h"

#include <boost/foreach.hpp>
#include <boost/format.hpp>

extern "C"
{
	#include <libavutil/avutil.h>
	#include <libavutil/opt.h>
	#include <libavutil/imgutils.h>
}

using namespace Limitless;

FfmpegInput::FfmpegInput(std::string name, SharedMediaFilter parent):
MediaAutoRegister(name, parent),
m_firstSample(true),
m_loop(false),
m_loopTime(0),
m_readThreadRunning(false)
{
	//Audio only
	m_formatDetails.push_back(Format("Mp3", "mp3", "mp3"));

	m_formatDetails.push_back(Format("Flash", "f4v", "f4v"));
	m_formatDetails.push_back(Format("Flash", "flv", "flv"));
	m_formatDetails.push_back(Format("Mpeg4", "h264", "mp4"));
	m_formatDetails.push_back(Format("HTTP Live Streaming", "hls", "m38u"));
	m_formatDetails.push_back(Format("Mpeg4", "mp4", "mp4"));
	m_formatDetails.push_back(Format("Mpeg", "mpeg", "mpg"));
	m_formatDetails.push_back(Format("Mpeg2", "mpeg2video", "mpg"));
	m_formatDetails.push_back(Format("Ogg", "ogg", "ogg"));
	m_formatDetails.push_back(Format("webM", "webm", "webm"));
	m_formatDetails.push_back(Format("mpegTs", "mpegts", "ts"));
	m_formatDetails.push_back(Format("Pipe", "yuv4mpegpipe", ""));
	m_formatDetails.push_back(Format("RTP", "rtp", ""));
	m_formatDetails.push_back(Format("RTSP", "rtsp", ""));

	m_accessibleFormats.push_back("mp3");
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

FfmpegInput::~FfmpegInput()
{

}

bool FfmpegInput::initialize(const Attributes &attributes)
{
	FfmpegResources::instance().registerAll();

	queryFormats();

	Strings avFormats;
	for(int i=0; i<m_avFormats.size(); ++i)
	{
		avFormats.push_back(m_avFormats[i].name);
	}

	addAttribute("format", "");
	addAttribute("location", "");
	addAttribute("loop", m_loop);
	addAttribute("directory", "");

//	m_bufferSampleId=MediaSampleFactory::getTypeId("BufferSample");
	m_ffmpegPacketSampleId=Limitless::MediaSampleFactory::getTypeId("FfmpegPacketSample");
    m_eventSampleId=Limitless::MediaSampleFactory::getTypeId("EventSample");

	av_init_packet(&m_pkt);
//	addSourcePad("[{\"mime\":\"video/*\"}, {\"mime\":\"image/*\"}]");

	return true;
}

SharedPluginView FfmpegInput::getView()
{
	return SharedPluginView();
}

void FfmpegInput::startReadThread()
{
	m_readThreadRunning=false;
	m_stopReadThread=false;
	m_readThread=std::thread(std::bind(&FfmpegInput::readThread, this));

	{//wait for thread to start
		std::unique_lock<std::mutex> lock(m_readThreadMutex);

		while(!m_readThreadRunning)
			m_readThreadEvent.wait(lock);
	}
}

void FfmpegInput::stopReadThread()
{
	{
		std::unique_lock<std::mutex> lock(m_readThreadMutex);

		if(!m_readThreadRunning)
			return;

		m_stopReadThread=true;
	}
	m_readThreadEvent.notify_all();

	//interupt all threads waiting on queue functions
	for(size_t i=0; i<m_streamPads.size(); ++i)
		m_streamPads[i].m_queue.breakWait();

	m_readThread.join();
}

void FfmpegInput::readThread()
{
	std::unique_lock<std::mutex> lock(m_readThreadMutex);

	m_readThreadRunning=true;
	int avError;
	
	while(!m_stopReadThread)
	{
		lock.unlock();

		Limitless::SharedMediaSample sourceSample=newSample(m_ffmpegPacketSampleId);

		if(sourceSample == Limitless::SharedMediaSample())
		{
			lock.lock();
			continue;
		}

		FfmpegPacketSample *packetSample=dynamic_cast<FfmpegPacketSample *>(sourceSample.get());
//		OutputDebugStringA((boost::format("****FFMpegInput got sample 0x%08x\n")%sourceSample.get()).str().c_str());
		AVPacket *packet=packetSample->getPacket();
		bool hasPacket=false;

		avError=av_read_frame(m_avFormatContext, packet);

		if(avError == 0)
		{
			hasPacket=true;
		}
		else
		{
			if(avError == AVERROR_EOF)
			{
                if(m_loop)
                {
                    avError=av_seek_frame(m_avFormatContext, -1, 0, 0);

                    if(avError>=0)
                    {
                        avError=av_read_frame(m_avFormatContext, packet);

                        if(avError==0)
                            hasPacket=true;
                    }

                    m_loopTime=m_lastFrameTime;
                }
                else
                {
                    if(packet->stream_index<m_streamPads.size())
                    {
                        StreamPad &streamPad=m_streamPads[packet->stream_index];
                        SharedMediaPad mediaPad=streamPad.pad;

                        if(mediaPad!=SharedMediaPad()&&mediaPad->linked())
                        {
                            SharedEventSample eventSample=newSampleType<EventSample>(m_eventSampleId);

                            eventSample->setEvent(Limitless::Event::EndOf);
                            streamPad.m_queue.push_back(eventSample);
                        }
                    }
                    return;
                }
			}
		}

		if(hasPacket)
		{
			if(packet->stream_index < m_streamPads.size())
			{
				StreamPad &streamPad=m_streamPads[packet->stream_index];
				SharedMediaPad mediaPad=streamPad.pad;

				if(mediaPad != SharedMediaPad() &&mediaPad->linked())
				{
					auto stream=m_avFormatContext->streams[packet->stream_index];
					AVRational timeBase{1, AV_TIME_BASE};

					m_lastFrameTime=av_rescale_q(packet->pts, stream->time_base, timeBase);
					sourceSample->setTimestamp(m_playStreamTime+m_loopTime+m_lastFrameTime);
					sourceSample->setSourceTimestamp(m_lastFrameTime);
//					pushSample(mediaPad, sourceSample);
//					OutputDebugStringA((boost::format("****FFMpegInput queued sample 0x%08x\n")%sourceSample.get()).str().c_str());
					streamPad.m_queue.push_back(sourceSample);
				}
			}
		}
		else
		{
			char errorString[AV_ERROR_MAX_STRING_SIZE];


			av_strerror(avError, errorString, AV_ERROR_MAX_STRING_SIZE);
			Log::error("FfmpegInput", (boost::format("av_read_frame error: %s")%errorString).str());

			return;
		}

		lock.lock();
	}

	m_readThreadRunning=false;
}

bool FfmpegInput::processSample(SharedMediaPad sinkPad, SharedMediaSample sample)
{
	//find StreamPad for this pad
	StreamPad *streamPad=nullptr;

	for(size_t i=0; i<m_streamPads.size(); ++i)
	{
		if(m_streamPads[i].pad==sinkPad)
		{
			streamPad=&m_streamPads[i];
			break;
		}
	}

	if(streamPad == nullptr)
		return false;

	SharedMediaSample streamSample=streamPad->m_queue.pop_front();

//	OutputDebugStringA((boost::format("****FFMpegInput process sample 0x%08x\n")%streamSample.get()).str().c_str());
	if(streamSample)
	{
        if(!streamSample->isType(m_ffmpegPacketSampleId))
            OutputDebugStringA((boost::format("****FFMpegInput push sample 0x%08x\n")%streamSample.get()).str().c_str());
//		OutputDebugStringA((boost::format("****FFMpegInput push sample 0x%08x\n")%streamSample.get()).str().c_str());
		pushSample(sinkPad, streamSample);
	}

    return true;
}

//bool FfmpegInput::processSample(SharedMediaPad sinkPad, SharedMediaSample sample)
//{
//	int avError;
//
////	if(m_firstSample)
////	{
////		std::string location=attribute("location")->toString();
////
////		if(!(m_avOutputFormat->flags & AVFMT_NOFILE))
////		{
////			avError=avformat_open_input(&m_avFormatContext, location.c_str(), NULL, NULL);
////		}
////		m_firstSample=false;
////	}
//
////	if(sample->type() == m_bufferSampleId)
////	{
////		m_pkt.data=sample->buffer();
////		m_pkt.size=sample->size();
////		avError=av_read_frame(m_avFormatContext, &m_pkt);
////	}
////	deleteSample(sample);
//	Limitless::SharedMediaSample sourceSample=newSample(m_ffmpegPacketSampleId);
//
//	if(sourceSample == Limitless::SharedMediaSample())
//		return false;
//
//	FfmpegPacketSample *packetSample=dynamic_cast<FfmpegPacketSample *>(sourceSample.get());
//	AVPacket *packet=packetSample->getPacket();
//	bool hasPacket=false;
//
//	avError=av_read_frame(m_avFormatContext, packet);
//
////	Log::write((boost::format("Input %08x,%08x: %08x")%GetCurrentThreadId()%this%packet->buf).str());
//
//	if(avError == 0)
//	{
//		hasPacket=true;
////		pushSample(sourceSample);
////		return true;
//	}
//	else
//	{
//		if((avError == AVERROR_EOF) && m_loop)
//		{
//			avError=av_seek_frame(m_avFormatContext, -1, 0, 0);
//
//			if(avError >= 0)
//			{
//				avError=av_read_frame(m_avFormatContext, packet);
//
//				if(avError == 0)
//					hasPacket=true;
//			}
//
//			m_loopTime=m_lastFrameTime;
//		}
//	}
//
//	if(hasPacket)
//	{
//		if(packet->stream_index < m_streamPads.size())
//		{
//			SharedMediaPad mediaPad=m_streamPads[packet->stream_index];
//
//			if(mediaPad != SharedMediaPad())
//			{
//				auto stream=m_avFormatContext->streams[packet->stream_index];
//				AVRational timeBase{1, AV_TIME_BASE};
//
//				m_lastFrameTime=av_rescale_q(packet->pts, stream->time_base, timeBase);
//				sourceSample->setTimestamp(m_playStreamTime+m_loopTime+m_lastFrameTime);
//				sourceSample->setSourceTimestamp(m_lastFrameTime);
//				pushSample(mediaPad, sourceSample);
//				return true;
//			}
//		}
//		return false;
//	}
//	else
//	{
//		char errorString[AV_ERROR_MAX_STRING_SIZE];
//
//
//		av_strerror(avError, errorString, AV_ERROR_MAX_STRING_SIZE);
//		Log::error("FfmpegInput", (boost::format("av_read_frame error: %s")%errorString).str());
//
//		return false;
//	}
//}

std::vector<FfmpegInput::Format> FfmpegInput::getFormats()
{ 
	std::vector<FfmpegInput::Format> formats;

	for(std::string &shortName:m_accessibleFormats)
	{
		auto item=std::find_if(m_formatDetails.begin(), m_formatDetails.end(), [&shortName](FfmpegInput::Format &format) { return (format.shortName == shortName); });

		if(item != m_formatDetails.end())
			formats.push_back(*item);
	}
	return formats;
}

IMediaFilter::StateChange FfmpegInput::onReady()
{
	if(!exists("location"))
		return FAILED;

	int avError;
	std::string location=attribute("location")->toString();

//	setupFormat();

	m_avFormatContext=avformat_alloc_context();

	if(m_avFormatContext == NULL)
		return FAILED;

	int error;
	AVInputFormat *fileInputFormat;

	error=avformat_open_input(&m_avFormatContext, location.c_str(), NULL, NULL);

	if(error < 0)
		return FAILED;

	error=avformat_find_stream_info(m_avFormatContext, nullptr);
	
	if(error < 0)
		return FAILED;
	
	for(unsigned int i = 0; i < m_avFormatContext->nb_streams; ++i)
	{
		auto stream=m_avFormatContext->streams[i];
		AVCodecContext * codecContext=stream->codec;
		auto codecType=codecContext->codec_type;
		auto codecID=codecContext->codec_id;

		const auto codec=avcodec_find_decoder(codecContext->codec_id);

		if(codecContext->codec_type == AVMEDIA_TYPE_VIDEO)
		{
			SharedMediaFormat mediaFormat(new MediaFormat());
			std::string mimeType=(boost::format("video/%s")%codec->name).str();

			unsigned int contextId=FfmpegResources::pushCodecContext(codecContext);

			mediaFormat->addAttribute("mime", mimeType);
			mediaFormat->addAttribute("width", codecContext->width);
			mediaFormat->addAttribute("height", codecContext->height);
			mediaFormat->addAttribute("format", FfmpegResources::getAvPixelFormatName(codecContext->pix_fmt));
			mediaFormat->addAttribute("ffmpegCodecContext", contextId);

			if(m_streamPads.size() < i+1)
				m_streamPads.resize(i+1);

			SharedMediaPad mediaPad=addSourcePad((boost::format("Source%d")%i).str(), mediaFormat);
			
			m_streamPads[i].pad=mediaPad;
		}
		else if(codecContext->codec_type == AVMEDIA_TYPE_AUDIO)
		{
			SharedMediaFormat mediaFormat(new MediaFormat());
			std::string mimeType=(boost::format("audio/%s")%codec->name).str();

			unsigned int contextId=FfmpegResources::pushCodecContext(codecContext);

			int sample_rate; ///< samples per second
			int channels;    ///< number of audio channels

			/**
			* audio sample format
			* - encoding: Set by user.
			* - decoding: Set by libavcodec.
			*/
			enum AVSampleFormat sample_fmt;

			mediaFormat->addAttribute("mime", mimeType);
			mediaFormat->addAttribute("sampleRate", codecContext->sample_rate);
			mediaFormat->addAttribute("channels", codecContext->channels);
			mediaFormat->addAttribute("format", (int)FfmpegResources::getAudioFormat(codecContext->sample_fmt));
			mediaFormat->addAttribute("ffmpegCodecContext", contextId);

			if(m_streamPads.size() < i+1)
				m_streamPads.resize(i+1);

			SharedMediaPad mediaPad=addSourcePad((boost::format("Source%d")%i).str(), mediaFormat);

			m_streamPads[i].pad=mediaPad;
		}
	}

	return SUCCESS;
}

IMediaFilter::StateChange FfmpegInput::onPaused()
{
	stopReadThread();

	return SUCCESS;
}

IMediaFilter::StateChange FfmpegInput::onPlaying()
{
	startReadThread();
	m_playStreamTime=getStreamTime();
	m_loopTime=0;
	return SUCCESS;
}

void FfmpegInput::onLinkFormatChanged(SharedMediaPad pad, SharedMediaFormat format)
{
	if(pad->type() == MediaPad::SOURCE)
	{
//		if(format->exists("mime"))
//			m_codecId=FfmpegResources::instance().getAvCodecID(format->attribute("mime")->toString());
//		if(format->exists("bitrate"))
//			m_bitrate=format->attribute("bitrate")->toInt();
//		if(format->exists("timeBaseNum"))
//			m_timeBase.num=format->attribute("timeBaseNum")->toInt();
//		if(format->exists("timeBaseDen"))
//			m_timeBase.den=format->attribute("timeBaseDen")->toInt();
//		if(format->exists("keyframeRate"))
//			m_keyframeRate=format->attribute("keyframeRate")->toInt();
//		if(format->exists("format"))
//			m_pixelFormat=FfmpegResources::instance().getAvPixelFormat(format->attribute("format")->toString());
//
//		if(format->exists("width"))
//			m_width=format->attribute("width")->toInt();
//		if(format->exists("height"))
//			m_height=format->attribute("height")->toInt();
	}
}

bool FfmpegInput::onAcceptMediaFormat(SharedMediaPad pad, SharedMediaFormat format)
{
	if(pad->type() == MediaPad::SOURCE)
	{
		SharedMediaPads pads=getSourcePads();
		SharedMediaPads::iterator iter=std::find(pads.begin(), pads.end(), pad);

		if(iter != pads.end())
		{
			if(format->exists("mime"))
			{
				std::string mime=format->attribute("mime")->toString();

				if(mime.compare(0, 5, "video") == 0)
					return true;
				if(mime.compare(0, 5, "image") == 0)
					return true;
				if(mime.compare(0, 5, "audio") == 0)
					return true;
			}
		}
	}
	return false;
}

SharedPluginView FfmpegInput::getAttributeView()
{
	SharedQtPluginView currentView=m_attributeView.lock();

	if(currentView == SharedQtPluginView())
	{
		FfmpegInputAttributeView *attributeView=new FfmpegInputAttributeView(this);

		currentView.reset(new QtPluginView(attributeView));
		m_attributeView=currentView;
	}
	return currentView;
}

void FfmpegInput::onAttributeChanged(std::string name, SharedAttribute attribute)
{
	if(name == "loop")
		m_loop=attribute->toBool();
}

void FfmpegInput::queryFormats()
{
	AVOutputFormat *avFormat=NULL;

	while((avFormat=av_oformat_next(avFormat)) != NULL)
	{
		Strings::iterator iter=std::find(m_accessibleFormats.begin(), m_accessibleFormats.end(), avFormat->name);

		if(iter != m_accessibleFormats.end())
			m_avFormats.push_back(FormatDescription(avFormat));
	}
}