#include "branch.h"
#include "Media/MediaPad.h"

#include <boost/foreach.hpp>
#include <boost/format.hpp>

#define WINDOWS_LEAN_AND_MEAN
#include <windows.h>

using namespace Limitless;

BranchFilter::BranchFilter(std::string name, SharedMediaFilter parent):
MediaAutoRegister(name, parent),
m_sourceCount(0),
m_lastSequence(0)
{
}

BranchFilter::~BranchFilter()
{

}

bool BranchFilter::initialize(const Attributes &attributes)
{
	addSinkPad("Sink", "[{\"mime\":\"any\"}]");
	addSourcePad((boost::format("Source%d")%m_sourceCount).str(), "[{\"mime\":\"any\"}]");
	++m_sourceCount;

	return true;
}

SharedPluginView BranchFilter::getView()
{
	return SharedPluginView();
}

bool BranchFilter::processSample(SharedMediaPad sinkPad, SharedMediaSample sample)
{
	bool notify=false;

	{
		boost::unique_lock<boost::mutex> lock(m_sampleQueueMutex);
		
		if(!m_threads.empty())
		{
			while(m_sampleQueue.size() > 4)
				m_sampleProcessEvent.wait(lock);

		//		OutputDebugStringA((boost::format("0x%08x Branch push sample %d (%d)\n")%GetCurrentThreadId()%sample->sequenceNumber()%sample->uniqueId()).str().c_str());
			m_sampleQueue.push(sample);
			notify=true;
		}
	}

	if(notify)
		m_sampleEvent.notify_all();
	return true;
}

IMediaFilter::StateChange BranchFilter::onReady()
{
	return SUCCESS;
}

IMediaFilter::StateChange BranchFilter::onPaused()
{
	return SUCCESS;
}

IMediaFilter::StateChange BranchFilter::onPlaying()
{
	return SUCCESS;
}

bool BranchFilter::onAcceptMediaFormat(SharedMediaPad pad, SharedMediaFormat format)
{
	if(pad->type() == MediaPad::SINK)
		return true; //we accept anything
	else
		return true;
	return false;
}

void BranchFilter::onLinkFormatChanged(SharedMediaPad pad, SharedMediaFormat format)
{
	if(pad->type() == MediaPad::SINK)
	{
		if(m_outputFormat == SharedMediaFormat())
			m_outputFormat.reset(new MediaFormat(*format));
		
//		DEBUG_MEDIA_FORMAT(format);
//		DEBUG_MEDIA_FORMAT(m_outputFormat);

		SharedMediaPads sourcePads=getSourcePads();

		BOOST_FOREACH(SharedMediaPad &sourcePad, sourcePads)
		{
			sourcePad->removeAllMediaFormats();
			sourcePad->addMediaFormat(m_outputFormat);
			sourcePad->setFormat(*m_outputFormat);
		}
	}
//	else if(pad->type() == MediaPad::SOURCE)
//	{
//		PadThreads::iterator iter=std::find(m_threads.begin(), m_threads.end(), pad);
//
//		if(iter == m_threads.end())
//		{
//			m_threads.push_back(PadThread(pad, boost::bind(&BranchFilter::processSourceSample, this, pad)));
//			addSourcePad(m_outputFormat);
//		}
//	}
}

void BranchFilter::onLinked(SharedMediaPad pad, SharedMediaPad filterPad)
{
	if(pad->type() == MediaPad::SOURCE)
	{
		PadThreads::iterator iter=std::find(m_threads.begin(), m_threads.end(), pad);

		if(iter == m_threads.end())
		{
			m_threads.push_back(PadThread(pad, boost::bind(&BranchFilter::processSourceSample, this, pad)));
			addSourcePad((boost::format("Source%d")%m_sourceCount).str(), m_outputFormat);
			++m_sourceCount;
		}
	}
}

void BranchFilter::processSourceSample(SharedMediaPad pad)
{
	while(true)
	{
		SharedMediaSample mediaSample;

		{
			boost::unique_lock<boost::mutex> lock(m_sampleQueueMutex);
		
			if(m_sampleQueue.empty())
			{
//				OutputDebugStringA("Wait\n");
				m_sampleEvent.wait(lock);
			}
		
//			OutputDebugStringA("Wait complete\n");
			if(m_sampleQueue.empty())
				continue;

//			OutputDebugStringA("Getting Sample\n");
			mediaSample=m_sampleQueue.front();
			m_sampleQueue.pop();
		}
		
//		OutputDebugStringA("Lock released\n");
		
		m_sampleProcessEvent.notify_one();

		unsigned int sequence=mediaSample->sequenceNumber();

		if(sequence < m_lastSequence)
			m_lastSequence=sequence;
		m_lastSequence=sequence;

//		OutputDebugStringA((boost::format("0x%08x(0x%08x) Branch processing sample %u(%u)\n")%this%GetCurrentThreadId()%sequence%mediaSample->uniqueId()).str().c_str());

		if(pad->linked())
		{
//			OutputDebugStringA("Processing Sample\n");
			pad->linkedPad()->processSample(mediaSample);
//			OutputDebugStringA("Processing Sample complete\n");
		}
	}	
}