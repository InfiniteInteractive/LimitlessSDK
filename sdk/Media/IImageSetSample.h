#ifndef _IImageSampleSet_h_
#define _IImageSampleSet_h_

#include "Media/MediaSample.h"
#include "Media/IImageSample.h"
#include "Media/media_define.h"

#include <boost/shared_ptr.hpp>

namespace Limitless
{

//class MEDIA_EXPORT IImageSampleSet:public MediaSample
class MEDIA_EXPORT IImageSampleSet:public AutoRegisterMediaSample<IImageSampleSet, MediaSample, true>
{
public:
	IImageSampleSet() {};
	virtual ~IImageSampleSet() {};

//	virtual SharedIImageSample operator[](size_t index)=0;
	virtual void addSample(SharedIImageSample sample)=0;
	virtual SharedIImageSample sample(size_t index)=0;
	virtual size_t sampleSetSize()=0;
	virtual bool sampleSetEmpty()=0;
	virtual void setSampleSetSize(size_t size)=0;
	virtual void clearSampleSet()=0;
};

typedef boost::shared_ptr<IImageSampleSet> SharedIImageSampleSet;
}//namespace Limitless

#endif //_IImageSampleSet_h_