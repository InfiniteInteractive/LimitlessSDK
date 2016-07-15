#include "GpuImageGLSampleSet.h"

using namespace Limitless;

GpuImageGLSampleSet::GpuImageGLSampleSet()
{}

GpuImageGLSampleSet::~GpuImageGLSampleSet()
{}

void GpuImageGLSampleSet::addSample(SharedIImageSample sample)
{
	SharedGPUImageGLSample gpuImageSample=boost::dynamic_pointer_cast<GPUImageGLSample>(sample);

	if(gpuImageSample != SharedGPUImageGLSample())
		m_gpuImageSamples.push_back(gpuImageSample);
}

SharedIImageSample GpuImageGLSampleSet::sample(size_t index)
{
	return m_gpuImageSamples[index];
}

size_t GpuImageGLSampleSet::sampleSetSize()
{
	return m_gpuImageSamples.size();
}

bool GpuImageGLSampleSet::sampleSetEmpty()
{
	return m_gpuImageSamples.empty();
}

void GpuImageGLSampleSet::setSampleSetSize(size_t size)
{
	m_gpuImageSamples.resize(size);
}

void GpuImageGLSampleSet::clearSampleSet()
{
	m_gpuImageSamples.clear();
}