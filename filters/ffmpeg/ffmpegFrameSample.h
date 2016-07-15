#ifndef _FfmpegFrameSample_h_
#define _FfmpegFrameSample_h_

#include "Media/IImageSample.h"
#include "Media/MediaSampleFactory.h"
#include "Media/MediaClock.h"

extern "C"
{
	#include "libavcodec/avcodec.h"
	#include "libavformat/avformat.h"
}

class FfmpegFrameSample:public Limitless::AutoRegisterMediaSample<FfmpegFrameSample, Limitless::IImageSample>
{
public:
	FfmpegFrameSample();
	virtual ~FfmpegFrameSample();

	AVFrame *getFrame(){return m_frame;}

	int64_t presentationTimestamp() { return m_frame->pts; }
//	virtual Limitless::MediaTime timestamp() const { return m_frame->pts; }
//	virtual Limitless::MediaTime sourceTimestamp() const { return m_frame->pts; }

//MediaSample
	virtual unsigned char *buffer();
	virtual size_t size() const;
	virtual size_t actualSize() const;
	
//IImageSample
	virtual std::string imageFormat() const { return m_format; }
	virtual unsigned int width() const { return m_frame->width; }
	virtual unsigned int pitch() const { return m_frame->linesize[0]/m_frame->channels; }
	virtual unsigned int height() const { return m_frame->height; }
	virtual unsigned int channels() const { return m_frame->channels; }
	virtual unsigned int channelBits() const { return m_frame->linesize[0]/m_frame->width; }

	virtual bool save(std::string fileName) { return false; }

	void setFormat(AVPixelFormat format);
	void unref();
private:
	std::string m_format;
	AVFrame *m_frame;
};
typedef boost::shared_ptr<FfmpegFrameSample> SharedFfmpegFrameSample;
#endif //_FfmpegFrameSample_h_