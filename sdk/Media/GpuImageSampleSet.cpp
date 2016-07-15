#include "GpuImageSampleSet.h"

/*
using namespace Limitless;

GpuImageSampleSet::GpuImageSampleSet()
{
}

GpuImageSampleSet::~GpuImageSampleSet()
{
}

void GpuImageSampleSet::addSample(SharedIImageSample sample)
{
	SharedGpuImageSample gpuImageSample=boost::dynamic_pointer_cast<GpuImageSample>(sample);

	if(gpuImageSample != SharedGpuImageSample())
		m_gpuImageSamples.push_back(gpuImageSample);
}

SharedIImageSample &GpuImageSampleSet::sample(size_t index)
{
	return m_gpuImageSamples[index];
}

void GpuImageSampleSet::setSample(size_t index, SharedIImageSample imageSample)
{
	if((index < 0) || (index > m_gpuImageSamples.size()-1))
		return;

	SharedGpuImageSample gpuImageSample=boost::dynamic_pointer_cast<GpuImageSample>(imageSample);

	if(gpuImageSample == SharedGpuImageSample())
		return;

	m_gpuImageSamples[index]=gpuImageSample;
}

size_t GpuImageSampleSet::sampleSetSize()
{
	return m_gpuImageSamples.size();
}

bool GpuImageSampleSet::sampleSetEmpty()
{
	return m_gpuImageSamples.empty();
}

void GpuImageSampleSet::setSampleSetSize(size_t size)
{
	m_gpuImageSamples.resize(size);
}

void GpuImageSampleSet::clearSampleSet()
{
	m_gpuImageSamples.clear();
}
*/