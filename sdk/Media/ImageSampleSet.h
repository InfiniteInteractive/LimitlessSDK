#ifndef _ImageSampleSet_h_
#define _ImageSampleSet_h_

#include <boost/shared_ptr.hpp>
#include "Media/media_define.h"
#include "Media/MediaSampleFactory.h"
#include "Media/IImageSampleSet.h"
#include "Media/ImageSample.h"

namespace Limitless
{

#pragma warning(push)
#pragma warning(disable:4251)

class MEDIA_EXPORT ImageSampleSet:public AutoRegisterMediaSample<ImageSampleSet, IImageSampleSet>
{
public:
	ImageSampleSet();
	virtual ~ImageSampleSet();

//MediaSample
    virtual size_t buffers() { return 0; }
    virtual unsigned char *buffer(size_t index=0) { return nullptr; }
    virtual size_t bufferSize(size_t index=0) { return 0; }
	virtual size_t size() const{return 0;}

//IImageSampleSet
	virtual void addSample(SharedIImageSample sample);
	virtual SharedIImageSample &sample(size_t index);
	virtual size_t sampleSetSize();
	virtual bool sampleSetEmpty();
	virtual void setSampleSetSize(size_t size);
	virtual void clearSampleSet();

//	void resize(unsigned int count, int width, int height, int channels=3, int channelBits=8);
//	void resize(unsigned int count, int width, int pitch, int height, int channels, int channelBits);

//	void addSample(SharedIImageSample sample);
//	void clear();
	
private:
//	void freeBuffer();

//	std::vector<ImageSample *> m_imageSamples;
	std::vector<SharedIImageSample> m_imageSamples;
//	unsigned char *m_buffer;
//	int m_width;
//	int m_pitch;
//	int m_height;
//	size_t m_size;
//	int m_channels;
//	int m_channelBits;
};
typedef boost::shared_ptr<ImageSampleSet> SharedImageSampleSet;

#pragma warning(pop)

}//namespace Limitless

#endif //_ImageSampleSet_h_