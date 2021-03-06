#include "FfmpegAudioSample.h"
#include "ffmpegResources.h"

FfmpegAudioSample::FfmpegAudioSample()
{
	m_frame=av_frame_alloc();
}

FfmpegAudioSample::~FfmpegAudioSample()
{
	av_frame_free(&m_frame);
}

unsigned char *FfmpegAudioSample::buffer(size_t index)
{
    if(index == 0)
	    return m_frame->data[0];
    return nullptr;
}

size_t FfmpegAudioSample::bufferSize(size_t index)
{
    if(index==0)
        return size();
    return 0;
}

size_t FfmpegAudioSample::size() const
{
	return m_frame->nb_samples*av_get_bytes_per_sample((AVSampleFormat)m_frame->format);
}

size_t FfmpegAudioSample::actualSize() const
{
	return m_frame->nb_samples*av_get_bytes_per_sample((AVSampleFormat)m_frame->format);
}

Limitless::AudioSampleFormat FfmpegAudioSample::format() const
{
	return FfmpegResources::getAudioFormat((AVSampleFormat)m_frame->format);
}

unsigned int FfmpegAudioSample::channels() const
{
	return m_frame->channels;
}

unsigned int FfmpegAudioSample::samples() const
{
	return m_frame->nb_samples;
}

unsigned int FfmpegAudioSample::sampleRate() const
{
	return m_frame->sample_rate;
}

void FfmpegAudioSample::unref()
{
	av_frame_unref(m_frame);
}