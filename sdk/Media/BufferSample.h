#ifndef _BufferSample_h_
#define _BufferSample_h_

#include "Media/media_define.h"
#include "Media/MediaSample.h"
#include "Media/MediaSampleFactory.h"

namespace Limitless
{

class MEDIA_EXPORT BufferSample:public AutoRegisterMediaSample<BufferSample, MediaSample>
{
public:
	BufferSample();
	BufferSample(unsigned char *buffer, size_t size);
	virtual ~BufferSample();

	void allocate(size_t size);
	//MediaSample
	virtual unsigned char *buffer(){return m_buffer;}
	virtual size_t size() const{return m_size;}
	virtual size_t actualSize() const{return m_actualSize;}

	void setSize(size_t newSize){m_size=newSize;}
private:
	void freeBuffer();

	bool m_externalBuffer;
	unsigned char *m_buffer;
	size_t m_size;
	size_t m_actualSize;
};

typedef boost::shared_ptr<BufferSample> SharedBufferSample;

}//namespace Limitless

#endif //_BufferSample_h_