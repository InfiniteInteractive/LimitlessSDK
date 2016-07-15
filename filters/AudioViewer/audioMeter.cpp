#include "imageviewer.h"
#include "Base/QtPluginView.h"
#include "Media/MediaPad.h"

#include "glView.h"

#include <boost/foreach.hpp>

using namespace Limitless;

ImageViewer::ImageViewer(std::string name, SharedMediaFilter parent):
MediaAutoRegister(name, parent),
m_glView(nullptr),
frameCount(0)
{
}

ImageViewer::~ImageViewer()
{

}

void ImageViewer::attachViewer(ImageViewer *imageViewer)
{
	GLWidget *glWidget=getGlWidget();

	glWidget->attachViewer(imageViewer->getGlWidget());
}

void ImageViewer::removeViewer(ImageViewer *imageViewer)
{
	GLWidget *glWidget=getGlWidget();

	glWidget->removeViewer(imageViewer->getGlWidget());
}

bool ImageViewer::initialize(const Attributes &attributes)
{
	m_iAudioSampleId=MediaSampleFactory::getTypeId("IAudioSample");

	addSinkPad("Sink", "[{\"mime\":\"audio/raw\"}]");
	addSourcePad("Source", "[{\"mime\":\"audio/raw\"}]");
	return true;
}

SharedPluginView ImageViewer::getView()
{
//	return SharedPluginView();

	if(m_view == SharedPluginView())
	{
		m_glView=new GlView();

		m_view.reset(new QtPluginView(m_glView));
	}
	return m_view;
}

void ImageViewer::setSample(Limitless::SharedMediaSample sample)
{
	if(m_glView != NULL)
		m_glView->displaySample(sample);
}

bool ImageViewer::processSample(SharedMediaPad sinkPad, SharedMediaSample sample)
{
//	OutputDebugStringA("Enter ImageViewer::processSample\n");
	//	sourcePad->processSample(sample);
//	frameCount++;
//	if(frameCount%10 == 0)
//	{
//		m_timeStamps.push(sample->timestamp());
//	
//		double frameRate=0;
//		if((frameCount != 0) && !m_timeStamps.empty())
//			frameRate=((double)frameCount*1000000000.0)/(m_timeStamps.back()-m_timeStamps.front());
//		m_glView->setFrameRate(frameRate);
//	}

//	if(m_timeStamps.size() > 10)
//	{
//		m_timeStamps.pop();
//		frameCount-=10;
//	}
	if(sample->isType(m_gpuImageSampleId))
	{
		SharedGpuImageSample gpuImageSample=boost::dynamic_pointer_cast<GpuImageSample>(sample);
		SharedGpuImageSample openGLSample=newSampleType<GpuImageSample>(m_gpuImageSampleId);

		cl::Event event;
		std::vector<cl::Event> waitEvents;

		openGLSample->copy(gpuImageSample.get(), event);
		waitEvents.push_back(event);

		openGLSample->releaseOpenCl(event, &waitEvents);

		event.wait();

		if(m_glView != NULL)
			m_glView->displaySample(openGLSample);
	}

	pushSample(sample);
//	OutputDebugStringA("Exit ImageViewer::processSample\n");
	return true;
}

void ImageViewer::showControls(bool show)
{
	if(show)
	{
	}
}

GLWidget *ImageViewer::getGlWidget()
{ 
	return m_glView->getGlWidget();
}

IMediaFilter::StateChange ImageViewer::onReady()
{
	return SUCCESS;
}

IMediaFilter::StateChange ImageViewer::onPaused()
{
	return SUCCESS;
}

IMediaFilter::StateChange ImageViewer::onPlaying()
{
	return SUCCESS;
}

bool ImageViewer::onAcceptMediaFormat(SharedMediaPad pad, SharedMediaFormat format)
{
	if(pad->type() == MediaPad::SINK)
	{
		bool accept=false;

		if(format->exists("mime"))
		{
			std::string mimeType=format->attribute("mime")->toString();

			if(mimeType == "video/raw")
				accept=true;
			else if(mimeType == "image/raw")
				accept=true;
			else if(mimeType == "image/gpu")
				accept=true;
		}
		return accept;
	}
	else
		return true;
	return false;
}

void ImageViewer::onLinkFormatChanged(SharedMediaPad pad, SharedMediaFormat format)
{
	if(pad->type() == MediaPad::SINK)
	{
		if(!format->exists("mime"))
			return;
		if((format->attribute("mime")->toString() != "video/raw") &&
			(format->attribute("mime")->toString() != "image/raw") && 
			(format->attribute("mime")->toString() != "image/gpu"))
			return;

		MediaFormat sourceFormat(*format);

//		sourceFormat.addAttribute("mime", format->attribute("mime")->toString());
//		if(format->exists("width"))
//			sourceFormat.addAttribute("width", format->attribute("width")->toString());
//		if(format->exists("height"))
//			sourceFormat.addAttribute("height", format->attribute("height")->toString());

		SharedMediaPads sourcePads=getSourcePads();

		BOOST_FOREACH(SharedMediaPad &sourcePad, sourcePads)
		{
			sourcePad->setFormat(sourceFormat);
		}
	}
}