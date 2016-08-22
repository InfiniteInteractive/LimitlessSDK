#ifndef _IAudioSample_h_
#define _IAudioSample_h_

#include "Media/media_define.h"
#include "Media/MediaSample.h"
#include "Media/MediaSampleFactory.h"
#include <boost/shared_ptr.hpp>
#include <deque>

namespace Limitless
{

enum class AudioSampleFormat
{
	Unknown,
	UInt8, //unsigned 8 bit
	Int16,
	Int32,
	Float,
	Double,
//Planar
	UInt8P, //unsigned 8 bit
	Int16P,
	Int32P,
	FloatP,
	DoubleP
};

size_t MEDIA_EXPORT sampleSize(AudioSampleFormat format);
size_t MEDIA_EXPORT calculateAudioBufferSize(AudioSampleFormat format, unsigned int channels, unsigned int samples);

class MEDIA_EXPORT IAudioSample:public AutoRegisterMediaSample<IAudioSample, MediaSample, true>
{
public:
	IAudioSample() {};
	virtual ~IAudioSample() {};

	virtual AudioSampleFormat format() const=0;
	virtual unsigned int channels() const=0;
	virtual unsigned int samples() const=0; //number of sample per channel
	virtual unsigned int sampleRate() const=0;
};

typedef boost::shared_ptr<IAudioSample> SharedIAudioSample;
typedef std::deque<SharedIAudioSample> SharedIAudioSampleQueue;

}//namespace Limitless

#endif //_IAudioSample_h_