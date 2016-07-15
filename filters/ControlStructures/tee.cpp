#include "tee.h"
#include "Media/MediaPad.h"

#include <boost/foreach.hpp>
#include <boost/format.hpp>

#define WINDOWS_LEAN_AND_MEAN
#include <windows.h>

using namespace Limitless;

TeeFilter::TeeFilter(std::string name, SharedMediaFilter parent):
MediaAutoRegister(name, parent),
m_sourceCount(0),
m_lastSequence(0)
{
}

TeeFilter::~TeeFilter()
{

}

bool TeeFilter::initialize(const Attributes &attributes)
{
	addSinkPad("Sink", "[{\"mime\":\"any\"}]");
	addSourcePad((boost::format("Source%d")%m_sourceCount).str(), "[{\"mime\":\"any\"}]");
	++m_sourceCount;

	return true;
}

SharedPluginView TeeFilter::getView()
{
	return SharedPluginView();
}

bool TeeFilter::processSample(SharedMediaPad sinkPad, SharedMediaSample sample)
{
	{
		boost::unique_lock<boost::mutex> lock(m_sampleQueueMutex);
		
		for(size_t i=0; i<m_threads.size(); ++i)
		{
			std::queue<Limitless::SharedMediaSample> &sampleQueue=m_threads[i]->sampleQueue;

			while(sampleQueue.size() > 4)
				m_sampleProcessEvent.wait(lock);

//			OutputDebugStringA((boost::format("0x%08x(0x%08x) Tee add sample %u to index %d\n")%this%GetCurrentThreadId()%sample->uniqueId()%i).str().c_str());

			sampleQueue.push(sample);
		}
	}
	m_sampleEvent.notify_all();
	return true;
}

IMediaFilter::StateChange TeeFilter::onReady()
{
	return SUCCESS;
}

IMediaFilter::StateChange TeeFilter::onPaused()
{
	return SUCCESS;
}

IMediaFilter::StateChange TeeFilter::onPlaying()
{
	return SUCCESS;
}

bool TeeFilter::onAcceptMediaFormat(SharedMediaPad pad, SharedMediaFormat format)
{
	if(pad->type() == MediaPad::SINK)
		return true; //we accept anything
	else
		return true;
	return false;
}

void TeeFilter::onLinkFormatChanged(SharedMediaPad pad, SharedMediaFormat format)
{
	if(pad->type() == MediaPad::SINK)
	{
		m_outputFormat.reset(new MediaFormat(*format));
		
		SharedMediaPads sourcePads=getSourcePads();

		BOOST_FOREACH(SharedMediaPad &sourcePad, sourcePads)
		{
			sourcePad->removeAllMediaFormats();
			sourcePad->addMediaFormat(m_outputFormat);
			sourcePad->setFormat(*m_outputFormat);
		}
	}
}

void TeeFilter::onLinked(SharedMediaPad pad, SharedMediaPad filterPad)
{
	if(pad->type() == MediaPad::SOURCE)
	{
		boost::unique_lock<boost::mutex> lock(m_sampleQueueMutex);

		SharedPadThreads::iterator iter=std::find_if(m_threads.begin(), m_threads.end(), [&pad](SharedPadThread const&item){return item->pad == pad;});

		if(iter == m_threads.end())
		{
//			m_threads.push_back(PadThread(pad, boost::bind(&TeeFilter::processSourceSample, this, pad)));
			SharedPadThread padThread(new PadThread(pad));

			m_threads.push_back(padThread);
			padThread->startThread(boost::bind(&TeeFilter::processSourceSample, this, pad));

			SharedMediaPad mediaPad=addSourcePad((boost::format("Source%d")%m_sourceCount).str(), m_outputFormat);

			mediaPad->setFormat(*m_outputFormat);
			++m_sourceCount;
		}
	}
}

void TeeFilter::processSourceSample(SharedMediaPad pad)
{
	m_sampleQueueMutex.lock();
	
//	PadThreads::iterator iter=std::find(m_threads.begin(), m_threads.end(), pad);
//
//	if(iter == m_threads.end())
//		assert(false);

	bool found=false;
	size_t threadIndex;

	for(size_t i=0; i<m_threads.size(); ++i)
	{
		if(m_threads[i]->pad == pad)
		{
			found=true;
			threadIndex=i;
		}
	}

	assert(found);

//	std::queue<Limitless::SharedMediaSample> &sampleQueue=iter->sampleQueue;
	std::queue<Limitless::SharedMediaSample> *sampleQueue=&m_threads[threadIndex]->sampleQueue;

//	OutputDebugStringA((boost::format("0x%08x(0x%08x) Tee set sampleQueue 0x%08x index %d\n")%this%GetCurrentThreadId()%sampleQueue%threadIndex).str().c_str());

	m_sampleQueueMutex.unlock();

	while(true)
	{
		SharedMediaSample mediaSample;

		{
			boost::unique_lock<boost::mutex> lock(m_sampleQueueMutex);
		
			if(sampleQueue->empty())
			{
//				OutputDebugStringA("Wait\n");
				m_sampleEvent.wait(lock);
			}
		
//			OutputDebugStringA("Wait complete\n");
			if(sampleQueue->empty())
				continue;

//			OutputDebugStringA("Getting Sample\n");
			mediaSample=sampleQueue->front();
			sampleQueue->pop();
		}
		
//		OutputDebugStringA("Lock released\n");
		
		m_sampleProcessEvent.notify_one();

//		unsigned int sequence=mediaSample->sequenceNumber();
//
//		if(sequence < m_lastSequence)
//			m_lastSequence=sequence;
//		m_lastSequence=sequence;

//		OutputDebugStringA((boost::format("0x%08x(0x%08x) Branch processing sample %u(%u)\n")%this%GetCurrentThreadId()%sequence%mediaSample->uniqueId()).str().c_str());
//		OutputDebugStringA((boost::format("0x%08x(0x%08x) Tee processing sample %u index %d\n")%this%GetCurrentThreadId()%mediaSample->uniqueId()%threadIndex).str().c_str());

		if(pad->linked())
		{
			pushSample(pad, mediaSample);
//			OutputDebugStringA("Processing Sample\n");
//			pad->linkedPad()->processSample(mediaSample);
//			OutputDebugStringA("Processing Sample complete\n");
		}
	}	
}