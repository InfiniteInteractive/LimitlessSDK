#include "combine.h"
#include "Media/MediaPad.h"
#include "Media/MediaSampleFactory.h"
#include "Media/ImageSampleSet.h"

#include <boost/foreach.hpp>
#include <boost/format.hpp>

#define WINDOWS_LEAN_AND_MEAN
#include <windows.h>

using namespace Limitless;

CombineFilter::CombineFilter(std::string name, SharedMediaFilter parent):
MediaAutoRegister(name, parent),
m_sinkCount(0),
m_firstSample(true)
{

}

CombineFilter::~CombineFilter()
{

}

bool CombineFilter::initialize(const Attributes &attributes)
{
	m_imageSetSampleId=MediaSampleFactory::getTypeId("ImageSetSample");

	addSinkPad((boost::format("Sink%d")%m_sinkCount).str(), "[{\"mime\":\"video/raw\"}, {\"mime\":\"image/raw\"}]");
	addSourcePad("Source", "[{\"mime\":\"video/raw\"}, {\"mime\":\"image/raw\"}]");
	++m_sinkCount;

	return true;
}

SharedPluginView CombineFilter::getView()
{
	return SharedPluginView();
}

bool CombineFilter::processSample(SharedMediaPad sinkPad, SharedMediaSample sample)
{
	if(m_firstSample)
	{
		m_firstSample=false;
	}

	{
		std::unique_lock<std::mutex> lock(m_samplesMutex);
		size_t padIndex=sinkPadIndex(sinkPad);

		if(padIndex != IMediaFilter::InvalidPin)
		{
			if(padIndex > m_samples.size())
				m_samples.resize(padIndex);

			m_samples[padIndex].push_back(sample);
		}
		
	}
	return true;
}

IMediaFilter::StateChange CombineFilter::onReady()
{
	m_firstSample=true;

	if(!m_processThread.joinable())
		m_processThread=std::thread(std::bind(&CombineFilter::processSourceSample, this));

	return SUCCESS;
}

IMediaFilter::StateChange CombineFilter::onPaused()
{
	return SUCCESS;
}

IMediaFilter::StateChange CombineFilter::onPlaying()
{
	return SUCCESS;
}

bool CombineFilter::onAcceptMediaFormat(SharedMediaPad pad, SharedMediaFormat format)
{
	if(pad->type() == MediaPad::SINK)
	{
		if(m_outputFormat == SharedMediaFormat())
			return true;
		else
			return m_outputFormat->match(format);
	}
	else
		return true;
	return false;
}

void CombineFilter::onLinkFormatChanged(SharedMediaPad pad, SharedMediaFormat format)
{
	if(pad->type() == MediaPad::SINK)
	{
//		if(m_outputFormat == SharedMediaFormat())
		m_outputFormat.reset(new MediaFormat(*format));

		DEBUG_MEDIA_FORMAT(format);
		DEBUG_MEDIA_FORMAT(m_outputFormat);

		if(format->exists("width"))
			m_outputWidth=format->attribute("width")->toInt();
		if(format->exists("height"))
			m_outputHeight=format->attribute("width")->toInt();
		if(format->exists("format"))
		{
			std::string colorFormat=format->attribute("format")->toString();

			if(colorFormat == "YUV420")
				m_outputChannels=3;
			else if(colorFormat == "RGB")
				m_outputChannels=3;
			else if(colorFormat == "RGBA")
				m_outputChannels=4;
		}

		SharedMediaPads sourcePads=getSourcePads();


		BOOST_FOREACH(SharedMediaPad &sourcePad, sourcePads)
		{
			sourcePad->removeAllMediaFormats();
			sourcePad->addMediaFormat(m_outputFormat);
			sourcePad->setFormat(*m_outputFormat);
		}

		addSinkPad((boost::format("Sink%d")%m_sinkCount).str(), "[{\"mime\":\"video/raw\"}, {\"mime\":\"image/raw\"}]");
	}
}


void CombineFilter::processSourceSample()
{
	std::unique_lock<std::mutex> lock(m_samplesMutex);
	
	while(true)
	{
		bool trySync=true;

		for(Limitless::SharedMediaSampleQueue &samples:m_samples)
		{
			if(samples.empty())
			{
				trySync=false;
				break;
			}
		}

		if(trySync)
		{
			SharedMediaSample mediaSample=newSample(m_imageSetSampleId);
			SharedImageSampleSet imageSetSample=boost::dynamic_pointer_cast<ImageSampleSet>(mediaSample);

			if(imageSetSample != SharedImageSampleSet())
			{
				int imageCount=imageSetSample->sampleSetSize();

//				imageSetSample->resize(imageCount, m_outputWidth, m_outputHeight, m_outputChannels);
				imageSetSample->clearSampleSet();

				for(size_t i=0; i<imageCount; ++i)
				{
					SharedImageSample &mediaSample=boost::dynamic_pointer_cast<ImageSample>(m_samples[i].front());

					if(mediaSample != SharedImageSample())
					{
						m_samples[i].pop_front();
						imageSetSample->addSample(mediaSample);
					}
				}

				m_samplesMutex.unlock();
				pushSample(imageSetSample);
				m_samplesMutex.lock();
				m_sampleEvent.notify_one();
			}
		}
		else
			m_sampleEvent.wait(lock);
	}
}