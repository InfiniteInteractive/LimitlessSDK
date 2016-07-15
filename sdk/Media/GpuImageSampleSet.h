#ifndef _GpuImageSampleSet_h_
#define _GpuImageSampleSet_h_

#include <boost/shared_ptr.hpp>
#include "Media/media_define.h"
#include "Media/MediaSampleFactory.h"
#include "Media/IImageSampleSet.h"
#include "Media/GPUImageSample.h"

/*
namespace Limitless
{

#pragma warning(push)
#pragma warning(disable:4251)

class MEDIA_EXPORT GpuImageSampleSet:public AutoRegisterMediaSample<GpuImageSampleSet, IImageSampleSet>
{
public:
	GpuImageSampleSet();
	virtual ~GpuImageSampleSet();

//MediaSample
	virtual unsigned char *buffer(){return NULL;}
	virtual size_t size() const{return 0;}

//IImageSampleSet
	virtual void addSample(SharedIImageSample sample);
	virtual SharedIImageSample &sample(size_t index);
	virtual void setSample(size_t index, SharedIImageSample imageSample);
	virtual size_t sampleSetSize();
	virtual bool sampleSetEmpty();
	virtual void setSampleSetSize(size_t size);
	virtual void clearSampleSet();

private:
	std::vector<SharedGpuImageSample> m_gpuImageSamples;
};
typedef boost::shared_ptr<GpuImageSampleSet> SharedGpuImageSampleSet;

#pragma warning(pop)

}//namespace Limitless

*/
#endif //_GpuImageSampleSet_h_