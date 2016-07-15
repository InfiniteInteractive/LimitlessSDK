#ifndef _FfmpegAudioSample_h_
#define _FfmpegAudioSample_h_

#include "Media/IAudioSample.h"
#include "Media/MediaSampleFactory.h"
#include "Media/MediaClock.h"

extern "C"
{
	#include "libavcodec/avcodec.h"
	#include "libavformat/avformat.h"
}

class FfmpegAudioSample:public Limitless::AutoRegisterMediaSample<FfmpegAudioSample, Limitless::IAudioSample>
{
public:
	FfmpegAudioSample();
	virtual ~FfmpegAudioSample();

	AVFrame *getFrame(){return m_frame;}

	int64_t presentationTimestamp() { return m_frame->pts; }
//	virtual Limitless::MediaTime timestamp() const { return m_frame->pts; }
//	virtual Limitless::MediaTime sourceTimestamp() const { return m_frame->pts; }

//MediaSample
	virtual unsigned char *buffer();
	virtual size_t size() const;
	virtual size_t actualSize() const;
	
//IImageSample
	virtual Limitless::AudioSampleFormat format() const;
	virtual unsigned int channels() const;
	virtual unsigned int samples() const;

	void unref();

private:
	AVFrame *m_frame;
};
typedef boost::shared_ptr<FfmpegAudioSample> SharedFfmpegAudioSample;
#endif //_FfmpegAudioSample_h_