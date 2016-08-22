#include "Media/AudioSample.h"

namespace Limitless
{

void AudioSample::resize(AudioSampleFormat format, unsigned int channels, unsigned int samples, unsigned int sampleRate)
{
	m_format=format;
	m_channels=channels;
	m_samples=samples;
	m_sampleRate=sampleRate;
	m_buffer.resize(calculateAudioBufferSize(format, channels, samples));
}

}//namespace Limitless