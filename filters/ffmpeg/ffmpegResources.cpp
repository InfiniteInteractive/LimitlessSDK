#include "ffmpegResources.h"
#include <boost/format.hpp>
#include "Base/Log.h"

extern "C"
{
	#include <libavcodec/avcodec.h>
	#include <libavformat/avformat.h>
}

#include "windows.h"


void avLogCallback(void *ptr, int level, const char* format, va_list args)
{
	static char logEntry[2048];

	vsnprintf(logEntry, 2048, format, args); 
//#ifdef DEBUG
//	OutputDebugStringA(logEntry);
//#endif //DEBUG
    std::string system="Ffmpeg";
    std::string subSystem="";

    FfmpegResources::getContextSystem((AVCodecContext *)ptr, system, subSystem);

    Limitless::Log::message(system, subSystem, logEntry);
}


FfmpegResources *FfmpegResources::s_instance=0;

FfmpegResources::FfmpegResources():
m_registered(false),
m_uniqueContextId(0)
{
	m_formats.push_back(FfmpegFormat("YUV420P", AV_PIX_FMT_YUV420P, 1));
	m_formats.push_back(FfmpegFormat("YUVJ420P", AV_PIX_FMT_YUVJ420P, 1));
	m_formats.push_back(FfmpegFormat("YUV422P", AV_PIX_FMT_YUV422P, 1));
	m_formats.push_back(FfmpegFormat("YUVJ422", AV_PIX_FMT_YUVJ422P, 1));
	m_formats.push_back(FfmpegFormat("YUYV422", AV_PIX_FMT_YUYV422, 1));
	m_formats.push_back(FfmpegFormat("YUV444P", AV_PIX_FMT_YUV444P, 3));
	m_formats.push_back(FfmpegFormat("YUVJ444P", AV_PIX_FMT_YUVJ444P, 3));
	m_formats.push_back(FfmpegFormat("RGB24", AV_PIX_FMT_RGB24, 3));
	m_formats.push_back(FfmpegFormat("BGR24", AV_PIX_FMT_BGR24, 3));
	m_formats.push_back(FfmpegFormat("ARGB", AV_PIX_FMT_ARGB, 4));
	m_formats.push_back(FfmpegFormat("RGBA", AV_PIX_FMT_RGBA, 4));
	m_formats.push_back(FfmpegFormat("ABGR", AV_PIX_FMT_ABGR, 4));
	m_formats.push_back(FfmpegFormat("BGRA", AV_PIX_FMT_BGRA, 4));

	m_audioFormats.insert(FfmpegAudioFormats::value_type(Limitless::AudioSampleFormat::Unknown, AV_SAMPLE_FMT_NONE));
	m_audioFormats.insert(FfmpegAudioFormats::value_type(Limitless::AudioSampleFormat::UInt8, AV_SAMPLE_FMT_U8));
	m_audioFormats.insert(FfmpegAudioFormats::value_type(Limitless::AudioSampleFormat::Int16, AV_SAMPLE_FMT_S16));
	m_audioFormats.insert(FfmpegAudioFormats::value_type(Limitless::AudioSampleFormat::Int32, AV_SAMPLE_FMT_S32));
	m_audioFormats.insert(FfmpegAudioFormats::value_type(Limitless::AudioSampleFormat::Float, AV_SAMPLE_FMT_FLT));
	m_audioFormats.insert(FfmpegAudioFormats::value_type(Limitless::AudioSampleFormat::Double, AV_SAMPLE_FMT_DBL));
	m_audioFormats.insert(FfmpegAudioFormats::value_type(Limitless::AudioSampleFormat::UInt8P, AV_SAMPLE_FMT_U8P));
	m_audioFormats.insert(FfmpegAudioFormats::value_type(Limitless::AudioSampleFormat::Int16P, AV_SAMPLE_FMT_S16P));
	m_audioFormats.insert(FfmpegAudioFormats::value_type(Limitless::AudioSampleFormat::Int32P, AV_SAMPLE_FMT_S32P));
	m_audioFormats.insert(FfmpegAudioFormats::value_type(Limitless::AudioSampleFormat::FloatP, AV_SAMPLE_FMT_FLTP));
	m_audioFormats.insert(FfmpegAudioFormats::value_type(Limitless::AudioSampleFormat::DoubleP, AV_SAMPLE_FMT_DBLP));

	m_codecs.push_back(FfmpegCodec(AV_CODEC_ID_MPEG2VIDEO, "video/mpeg"));
	m_codecs.push_back(FfmpegCodec(AV_CODEC_ID_MPEG4, "video/mp4"));
	m_codecs.push_back(FfmpegCodec(AV_CODEC_ID_H264, "video/h264"));
	m_codecs.push_back(FfmpegCodec(AV_CODEC_ID_THEORA, "video/theora"));

	m_codecs.push_back(FfmpegCodec(AV_CODEC_ID_MP3, "audio/mp3"));
	m_codecs.push_back(FfmpegCodec(AV_CODEC_ID_AAC, "audio/aac"));
	m_codecs.push_back(FfmpegCodec(AV_CODEC_ID_AC3, "audio/ac3"));
	m_codecs.push_back(FfmpegCodec(AV_CODEC_ID_VORBIS, "image/vorbis"));
}

FfmpegResources &FfmpegResources::instance()
{
	if(s_instance == 0)
		s_instance=new FfmpegResources();

	return *s_instance;
}

FfmpegFormats FfmpegResources::formats()
{
	FfmpegResources &resource=instance();

	return resource.m_formats;
}

FfmpegFormat FfmpegResources::getFormat(std::string name)
{
	FfmpegResources &resource=instance();

	FfmpegFormats::iterator iter=std::find(resource.m_formats.begin(), resource.m_formats.end(), name);

	if(iter != resource.m_formats.end())
		return (*iter);
	return FfmpegFormat();
}

AVPixelFormat FfmpegResources::getAvPixelFormat(std::string name)
{
	FfmpegResources &resource=instance();

	FfmpegFormats::iterator iter=std::find(resource.m_formats.begin(), resource.m_formats.end(), name);

	if(iter != resource.m_formats.end())
		return iter->avFormat;
	return AV_PIX_FMT_NONE;
}

std::string FfmpegResources::getAvPixelFormatName(AVPixelFormat format)
{
	FfmpegResources &resource=instance();

	FfmpegFormats::iterator iter=std::find(resource.m_formats.begin(), resource.m_formats.end(), format);

	if(iter != resource.m_formats.end())
		return iter->name;
	return "None";
}

FfmpegCodecs FfmpegResources::codecs()
{
	FfmpegResources &resource=instance();

	return resource.m_codecs;
}

AVCodecID FfmpegResources::getAvCodecID(std::string mime)
{
	FfmpegResources &resource=instance();

	FfmpegCodecs::iterator iter=std::find(resource.m_codecs.begin(), resource.m_codecs.end(), mime);

	if(iter != resource.m_codecs.end())
		return iter->id;
	return AV_CODEC_ID_NONE;
}

std::string FfmpegResources::getAvCodecMime(AVCodecID id)
{
	FfmpegResources &resource=instance();

	FfmpegCodecs::iterator iter=std::find(resource.m_codecs.begin(), resource.m_codecs.end(), id);

	if(iter != resource.m_codecs.end())
		return iter->mime;
	return "None";
}

void FfmpegResources::registerAll()
{
	FfmpegResources &resource=instance();

	if(!resource.m_registered)
	{
		av_register_all();
		avformat_network_init()	;
	}


//#ifndef DEBUG
	av_log_set_callback(avLogCallback);
//#endif //DEBUG
}

unsigned int FfmpegResources::pushCodecContext(Limitless::IMediaFilter *mediaFilter, AVCodecContext *codecContext)
{
	FfmpegResources &resource=instance();

	boost::unique_lock<boost::mutex> lock(resource.m_contextMutex);
	
	resource.m_codecContexts.insert(std::pair<unsigned int, AVCodecContext *>(resource.m_uniqueContextId, codecContext));
    resource.m_codecContextsToFilter.insert(std::pair<AVCodecContext *, Limitless::IMediaFilter *>(codecContext, mediaFilter));

	unsigned int id=resource.m_uniqueContextId++;

	return id;
}

void FfmpegResources::popCodecContext(unsigned int id)
{
    FfmpegResources &resource=instance();

    auto iter=resource.m_codecContexts.find(id);

    if(iter!=resource.m_codecContexts.end())
    {
        auto filterIter=resource.m_codecContextsToFilter.find(iter->second);

        if(filterIter!=resource.m_codecContextsToFilter.end())
            resource.m_codecContextsToFilter.erase(filterIter);

        resource.m_codecContexts.erase(iter);
    }
}

AVCodecContext *FfmpegResources::getCodecContext(unsigned int id)
{
	FfmpegResources &resource=instance();

	CodecContextMap::iterator iter=resource.m_codecContexts.find(id);

	if(iter != resource.m_codecContexts.end())
		return iter->second;
	return NULL;
}


void FfmpegResources::getContextSystem(AVCodecContext *ptr, std::string &system, std::string &subSystem)
{
    FfmpegResources &resource=instance();

    if(ptr == nullptr)//assuming it is an output info
    {
        system="FfmpegOutput";
        return;
    }
    
    CodecContextFilterMap::iterator iter=resource.m_codecContextsToFilter.find(ptr);
    
    if(iter!=resource.m_codecContextsToFilter.end())
    {
        system=iter->second->typeName();
        subSystem=iter->second->instance();
    }
}

AVSampleFormat FfmpegResources::getAudioFormat(Limitless::AudioSampleFormat format)
{
	FfmpegResources &resource=instance();

	FfmpegAudioFormats::iterator iter=resource.m_audioFormats.find(format);

	if(iter != resource.m_audioFormats.end())
		return iter->second;

	return AV_SAMPLE_FMT_NONE;
}

Limitless::AudioSampleFormat FfmpegResources::getAudioFormat(AVSampleFormat format)
{
	FfmpegResources &resource=instance();

	for(FfmpegAudioFormats::iterator iter=resource.m_audioFormats.begin(); iter!=resource.m_audioFormats.end(); ++iter)
	{
		if(iter->second == format)
			return iter->first;
	}

	return Limitless::AudioSampleFormat::Unknown;
}

std::string FfmpegResources::getAudioFormatName(AVSampleFormat format)
{
	std::string sampleFormat="Unknown";

	if(format==AV_SAMPLE_FMT_U8)
		sampleFormat="UInt8";
	else if(format==AV_SAMPLE_FMT_S16)
		sampleFormat="Int16";
	else if(format==AV_SAMPLE_FMT_S32)
		sampleFormat="Int32";
	else if(format==AV_SAMPLE_FMT_FLT)
		sampleFormat="Float";
	else if(format==AV_SAMPLE_FMT_DBL)
		sampleFormat="Double";
	else if(format==AV_SAMPLE_FMT_U8P)
		sampleFormat="UInt8P";
	else if(format==AV_SAMPLE_FMT_S16P)
		sampleFormat="Int16P";
	else if(format==AV_SAMPLE_FMT_S32P)
		sampleFormat="Int32P";
	else if(format==AV_SAMPLE_FMT_FLTP)
		sampleFormat="FloatP";
	else if(format==AV_SAMPLE_FMT_DBLP)
		sampleFormat="DoubleP";

	return sampleFormat;
}

AVSampleFormat FfmpegResources::getAudioFormatFromName(std::string format)
{
	AVSampleFormat sampleFormat=AV_SAMPLE_FMT_NONE;

	if(format=="UInt8")
		sampleFormat=AV_SAMPLE_FMT_U8;
	else if(format=="Int16")
		sampleFormat=AV_SAMPLE_FMT_S16;
	else if(format=="Int32")
		sampleFormat=AV_SAMPLE_FMT_S32;
	else if(format=="Float")
		sampleFormat=AV_SAMPLE_FMT_FLT;
	else if(format=="Double")
		sampleFormat=AV_SAMPLE_FMT_DBL;
	else if(format=="UInt8P")
		sampleFormat=AV_SAMPLE_FMT_U8P;
	else if(format=="Int16P")
		sampleFormat=AV_SAMPLE_FMT_S16P;
	else if(format=="Int32P")
		sampleFormat=AV_SAMPLE_FMT_S32P;
	else if(format=="FloatP")
		sampleFormat=AV_SAMPLE_FMT_FLTP;
	else if(format=="DoubleP")
		sampleFormat=AV_SAMPLE_FMT_DBLP;

	return sampleFormat;
}

uint64_t FfmpegResources::guessAudioChannelLayout(int channels)
{
	uint64_t channelLayout;

	switch(channels)
	{
	case 1:
		channelLayout=AV_CH_LAYOUT_MONO;
		break;
	default:
	case 2:
		channelLayout=AV_CH_LAYOUT_STEREO;
		break;
	case 3:
		channelLayout=AV_CH_LAYOUT_SURROUND;
		break;
	case 4:
		channelLayout=AV_CH_LAYOUT_4POINT0;
		break;
	case 5:
		channelLayout=AV_CH_LAYOUT_5POINT0;
		break;
	case 6:
		channelLayout=AV_CH_LAYOUT_6POINT0;
		break;
	case 7:
		channelLayout=AV_CH_LAYOUT_7POINT0;
		break;
	}

	return channelLayout;
}