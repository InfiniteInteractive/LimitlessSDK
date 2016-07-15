#include "gpuUploadSample.h"
#include "Media/GPUContext.h"

using namespace Limitless;

GpuUploadSample::GpuUploadSample()
{}

GpuUploadSample::~GpuUploadSample()
{
}

void GpuUploadSample::setSample(SharedMediaSample sample, cl::Event &event)
{
	m_sample=sample;
	m_event=event;
}

void GpuUploadSample::clearSample()
{
	m_sample.reset();
}