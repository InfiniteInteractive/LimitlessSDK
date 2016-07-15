#include "MoveToGpu.h"

#include "Media/GPUContext.h"
#include "Media/MediaSampleFactory.h"
#include "Media/ImageSample.h"
#include "Media/ImageSampleSet.h"
#include "Media/GpuImageSample.h"
#include "Media/GpuImageSampleSet.h"

#include <boost/filesystem.hpp>
#include <boost/math/constants/constants.hpp>

using namespace Limitless;

MoveToGpu::MoveToGpu(std::string name, Limitless::SharedMediaFilter parent):
MediaAutoRegister(name, parent),
m_sampleSetBin(0)
{
}

MoveToGpu::~MoveToGpu()
{
}

bool MoveToGpu::initialize(const Attributes &attributes)
{
	m_iImageSampleId=MediaSampleFactory::getTypeId("IImageSample");
	m_iImageSampleSetId=MediaSampleFactory::getTypeId("IImageSampleSet");
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
	sourcePadDescription+="{\"mime\":\"image/gpu\"}";
	sourcePadDescription+=", {\"mime\":\"video/gpu\"}";
	sourcePadDescription+="]";
	addSourcePad("Source", sourcePadDescription);

	return true;
}

SharedPluginView MoveToGpu::getView()
{
	return SharedPluginView();
}

IMediaFilter::StateChange MoveToGpu::onReady()
{
	if(m_sampleSetBin == 0)
		m_sampleSetBin=allocSampleBin();

	return IMediaFilter::SUCCESS;
}

IMediaFilter::StateChange MoveToGpu::onPaused()
{
	return IMediaFilter::SUCCESS;
}

IMediaFilter::StateChange MoveToGpu::onPlaying()
{
	return IMediaFilter::SUCCESS;
}

bool MoveToGpu::onAcceptMediaFormat(Limitless::SharedMediaPad pad, Limitless::SharedMediaFormat format)
{
	return true;
}

void MoveToGpu::onLinkFormatChanged(Limitless::SharedMediaPad mediaPad, Limitless::SharedMediaFormat format)
{
	if(mediaPad->type() == MediaPad::SINK)
	{
		Limitless::SharedMediaFormat outputFormat(new Limitless::MediaFormat(*format));

		if(outputFormat->exists("format"))
			outputFormat->attribute("format")->fromString("RGBA");
		else
			outputFormat->addAttribute("format", "RGBA");

		Limitless::SharedMediaPads sourcePads=getSourcePads();

		for(SharedMediaPad &sourcePad:sourcePads)
		{
			sourcePad->removeAllMediaFormats();
			sourcePad->addMediaFormat(outputFormat);
			sourcePad->setFormat(*outputFormat);
		}
	}
}

void MoveToGpu::onAttributeChanged(std::string name, Limitless::SharedAttribute attribute)
{
}

bool MoveToGpu::processSample(Limitless::SharedMediaPad sinkPad, Limitless::SharedMediaSample sample)
{
	bool convert=true;
	std::vector<SharedGpuImageSample> gpuImageSamples;
	std::vector<std::vector<cl::Event> > sampleEvents;
	
	if(sample->isType(m_gpuImageSampleId))
	{
		pushSample(sample);
		return true;
	}
	else if(sample->isType(m_iImageSampleId))
	{
		IImageSample *imageSample=dynamic_cast<IImageSample *>(sample.get());

		SharedGpuImageSample gpuImageSample=newSampleType<GpuImageSample>(m_gpuImageSampleId);

		if(!gpuImageSample)
			return false;

		gpuImageSample->copyHeader(sample, instance());

		cl::Event event;

		gpuImageSample->write(imageSample, event);
		event.wait();

		pushSample(gpuImageSample);
	}
	else if(sample->isType(m_iImageSampleSetId))
	{
		IImageSampleSet *imageSampleSet=dynamic_cast<IImageSampleSet *>(sample.get());

		//check for gpuImages
		for(size_t i=0; i<imageSampleSet->sampleSetSize(); ++i)
		{
			SharedIImageSample iImageSample=imageSampleSet->sample(i);
			if(iImageSample->isType(m_gpuImageSampleId))
			{
				pushSample(sample);
				return true;
			}
		}

		//need to allocate set out of another bin
		SharedImageSampleSet gpuImageSampleSet=newSampleType<ImageSampleSet>(m_imageSampleSetId, m_sampleSetBin);
		std::vector<cl::Event> events;

		gpuImageSampleSet->copyHeader(sample, instance());

		gpuImageSampleSet->setSampleSetSize(imageSampleSet->sampleSetSize());

		std::vector<GpuImageSample *> gpuSamples;
		for(size_t i=0; i<imageSampleSet->sampleSetSize(); ++i)
		{
			SharedIImageSample iImageSample=imageSampleSet->sample(i);
			SharedGpuImageSample mediaSample=newSampleType<GpuImageSample>(m_gpuImageSampleId);

			gpuSamples.push_back(mediaSample.get());
			gpuImageSampleSet->sample(i)=mediaSample;
		}

		//acquire Images for opencl use
		cl::Event event;

		GpuImageSample::acquireMultipleOpenCl(gpuSamples, event);

		event.wait();

		for(size_t i=0; i<imageSampleSet->sampleSetSize(); ++i)
		{
			SharedIImageSample iImageSample=imageSampleSet->sample(i);
//			SharedGpuImageSample mediaSample=newSampleType<GpuImageSample>(m_gpuImageSampleId);
			GpuImageSample *mediaSample=gpuSamples[i];
			cl::Event event;

			mediaSample->write(iImageSample.get(), event);
			events.push_back(event);

//			gpuImageSampleSet->sample(i)=mediaSample;
		}

		cl::WaitForEvents(events);
		pushSample(gpuImageSampleSet);
		return true;
	}
    else
        pushSample(sample);

	return true;
}
