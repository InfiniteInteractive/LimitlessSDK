#include "ClippingMask.h"

#include "Media/GPUContext.h"
#include "Media/MediaSampleFactory.h"
#include "Media/ImageSample.h"
#include "Media/ImageSampleSet.h"
#include "Media/GpuImageSample.h"
#include "Media/GpuImageSampleSet.h"

using namespace Limitless;

ClippingMask::ClippingMask(std::string name, Limitless::SharedMediaFilter parent):
MediaAutoRegister(name, parent),
m_enabled(false)
{
}

ClippingMask::~ClippingMask()
{
}

bool ClippingMask::initialize(const Attributes &attributes)
{
	m_imageSampleId=MediaSampleFactory::getTypeId("ImageSample");
	m_gpuImageSampleId=MediaSampleFactory::getTypeId("GpuImageSample");

    addAttribute("enable", m_enabled);
		
	std::string sinkPadDescription;

	sinkPadDescription="[";
	sinkPadDescription+="{\"mime\":\"image/gpu\"}";
	sinkPadDescription+=", {\"mime\":\"video/gpu\"}";
//	sinkPadDescription+=", {\"mime\":\"image\"}";
//	sinkPadDescription+=", {\"mime\":\"video\"}";
	sinkPadDescription+="]";

	addSinkPad("Sink", sinkPadDescription);
	addSourcePad("Source", sinkPadDescription);

	m_openCLContext=GPUContext::openCLContext();
	m_openCLComandQueue=GPUContext::openCLCommandQueue();
	m_openCLDevice=GPUContext::openCLDevice();

    initOpenCl();
	return true;
}

SharedPluginView ClippingMask::getView()
{
	return SharedPluginView();
}

IMediaFilter::StateChange ClippingMask::onReady()
{
	return IMediaFilter::SUCCESS;
}

IMediaFilter::StateChange ClippingMask::onPaused()
{
	return IMediaFilter::SUCCESS;
}

IMediaFilter::StateChange ClippingMask::onPlaying()
{
	return IMediaFilter::SUCCESS;
}

bool ClippingMask::onAcceptMediaFormat(Limitless::SharedMediaPad pad, Limitless::SharedMediaFormat format)
{
	return true;
}

void ClippingMask::onLinkFormatChanged(Limitless::SharedMediaPad mediaPad, Limitless::SharedMediaFormat format)
{
	if(mediaPad->type() == MediaPad::SOURCE)
	{
	}
}

void ClippingMask::onAttributeChanged(std::string name, Limitless::SharedAttribute attribute)
{
    if(name=="enable")
        m_enabled=attribute->toBool();
}

bool ClippingMask::processSample(Limitless::SharedMediaPad sinkPad, Limitless::SharedMediaSample sample)
{
	if(!m_enabled)
		pushSample(sample);

	if(sample->isType(m_gpuImageSampleId))
	{
        cl_int status;
		cl_uint index=0;
		Limitless::SharedGpuImageSample gpuImageSample=boost::dynamic_pointer_cast<GpuImageSample>(sample);
		Limitless::SharedGpuImageSample dstSample=newSampleType<Limitless::GpuImageSample>(m_gpuImageSampleId);

		dstSample->resize(gpuImageSample->width(), gpuImageSample->height(), gpuImageSample->channels());

//		cl_float xScale=(float)m_ClippingMaskImage->width()/gpuImageSample->width();
//		cl_float yScale=(float)m_ClippingMaskImage->height()/gpuImageSample->height();
//		
//		std::vector<Limitless::GpuImageSample *> images(3);
//		std::vector<cl::Event> imageAcquireEvent;
//		cl::Event kernelEvent;
//
//		images[0]=gpuImageSample.get();
//		images[1]=dstSample.get();
//		images[2]=m_ClippingMaskImage.get();
//
//        cl::Event event;
//		
//        if(Limitless::GpuImageSample::acquireMultipleOpenCl(images, event))
//            imageAcquireEvent.push_back(event);
//
//        status=m_kernel.setArg(index++, gpuImageSample->glImage());
//        status=m_kernel.setArg(index++, m_ClippingMaskImage->glImage());
//		status=m_kernel.setArg(index++, dstSample->glImage());
//		status=m_kernel.setArg(index++, xScale);
//		status=m_kernel.setArg(index++, yScale);
//		
//        cl::NDRange globalThreads(gpuImageSample->width(), gpuImageSample->height());
//
//		status=m_openCLComandQueue.enqueueNDRangeKernel(m_kernel, cl::NullRange, globalThreads, cl::NullRange, &imageAcquireEvent, &kernelEvent);
//
//        if(status!=CL_SUCCESS)
//        {
//            assert(false);
//            return false;
//        }
//
//		kernelEvent.wait();

		pushSample(dstSample);
		return true;
	}
//	else if(sample->isType(m_imageSampleId))
//	{
//		ImageSample *imageSample=dynamic_cast<ImageSample *>(sample.get());
//
//		imageSamples.push_back(imageSample);
//	}
	
    pushSample(sample);
	return true;
}

void ClippingMask::initOpenCl()
{
//	extern std::string ClippingMask_cl;
//
//    cl::Program::Sources programSource(1, std::make_pair(ClippingMask_cl.data(), ClippingMask_cl.size()));
//    cl::Program program=cl::Program(m_openCLContext, programSource);
//
//    cl_int error=CL_SUCCESS;
//
//    std::vector<cl::Device> programDevices;
//
//    programDevices.push_back(m_openCLDevice);
//    error=program.build(programDevices);
//    if(error != CL_SUCCESS)
//    {
//	    if(error == CL_BUILD_PROGRAM_FAILURE)
//	    {
//		    std::string str=program.getBuildInfo<CL_PROGRAM_BUILD_LOG>(m_openCLDevice);
//
//		    OutputDebugStringA(" \n\t\t\tBUILD LOG\n");
//		    OutputDebugStringA(" ************************************************\n");
//		    OutputDebugStringA(str.c_str());
//		    OutputDebugStringA(" ************************************************\n");
//	    }
//    }
//    m_kernel=cl::Kernel(program, "ClippingMask");
//
//    m_kernelWorkGroupSize=m_kernel.getWorkGroupInfo<CL_KERNEL_WORK_GROUP_SIZE>(m_openCLDevice);
//
//    if(m_blockSizeX > m_kernelWorkGroupSize)
//    {
//	    m_blockSizeX = m_kernelWorkGroupSize;
//	    m_blockSizeY = 1;
//    }
	
	m_openCLInitialized=true;
}
