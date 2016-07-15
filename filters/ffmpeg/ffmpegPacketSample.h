#ifndef _FfmpegPacketSample_h_
#define _FfmpegPacketSample_h_

#include "Media/MediaSample.h"
#include "Media/MediaSampleFactory.h"
#include "Media/MediaClock.h"

extern "C"
{
	#include "libavcodec/avcodec.h"
	#include "libavformat/avformat.h"
}

class FfmpegPacketSample:public Limitless::AutoRegisterMediaSample<FfmpegPacketSample, Limitless::MediaSample>
{
public:
	FfmpegPacketSample();
	virtual ~FfmpegPacketSample();

	void initPacket(){av_init_packet(&m_packet);}
	AVPacket *getPacket(){return &m_packet;}
	void allocate(size_t size);

	int64_t presentationTimestamp(){return m_packet.pts;}
	int64_t decompressionTimestamp(){return m_packet.dts;}

//	virtual Limitless::MediaTime sourceTimestamp() const{return m_packet.pts;}

//MediaSample
	virtual unsigned char *buffer();
	virtual size_t size() const;
	virtual size_t actualSize() const;

	void setSize(size_t size);
	void resetPacket();

private:
	void freeBuffer();

	AVPacket m_packet;
	size_t m_allocated;
	uint8_t *m_buffer;
};
typedef boost::shared_ptr<FfmpegPacketSample> SharedFfmpegPacketSample;
#endif //_FfmpegPacketSample_h_