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
#ifdef DEBUG
	OutputDebugStringA(logEntry);
#endif //DEBUG
	Limitless::Log::message("Ffmpeg", logEntry);
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

unsigned int FfmpegResources::pushCodecContext(AVCodecContext *codecContext)
{
	FfmpegResources &resource=instance();

	boost::unique_lock<boost::mutex> lock(resource.m_contextMutex);
	
	resource.m_codecContexts.insert(std::pair<unsigned int, AVCodecContext *>(resource.m_uniqueContextId, codecContext));

	unsigned int id=resource.m_uniqueContextId++;

	return id;
}

AVCodecContext *FfmpegResources::getCodecContext(unsigned int id)
{
	FfmpegResources &resource=instance();

	CodecContextMap::iterator iter=resource.m_codecContexts.find(id);

	if(iter != resource.m_codecContexts.end())
		return iter->second;
	return NULL;
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