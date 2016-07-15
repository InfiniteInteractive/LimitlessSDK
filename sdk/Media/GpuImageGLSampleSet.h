#ifndef _GpuImageGLSampleSet_h_
#define _GpuImageGLSampleSet_h_

#include <boost/shared_ptr.hpp>
#include "Media/media_define.h"
#include "Media/MediaSampleFactory.h"
#include "Media/IImageSetSample.h"
#include "Media/GPUImageGLSample.h"

namespace Limitless
{

#pragma warning(push)
#pragma warning(disable:4251)

	class MEDIA_EXPORT GpuImageGLSampleSet:public AutoRegisterMediaSample<GpuImageGLSampleSet, IImageSampleSet>
	{
	public:
		GpuImageGLSampleSet();
		virtual ~GpuImageGLSampleSet();

		//MediaSample
		virtual unsigned char *buffer() { return NULL; }
		virtual size_t size() const { return 0; }

		//IImageSampleSet
		virtual void addSample(SharedIImageSample sample);
		virtual SharedIImageSample sample(size_t index);
		virtual size_t sampleSetSize();
		virtual bool sampleSetEmpty();
		virtual void setSampleSetSize(size_t size);
		virtual void clearSampleSet();

	private:
		std::vector<SharedGPUImageGLSample> m_gpuImageSamples;
	};
	typedef boost::shared_ptr<GpuImageGLSampleSet> SharedGpuImageGLSampleSet;

#pragma warning(pop)

}//namespace Limitless

#endif //_GpuImageGLSampleSet_h_