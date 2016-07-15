#include "FfmpegFrameSample.h"
#include "ffmpegResources.h"

FfmpegFrameSample::FfmpegFrameSample()
{
	m_frame=av_frame_alloc();
}

FfmpegFrameSample::~FfmpegFrameSample()
{
	av_frame_free(&m_frame);
}

unsigned char *FfmpegFrameSample::buffer()
{
	return m_frame->data[0];
}

size_t FfmpegFrameSample::size() const
{
	return m_frame->linesize[0]*m_frame->height;
}

size_t FfmpegFrameSample::actualSize() const
{
	return m_frame->linesize[0]*m_frame->height;
}

void FfmpegFrameSample::setFormat(AVPixelFormat format)
{
	m_format=FfmpegResources::getAvPixelFormatName(format);
}

void FfmpegFrameSample::unref()
{
	av_frame_unref(m_frame);
}