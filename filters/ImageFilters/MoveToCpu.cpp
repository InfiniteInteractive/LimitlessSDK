#include "MoveToCpu.h"

#include "Media/GPUContext.h"
#include "Media/MediaSampleFactory.h"
#include "Media/ImageSample.h"
#include "Media/ImageSampleSet.h"
#include "Media/GpuImageSample.h"
#include "Media/GpuImageSampleSet.h"

#include <boost/filesystem.hpp>
#include <boost/math/constants/constants.hpp>

using namespace Limitless;

MoveToCpu::MoveToCpu(std::string name, Limitless::SharedMediaFilter parent):
MediaAutoRegister(name, parent),
m_sampleSetBin(0)
{
}

MoveToCpu::~MoveToCpu()
{
}

bool MoveToCpu::initialize(const Attributes &attributes)
{
	m_interfaceImageSampleId=MediaSampleFactory::getTypeId("IImageSample");
	m_imageSampleId=MediaSampleFactory::getTypeId("ImageSample");
	m_gpuImageSampleId=MediaSampleFactory::getTypeId("GpuImageSample");
	m_imageSampleSetId=MediaSampleFactory::getTypeId("ImageSampleSet");

	std::string sinkPadDescription;

	sinkPadDescription="[";
	sinkPadDescription+="{\"mime\":\"image/gpu\"}";
	sinkPadDescription+=", {\"mime\":\"video/gpu\"}";
	sinkPadDescription+=",{\"mime\":\"image\"}";
	sinkPadDescription+=", {\"mime\":\"video\"}";
	sinkPadDescription+="]";
	addSinkPad("Sink", sinkPadDescription);

	std::string sourcePadDescription;

	sourcePadDescription="[";
	sourcePadDescription+="{\"mime\":\"image\"}";
	sourcePadDescription+=", {\"mime\":\"video\"}";
	sourcePadDescription+="]";
	addSourcePad("Source", sourcePadDescription);

	return true;
}

SharedPluginView MoveToCpu::getView()
{
	return SharedPluginView();
}

IMediaFilter::StateChange MoveToCpu::onReady()
{
	if(m_sampleSetBin == 0)
		m_sampleSetBin=allocSampleBin();

	return IMediaFilter::SUCCESS;
}

IMediaFilter::StateChange MoveToCpu::onPaused()
{
	return IMediaFilter::SUCCESS;
}

IMediaFilter::StateChange MoveToCpu::onPlaying()
{
	return IMediaFilter::SUCCESS;
}

bool MoveToCpu::onAcceptMediaFormat(Limitless::SharedMediaPad pad, Limitless::SharedMediaFormat format)
{
	return true;
}

void MoveToCpu::onLinkFormatChanged(Limitless::SharedMediaPad mediaPad, Limitless::SharedMediaFormat format)
{
	if(mediaPad->type() == MediaPad::SINK)
	{
		Limitless::SharedMediaFormat outputFormat(new Limitless::MediaFormat(*format));
		Limitless::SharedMediaPads sourcePads=getSourcePads();

		for(SharedMediaPad &sourcePad:sourcePads)
		{
			sourcePad->removeAllMediaFormats();
			sourcePad->addMediaFormat(outputFormat);
			sourcePad->setFormat(*outputFormat);
		}
	}
}

void MoveToCpu::onAttributeChanged(std::string name, Limitless::SharedAttribute attribute)
{
}

bool MoveToCpu::processSample(Limitless::SharedMediaPad sinkPad, Limitless::SharedMediaSample sample)
{
	bool convert=true;
	std::vector<SharedGpuImageSample> gpuImageSamples;
	std::vector<std::vector<cl::Event> > sampleEvents;
	
	if(sample->isType(m_gpuImageSampleId))
	{
		GpuImageSample *gpuImageSample=dynamic_cast<GpuImageSample *>(sample.get());
		SharedImageSample imageSample=newSampleType<ImageSample>(m_imageSampleId);

		imageSample->copyHeader(sample, instance());

		if(!imageSample)
			return false;

		imageSample->resize(gpuImageSample->width(), gpuImageSample->height(), 4);

		cl::Event event;

		gpuImageSample->read(imageSample.get(), event);
		event.wait();

		pushSample(imageSample);
	}
	else if(sample->isType(m_imageSampleSetId))
	{
		ImageSampleSet *gpuImageSampleSet=dynamic_cast<ImageSampleSet *>(sample.get());

		gpuImageSampleSet->copyHeader(sample, instance());
		//check for gpuImages
		for(size_t i=0; i<gpuImageSampleSet->sampleSetSize(); ++i)
		{
			SharedIImageSample imageSample=gpuImageSampleSet->sample(i);

			if(!imageSample->isType(m_gpuImageSampleId))
			{
				pushSample(sample);
				return true;
			}
		}

		//need to allocate set out of another bin
		SharedImageSampleSet imageSampleSet=newSampleType<ImageSampleSet>(m_imageSampleSetId, m_sampleSetBin);
		std::vector<cl::Event> events;

		imageSampleSet->setSampleSetSize(gpuImageSampleSet->sampleSetSize());

		std::vector<GpuImageSample *> gpuSamples;
		for(size_t i=0; i<gpuImageSampleSet->sampleSetSize(); ++i)
		{
			SharedGpuImageSample gpuSample=boost::dynamic_pointer_cast<GpuImageSample>(gpuImageSampleSet->sample(i));
			
			gpuSamples.push_back(gpuSample.get());
		}

		//acquire Images for opencl use
		cl::Event event;

		GpuImageSample::acquireMultipleOpenCl(gpuSamples, event);

		event.wait();

		for(size_t i=0; i<imageSampleSet->sampleSetSize(); ++i)
		{
			SharedImageSample mediaSample=newSampleType<ImageSample>(m_imageSampleId);
			GpuImageSample *gpuImagSmaple=gpuSamples[i];
			cl::Event event;

			gpuImagSmaple->read(mediaSample.get(), event);
			events.push_back(event);

			imageSampleSet->sample(i)=mediaSample;
		}

		cl::WaitForEvents(events);
		pushSample(imageSampleSet);
		return true;
	}
	else if(sample->isType(m_imageSampleId))
	{
		pushSample(sample);
		return true;
	}
    else
        pushSample(sample);

	return true;
}
