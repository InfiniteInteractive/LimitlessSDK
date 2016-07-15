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

	virtual unsigned char *buffer();
	virtual size_t size() const;

	virtual AudioSampleFormat format() const;
	virtual unsigned int channels() const;
	virtual unsigned int samples() const;
};

typedef boost::shared_ptr<AudioSample> SharedAudioSample;
typedef std::deque<SharedAudioSample> SharedAudioSampleQueue;

}//namespace Limitless

#endif //_AudioSample_h_