#include "Media/GPUBufferGLSample.h"
#include "GPUContext.h"

using namespace Limitless;

GPUBufferGLSample::GPUBufferGLSample():
m_flags(CL_MEM_READ_WRITE),
m_size(0),
m_actualSize(0)
{}

GPUBufferGLSample::GPUBufferGLSample(size_t size):
m_flags(CL_MEM_READ_WRITE)
{
	resize(size);
}

GPUBufferGLSample::GPUBufferGLSample(unsigned char *buffer, size_t size):
m_flags(CL_MEM_READ_WRITE)
{
	resize(size);

	cl::Event event;

	if(write(buffer, size, event))
		event.wait();
}

GPUBufferGLSample::~GPUBufferGLSample()
{
}

void GPUBufferGLSample::resize(size_t bufferSize)
{
	if(size() != bufferSize)
	{
		cl_int error = CL_SUCCESS;

		GPUContext::makeOpenGLCurrent();

		glGenBuffers(1, &m_pbo);
		glBindBuffer(GL_ARRAY_BUFFER, m_pbo);
		glBufferData(GL_ARRAY_BUFFER, bufferSize, NULL, GL_DYNAMIC_DRAW);
		glBindBuffer(GL_PIXEL_UNPACK_BUFFER_ARB, m_texture);
		glFinish();

		m_buffer=cl::BufferGL(GPUContext::openCLContext(), m_flags, m_texture, &error);
		m_size=bufferSize;
		m_actualSize=bufferSize;
		m_hostBuffer.clear();

		if(error != CL_SUCCESS)
		{
			assert(false);
		}
	}
}

bool GPUBufferGLSample::write(unsigned char *buffer, size_t size, cl::Event &event)
{
	if(size == 0)
		return false;

	m_hostBuffer.clear();
	if(m_size != size)
	{
		cl_int error = CL_SUCCESS;

		m_buffer=cl::BufferGL(m_openCLContext, m_flags, m_texture, &error);
		m_size=size;
	}

	cl_int status;

	status=GPUContext::openCLCommandQueue().enqueueWriteBuffer(m_buffer, CL_FALSE, 0, size, buffer, NULL, &event);

	if(status != CL_SUCCESS)
	{
		return false;
		assert(false);
	}
	return true;
}

bool GPUBufferGLSample::read(unsigned char *buffer, size_t size, cl::Event &event)
{
	if(size == 0)
		return false;

	cl_int status;

	status=GPUContext::openCLCommandQueue().enqueueReadBuffer(m_buffer, CL_FALSE, 0, size, buffer, NULL, &event);

	if(status != CL_SUCCESS)
	{
		return false;
		assert(false);
	}
	return true;
}

unsigned char *GPUBufferGLSample::buffer()
{
	if(m_hostBuffer.size() == m_size)
		return m_hostBuffer.data();

	cl::Event event;

	m_hostBuffer.resize(m_size);

	if(!read(m_hostBuffer.data(), m_size, event))
		return NULL;

	event.wait();
	return m_hostBuffer.data();
}