#ifndef _FfmpegResources_h_
#define _FfmpegResources_h_

#include "Media/IAudioSample.h"

extern "C"
{
	#include <libavcodec/avcodec.h>
	#include <libavutil/avutil.h>
	#include <libavutil/opt.h>
}

#include <string>
#include <vector>

#include <boost/thread.hpp>

class FfmpegFormat
{
public:
	FfmpegFormat():name(""), avFormat(AV_PIX_FMT_NONE), channels(0) {}
	FfmpegFormat(std::string name, AVPixelFormat avFormat, unsigned int channels):name(name), avFormat(avFormat), channels(channels){}
	~FfmpegFormat(){}

	bool operator==(const std::string &thatName){return name == thatName;}
	bool operator==(const AVPixelFormat &thatAvFormat){return avFormat == thatAvFormat;}

	std::string name;
	AVPixelFormat avFormat;
	unsigned int channels;
};
typedef std::vector<FfmpegFormat> FfmpegFormats;

class FfmpegCodec
{
public:
	FfmpegCodec(AVCodecID id, std::string mime):id(id), mime(mime){}
	AVCodecID id;
	std::string mime;

	bool operator==(const AVCodecID &thatId){return id == thatId;}
	bool operator==(const std::string &thatMime){return mime == thatMime;}
};
typedef std::vector<FfmpegCodec> FfmpegCodecs;

typedef std::map<Limitless::AudioSampleFormat, AVSampleFormat> FfmpegAudioFormats;

class FfmpegResources
{
public:
	~FfmpegResources();

	static FfmpegResources &instance();
	static FfmpegFormats formats();
	static FfmpegFormat getFormat(std::string name);
	static AVPixelFormat getAvPixelFormat(std::string name);
	static std::string getAvPixelFormatName(AVPixelFormat format);

	static FfmpegCodecs codecs();
	static AVCodecID getAvCodecID(std::string name);
	static std::string getAvCodecMime(AVCodecID id);

	static AVSampleFormat getAudioFormat(Limitless::AudioSampleFormat format);
	static Limitless::AudioSampleFormat getAudioFormat(AVSampleFormat format);

	static unsigned int pushCodecContext(AVCodecContext *codecContext);
	static AVCodecContext *getCodecContext(unsigned int id);

	static void registerAll();
private:
	FfmpegResources();

	static FfmpegResources *s_instance;

	bool m_registered;
	FfmpegFormats m_formats;
	FfmpegCodecs m_codecs;
	FfmpegAudioFormats m_audioFormats;

	boost::mutex m_contextMutex;
	unsigned int m_uniqueContextId;

	typedef std::map<unsigned int, AVCodecContext *> CodecContextMap;
	CodecContextMap m_codecContexts;
};


template <class PluginObject> void buildAttribute(PluginObject *object, const AVOption *option)
{
	void *dst=((uint8_t*)object)+option->offset;

	switch(option->type)
	{
	case AV_OPT_TYPE_FLAGS:
		object->addAttribute(option->name, *(int *)dst);
		break;
//	case AV_OPT_TYPE_PIXEL_FMT:
//	case AV_OPT_TYPE_SAMPLE_FMT:
	case AV_OPT_TYPE_INT:       
	case AV_OPT_TYPE_INT64:
		object->addAttribute(option->name, *(int *)dst);
		break;
	case AV_OPT_TYPE_FLOAT:
	case AV_OPT_TYPE_DOUBLE:
		object->addAttribute(option->name, *(double *)dst);
		break;
	case AV_OPT_TYPE_STRING:
		object->addAttribute(option->name, *(char **)dst);
		break;
//	case AV_OPT_TYPE_RATIONAL:  *intnum = ((AVRational*)dst)->num;
//	                            *den    = ((AVRational*)dst)->den;
//	                                                    return 0;
//	case AV_OPT_TYPE_CONST:     *num    = o->default_val.dbl; return 0;
	}
}
#endif //_FfmpegResources_h_