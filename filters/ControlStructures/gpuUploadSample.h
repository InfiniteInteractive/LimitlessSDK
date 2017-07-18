#ifndef _GPUImageUploadSample_h_
#define _GPUImageUploadSample_h_

#include "Media/media_define.h"
#include "Media/GPUImageSample.h"
#include "Media/MediaSampleFactory.h"

//#include <GL/glew.h>
//#include <gl/gl.h>
#include <CL/cl.hpp>

//namespace Limitless
//{

//class GpuUploadSample:public Limitless::AutoRegisterMediaSample<GpuUploadSample, Limitless::MediaSample>
//{
//public:
//	GpuUploadSample();
//	virtual ~GpuUploadSample();
//
////MediaSample interface
//	virtual unsigned char *buffer() { return m_sample->buffer(); }
//	virtual size_t size() const { return m_sample->size(); }
//
//	void setSample(Limitless::SharedMediaSample sample, cl::Event &event);
//	Limitless::SharedMediaSample sample() { return m_sample; }
//	void clearSample();
//	cl::Event event() { return m_event; }
//
//private:
//	Limitless::SharedMediaSample m_sample;
//	cl::Event m_event;
//};
//typedef boost::shared_ptr<GpuUploadSample> SharedGpuUploadSample;

//}//namespace Limitless

#endif //_GPUImageUploadSample_h_