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
    virtual size_t buffers() { return 1; }
    virtual unsigned char *buffer(size_t index=0) const { if(index==0) return m_buffer; return nullptr; }
    virtual unsigned char *buffer(size_t index=0) { if(index==0) return m_buffer; return nullptr; }
    virtual size_t bufferSize(size_t index=0) { if(index==0) return m_size; return 0; }
    virtual size_t size() const { return m_size;}

//IImageSample
	virtual std::string imageFormat() const{return m_imageFormat;}
	virtual unsigned int width() const{return m_width;}
	virtual unsigned int pitch() const{return m_pitch;}
	virtual unsigned int height() const{return m_height;}
	virtual unsigned int channels() const{return m_channels;}
	virtual unsigned int channelBits() const{return m_channelBits;}

    void setImageFormat(std::string &format) { m_imageFormat=format; }
	void setImage(unsigned char *buffer, unsigned int width, unsigned int height, unsigned int channels=3, unsigned int channelBits=8);
	void setImage(unsigned char *buffer, unsigned int width, unsigned int pitch, unsigned int height, unsigned int channles, unsigned int channelBits);
	void resize(unsigned int width, unsigned int height, unsigned int channels=3, unsigned int channelBits=8);
	void resize(unsigned int width, unsigned int pitch, unsigned int height, unsigned int channles, unsigned int channelBits);
    void resizeBuffer(unsigned int width, unsigned int height, unsigned int channels, unsigned int channelBits, unsigned int size);

    void copy(ImageSample *imageSample);

	virtual bool save(std::string fileName);

private:
	void freeBuffer();

	bool m_externalBuffer;
	unsigned char *m_buffer;
	size_t m_size;
    size_t m_actualSize;

    std::string m_imageFormat;
	unsigned int m_width, m_pitch;
	unsigned int m_height;

	unsigned int m_channels;
	unsigned int m_channelBits;
};
typedef boost::shared_ptr<ImageSample> SharedImageSample;

}//namespace Limitless

#endif //_ImageSample_h_