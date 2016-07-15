#include "AvStreamOutput.h"
//#include "Base/QtPluginView.h"
#include "Media/MediaPad.h"
#include "Media/ImageSampleSet.h"

#include <boost/foreach.hpp>

using namespace Limitless;

AvStreamOutput::AvStreamOutput(std::string name, SharedMediaFilter parent):
MediaAutoRegister(name, parent)
{
	m_imageSampleId=MediaSampleFactory::getTypeId("ImageSample");
	m_imageSetSampleId=MediaSampleFactory::getTypeId("ImageSetSample");

	m_fmleName="Flash Media Live Encoder";
}

AvStreamOutput::~AvStreamOutput()
{

}

bool AvStreamOutput::initialize(const Attributes &attributes)
{
	addSinkPad("Sink", "[{\"mime\":\"video/raw\"}, {\"mime\":\"image/raw\"}]");
	addSourcePad("Source", "[{\"mime\":\"video/raw\"}, {\"mime\":\"image/raw\"}]");

	return true;
}

SharedPluginView AvStreamOutput::getView()
{
	return SharedPluginView();
}

bool AvStreamOutput::processSample(SharedMediaPad sinkPad, SharedMediaSample sample)
{
	unsigned int type=sample->type();

	if(type == m_imageSampleId)
	{
		SharedImageSample imageSample=boost::dynamic_pointer_cast<ImageSample>(sample);
		
		if(imageSample != SharedImageSample())
			m_streamServer.sendFrame(imageSample.get());
	}
	else if(type == m_imageSetSampleId)
	{
		SharedImageSampleSet imageSampleSet=boost::dynamic_pointer_cast<ImageSampleSet>(sample);

		if(imageSampleSet != SharedImageSampleSet())
		{
//			ImageSample *imageSample=dynamic_cast<ImageSample *>((*imageSampleSet)[0]);
			SharedImageSample imageSample=boost::dynamic_pointer_cast<ImageSample>(imageSampleSet->sample(0));

			imageSample->setSequenceNumber(sample->sequenceNumber());
			imageSample->setMediaIndex(sample->mediaIndex());
			imageSample->setTimestamp(sample->timestamp());
			m_streamServer.sendFrame(imageSample.get());
		}
	}

//	deleteSample(sample);
	pushSample(sample);
	return true;
}

IMediaFilter::StateChange AvStreamOutput::onReady()
{
	m_streamServer.open("StreamServer");
	return SUCCESS;
}

IMediaFilter::StateChange AvStreamOutput::onPaused()
{
	return SUCCESS;
}

IMediaFilter::StateChange AvStreamOutput::onPlaying()
{
	return SUCCESS;
}

bool AvStreamOutput::onAcceptMediaFormat(SharedMediaPad pad, SharedMediaFormat format)
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
		}
		return accept;
	}
	else
		return true; //accept anything on source pad
	return false;
}

void AvStreamOutput::onLinkFormatChanged(SharedMediaPad pad, SharedMediaFormat format)
{
	if(pad->type() == MediaPad::SINK)
	{
//		if(!format->exists("mime"))
//			return;
//		if((format->attribute("mime")->toString() != "video/raw") &&
//			(format->attribute("mime")->toString() != "image/raw"))
//			return;
		if(format->exists("width"))
			m_width=format->attribute("width")->toInt();
		if(format->exists("height"))
			m_height=format->attribute("height")->toInt();

		m_streamServer.setFormat(format);
	}
}