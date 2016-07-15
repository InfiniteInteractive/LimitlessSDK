#include "FfmpegSwScale.h"

#include "QtComponents/QtPluginView.h"
#include "Media/MediaPad.h"
#include "Media/MediaSampleFactory.h"
#include "Media/ImageSample.h"

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

FfmpegSwScale::FfmpegSwScale(std::string name, SharedMediaFilter parent):
MediaAutoRegister(name, parent),
m_swsContext(NULL),
m_widthChanged(false),
m_heightChanged(false),
m_formatChanged(false)
{
	addAttribute("width", 0);
	addAttribute("height", 0);
	addAttribute("format", "RGB24");
}

FfmpegSwScale::~FfmpegSwScale()
{

}

bool FfmpegSwScale::initialize(const Attributes &attributes)
{
	FfmpegResources::instance().registerAll();

	m_ffmpegFrameSampleId=MediaSampleFactory::getTypeId("FfmpegFrameSample");
	m_imageInterfaceSampleId=MediaSampleFactory::getTypeId("IImageSample");
	m_imageSampleId=MediaSampleFactory::getTypeId("ImageSample");

	addSinkPad("Sink", "{\"mime\":\"video/raw\"}");

	return true;
}

SharedPluginView FfmpegSwScale::getView()
{
	return SharedPluginView();
//	if(m_view == SharedPluginView())
//	{
//	}
//	return m_view;
}

bool FfmpegSwScale::processSample(SharedMediaPad sinkPad, SharedMediaSample sample)
{
	if(m_convert)
	{
		if(sample->isType(m_ffmpegFrameSampleId))
		{
			SharedFfmpegFrameSample frameSample=boost::dynamic_pointer_cast<FfmpegFrameSample>(sample);

			if(frameSample == SharedFfmpegFrameSample())
				return false;
			
			AVFrame *avFrame=frameSample->getFrame();
			SharedMediaSample mediaSample=newSample(m_imageSampleId);
			SharedImageSample imageSample=boost::dynamic_pointer_cast<ImageSample>(mediaSample);

			FfmpegFormat format=FfmpegResources::getFormat(m_outputInfo.format);

			imageSample->resize(m_outputInfo.width, m_outputInfo.height, format.channels);
			
			uint8_t *rgbaPlane[]={imageSample->buffer()};
			int rgbaPitch[]={imageSample->width()*format.channels};

//			Limitless::savePGM("swScaleInputImage.pgm", Limitless::GREY, avFrame->data[0], avFrame->linesize[0], avFrame->height);
//			Log::write((boost::format("SwScale %08x,%08x: %08x -> %08x")%GetCurrentThreadId()%this%(void *)avFrame->data[0]%(void *)imageSample->buffer()).str());
			sws_scale(m_swsContext, avFrame->data, avFrame->linesize, 0, avFrame->height, rgbaPlane, rgbaPitch);
			imageSample->copyHeader(sample, instance());
			pushSample(imageSample);
		}
		else if(sample->isType(m_imageInterfaceSampleId))
		{
			SharedIImageSample imageSample=boost::dynamic_pointer_cast<IImageSample>(sample);

			if(!imageSample)
				return false;
			
			SharedImageSample mediaSample=newSampleType<ImageSample>(m_imageSampleId);

			FfmpegFormat format=FfmpegResources::getFormat(m_outputInfo.format);

			mediaSample->resize(m_outputInfo.width, m_outputInfo.height, format.channels);
			
			uint8_t *srcPlane[]={imageSample->buffer()};
			int srcPitch[]={imageSample->width()*format.channels};

			uint8_t *dstPlane[]={mediaSample->buffer()};
			int dstPitch[]={mediaSample->width()*format.channels};

			sws_scale(m_swsContext, srcPlane, srcPitch, 0, m_outputInfo.height, dstPlane, dstPitch);
			imageSample->copyHeader(sample, instance());
			pushSample(imageSample);
		}
        else
            pushSample(sample);
	}
	else
		pushSample(sample);

	return true;
}

IMediaFilter::StateChange FfmpegSwScale::onReady()
{
	SharedMediaPads sinkMediaPads=getSinkPads();

	if(sinkMediaPads.size() <= 0)
		return FAILED;

	return SUCCESS;
}

IMediaFilter::StateChange FfmpegSwScale::onPaused()
{
	return SUCCESS;
}

IMediaFilter::StateChange FfmpegSwScale::onPlaying()
{
	return SUCCESS;
}

bool FfmpegSwScale::onAcceptMediaFormat(SharedMediaPad pad, SharedMediaFormat format)
{
	if(pad->type() == MediaPad::SINK)
	{
		if(!format->exists("mime"))
			return false;

		MimeDetail mimeDetail=parseMimeDetail(format->attribute("mime")->toString());
		
		if((mimeDetail.type != "video") && (mimeDetail.type != "image"))
			return false;
		if((mimeDetail.codec != "raw") && (mimeDetail.codec != ""))
			return false;

		return true;
	}
	else if(pad->type() == MediaPad::SOURCE)
		return true;
	return false;
}

void FfmpegSwScale::onLinkFormatChanged(SharedMediaPad pad, SharedMediaFormat format)
{
	if(pad->type() == MediaPad::SINK)
	{
		if(!format->exists("mime"))
			return;

		std::string mime=format->attribute("mime")->toString();
		MimeDetail mimeDetail=parseMimeDetail(format->attribute("mime")->toString());
		
		if((mimeDetail.type != "video") && (mimeDetail.type != "image"))
			return;
		if((mimeDetail.codec != "raw") && (mimeDetail.codec != ""))
			return;

		if(format->exists("width"))
			m_inputInfo.width=format->attribute("width")->toInt();
		if(format->exists("height"))
			m_inputInfo.height=format->attribute("height")->toInt();
		if(format->exists("format"))
			m_inputInfo.format=format->attribute("format")->toString();

		//set output values if not set by user
		if(!m_widthChanged)
			setAttribute("width", (int)m_inputInfo.width);
//			m_outputInfo.width=m_inputInfo.width;
		if(!m_heightChanged)
			setAttribute("height", (int)m_inputInfo.height);
//			m_outputInfo.height=m_inputInfo.height;
		if(!m_formatChanged)
			setAttribute("format", m_inputInfo.format);
//			m_outputInfo.format=m_inputInfo.format;

		updateScaler(false);
	}
}

void FfmpegSwScale::onAttributeChanged(std::string name, SharedAttribute attribute)
{
	bool update=false;
	
	if(name == "width")
	{
		m_outputInfo.width=attribute->toInt();
		if(m_outputInfo.width > 0)
			m_widthChanged=true;
		update=true;
	}
	if(name == "height")
	{
		m_outputInfo.height=attribute->toInt();
		if(m_outputInfo.height > 0)
			m_heightChanged=true;
		update=true;
	}
	if(name == "format")
	{
		m_outputInfo.format=attribute->toString();
		m_formatChanged=true;
		update=true;
	}

	if(update)
		updateScaler();
}

void FfmpegSwScale::updateScaler(bool checkLink)
{
	m_convert=false;

	SharedMediaPads sourcePads=getSourcePads();

	if(checkLink)
	{
		if(sourcePads.empty())
			return;

		bool linked=false;
		for(SharedMediaPad &sourcePad:sourcePads)
		{
			if(sourcePad->linked())
			{
				linked=true;
				break;
			}
		}

		if(!linked)
			return;
	}

	bool update=false;

	if(m_outputInfo != m_inputInfo)
	{
		AVPixelFormat inputFormat=FfmpegResources::getAvPixelFormat(m_inputInfo.format);
		AVPixelFormat outputFormat=FfmpegResources::getAvPixelFormat(m_outputInfo.format);

		m_swsContext=sws_getCachedContext(m_swsContext, m_inputInfo.width, m_inputInfo.height, inputFormat,
			m_outputInfo.width, m_outputInfo.height, outputFormat, SWS_LANCZOS|SWS_ACCURATE_RND, NULL, NULL, NULL);

		m_convert=true;
	}

	SharedMediaFormat sourceFormat(new MediaFormat());

	sourceFormat->addAttribute("mime", "video/raw");
	sourceFormat->addAttribute("width", m_outputInfo.width);
	sourceFormat->addAttribute("height", m_outputInfo.height);
	sourceFormat->addAttribute("format", m_outputInfo.format);

	if(sourcePads.empty())
		addSourcePad("Source", sourceFormat);
	else
	{
		for(SharedMediaPad &sourcePad:sourcePads)
		{
			sourcePad->setFormat(*sourceFormat.get());
		}
	}
}
