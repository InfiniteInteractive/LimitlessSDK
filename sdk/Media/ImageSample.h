#ifndef _ImageSample_h_
#define _ImageSample_h_

#include "Media/media_define.h"
#include "Media/IImageSample.h"
#include "Media/MediaSampleFactory.h"

namespace Limitless
{

class MEDIA_EXPORT ImageSample:public AutoRegisterMediaSample<ImageSample, IImageSample>
{
public:
	ImageSample();
	ImageSample(unsigned int width, unsigned int height, unsigned int channles=3, unsigned int channelBits=8);
	ImageSample(unsigned int width, unsigned int pitch, unsigned int height, unsigned int channles, unsigned int channelBits);
	ImageSample(unsigned char *buffer, unsigned int width, unsigned int height, unsigned int channles=3, unsigned int channelBits=8);
	ImageSample(unsigned char *buffer, unsigned int width, unsigned int pitch, unsigned int height, unsigned int channles, unsigned int channelBits);
	virtual ~ImageSample();

//MediaSample
	virtual unsigned char *buffer() const{return m_buffer;}
	virtual unsigned char *buffer() { return m_buffer; }
	virtual size_t size() const{return m_size;}

//IImageSample
	virtual std::string imageFormat() const{return "RGB";}
	virtual unsigned int width() const{return m_width;}
	virtual unsigned int pitch() const{return m_pitch;}
	virtual unsigned int height() const{return m_height;}
	virtual unsigned int channels() const{return m_channels;}
	virtual unsigned int channelBits() const{return m_channelBits;}

	void setImage(unsigned char *buffer, unsigned int width, unsigned int height, unsigned int channles=3, unsigned int channelBits=8);
	void setImage(unsigned char *buffer, unsigned int width, unsigned int pitch, unsigned int height, unsigned int channles, unsigned int channelBits);
	void resize(unsigned int width, unsigned int height, unsigned int channles=3, unsigned int channelBits=8);
	void resize(unsigned int width, unsigned int pitch, unsigned int height, unsigned int channles, unsigned int channelBits);

	virtual bool save(std::string fileName);

private:
	void freeBuffer();

	bool m_externalBuffer;
	unsigned char *m_buffer;
	size_t m_size;

	unsigned int m_width, m_pitch;
	unsigned int m_height;

	unsigned int m_channels;
	unsigned int m_channelBits;
};
typedef boost::shared_ptr<ImageSample> SharedImageSample;

}//namespace Limitless

#endif //_ImageSample_h_