#include "FfmpegDecoder.h"

#include "QtComponents/QtPluginView.h"
#include "Media/MediaPad.h"
#include "Media/MediaSampleFactory.h"

#include "ffmpegResources.h"
#include "ffmpegControls.h"
#include "ffmpegPacketSample.h"

#include <boost/foreach.hpp>
#include <boost/tokenizer.hpp>

extern "C"
{
	#include <libavutil/avutil.h>
	#include <libavutil/opt.h>
	#include <libavutil/imgutils.h>
}

//#include "Utilities\utilitiesImage.h"

using namespace Limitless;

FfmpegDecoder::FfmpegDecoder(std::string name, SharedMediaFilter parent):
MediaAutoRegister(name, parent),
m_enabled(false),
m_currentVideoCodec(0),
m_videoCodec(nullptr),
m_audioCodec(nullptr),
m_videoCodecInit(false)//,
//m_sequenceNumber(0)
{
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
}

FfmpegDecoder::~FfmpegDecoder()
{

}

bool FfmpegDecoder::initialize(const Attributes &attributes)
{
	FfmpegResources::instance().registerAll();

	m_ffmpegPacketSampleId=MediaSampleFactory::getTypeId("FfmpegPacketSample");
	m_ffmpegFrameSampleId=MediaSampleFactory::getTypeId("FfmpegFrameSample");
	m_ffmpegAudioSampleId=MediaSampleFactory::getTypeId("FfmpegAudioSample");
	m_imageSampleId=MediaSampleFactory::getTypeId("ImageSample");
	m_bufferSampleId=MediaSampleFactory::getTypeId("BufferSample");

	queryCodecs();

	std::string sinkPadDescription="[";
	bool first=true;

	foreach(const CodecDescription &description, m_videoCodecs)
	{
		if(first)
		{
			sinkPadDescription+=(boost::format("{\"mime\":\"video/%s\"}")%description.name).str();
			first=false;
		}
		else
			sinkPadDescription+=(boost::format(", {\"mime\":\"video/%s\"}")%description.name).str();
	}
	foreach(const CodecDescription &description, m_audioCodecs)
	{
		if(first)
		{
			sinkPadDescription+=(boost::format("{\"mime\":\"audio/%s\"}")%description.name).str();
			first=false;
		}
		else
			sinkPadDescription+=(boost::format(", {\"mime\":\"audio/%s\"}")%description.name).str();
	}
	sinkPadDescription+="]";

    addAttribute("enable", false);
	addSinkPad("Sink", sinkPadDescription);

	return true;
}

SharedPluginView FfmpegDecoder::getView()
{
	if(m_view == SharedPluginView())
	{
//		FfmpegControls *controls=new FfmpegControls(this);
//		m_view.reset(new QtPluginView(controls));
	}
	return m_view;
}

bool FfmpegDecoder::processSample(SharedMediaPad sinkPad, SharedMediaSample sample)
{
    if(!m_enabled)
        return false;

	if(sample->type() == m_ffmpegPacketSampleId)
	{
		SharedFfmpegPacketSample packetSample=boost::dynamic_pointer_cast<FfmpegPacketSample>(sample);

		if(packetSample == SharedFfmpegPacketSample())
			return false;

		int output;
		int avError;
		int frame=0;

		//codec not selected yet wait for the codec.
		if(!m_videoCodecInit)
		{
			std::unique_lock<std::mutex> lock(m_codecMutex);

			while(!m_videoCodecInit)
			{
				m_codecEvent.wait(lock);
			}
		}

		if(m_videoCodec != nullptr)
		{
			if(m_frameSample == SharedFfmpegFrameSample())
			{
				SharedMediaSample mediaSample=newSample(m_ffmpegFrameSampleId);
				m_frameSample=boost::dynamic_pointer_cast<FfmpegFrameSample>(mediaSample);

				if(m_frameSample == SharedFfmpegFrameSample())
					return false;

				m_frameSample->unref();
			}

			AVFrame *inputFrame=m_frameSample->getFrame();

			avError=avcodec_decode_video2(m_videoCodec, inputFrame, &frame, packetSample->getPacket());

			if(frame)
			{
				//			Limitless::savePGM("decodeImage.pgm", Limitless::GREY, inputFrame->data[0], inputFrame->linesize[0], inputFrame->height);
				//			Log::write((boost::format("Decoder %08x,%08x: %08x -> %08x")%GetCurrentThreadId()%this%(void *)packetSample->getPacket()->buf%(void *)inputFrame->data[0]).str());

				//			SharedMediaSample sourceSample=newSample(m_imageSampleId);
				//			SharedImageSample imageSample=boost::dynamic_pointer_cast<ImageSample>(sourceSample);

				inputFrame->pts=av_frame_get_best_effort_timestamp(inputFrame);
				Log::message("FfmpegDecoder", (boost::format("Decoder %08x - frame time %d\n")%this%inputFrame->pts).str());

				m_frameSample->copyHeader(sample, instance());
				m_frameSample->setSourceTimestamp(inputFrame->pts);
                m_frameSample->setSequenceNumber(inputFrame->pts);
				m_frameSample->setFormat(m_videoCodec->pix_fmt);
				pushSample(m_frameSample);
				m_frameSample.reset();
			}
		}
		else if(m_audioCodec != nullptr)
		{
			int length;
			int outSize;
			AVPacket *packet=packetSample->getPacket();
			
			while(packet->size > 0)
			{
				//get audio sample
				if(m_audioSample == SharedFfmpegAudioSample())
				{
					m_audioSample=newSampleType<FfmpegAudioSample>(m_ffmpegAudioSampleId);

					if(!m_audioSample)
						return false;

					m_audioSample->unref();
				}
				AVFrame *inputFrame=m_audioSample->getFrame();

				length=avcodec_decode_audio4(m_audioCodec, inputFrame, &frame, packet);

				if(length < 0)
					break;

				packet->size-=length;
				packet->data+=length;

				if(frame)
				{
//					size_t unpadded_linesize=inputFrame->nb_samples*av_get_bytes_per_sample((AVSampleFormat)inputFrame->format);

					inputFrame->pts=av_frame_get_best_effort_timestamp(inputFrame);
					m_audioSample->copyHeader(sample, instance());
					//				m_frameSample->setFormat(m_videoCodec->pix_fmt);
					pushSample(m_audioSample);
					m_audioSample.reset();
				}
			}
		}
	}
	else if(sample->type() == m_bufferSampleId)
	{
//		SharedImageSample imageSample=boost::dynamic_pointer_cast<ImageSample>(sample);
//
//		if(imageSample == SharedImageSample())
//			return false;
//
//		int output;
//		int avError;
//	//	pushSample(sample);
//		SharedMediaSample sourceSample=newSample(m_bufferSampleId);
//		SharedBufferSample bufferSample=boost::dynamic_pointer_cast<BufferSample>(sourceSample);
//
//		bufferSample->allocate(imageSample->width()*imageSample->height()*3);
//
//		int imageSize=(imageSample->size() < m_avFrameSize)?imageSample->size():m_avFrameSize;
//		
//		if(imageSize <= 0)
//			return false;
//
////		savePPM("test.ppm", imageSample->buffer(), imageSample->width(), imageSample->height());
////		memcpy(m_frame->data[0], imageSample->buffer(), imageSize);
////		m_frame->data[0]=imageSample->buffer();
//		uint8_t *bufferArray[1];
//		int bufferSizeArray[1];
//
//		bufferArray[0]=imageSample->buffer();
//		bufferSizeArray[0]=imageSample->width()*3;
//
////		sws_scale(m_swsContext, bufferArray, bufferSizeArray, imageSample->width(), imageSample->height(), m_frame->data, m_frame->linesize);
//		sws_scale(m_swsContext, bufferArray, bufferSizeArray, 0, imageSample->height(), m_frame->data, m_frame->linesize);
//
//		m_pkt.data=bufferSample->buffer();
//		m_pkt.size=bufferSample->actualSize();
//
//		avError=avcodec_encode_video2(m_videoCodec, &m_pkt, m_frame, &output);
//
//		if(output == 1)
//		{
//			bufferSample->setSize(m_pkt.size);
//			pushSample(bufferSample);
//		}
//
//		deleteSample(sample);
	}
    else
        pushSample(sample);
	return true;
}

IMediaFilter::StateChange FfmpegDecoder::onReady()
{
	SharedMediaPads sinkMediaPads=getSinkPads();

	if(sinkMediaPads.size() <= 0)
		return FAILED;

//	foreach(SharedMediaPad sinkPad, sinkMediaPads)
//	{
//		if(!sinkPad->linked())
//			continue;
//
//		SharedMediaFormat mediaFormat=sinkPad->format();
//
//		if(!mediaFormat->exists("mime"))
//			continue;
//
//		std::string mime=mediaFormat->attribute("mime")->toString();
//
//		if(mime.compare(0, 5, "video"))
//		{
//			std::string type=mime.substr(6, mime.size()-6);
//
//			foreach(const CodecDescription &description, m_videoCodecs)
//			{
//				if(type == description.name)
//				{
//					SharedMediaFormat mediaFormat(new MediaFormat());
//
//					mediaFormat->addAttribute("mime", "video/raw");
//
//					addSourcePad("Source", mediaFormat);
//				}
//			}
//		}
//	}
	return SUCCESS;
}

IMediaFilter::StateChange FfmpegDecoder::onPaused()
{
	return SUCCESS;
}

IMediaFilter::StateChange FfmpegDecoder::onPlaying()
{
	return SUCCESS;
}

bool FfmpegDecoder::onAcceptMediaFormat(SharedMediaPad pad, SharedMediaFormat format)
{
	if(pad->type() == MediaPad::SINK)
	{
		if(!format->exists("mime"))
			return false;

		//MimeDetail mimeDetail=parseMimeDetail(format->attribute("mime")->toString());
		//
		//if(mimeDetail.type != "video")
		//	return false;

		FfmpegCodecs codecs=FfmpegResources::instance().codecs();

		//FfmpegCodecs::iterator iter=std::find(codecs.begin(), codecs.end(), mimeDetail.codec);
		FfmpegCodecs::iterator iter=std::find(codecs.begin(), codecs.end(), format->attribute("mime")->toString());

		if(iter != codecs.end())
			return true;

//		return IMediaFilter::onAcceptMediaFormat(pad, format);
	}
	else if(pad->type() == MediaPad::SOURCE)
		return true;
	return false;
}

void FfmpegDecoder::onLinked(SharedMediaPad pad, SharedMediaPad filterPad)
{
	if(pad->type() == MediaPad::SINK)
	{
		std::unique_lock<std::mutex> lock(m_codecMutex);

		m_videoCodecInit=false;
	}
}

void FfmpegDecoder::onLinkFormatChanged(SharedMediaPad pad, SharedMediaFormat format)
{
	if(pad->type() == MediaPad::SINK)
	{
		if(!format->exists("mime"))
			return;

		std::string mime=format->attribute("mime")->toString();
		//MimeDetail mimeDetail=parseMimeDetail(format->attribute("mime")->toString());
		//
		//if(mimeDetail.type != "video")
		//	return;

		FfmpegCodecs codecs=FfmpegResources::instance().codecs();

		//FfmpegCodecs::iterator iter=std::find(codecs.begin(), codecs.end(), mimeDetail.codec);
		FfmpegCodecs::iterator iter=std::find(codecs.begin(), codecs.end(), format->attribute("mime")->toString());

//		if(iter != m_accessibleCodecs.end())
		if(iter != codecs.end())
		{
			std::unique_lock<std::mutex> lock(m_codecMutex);

			SharedMediaFormat sourceFormat(new MediaFormat());
	
			if(format->isVideo())
			{
				sourceFormat->addAttribute("mime", "video/raw");

				if(format->exists("ffmpegCodecContext"))
				{
					unsigned int contextId=format->attribute("ffmpegCodecContext")->toInt();

					m_videoCodec=FfmpegResources::getCodecContext(contextId);

					if(m_videoCodec != nullptr)
					{
						AVCodec *codec=avcodec_find_decoder(m_videoCodec->codec_id);
						int avError;

						m_videoCodec->refcounted_frames=1;
						if((avError=avcodec_open2(m_videoCodec, codec, NULL)) < 0)
						{
							Log::error("FfmpegDecoder", (boost::format("Failed to open codec %d, avcodec_open2")%m_videoCodec->codec_id).str());
							return;
						}

						sourceFormat->addAttribute("width", m_videoCodec->width);
						sourceFormat->addAttribute("height", m_videoCodec->height);
						sourceFormat->addAttribute("format", FfmpegResources::getAvPixelFormatName(m_videoCodec->pix_fmt));
					}
					else
					{
						Log::error("FfmpegDecoder", (boost::format("Failed to locate codec from FfmpegResources contextID:%d")%contextId).str());
						return;
					}
				}
				else
				{
					AVCodec *codec=avcodec_find_decoder(iter->id);

					if(!codec)
					{
						Log::error("FfmpegDecoder", (boost::format("Failed to create codec %d, %s")%iter->id%mime).str());
						return;
					}
					m_videoCodec=avcodec_alloc_context3(codec);

					if(!m_videoCodec)
					{
						Log::error("FfmpegDecoder", (boost::format("Failed to create codec context %d, %s")%iter->id%mime).str());
						return;
					}

					int avError;

					if((avError=avcodec_open2(m_videoCodec, codec, NULL)) < 0)
					{
						char errorString[AV_ERROR_MAX_STRING_SIZE];

						av_strerror(avError, errorString, AV_ERROR_MAX_STRING_SIZE);
						Log::error("FfmpegDecoder", (boost::format("av_read_frameavcodec_open2 error: %s")%errorString).str());
						return;
					}

					if(format->exists("width"))
					{
						sourceFormat->addAttribute("width", format->attribute("width")->toString());
						m_videoCodec->width=format->attribute("width")->toInt();
					}
					if(format->exists("height"))
					{
						sourceFormat->addAttribute("height", format->attribute("height")->toString());
						m_videoCodec->height=format->attribute("height")->toInt();
					}
					if(format->exists("format"))
					{
						sourceFormat->addAttribute("format", format->attribute("format")->toString());
						m_videoCodec->pix_fmt=FfmpegResources::getAvPixelFormat(format->attribute("format")->toString());
					}
				}
			}
			else if(format->isAudio())
			{
				sourceFormat->addAttribute("mime", "audio/raw");

				if(format->exists("ffmpegCodecContext"))
				{
					unsigned int contextId=format->attribute("ffmpegCodecContext")->toInt();

					m_audioCodec=FfmpegResources::getCodecContext(contextId);

					if(m_audioCodec != nullptr)
					{
						AVCodec *codec=avcodec_find_decoder(m_audioCodec->codec_id);
						int avError;

						m_audioCodec->refcounted_frames=1;
						if((avError=avcodec_open2(m_audioCodec, codec, NULL)) < 0)
						{
							Log::error("FfmpegDecoder", (boost::format("Failed to open codec %d, avcodec_open2")%m_audioCodec->codec_id).str());
							return;
						}
						
						sourceFormat->addAttribute("sampleRate", m_audioCodec->sample_rate);
						sourceFormat->addAttribute("channels", m_audioCodec->channels);
						sourceFormat->addAttribute("format", (int)FfmpegResources::getAudioFormat(m_audioCodec->sample_fmt));
					}
					else
					{
						Log::error("FfmpegDecoder", (boost::format("Failed to locate codec from FfmpegResources contextID:%d")%contextId).str());
						return;
					}
				}
				else
				{
					AVCodec *codec=avcodec_find_decoder(iter->id);

					if(!codec)
					{
						Log::error("FfmpegDecoder", (boost::format("Failed to create codec %d, %s")%iter->id%mime).str());
						return;
					}
					m_audioCodec=avcodec_alloc_context3(codec);

					if(!m_audioCodec)
					{
						Log::error("FfmpegDecoder", (boost::format("Failed to create codec context %d, %s")%iter->id%mime).str());
						return;
					}

					int avError;

					if((avError=avcodec_open2(m_audioCodec, codec, NULL)) < 0)
					{
						char errorString[AV_ERROR_MAX_STRING_SIZE];

						av_strerror(avError, errorString, AV_ERROR_MAX_STRING_SIZE);
						Log::error("FfmpegDecoder", (boost::format("av_read_frameavcodec_open2 error: %s")%errorString).str());
						return;
					}

					if(format->exists("sampleRate"))
					{
						sourceFormat->addAttribute("sampleRate", format->attribute("sampleRate")->toInt());
						m_audioCodec->sample_rate=format->attribute("sampleRate")->toInt();
					}
					if(format->exists("channels"))
					{
						sourceFormat->addAttribute("channels", format->attribute("channels")->toInt());
						m_audioCodec->channels=format->attribute("channels")->toInt();
					}
					if(format->exists("format"))
					{
						sourceFormat->addAttribute("format", format->attribute("format")->toInt());
						m_audioCodec->sample_fmt=FfmpegResources::getAudioFormat((Limitless::AudioSampleFormat)format->attribute("format")->toInt());
					}
				}
			}

			SharedMediaPads sourcePads=getSourcePads();

			if(sourcePads.empty())
			{
				addSourcePad("Source", sourceFormat);
			}
			else
			{
				BOOST_FOREACH(SharedMediaPad &sourcePad, sourcePads)
				{
					sourcePad->setFormat(*sourceFormat.get());
				}
			}

			m_videoCodecInit=true;
			m_codecEvent.notify_all();
		}
	}
}

void FfmpegDecoder::onAttributeChanged(std::string name, SharedAttribute attribute)
{
    if(name=="enable")
        m_enabled=attribute->toBool();

//	if(name == "videoEncoder")
//	{
//		std::string videoEncoder=attribute->toString();
//
//		if(m_videoCodecs[m_currentVideoCodec].name != videoEncoder)
//			m_currentVideoCodec=getVideoEncoderIndex(videoEncoder);
//		updateVideoCodecAttributes();
//	}
}

void FfmpegDecoder::queryCodecs()
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
			if(avCodec->decode == NULL)
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
}

int FfmpegDecoder::getVideoCodecIndex(std::string name)
{
	for(size_t i=0; i<m_videoCodecs.size(); ++i)
	{
		if(m_videoCodecs[i].name == name)
			return i;
	}
	return -1;
}

int FfmpegDecoder::getVideoCodecIndexFromId(AVCodecID id)
{
	for(size_t i=0; i<m_videoCodecs.size(); ++i)
	{
		if(m_videoCodecs[i].id == id)
			return i;
	}
	return -1;
}

int FfmpegDecoder::getAudioCodecIndex(std::string name)
{
	for(size_t i=0; i<m_audioCodecs.size(); ++i)
	{
		if(m_audioCodecs[i].name == name)
			return i;
	}
	return -1;
}