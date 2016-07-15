#ifndef _FfmpegSwScale_h_
#define _FfmpegSwScale_h_

#include "Media/MediaPluginFactory.h"
#include "Media/IMediaFilter.h"
#include "codecDescription.h"
#include "ffmpegMediaFilter.h"
#include "ffmpegFrameSample.h"

extern "C"
{
	#include <libavcodec/avcodec.h>
	#include <libswscale/swscale.h>
}

class FfmpegFrameSample;

class FfmpegSwScale:public Limitless::MediaAutoRegister<FfmpegSwScale, FfmpegMediaFilter>
{
public:
	FfmpegSwScale(std::string name, Limitless::SharedMediaFilter parent);
	~FfmpegSwScale();

	virtual bool initialize(const Limitless::Attributes &attributes);
	virtual bool shutdown(){return true;}

	virtual Limitless::SharedPluginView getView();

	virtual bool processSample(Limitless::SharedMediaPad sinkPad, Limitless::SharedMediaSample sample);

	struct MediaInfo
	{
		MediaInfo():width(0), height(0), format("") {}

		bool operator==(const MediaInfo &that) const { return ((width == that.width) && (height == that.height) && (format == that.format)); }
		bool operator!=(const MediaInfo &that) const { return ((width != that.width) || (height != that.height) || (format != that.format)); }

		size_t width;
		size_t height;
		std::string format;
	};
protected:
	//IMediaFilter
	virtual StateChange onReady();
	virtual StateChange onPaused();
	virtual StateChange onPlaying();

	virtual bool FfmpegSwScale::onAcceptMediaFormat(Limitless::SharedMediaPad pad, Limitless::SharedMediaFormat format);
	virtual void onLinkFormatChanged(Limitless::SharedMediaPad mediaPad, Limitless::SharedMediaFormat format);
	//PluginObject
	virtual void onAttributeChanged(std::string name, Limitless::SharedAttribute attribute);
	void updateScaler(bool checkLink=true);

private:
	size_t m_ffmpegFrameSampleId;
	size_t m_imageInterfaceSampleId;
	size_t m_imageSampleId;

	MediaInfo m_inputInfo;
	MediaInfo m_outputInfo;

	
	bool m_widthChanged;
	bool m_heightChanged;
	bool m_formatChanged;
	
	bool m_convert;
	SwsContext *m_swsContext;
};

namespace Limitless{namespace traits
{
	template<> struct type<FfmpegSwScale>
	{
		static FilterType get()
		{return Filter;}
	};
	template<> struct category<FfmpegSwScale>
	{
		static std::string get()
		{return "filter";}
	};
}}//Limitless::traits

#endif // _FfmpegSwScale_h_
