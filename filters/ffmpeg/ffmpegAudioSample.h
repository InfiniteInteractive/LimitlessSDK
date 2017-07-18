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
    virtual size_t buffers() { return 1; }
	virtual unsigned char *buffer(size_t index=0);
    virtual size_t bufferSize(size_t index=0);
	virtual size_t size() const;
	virtual size_t actualSize() const;
	
//IImageSample
	virtual Limitless::AudioSampleFormat format() const;
	virtual unsigned int channels() const;
	virtual unsigned int samples() const;
	virtual unsigned int sampleRate() const;

	void unref();

private:
	AVFrame *m_frame;
};
typedef boost::shared_ptr<FfmpegAudioSample> SharedFfmpegAudioSample;
#endif //_FfmpegAudioSample_h_