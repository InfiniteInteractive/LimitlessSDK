#include "GPUImageGLSample.h"
#include "GPUContext.h"

using namespace Limitless;

GPUImageGLSample::GPUImageGLSample():
m_flags(CL_MEM_READ_WRITE),
m_width(0),
m_height(0),
m_channels(0),
m_channelBits(0)
{}

GPUImageGLSample::GPUImageGLSample(unsigned int width, unsigned int height, unsigned int channels, unsigned int channelBits):
m_flags(CL_MEM_READ_WRITE),
m_channels(channels),
m_channelBits(channelBits)
{
	resize(width, height);
}

GPUImageGLSample::GPUImageGLSample(unsigned char *buffer, unsigned int width, unsigned int height, unsigned int channels, unsigned int channelBits):
m_flags(CL_MEM_READ_WRITE),
m_channels(channels),
m_channelBits(channelBits)
{
	cl::Event event;

	resize(width, height);
	if(write(buffer, width, height, event))
		event.wait();
}

GPUImageGLSample::~GPUImageGLSample()
{
}

unsigned char *GPUImageGLSample::buffer()
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

bool GPUImageGLSample::resize(unsigned int width, unsigned int height, unsigned int channles, unsigned int channelBits)
{
	cl_int error = CL_SUCCESS;

	if((width == 0) || (height == 0))
		return false;

	GPUContext::makeOpenGLCurrent();

	glGenTextures(1, &m_texture);
	glBindTexture(GL_TEXTURE_2D, m_texture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8UI, width, height, 0, GL_RGBA_INTEGER, GL_UNSIGNED_BYTE, NULL); 
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR); 
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glBindTexture(GL_TEXTURE_2D, 0);

	glFinish();

	m_image=cl::ImageGL(GPUContext::openCLContext(), m_flags, GL_TEXTURE_2D, 0, m_texture, &error);

	if(error != CL_SUCCESS)
	{
		m_width=0;
		m_height=0;
		assert(false);
		return false;
	}

	m_width=width;
	m_height=height;
	m_size=m_width*m_height*4*sizeof(unsigned char);

	return true;
}

bool GPUImageGLSample::write(unsigned char *buffer, unsigned int width, unsigned int height, cl::Event &event)
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

	status=GPUContext::openCLCommandQueue().enqueueWriteImage(m_image, CL_FALSE, origin, region, 0, 0, buffer, NULL, &event);

	if(status != CL_SUCCESS)
	{
		assert(false);
		return false;
	}
	return true;
}

bool GPUImageGLSample::read(unsigned char *buffer, unsigned int width, unsigned int height, cl::Event &event)
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

	status=GPUContext::openCLCommandQueue().enqueueReadImage(m_image, CL_FALSE, origin, region, 0, 0, buffer, NULL, &event);

	if(status != CL_SUCCESS)
	{
		assert(false);
		return false;
	}
	return true;
}