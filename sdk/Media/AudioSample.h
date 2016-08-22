#ifndef _AudioSample_h_
#define _AudioSample_h_

#include "Media/media_define.h"
#include "Media/MediaSample.h"
#include "Media/MediaSampleFactory.h"
#include "Media/IAudioSample.h"
#include <boost/shared_ptr.hpp>
#include <deque>

namespace Limitless
{

class MEDIA_EXPORT AudioSample:public AutoRegisterMediaSample<AudioSample, IAudioSample>
{
public:
	AudioSample() {};
	virtual ~AudioSample() {};

//MediaSample
	virtual unsigned char *buffer() { return m_buffer.data(); }
	virtual size_t size() const { return m_buffer.size(); }

//IAudioSample
	virtual AudioSampleFormat format() const { return m_format; }
	virtual unsigned int channels() const { return m_channels; }
	virtual unsigned int samples() const { return m_samples; }
	virtual unsigned int sampleRate() const { return m_sampleRate; }

	void resize(AudioSampleFormat format, unsigned int channels, unsigned int samples, unsigned int sampleRate);

private:
	AudioSampleFormat m_format;
	unsigned int m_channels;
	unsigned int m_samples;
	unsigned int m_sampleRate;

	std::vector<uint8_t> m_buffer;
};

typedef boost::shared_ptr<AudioSample> SharedAudioSample;
typedef std::deque<SharedAudioSample> SharedAudioSampleQueue;

}//namespace Limitless

#endif //_AudioSample_h_