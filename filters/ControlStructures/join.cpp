#include "join.h"
#include "Media/MediaPad.h"

#include <boost/foreach.hpp>
#include <boost/format.hpp>

#define WINDOWS_LEAN_AND_MEAN
#include <windows.h>

using namespace Limitless;

JoinFilter::JoinFilter(std::string name, SharedMediaFilter parent):
MediaAutoRegister(name, parent),
m_sinkCount(0),
m_firstSample(true),
m_currentSequence(0)
{
}

JoinFilter::~JoinFilter()
{

}

bool JoinFilter::initialize(const Attributes &attributes)
{
	addSinkPad((boost::format("Sink%d")%m_sinkCount).str(), "[{\"mime\":\"video/raw\"}, {\"mime\":\"image/raw\"}]");
	addSourcePad("Source", "[{\"mime\":\"video/raw\"}, {\"mime\":\"image/raw\"}]");
	++m_sinkCount;

	return true;
}

SharedPluginView JoinFilter::getView()
{
	return SharedPluginView();
}

bool JoinFilter::processSample(SharedMediaPad sinkPad, SharedMediaSample sample)
{
	if(m_firstSample)
	{
		m_currentSequence=sample->sequenceNumber();
		m_firstSample=false;
	}

	{
		boost::unique_lock<boost::mutex> lock(m_samplesMutex);

//		OutputDebugStringA((boost::format("0x%08x Join received sample %d (%d)\n")%GetCurrentThreadId()%sample->sequenceNumber()%sample->uniqueId()).str().c_str());
		m_samples.push_back(sample);
	}

	m_sampleEvent.notify_one();
//	pushSample(sample);
	return true;
}

IMediaFilter::StateChange JoinFilter::onReady()
{
	m_firstSample=true;
	m_currentSequence=0;
	return SUCCESS;
}

IMediaFilter::StateChange JoinFilter::onPaused()
{
	return SUCCESS;
}

IMediaFilter::StateChange JoinFilter::onPlaying()
{
	return SUCCESS;
}

bool JoinFilter::onAcceptMediaFormat(SharedMediaPad pad, SharedMediaFormat format)
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

void JoinFilter::onLinkFormatChanged(SharedMediaPad pad, SharedMediaFormat format)
{
	if(pad->type() == MediaPad::SINK)
	{
//		if(m_outputFormat == SharedMediaFormat())
		m_outputFormat.reset(new MediaFormat(*format));

		DEBUG_MEDIA_FORMAT(format);
		DEBUG_MEDIA_FORMAT(m_outputFormat);

		SharedMediaPads sourcePads=getSourcePads();

		BOOST_FOREACH(SharedMediaPad &sourcePad, sourcePads)
		{
			sourcePad->removeAllMediaFormats();
			sourcePad->addMediaFormat(m_outputFormat);
			sourcePad->setFormat(*m_outputFormat);
		}

		if(m_processThread == boost::thread())
			m_processThread=boost::thread(boost::bind(&JoinFilter::processSourceSample, this));

		addSinkPad((boost::format("Sink%d")%m_sinkCount).str(), "[{\"mime\":\"video/raw\"}, {\"mime\":\"image/raw\"}]");
	}
}


void JoinFilter::processSourceSample()
{
	boost::unique_lock<boost::mutex> lock(m_samplesMutex);

	while(true)
	{
		SharedMediaSample mediaSample;
		bool found=false;

//		BOOST_FOREACH(SharedMediaSample &sample, m_samples)
		for(SharedMediaSamples::iterator iter=m_samples.begin(); iter != m_samples.end(); ++iter)
		{
			if((*iter)->sequenceNumber() == m_currentSequence)
			{
				mediaSample=(*iter);
				m_samples.erase(iter);
				found=true;
				break;
			}
		}

		if(found)
		{
			lock.unlock();
//			OutputDebugStringA((boost::format("0x%08x(0x%08x) Join process sample %u(%u)\n")%this%GetCurrentThreadId()%mediaSample->sequenceNumber()%mediaSample->uniqueId()).str().c_str());
			pushSample(mediaSample);
			lock.lock();
			m_currentSequence++;
		}
		else
			m_sampleEvent.wait(lock);
	}	
}