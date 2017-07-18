#include "FfmpegPacketSample.h"

FfmpegPacketSample::FfmpegPacketSample():
m_allocated(0)
{
	av_init_packet(&m_packet);
	
	m_packet.data=nullptr;
	m_packet.size=0;
}

FfmpegPacketSample::~FfmpegPacketSample()
{
	if(m_packet.data)
		av_free_packet(&m_packet);

}

unsigned char *FfmpegPacketSample::buffer(size_t index)
{
    if(index == 0)
	    return m_packet.data;
    return nullptr;
}

size_t FfmpegPacketSample::size() const
{
	return m_packet.size;
}

void FfmpegPacketSample::resetPacket()
{
	m_packet.data=m_buffer;
	m_packet.size=m_allocated;
}

void FfmpegPacketSample::setSize(size_t size)
{
	assert(size <= m_allocated);
	m_packet.size=size;
}

void FfmpegPacketSample::allocate(size_t size)
{
	if(size > m_allocated)
	{
		m_buffer=(uint8_t *)malloc(size);
		m_packet.data=m_buffer;
		m_allocated=m_packet.size=size;
	}
}

void FfmpegPacketSample::freeBuffer()
{
	m_packet.size=0;
	free(m_packet.data);
}


size_t FfmpegPacketSample::actualSize() const
{
	return m_allocated;
}

