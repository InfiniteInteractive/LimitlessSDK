#include "FfmpegFrameSample.h"
#include "ffmpegResources.h"

#include "pixelFormatDescription.h"

FfmpegFrameSample::FfmpegFrameSample()
{
	m_frame=av_frame_alloc();
}

FfmpegFrameSample::~FfmpegFrameSample()
{
	av_frame_free(&m_frame);
}

size_t FfmpegFrameSample::buffers()
{
    const PixelFormatDescription &descriptor=getPixelFormatDescription((AVPixelFormat)m_frame->format);

    return descriptor.planes;
}

unsigned char *FfmpegFrameSample::buffer(size_t index)
{
    if(index>=AV_NUM_DATA_POINTERS)
        return nullptr;

    return m_frame->data[index];
}

size_t FfmpegFrameSample::bufferSize(size_t index)
{
    if(index>=AV_NUM_DATA_POINTERS)
        return 0;

    const PixelFormatDescription &descriptor=getPixelFormatDescription((AVPixelFormat)m_frame->format);

    return m_frame->linesize[index]*(m_frame->height/descriptor.divisors[index]);
}

size_t FfmpegFrameSample::size() const
{
    const PixelFormatDescription &descriptor=getPixelFormatDescription((AVPixelFormat)m_frame->format);

    size_t totalSize=0;

    for(int i=0; i<descriptor.planes; ++i)
        totalSize+=m_frame->linesize[i]*(m_frame->height/descriptor.divisors[i]);
	return totalSize;
}

size_t FfmpegFrameSample::actualSize() const
{
	return size();
}

void FfmpegFrameSample::setFormat(AVPixelFormat format)
{
	m_format=FfmpegResources::getAvPixelFormatName(format);
}

void FfmpegFrameSample::unref()
{
	av_frame_unref(m_frame);
}