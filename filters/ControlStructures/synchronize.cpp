#include "synchronize.h"
#include "Media/MediaPad.h"
#include "Media/MediaSampleFactory.h"
#include "Media/IImageSampleSet.h"
#include "Media/EventSample.h"

#include <boost/foreach.hpp>
#include <boost/format.hpp>

#define WINDOWS_LEAN_AND_MEAN
#include <windows.h>

#include <limits>

using namespace Limitless;

Synchronize::Synchronize(std::string name, SharedMediaFilter parent):
MediaAutoRegister(name, parent),
m_sinkCount(0),
m_firstSample(true),
m_captureIndex(0),
m_lastSyncTime(0),
m_captureImages(false)
{
	m_bufferLength=10;
	addAttribute("bufferLength", 10);

	std::vector<std::string> syncSources;

	syncSources.push_back("Stream");
	syncSources.push_back("Source");
	
	m_syncSource=Stream;
	addAttribute("syncSource", std::string("Stream"), syncSources);
}

Synchronize::~Synchronize()
{

}

bool Synchronize::initialize(const Attributes &attributes)
{
    m_imageSampleId=MediaSampleFactory::getTypeId("ImageSample");
	m_imageSetSampleId=MediaSampleFactory::getTypeId("ImageSampleSet");
    m_eventSampleId=MediaSampleFactory::getTypeId("EventSample");

	addSinkPad((boost::format("Sink%d")%m_sinkCount).str(), "[{\"mime\":\"video/raw\"}, {\"mime\":\"image/raw\"}]");
	addSourcePad("Source", "[{\"mime\":\"video/raw\"}, {\"mime\":\"image/raw\"}]");
	++m_sinkCount;

	return true;
}

SharedPluginView Synchronize::getView()
{
	return SharedPluginView();
}

bool Synchronize::processSample(SharedMediaPad sinkPad, SharedMediaSample sample)
{
	if(m_firstSample)
	{
		m_firstSample=false;
	}

    size_t padIndex=sinkPadIndex(sinkPad);

    if(padIndex!=IMediaFilter::InvalidPin)
    {
        std::unique_lock<std::mutex> lock(m_samplesMutex);

 //       if(padIndex < m_filterBuffers.size())
        {
//            size_t maxBufferSamples=sinkPads()*m_bufferLength;
//
//            while((m_filterBuffers[padIndex]>=m_bufferLength)||(m_samples.size() > maxBufferSamples))
//                m_sampleProcessedEvent.wait(lock);
            while(m_samples.size() > m_bufferLength)
                m_sampleProcessedEvent.wait(lock);
        }

        SampleInfo sampleInfo(padIndex, sample);

        m_samples.push_back(sampleInfo);
        m_sampleEvent.notify_all();
    }

//    if(sample->isType(m_imageSampleId))
//    {
//        Limitless::SharedIImageSample imageSample=boost::dynamic_pointer_cast<Limitless::IImageSample>(sample);
//
//        if(imageSample!=Limitless::SharedIImageSample())
//        {
//            size_t padIndex=sinkPadIndex(sinkPad);
//
//            if(padIndex!=IMediaFilter::InvalidPin)
//            {
//                std::unique_lock<std::mutex> lock(m_samplesMutex);
//
//                if(padIndex < m_filterBuffers.size())
//                {
//                    size_t maxBufferSamples=sinkPads()*m_bufferLength;
//
//                    while((m_filterBuffers[padIndex]>=m_bufferLength)||(m_samples.size() > maxBufferSamples))
//                        m_sampleProcessedEvent.wait(lock);
//                }
//
//                SampleInfo sampleInfo(padIndex, imageSample);
//
//                m_samples.push_back(sampleInfo);
//                m_sampleEvent.notify_all();
//            }
//
//        }
//    }
	return true;
}

IMediaFilter::StateChange Synchronize::onReady()
{
	m_firstSample=true;
	m_lastSyncTime=0;

	if(!m_processThread.joinable())
		m_processThread=std::thread(std::bind(&Synchronize::processSourceSample, this));

	return SUCCESS;
}

IMediaFilter::StateChange Synchronize::onPaused()
{
	return SUCCESS;
}

IMediaFilter::StateChange Synchronize::onPlaying()
{
	return SUCCESS;
}

void Synchronize::onLinked(Limitless::SharedMediaPad pad, Limitless::SharedMediaPad filterPad)
{
	if(pad->type() == Limitless::MediaPad::SINK)
	{
//		size_t padIndex=sinkPadIndex(pad);
//		{
//			std::unique_lock<std::mutex> lock(m_samplesMutex);
//
//			if(m_filterBuffers.size() <= padIndex)
//			{
//				size_t currentSize=m_filterBuffers.size();
//
//				m_filterBuffers.resize(padIndex+1);
//				for(int i=currentSize; i<=padIndex; ++i)
//					m_filterBuffers[i]=0;
//			}
//		}

		addSinkPad((boost::format("Sink%d")%m_sinkCount).str(), "[{\"mime\":\"video/raw\"}, {\"mime\":\"image/raw\"}]");
	}
}

void Synchronize::onDisconnected(SharedMediaPad pad, SharedMediaPad filterPad)
{
	//TODO: need to be able to remove pads
	assert(false);
}

bool Synchronize::onAcceptMediaFormat(SharedMediaPad pad, SharedMediaFormat format)
{
	if(pad->type() == MediaPad::SINK)
	{
		return true;
//		if(m_outputFormat == SharedMediaFormat())
//			return true;
//		else
//			return m_outputFormat->match(format);
	}
	else
		return true;
	return false;
}

void Synchronize::onLinkFormatChanged(SharedMediaPad pad, SharedMediaFormat format)
{
	if(pad->type() == MediaPad::SINK)
	{
		m_outputFormat.reset(new Limitless::MediaFormat(*format));
		Limitless::SharedMediaPads sourcePads=getSourcePads();

		for(SharedMediaPad &sourcePad:sourcePads)
		{
			sourcePad->removeAllMediaFormats();
			sourcePad->addMediaFormat(m_outputFormat);
			sourcePad->setFormat(*m_outputFormat);
		}
	}

//		if(m_outputFormat == SharedMediaFormat())
//		m_outputFormat.reset(new MediaFormat(*format));

//		DEBUG_MEDIA_FORMAT(m_outputFormat);

//		if(format->exists("width"))
//			m_outputWidth=format->attribute("width")->toInt();
//		if(format->exists("height"))
//			m_outputHeight=format->attribute("width")->toInt();
//		if(format->exists("format"))
//		{
//			std::string colorFormat=format->attribute("format")->toString();
//
//			if(colorFormat == "YUV420")
//				m_outputChannels=3;
//			else if(colorFormat == "RGB")
//				m_outputChannels=3;
//			else if(colorFormat == "RGBA")
//				m_outputChannels=4;
//		}

//		SharedMediaPads sourcePads=getSourcePads();
//
//		size_t padIndex=sinkPadIndex(pad);
//
//		if(padIndex != IMediaFilter::InvalidPin)
//		{
//			if(padIndex < sourcePads.size())
//				sourcePads[padIndex]->setFormat(*format);
//		}
//		BOOST_FOREACH(SharedMediaPad &sourcePad, sourcePads)
//		{
//			sourcePad->removeAllMediaFormats();
//			sourcePad->addMediaFormat(m_outputFormat);
//			sourcePad->setFormat(*m_outputFormat);
//		}

//		addSinkPad((boost::format("Sink%d")%m_sinkCount).str(), "[{\"mime\":\"video/raw\"}, {\"mime\":\"image/raw\"}]");
//	}
}

void Synchronize::processSourceSample()
{
    while(true)
    {
        bool synced=syncSamples();

        copySamples(synced);
        
    }
}

void Synchronize::copySamples(bool synced)
{
    std::unique_lock<std::mutex> lock(m_samplesMutex);

    if((!synced) && m_samples.empty())
        m_sampleEvent.wait(lock);

    while(!m_samples.empty())
    {
        SampleInfo sampleInfo=m_samples.front();

        m_samples.pop_front();

        if(sampleInfo.index >= m_sinks.size()) //new sink added
            m_sinks.resize(sampleInfo.index+1);

        lock.unlock(); //done with the dangerous stuff

        SinkInfo &sink=m_sinks[sampleInfo.index];

        sink.samples.push_back(sampleInfo.sample);
        m_sampleProcessedEvent.notify_all();

        lock.lock();
    }
}

void Synchronize::handleEvent(Limitless::SharedMediaSample sample)
{
    Limitless::SharedEventSample eventSample=boost::dynamic_pointer_cast<Limitless::EventSample>(sample);

    if(!eventSample)
        return;

    if(eventSample->getEvent()==Limitless::Event::EndOf)
    {
        //flush pipeline
        pushSample(sample);
    }
}

bool Synchronize::syncSamples()
{
//    if(m_eventSampleId)
    bool trySync=true;
    MediaTime lowestTime=InvalidMediaTime;
    MediaTime nextTime=InvalidMediaTime;

    if(!m_sinks.empty())
    {
        for(SinkInfo &sink:m_sinks)
        {
            if(sink.samples.empty())
            {
                trySync=false;
                continue;
            }

            Limitless::SharedMediaSample sample=sink.samples.front();

            if(sample->isType(m_eventSampleId))
            {
                sink.samples.pop_front();
                handleEvent(sample);
                break;
            }

            if(!trySync)
                continue;

            if(m_syncSource==Stream)
            {
                if(sample->timestamp()<lowestTime)
                    lowestTime=sample->timestamp();
            }
            else
            {
                if(sample->sourceTimestamp()<lowestTime)
                    lowestTime=sample->sourceTimestamp();

                if(sample->sourceTimestamp()>m_lastSyncTime)
                {
                    if(sample->sourceTimestamp()<nextTime)
                        nextTime=sample->sourceTimestamp();
                }
            }
        }
    }
    else
        trySync=false;

    if(trySync)
    {
        //get sample set if we don't have one
        if(!m_currentSampleSet)
        {
            Limitless::SharedMediaSample mediaSample=newSample(m_imageSetSampleId);

            m_currentSampleSet=boost::dynamic_pointer_cast<Limitless::IImageSampleSet>(mediaSample);
            m_currentSampleSet->clearSampleSet();
        }

        if(!m_currentSampleSet)
            return false;

        size_t imageCount=m_sinks.size();
        int syncedImages=0;
        int failedImages=0;
        std::string message;

        m_currentSampleSet->setSampleSetSize(imageCount);

//				message="Attempting sync lowest time:"+std::to_string(lowestTime)+"\n";
//				OutputDebugStringA((LPCSTR)message.c_str());

        for(size_t i=0; i<m_sinks.size(); ++i)
        {
            SinkInfo &sink=m_sinks[i];

            if(sink.samples.empty())
                continue;

            SharedMediaSample mediaSample=sink.samples.front();

//						message="Queue "+std::to_string(i)+" size:"+std::to_string(m_indexedSamples[i].size())+"\n";
//						OutputDebugStringA((LPCSTR)message.c_str());

            if(!mediaSample)
                continue;

            if(m_syncSource==Stream)
            {
                if(mediaSample->timestamp()<=lowestTime+10000) //10ms fudge TODO:fix
                {
                    sink.samples.pop_front();

                    Limitless::SharedIImageSample imageSample=boost::dynamic_pointer_cast<Limitless::IImageSample>(mediaSample);

                    m_currentSampleSet->sample(i)=imageSample;
                    syncedImages++;

//									message="Image "+std::to_string(i)+" success time:"+std::to_string(mediaSample->timestamp())+" source time:"+std::to_string(mediaSample->sourceTimestamp())+" seq:"+std::to_string(mediaSample->sequenceNumber())+"\n";
//									OutputDebugStringA((LPCSTR)message.c_str());
                }
                else
                {
//									message="Image "+std::to_string(i)+" failed time:"+std::to_string(mediaSample->timestamp())+" delta:"+std::to_string(mediaSample->timestamp()-lowestTime)+" source time:"+std::to_string(mediaSample->sourceTimestamp())+" seq:"+std::to_string(mediaSample->sequenceNumber())+"\n";
//									OutputDebugStringA((LPCSTR)message.c_str());
                    failedImages++;
                }
            }
            else
            {
                if(mediaSample->sourceTimestamp()<=lowestTime+10000) //10ms fudge TODO:fix
                {
                    sink.samples.pop_front();

                    Limitless::SharedIImageSample imageSample=boost::dynamic_pointer_cast<Limitless::IImageSample>(mediaSample);

                    m_currentSampleSet->sample(i)=imageSample;
                    syncedImages++;
                }
            }
        }

        if(syncedImages==imageCount)
        {
            m_lastSyncTime=lowestTime;

            if(m_captureImages)
            {
                m_captureImages=false;

                for(size_t i=0; i<m_currentSampleSet->sampleSetSize(); ++i)
                {
                    Limitless::SharedIImageSample imageSample=m_currentSampleSet->sample(i);
                    std::string fileName=(boost::format("%s/%d-%d.ppm")%m_captureDirectory%(i+1)%m_captureIndex).str();

                    imageSample->save(fileName);
                }
                m_captureIndex++;
            }

            if(m_currentSampleSet->sampleSetSize()>0)
                m_currentSampleSet->copyHeader(m_currentSampleSet->sample(0), instance());

            pushSample(m_currentSampleSet);
            m_currentSampleSet.reset();
        }
        else
        {
            //will dump samples if not all here
            m_currentSampleSet->clearSampleSet();
        }
        m_sampleProcessedEvent.notify_all();
        return true;
    }

    return false;
}

//void Synchronize::processSourceSample()
//{
//	while(true)
//	{
//		bool trySync=true;
//		MediaTime lowestTime=InvalidMediaTime;
//		MediaTime nextTime=InvalidMediaTime;
//
//		if(m_indexedSamples.empty())
//			trySync=false;
//		else
//		{
//			for(Limitless::SharedIImageSampleQueue &samples:m_indexedSamples)
//			{
//				if(samples.empty())
//				{
//					trySync=false;
//					break;
//				}
//				else
//				{
//					if(m_syncSource == Stream)
//					{
//						if(samples.front()->timestamp() < lowestTime)
//							lowestTime=samples.front()->timestamp();
//					}
//					else
//					{
//						if(samples.front()->sourceTimestamp() < lowestTime)
//							lowestTime=samples.front()->sourceTimestamp();
//
//						if(samples.front()->sourceTimestamp() > m_lastSyncTime)
//						{
//							if(samples.front()->sourceTimestamp() < nextTime)
//								nextTime=samples.front()->sourceTimestamp();
//						}
//					}
//				}
//			}
//		}
//
//		if(m_syncSource == Source)
//		{
//			if(lowestTime < m_lastSyncTime)
//			{
//				if(nextTime != InvalidMediaTime)
//					lowestTime=nextTime;
//			}
//		}
//
//		if(trySync)
//		{
//			if(m_currentSampleSet == Limitless::SharedIImageSampleSet())
//			{
//				Limitless::SharedMediaSample mediaSample=newSample(m_imageSetSampleId);
//				
//				m_currentSampleSet=boost::dynamic_pointer_cast<Limitless::IImageSampleSet>(mediaSample);
//				m_currentSampleSet->clearSampleSet();
//			}
//
//			if(m_currentSampleSet != Limitless::SharedIImageSampleSet())
//			{
//				int imageCount=m_indexedSamples.size();
//				int syncedImages=0;
//				int failedImages=0;
//
////				m_currentSampleSet->clearSampleSet();
//				m_currentSampleSet->setSampleSetSize(imageCount);
//
//				std::string message;
//
////				message="Attempting sync lowest time:"+std::to_string(lowestTime)+"\n";
////				OutputDebugStringA((LPCSTR)message.c_str());
//
//				for(size_t i=0; i<imageCount; ++i)
//				{
//					if(!m_indexedSamples[i].empty())
//					{
//						SharedIImageSample mediaSample=m_indexedSamples[i].front();
//
////						message="Queue "+std::to_string(i)+" size:"+std::to_string(m_indexedSamples[i].size())+"\n";
////						OutputDebugStringA((LPCSTR)message.c_str());
//
//						if(mediaSample != SharedIImageSample())
//						{
//							if(m_syncSource == Stream)
//							{
//								if(mediaSample->timestamp() <= lowestTime+10000) //10ms fudge TODO:fix
//								{
//									m_indexedSamples[i].pop_front();
//									m_filterBuffers[i]--;
//									m_currentSampleSet->sample(i)=mediaSample;
//									syncedImages++;
//
////									message="Image "+std::to_string(i)+" success time:"+std::to_string(mediaSample->timestamp())+" source time:"+std::to_string(mediaSample->sourceTimestamp())+" seq:"+std::to_string(mediaSample->sequenceNumber())+"\n";
////									OutputDebugStringA((LPCSTR)message.c_str());
//								}
//								else
//								{
////									message="Image "+std::to_string(i)+" failed time:"+std::to_string(mediaSample->timestamp())+" delta:"+std::to_string(mediaSample->timestamp()-lowestTime)+" source time:"+std::to_string(mediaSample->sourceTimestamp())+" seq:"+std::to_string(mediaSample->sequenceNumber())+"\n";
////									OutputDebugStringA((LPCSTR)message.c_str());
//									failedImages++;
//								}
//							}
//							else
//							{
//								if(mediaSample->sourceTimestamp() <= lowestTime+10000) //10ms fudge TODO:fix
//								{
//									m_indexedSamples[i].pop_front();
//									m_filterBuffers[i]--;
//									m_currentSampleSet->sample(i)=mediaSample;
//									syncedImages++;
//								}
//							}
//						}
//					}
//				}
//
//				if(syncedImages == imageCount)
//				{
//					m_lastSyncTime=lowestTime;
//
//					if(m_captureImages)
//					{
//						m_captureImages=false;
//
//						for(size_t i=0; i<m_currentSampleSet->sampleSetSize(); ++i)
//						{
//							Limitless::SharedIImageSample imageSample=m_currentSampleSet->sample(i);
//							std::string fileName=(boost::format("%s/%d-%d.ppm")%m_captureDirectory%(i+1)%m_captureIndex).str();
//
//							imageSample->save(fileName);
//						}
//						m_captureIndex++;
//					}
//					
//					if(m_currentSampleSet->sampleSetSize()>0)
//						m_currentSampleSet->copyHeader(m_currentSampleSet->sample(0), instance());
//
//					pushSample(m_currentSampleSet);
//					m_currentSampleSet.reset();
//				}
//				else
//				{
//					//will dump samples if not all here
//					m_currentSampleSet->clearSampleSet();
//				}
//				m_sampleProcessedEvent.notify_all();
//			}
//		}
//		else
//		{
//			std::unique_lock<std::mutex> lock(m_samplesMutex);
//
//			if(!m_samples.empty())
//			{
//				SampleInfo sampleInfo=m_samples.front();
//
//				m_samples.pop_front();
//
//				if(sampleInfo.index >= m_filterBuffers.size())
//					m_filterBuffers.resize(sampleInfo.index+1);
//				if(sampleInfo.index >= m_indexedSamples.size())
//					m_indexedSamples.resize(sampleInfo.index+1);
//				
//				m_filterBuffers[sampleInfo.index]++;
//
//				lock.unlock(); //done with the dangerous stuff
//
//				m_indexedSamples[sampleInfo.index].push_back(sampleInfo.sample);
//				m_sampleProcessedEvent.notify_all();
//			}
//			else
//				m_sampleEvent.wait(lock);
//		}
//	}
//}

bool Synchronize::capture(std::string directory)
{
	m_captureDirectory=directory;
	m_captureImages=true;
	return true;
}

void Synchronize::onAttributeChanged(std::string name, SharedAttribute attribute)
{
	if(name == "syncSource")
	{
		if(attribute->toString() == "Stream")
			m_syncSource=Stream;
		else
			m_syncSource=Source;
	}
	else if(name == "bufferLength")
		m_bufferLength=attribute->toInt();
}