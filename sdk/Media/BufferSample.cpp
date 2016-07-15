#include "Media/BufferSample.h"

using namespace Limitless;

BufferSample::BufferSample():
m_externalBuffer(false),
m_buffer(NULL),
m_size(0),
m_actualSize(0)
{}

BufferSample::BufferSample(unsigned char *buffer, size_t size):
m_externalBuffer(true),
m_buffer(buffer),
m_size(size),
m_actualSize(0)
{
}

BufferSample::~BufferSample()
{
	freeBuffer();
}

void BufferSample::allocate(size_t size)
{
	if((size > m_size) || (m_externalBuffer == true))
	{
		freeBuffer();

		m_buffer=(unsigned char *)malloc(size);
		m_externalBuffer=false;
		m_actualSize=m_size=size;
	}
	else
		m_size=size;
}

void BufferSample::freeBuffer()
{
	if((m_buffer != NULL) && (!m_externalBuffer))
	{
		delete m_buffer;
		m_buffer=NULL;
		m_size=0;
		m_actualSize=0;
		m_externalBuffer=false;
	}
}