#include "MediaPipeline/MediaPipeline.h"
#include <boost/thread.hpp>
#include <boost/format.hpp>
#include "Base/PluginFactory.h"
#include "Media/MediaSampleFactory.h"

#ifdef WIN32
#define WINDOWS_LEAN_AND_MEAN
#include "windows.h"
#endif

using namespace Limitless;

MediaPipeline::MediaPipeline(std::string instance, SharedMediaFilter parent):
MediaFilterContainer(instance, parent),
m_running(false),
m_streamPauseTime(0),
m_framesInFlight(0),
m_sampleBinSeq(0)
{
	m_sampleBinIdexMask=std::numeric_limits<size_t>::max();
	m_sampleBinIdexMask<<=16;
	m_sampleBinIdexMask=~m_sampleBinIdexMask;
//	for(int i=0; i<10; ++i)
//		m_sourceSamples.push_back(new MediaSample());

    m_eventSampleId=Limitless::MediaSampleFactory::getTypeId("EventSample");
    m_eventSampleBin=allocSampleBin(this);
}

MediaPipeline::~MediaPipeline()
{
	for(int i=0; i<m_sampleBins.size(); ++i)
		delete m_sampleBins[i];
}

IMediaFilter::StateChange MediaPipeline::onPaused()
{
	StateChange statChange=MediaFilterContainer::onPaused();

	if(statChange == SUCCESS)
	{
		m_streamPauseTime=getStreamTime()-m_streamStartTime;
//		if(getState() == IMediaFilter::PLAYING)
//		{
			m_running=false;
//			m_thread.interrupt();
			stopAllSourceThreads();
			return SUCCESS;
//		}
	}
	return FAILED;
}

IMediaFilter::StateChange MediaPipeline::onPlaying()
{
	StateChange statChange=MediaFilterContainer::onPlaying();

	m_running=true;

	m_streamStartTime=getTime()-m_streamPauseTime;
	startSourceFilters();

	return statChange;
}

bool MediaPipeline::onAddMediaFilter(SharedMediaFilter mediaFilter)
{
	if(mediaFilter->type() == Source)
	{
		SharedSourceFilterDetail detail(new SourceFilterDetail(mediaFilter));

		m_sourceFilterDetails.insert(SourceFilterDetailMap::value_type(mediaFilter.get(), detail));

		for(auto &pad:mediaFilter->getSourcePads())
		{
			SharedSourcePadDetail padDetail(new SourcePadDetail(mediaFilter, pad));

			detail->padDetails.push_back(padDetail);
		}

		if(m_running)//start filter if already running
			startSourceFilters();
	}
	return true;
}

bool MediaPipeline::onRemoveMediaFilter(SharedMediaFilter mediaFilter)
{
	if(mediaFilter->type() == Source)
	{
		SourceFilterDetailMap::iterator iter=m_sourceFilterDetails.find(mediaFilter.get());

		if(iter == m_sourceFilterDetails.end())
			return false;

		SharedSourceFilterDetail &filterDetail=iter->second;

		for(auto &padDetail:filterDetail->padDetails)
		{
			if((m_running) && (padDetail->thread.joinable()))
			{
				padDetail->threadStop=true;
				padDetail->thread.join();
			}
		}
	}
	return true;
}

void MediaPipeline::onLinked(SharedMediaPad pad, SharedMediaPad filterPad)
{
	if(m_running)
	{
		SharedMediaFilter mediaFilter=pad->parent();

		if(!mediaFilter)
			return;

		if(mediaFilter->type() != Source)
			return;
	
		//pad linkage likely needs thread started if we are running
		startSourceFilters();
	}
}

void MediaPipeline::onDisconnected(SharedMediaPad pad, SharedMediaPad filterPad)
{
	if(m_running)
	{
		SharedMediaFilter mediaFilter=pad->parent();

		if(!mediaFilter)
			return;

		if(mediaFilter->type() != Source)
			return;

		//pad disconnect likely needs thread stopped
		stopDisconnectedPads();
	}
}

size_t MediaPipeline::allocSampleBin(IMediaFilter *filter)
{
	std::unique_lock<std::mutex> lock(m_sampleBinMutex);

	for(size_t i=0; i<m_sampleBins.size(); ++i)
	{
		SampleBin &sampleBin=*m_sampleBins[i];

		if(!sampleBin.inUse)
		{
			sampleBin.inUse=true;
			sampleBin.filter=filter;
			sampleBin.seqId=m_sampleBinSeq++;
			return (sampleBin.seqId<<16)|(m_sampleBinIdexMask&(i+1));//reserve index 0 for invalid
		}
	}

	size_t index=m_sampleBins.size();

	m_sampleBins.push_back(new SampleBin());
	SampleBin &sampleBin=*m_sampleBins[index];

	sampleBin.inUse=true;
	sampleBin.filter=filter;
	sampleBin.seqId=m_sampleBinSeq++;
	return (sampleBin.seqId<<16)|(m_sampleBinIdexMask&(index+1));//reserve index 0 for invalid
}

void MediaPipeline::releaseSampleBin(size_t bin, IMediaFilter *filter)
{
	std::unique_lock<std::mutex> lock(m_sampleBinMutex);

	size_t binIndex=(bin&m_sampleBinIdexMask)-1;//reserve index 0 for invalid
	size_t seqId=bin>>16;

	if(binIndex >= m_sampleBins.size())
	{
		assert(false);
		return;
	}

	SampleBin &sampleBin=*m_sampleBins[binIndex];

	if(sampleBin.filter != filter)
	{
		assert(false);
		return;
	}

	//free samples left
	for(size_t i=0; i<sampleBin.freeSamples.size(); ++i)
		delete sampleBin.freeSamples[i];
	sampleBin.freeSamples.clear();
	sampleBin.inUse=false;
	sampleBin.filter=nullptr;
}

void MediaPipeline::startSourceFilters()
{
	for(auto &iter:m_sourceFilterDetails)
	{
		SharedSourceFilterDetail &detail=iter.second;

		for(auto &padDetail:detail->padDetails)
		{
			SharedMediaPad &pad=padDetail->mediaPad;

			if(pad->linked()) //only start if linked
			{
				if(!padDetail->threadRunning)
				{
					padDetail->threadStop=false;
					padDetail->threadRunning=true;
					padDetail->thread=boost::thread(boost::bind(&MediaPipeline::processThread, this, padDetail));
				}
			}
		}
	}
}

void MediaPipeline::stopDisconnectedPads()
{
	for(auto &iter:m_sourceFilterDetails)
	{
		SharedSourceFilterDetail &detail=iter.second;

		for(auto &padDetail:detail->padDetails)
		{
			SharedMediaPad &pad=padDetail->mediaPad;

			if(padDetail->threadRunning)
			{
				if(!pad->linked())//stop if no longer linked
				{
					padDetail->threadStop=true;
					padDetail->thread.join();
				}
			}
		}
	}
}

void MediaPipeline::stopAllSourceThreads()
{
	//set stop flag
	for(auto &sourceThread:m_sourceFilterDetails)
	{
		SharedSourceFilterDetail &filterDetail=sourceThread.second;

		for(auto &padThread:filterDetail->padDetails)
			padThread->threadStop=true;
	}

	//wait for each to stop
	for(auto &sourceThread:m_sourceFilterDetails)
	{
		SharedSourceFilterDetail &filterDetail=sourceThread.second;
	
		for(auto &padThread:filterDetail->padDetails)
		{
			if(padThread->threadRunning)
				padThread->thread.join();
		}
	}
}

void MediaPipeline::processThread(SharedSourcePadDetail sourceDetail)
{
//	IMediaSource *source=dynamic_cast<IMediaSource *>(m_source->getMediaFilter().get());
//	SharedMediaSourcePad sourcePad=mediaSourcePad();
//	
////	m_sourceSamples.pop_front();
//	if(sourcePad != SharedMediaSourcePad())
//	{
//		while(m_running)
//		{
//			MediaFilterContainer::processSample(SharedMediaPad(), SharedMediaSample());
//		}
//	}
	sourceDetail->threadRunning=true;
#ifdef WIN32
	SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_HIGHEST);
#endif

	SharedMediaFilter sourceMediaFilter=sourceDetail->filter;

	if(sourceMediaFilter)
	{
		while(!sourceDetail->threadStop)
		{
			sourceMediaFilter->processSample(sourceDetail->mediaPad, SharedMediaSample());
		}
	}
	sourceDetail->threadRunning=false;
}

//bool MediaPipeline::processSample(SharedMediaPad sinkPad, SharedMediaSample sample)
//{
//	//process eac filter then return
//	return false;
//}

//bool MediaPipeline::sampleCallback(MediaSample *pushSample, MediaSample **newSample)
//{
//	MediaSample *sample=m_sourceSamples.front();
//		
//	m_sourceSamples.pop_front();
//	
//	return true;
//}

SharedMediaSample MediaPipeline::newSample(unsigned int type, size_t bin)
{
    //Hack,if type is eventSample get out of the same bin
    if(type==m_eventSampleId)
        bin=m_eventSampleBin;

    size_t binIndex=(bin&m_sampleBinIdexMask)-1;
	MediaSample *freeSample=nullptr;
	bool bufferFull=false;

    if(binIndex < m_sampleBins.size())
	{
		SampleBin *sampleBin;

		{
			std::unique_lock<std::mutex> lock(m_sampleBinMutex);

			sampleBin=m_sampleBins[binIndex];
		}
		
		{
			std::unique_lock<std::mutex> lock(sampleBin->mutex);

			while(freeSample==nullptr)
			{
				if(!sampleBin->freeSamples.empty())
				{
					freeSample=sampleBin->freeSamples.back();
					sampleBin->freeSamples.pop_back();
					break;
				}
				else
				{
					if(sampleBin->samplesInUse>=sampleBin->maxBuffer)
					{
//						bufferFull=true;
						sampleBin->event.wait(lock);
					}
					else
					{
						sampleBin->samplesInUse++; //going to be creating a new sample so record it now while under mutex
						break;
					}
				}
			}
		}
	}
	else
	{//using bin that is not allocated
		assert(false);
	}

	SharedMediaSample sample;

	if(freeSample != nullptr)
	{
		sample.reset(freeSample, std::bind(&MediaPipeline::deleteSample, this, std::placeholders::_1));
	}
	else
	{
		if(bufferFull)
			return sample;

		sample=MediaSampleFactory::createType(type, std::bind(&MediaPipeline::deleteSample, this, std::placeholders::_1));

		if(!sample)
		{
			assert(false);
			return sample;//failed to get sample info
		}
//		OutputDebugStringA((boost::format("Sample (0x%08x:%02d) - New\n")%sample.get()%sample.use_count()).str().c_str());
		sample->setAllocBin(bin);
	}

//	OutputDebugStringA((boost::format("0x%08x(0x%08x) - newSample %u\n")%this%GetCurrentThreadId()%sample->uniqueId()).str().c_str());
//	OutputDebugStringA((boost::format("NewSample %u\n")%sample.get()).str().c_str());

	sample->setSequenceNumber(0);
//	if(sample->flags())
//		sample->setFlags(0x00);
//	sample->addFlags(0x02);
	return sample;
}

//void MediaPipeline::deleteSample(SharedMediaSample sample)
void MediaPipeline::deleteSample(MediaSample *sample)
{
	size_t binIndex=(sample->allocBin()&m_sampleBinIdexMask)-1;
	size_t seqId=sample->allocBin()>>16;

	if(binIndex >= m_sampleBins.size())
	{
		//unknown sample bin
		delete sample;
		assert(false);
		return;
	}

	SampleBin *sampleBin;

	{
		std::unique_lock<std::mutex> lock(m_sampleBinMutex);

		sampleBin=m_sampleBins[binIndex];
	}

	if(sampleBin->seqId == seqId)
	{
		{
			std::unique_lock<std::mutex> lock(sampleBin->mutex);

//		OutputDebugStringA((boost::format("return sample 0x%08x\n")%sample).str().c_str());
//		OutputDebugStringA((boost::format("****Sample returned 0x%08x, last filter %s\n")%sample%sample->lastFilter()).str().c_str());
//		if(!sample->copied())
//			OutputDebugStringA((boost::format("****Sample returned without copy, last filter %s\n")%sample->lastFilter()).str().c_str());

			sampleBin->freeSamples.push_back(sample);
		}
		sampleBin->event.notify_all();
	}
	else //sample from a bin that has been released, just delete sample
	{
		delete sample;
	}

//	boost::unique_lock<boost::mutex> lock(m_freeStackMutex);
//	
////	OutputDebugStringA((boost::format("Sample (0x%08x:-) - Delete\n")%sample).str().c_str());
////	MediaSampleStack &freeStack=m_freeSamples[sample->type()];
//	MediaSampleStack &freeStack=m_freeSamples[sample->allocBin()];
////	sample->setFlags(0x00);
//	freeStack.push_back(sample);
//	m_framesInFlight--;//returning frame

//	OutputDebugStringA((boost::format("0x%08x(0x%08x) - deleteSample %u\n")%this%GetCurrentThreadId()%sample->uniqueId()).str().c_str());
}

MediaTime MediaPipeline::getStreamTime()
{
	return getTime()-m_streamStartTime;
}