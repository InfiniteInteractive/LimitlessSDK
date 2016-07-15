#include "GpuImageSample.h"
#include "GPUContext.h"
#include "Utilities/utilitiesImage.h"

using namespace Limitless;

GpuImageSample::GpuImageSample():
m_flags(CL_MEM_READ_WRITE),
m_width(0),
m_height(0),
m_channels(0),
m_channelBits(0),
m_texture(0)
{}

GpuImageSample::GpuImageSample(unsigned int width, unsigned int height, unsigned int channels, unsigned int channelBits):
m_flags(CL_MEM_READ_WRITE),
m_channels(channels),
m_channelBits(channelBits),
m_texture(0)
{
	resize(width, height);
}

GpuImageSample::GpuImageSample(unsigned char *buffer, unsigned int width, unsigned int height, unsigned int channels, unsigned int channelBits):
m_flags(CL_MEM_READ_WRITE),
m_channels(channels),
m_channelBits(channelBits),
m_texture(0)
{
	cl::Event event;

	resize(width, height);
	if(write(buffer, width, height, event))
		event.wait();
}

GpuImageSample::~GpuImageSample()
{
}

unsigned char *GpuImageSample::buffer()
{
	if(m_hostBuffer.size() == m_size)
		return m_hostBuffer.data();

	cl::Event event;

	m_hostBuffer.resize(m_size);

	if(!read(m_hostBuffer.data(), m_width, m_height, event))
		return NULL;

	event.wait();
	return m_hostBuffer.data();
}

bool GpuImageSample::resize(unsigned int width, unsigned int height, unsigned int channles, unsigned int channelBits)
{
	cl_int error = CL_SUCCESS;

	if((width == 0) || (height == 0))
		return false;

//	GPUContext::makeOpenGLCurrent();
//
//	if(m_texture == 0)
//		glGenTextures(1, &m_texture);
//
//	glBindTexture(GL_TEXTURE_2D, m_texture);
//	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8UI, width, height, 0, GL_RGBA_INTEGER, GL_UNSIGNED_BYTE, NULL); 
//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR); 
//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
//	glBindTexture(GL_TEXTURE_2D, 0);
//
//	glFinish();

	if(m_texture == 0)
		m_texture=GPUContext::createTexture(GL_TEXTURE_2D, GL_RGBA8UI, width, height, GL_RGBA_INTEGER, GL_UNSIGNED_BYTE);

	m_image=cl::ImageGL(GPUContext::openCLContext(), m_flags, GL_TEXTURE_2D, 0, m_texture, &error);

	m_owned=OpenGl;
//	m_image=GPUContext::createClImage(m_flags, GL_TEXTURE_2D, GL_RGBA8UI, width, height, GL_RGBA_INTEGER, GL_UNSIGNED_BYTE, m_texture);

	if(error != CL_SUCCESS)
	{
		m_width=0;
		m_height=0;
		assert(false);
		return false;
	}

	m_width=width;
	m_height=height;
	m_channels=4;
	m_size=m_width*m_height*4*sizeof(unsigned char);

	return true;
}

bool GpuImageSample::save(std::string fileName)
{
	std::vector<unsigned char> m_size(m_size);
	cl::Event event;

	read(m_size.data(), m_width, m_height, event);
	event.wait();

	if(m_channels==4)
		savePPM(fileName.c_str(), RGBA, m_size.data(), m_width, m_height);
	else
		savePPM(fileName.c_str(), RGB, m_size.data(), m_width, m_height);

	return true;
}

bool GpuImageSample::write(IImageSample *imageSample, cl::Event &event, std::vector<cl::Event> *events)
{
	return write(imageSample->buffer(), imageSample->width(), imageSample->height(), event, events);
}

bool GpuImageSample::write(unsigned char *buffer, unsigned int width, unsigned int height, cl::Event &event, std::vector<cl::Event> *events)
{
	if(width <= 0)
		return false;
	if(height <= 0)
		return false;

	if((m_width != width) || (m_height != height))
		resize(width, height);

	cl_int status;
	cl::size_t<3> origin;
	cl::size_t<3> region;

	origin[0]=0;
	origin[1]=0;
	origin[2]=0;

	region[0]=m_width;
	region[1]=m_height;
	region[2]=1;

	cl::Event acquireEvent;
	
	if(acquireOpenCl(acquireEvent))
		acquireEvent.wait();

	status=GPUContext::openCLCommandQueue().enqueueWriteImage(m_image, CL_FALSE, origin, region, 0, 0, buffer, events, &event);
//	status=GPUContext::enqueueWriteImage(m_image, origin, region, 0, 0, buffer, events, &event);

	if(status != CL_SUCCESS)
	{
		assert(false);
		return false;
	}
	return true;
}

bool GpuImageSample::read(IImageSample *imageSample, cl::Event &event, std::vector<cl::Event> *events)
{
	return read(imageSample->buffer(), imageSample->width(), imageSample->height(), event, events);
}

bool GpuImageSample::read(unsigned char *buffer, unsigned int width, unsigned int height, cl::Event &event, std::vector<cl::Event> *events)
{
	if(width <= 0)
		return false;
	if(height <= 0)
		return false;

	if((m_width != width) || (m_height != height))
		return false;

	cl_int status;
	cl::size_t<3> origin;
	cl::size_t<3> region;

	origin[0]=0;
	origin[1]=0;
	origin[2]=0;

	region[0]=m_width;
	region[1]=m_height;
	region[2]=1;

	status=GPUContext::openCLCommandQueue().enqueueReadImage(m_image, CL_FALSE, origin, region, 0, 0, buffer, events, &event);

	if(status != CL_SUCCESS)
	{
		assert(false);
		return false;
	}
	return true;
}

bool GpuImageSample::copy(GpuImageSample *sample, cl::Event &event, std::vector<cl::Event> *waitEvents)
{
	cl::Event localEvent;
	std::vector<cl::Event> localWaitEvents;

	if((m_width != sample->width()) || (m_height != sample->height()))
		resize(sample->width(), sample->height());

	if(waitEvents != nullptr)
		localWaitEvents.insert(localWaitEvents.end(), waitEvents->begin(), waitEvents->end());

	if(acquireOpenCl(localEvent))
		localWaitEvents.push_back(localEvent);
	if(sample->acquireOpenCl(localEvent))
		localWaitEvents.push_back(localEvent);

	cl_int status;
	cl::size_t<3> origin;
	cl::size_t<3> region;

	origin[0]=0;
	origin[1]=0;
	origin[2]=0;

	region[0]=m_width;
	region[1]=m_height;
	region[2]=1;

	if(localWaitEvents.empty())
		status=GPUContext::openCLCommandQueue().enqueueCopyImage(sample->glImage(), m_image, origin, origin, region, nullptr, &event);
	else
		status=GPUContext::openCLCommandQueue().enqueueCopyImage(sample->glImage(), m_image, origin, origin, region, &localWaitEvents, &event);

	if(status != CL_SUCCESS)
	{
		assert(false);
		return false;
	}
	return true;
}


bool GpuImageSample::acquireMultipleOpenCl(std::vector<GpuImageSample *> samples, cl::Event &event, std::vector<cl::Event> *waitEvents)
{
	std::vector<cl::Memory> glImages;

	for(size_t i=0; i<samples.size(); ++i)
	{
		GpuImageSample *sample=samples[i];

		if(sample->m_owned != OpenCl)
			glImages.push_back(sample->m_image);
	}

	if(glImages.empty())
		return false;

	GPUContext::openCLCommandQueue().enqueueAcquireGLObjects(&glImages, waitEvents, &event);
	return true;
}

bool GpuImageSample::releaseMultipleOpenCl(std::vector<GpuImageSample *> samples, cl::Event &event, std::vector<cl::Event> *waitEvents)
{
	std::vector<cl::Memory> glImages;

	for(size_t i=0; i<samples.size(); ++i)
	{
		GpuImageSample *sample=samples[i];

		if(sample->m_owned == OpenCl)
			glImages.push_back(sample->m_image);
	}

	if(glImages.empty())
		return false;

	GPUContext::openCLCommandQueue().enqueueReleaseGLObjects(&glImages, waitEvents, &event);
	return true;
}

bool GpuImageSample::acquireOpenCl(cl::Event &event, std::vector<cl::Event> *waitEvents)
{
	if(m_owned == OpenCl)
		return false;

	std::vector<cl::Memory> glImages;

	glImages.push_back(m_image);
	GPUContext::openCLCommandQueue().enqueueAcquireGLObjects(&glImages, waitEvents, &event);
	m_owned=OpenCl;

	return true;
}

bool GpuImageSample::releaseOpenCl(cl::Event &event, std::vector<cl::Event> *waitEvents)
{
	if(m_owned != OpenCl)
		return false;

	std::vector<cl::Memory> glImages;

	glImages.push_back(m_image);
	GPUContext::openCLCommandQueue().enqueueReleaseGLObjects(&glImages, waitEvents, &event);
	m_owned=OpenGl;

	return true;
}

bool GpuImageSample::acquireOpenGl()
{
	if(m_owned == OpenCl)
	{
		cl::Event event;

		releaseOpenCl(event);
		event.wait();
	}
	m_owned=OpenGl;

	return true;
}

bool GpuImageSample::releaseOpenGl()
{
//	m_owned=None;
	return true;
}