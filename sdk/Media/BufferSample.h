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
    virtual size_t buffers() { return 1; }
    virtual unsigned char *buffer(size_t index=0) { if(index==0) return m_buffer; return nullptr; }
    virtual size_t bufferSize(size_t index=0) { if(index==0) return m_size; return 0; }
    virtual size_t size() const { return m_size; }
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