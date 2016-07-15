#include "gpuUpload.h"
#include "Media/MediaPad.h"
#include "Media/GpuImageSample.h"
#include "Media/GpuImageSampleSet.h"
#include "Media/ImageSample.h"
#include "Media/ImageSampleSet.h"
#include "gpuUploadSample.h"

#include <boost/foreach.hpp>
#include <boost/format.hpp>

#define WINDOWS_LEAN_AND_MEAN
#include <windows.h>

using namespace Limitless;

GpuUpload::GpuUpload(std::string name, SharedMediaFilter parent):
MediaAutoRegister(name, parent)
{
}

GpuUpload::~GpuUpload()
{

}

bool GpuUpload::initialize(const Attributes &attributes)
{
	m_gpuImageSampleSetId=MediaSampleFactory::getTypeId("GpuImageSampleSet");
	m_gpuImageSampleId=MediaSampleFactory::getTypeId("GpuImageSample");
	m_gpuUploadSampleId=MediaSampleFactory::getTypeId("GpuUploadSample");
	m_imageSampleSetId=MediaSampleFactory::getTypeId("ImageSampleSet");
	m_imageSampleId=MediaSampleFactory::getTypeId("IImageSample");

	addSinkPad("Sink", "[{\"mime\":\"image\\raw\"}]");
	addSourcePad("Source", "[{\"mime\":\"image\\gpu\"}]");

	return true;
}

bool GpuUpload::processSample(SharedMediaPad sinkPad, SharedMediaSample sample)
{
//	if(sample->isType(m_imageSampleSetId))
//	{
//		SharedImageSampleSet imageSampleSet=boost::dynamic_pointer_cast<ImageSampleSet>(sample);
//		SharedGpuImageSampleSet gpuImageSampleSet=boost::dynamic_pointer_cast<GpuImageSampleSet>(newSample(m_gpuImageSampleSetId));
//		SharedGpuUploadSample gpuUploadSample=boost::dynamic_pointer_cast<GpuUploadSample>(newSample(m_gpuUploadSampleId));
//		
//		cl::Event event;
//		std::vector<cl::Event> events;
//
//		events.resize(1);
//		gpuImageSampleSet->setSampleSetSize(imageSampleSet->sampleSetSize());
//		for(size_t i=0; imageSampleSet->sampleSetSize(); ++i)
//		{
//			SharedIImageSample imageSample=imageSampleSet->sample(i);
//			SharedGpuImageSample gpuImageSample=boost::dynamic_pointer_cast<GpuImageSample>(newSample(m_gpuImageSampleId));
//
//			if(i == 0)
//				gpuImageSample->write(imageSample->buffer(), gpuImageSample->width(), gpuImageSample->height(), event);
//			else
//				gpuImageSample->write(imageSample->buffer(), gpuImageSample->width(), gpuImageSample->height(), event, &events);
//			events[0]=event;
//
//			gpuImageSampleSet->setSample(i, gpuImageSample);
//		}
//
//		gpuUploadSample->setSample(gpuImageSampleSet, events[0]);
//
//		{
//			std::unique_lock<std::mutex> lock(m_samplesMutex);
//
//			while(m_sampleQueue.size() > 10)
//				m_sampleEvent.wait(lock);
//
//			m_sampleQueue.push(gpuUploadSample);
//		}
//		m_sampleEvent.notify_all();
//	}
//	else if(sample->isType(m_imageSampleId))
	if(sample->isType(m_imageSampleId))
	{
		SharedIImageSample imageSample=boost::dynamic_pointer_cast<IImageSample>(sample);
//		SharedGpuUploadSample gpuUploadSample=boost::dynamic_pointer_cast<GpuUploadSample>(newSample(m_gpuUploadSampleId));
//		SharedGpuImageSample gpuImageSample=boost::dynamic_pointer_cast<GpuImageSample>(newSample(m_gpuImageSampleId));
		
//		cl::Event event;
//
//		gpuImageSample->write(imageSample->buffer(), imageSample->width(), imageSample->height(), event);

//		gpuUploadSample->setSample(gpuImageSample, event);
		
//		GpuUploadTask uploadTask(sample, gpuImageSample, event);
		GpuUploadTask uploadTask(sample);

		{
			std::unique_lock<std::mutex> lock(m_samplesMutex);

			while(m_sampleQueue.size() > 10)
				m_sampleEvent.wait(lock);

//			m_sampleQueue.push(gpuUploadSample);
			m_sampleQueue.push(uploadTask);
		}
		m_sampleEvent.notify_all();
	}

//	else if(sample->isType(m_gpuImageSampleId))
//	{
//	}
	
	return true;
}

IMediaFilter::StateChange GpuUpload::onReady()
{
	if(!m_processThread.joinable())
		m_processThread=std::thread(std::bind(&GpuUpload::processUploadSamples, this));

	return SUCCESS;
}

IMediaFilter::StateChange GpuUpload::onPaused()
{
	return SUCCESS;
}

IMediaFilter::StateChange GpuUpload::onPlaying()
{
	return SUCCESS;
}

bool GpuUpload::onAcceptMediaFormat(SharedMediaPad pad, SharedMediaFormat format)
{
	if(pad->type() == MediaPad::SINK)
		return true; //we accept anything
	else
		return true;
	return false;
}

void GpuUpload::onLinkFormatChanged(SharedMediaPad pad, SharedMediaFormat format)
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
}

void GpuUpload::processUploadSamples()
{
//	std::unique_lock<std::mutex> lock(m_samplesMutex);
//	SharedGpuUploadSample mediaSample;

	GpuUploadTask uploadTask;

	while(true)
	{
		{
			std::unique_lock<std::mutex> lock(m_samplesMutex);
		
			if(m_sampleQueue.empty())
			{
//				OutputDebugStringA("Wait\n");
				m_sampleEvent.wait(lock);
			}
		
//			OutputDebugStringA("Wait complete\n");
			if(m_sampleQueue.empty())
				continue;

//			OutputDebugStringA("Getting Sample\n");
//			mediaSample=m_sampleQueue.front();
			uploadTask=m_sampleQueue.front();
			m_sampleQueue.pop();
		}
		
		m_sampleEvent.notify_all();
//		mediaSample->event().wait(); //wait for upload to complete
//		mediaSample->clearSample(); //release sample to go back into the pipeline
//		pushSample(mediaSample);
//		uploadTask.event.wait();
//		
//		if(uploadTask.gpuSamples.size() == 1)
//			pushSample(uploadTask.gpuSamples[0]);
//		else
//		{}

		SharedGpuImageSamples gpuSamples;
		cl::Event event;

		for(size_t i=0; i<uploadTask.samples.size(); ++i)
		{
			SharedGpuImageSample gpuImageSample=boost::dynamic_pointer_cast<GpuImageSample>(newSample(m_gpuImageSampleId));
			SharedIImageSample &imageSample=boost::dynamic_pointer_cast<IImageSample>(uploadTask.samples[i]);
			
			gpuImageSample->write(imageSample->buffer(), imageSample->width(), imageSample->height(), event);
			gpuSamples.push_back(gpuImageSample);

			event.wait();
		}

		if(gpuSamples.size() == 1)
			pushSample(gpuSamples[0]);
	}	
}