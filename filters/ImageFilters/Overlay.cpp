#include "Overlay.h"

#include "Media/GPUContext.h"
#include "Media/MediaSampleFactory.h"
#include "Media/ImageSample.h"
#include "Media/ImageSampleSet.h"
#include "Media/GpuImageSample.h"
#include "Media/GpuImageSampleSet.h"

#include <boost/filesystem.hpp>
#include <boost/math/constants/constants.hpp>

using namespace Limitless;

Overlay::Overlay(std::string name, Limitless::SharedMediaFilter parent):
MediaAutoRegister(name, parent),
m_enabled(false)
{
}

Overlay::~Overlay()
{
}

bool Overlay::initialize(const Attributes &attributes)
{
	m_imageSampleId=MediaSampleFactory::getTypeId("ImageSample");
	m_gpuImageSampleId=MediaSampleFactory::getTypeId("GPUImageSample");

    addAttribute("enable", m_enabled);
	addAttribute("location", "");
		
	std::string sinkPadDescription;

	sinkPadDescription+="{\"mime\":\"image/gpu\"}";
	sinkPadDescription+=", {\"mime\":\"video/gpu\"}";
	sinkPadDescription+="{\"mime\":\"image\"}";
	sinkPadDescription+=", {\"mime\":\"video\"}";
	sinkPadDescription+="]";

	addSinkPad("Sink", sinkPadDescription);
	addSinkPad("Source", sinkPadDescription);

	m_openCLContext=GPUContext::openCLContext();
	m_openCLComandQueue=GPUContext::openCLCommandQueue();

    initOpenCl();
	return true;
}

SharedPluginView Overlay::getView()
{
	return SharedPluginView();
}

IMediaFilter::StateChange Overlay::onReady()
{
	return IMediaFilter::SUCCESS;
}

IMediaFilter::StateChange Overlay::onPaused()
{
	return IMediaFilter::SUCCESS;
}

IMediaFilter::StateChange Overlay::onPlaying()
{
	return IMediaFilter::SUCCESS;
}

bool Overlay::onAcceptMediaFormat(Limitless::SharedMediaPad pad, Limitless::SharedMediaFormat format)
{
	return true;
}

void Overlay::onLinkFormatChanged(Limitless::SharedMediaPad mediaPad, Limitless::SharedMediaFormat format)
{
	if(mediaPad->type() == MediaPad::SOURCE)
	{
	}
}

void Overlay::onAttributeChanged(std::string name, Limitless::SharedAttribute attribute)
{
    if(name=="location")
        loadImage(attribute->toString());
    else if(name=="enable")
        m_enabled=attribute->toBool();
}

void Overlay::loadImage(std::string location)
{
}

bool Overlay::processSample(Limitless::SharedMediaPad sinkPad, Limitless::SharedMediaSample sample)
{
	std::vector<ImageSample *> imageSamples;
	std::vector<SharedGpuImageSample> gpuImageSamples;
	std::vector<std::vector<cl::Event> > sampleEvents;
	
	if(sample->isType(m_gpuImageSampleId))
	{
        cl_int status;
		SharedGpuImageSample gpuImageSample=boost::dynamic_pointer_cast<GpuImageSample>(sample);


        status=m_kernel.setArg(0, gpuImageSample->glImage());
        status=m_kernel.setArg(1, m_overlayImage->glImage());


        cl::NDRange globalThreads(gpuImageSample->width(), gpuImageSample->height());

            status=m_openCLComandQueue.enqueueNDRangeKernel(m_kernel, cl::NullRange, globalThreads, cl::NullRange, &sampleEvents[i], &kernelEvents[i]);
        }
	}
	else if(sample->isType(m_imageSampleId))
	{
		ImageSample *imageSample=dynamic_cast<ImageSample *>(sample.get());

		imageSamples.push_back(imageSample);
	}
	
    pushSample(sample);

	return true;
}

void Overlay::initOpenCl()
{
	extern std::string colorConversion_cl;

    cl::Program::Sources programSource(1, std::make_pair(colorConversion_cl.data(), colorConversion_cl.size()));
    cl::Program program=cl::Program(m_openCLContext, programSource);

    cl_int error=CL_SUCCESS;

    std::vector<cl::Device> programDevices;

    programDevices.push_back(m_openCLDevice);
    error=program.build(programDevices);
    if(error != CL_SUCCESS)
    {
	    if(error == CL_BUILD_PROGRAM_FAILURE)
	    {
		    std::string str=program.getBuildInfo<CL_PROGRAM_BUILD_LOG>(m_openCLDevice);

		    OutputDebugStringA(" \n\t\t\tBUILD LOG\n");
		    OutputDebugStringA(" ************************************************\n");
		    OutputDebugStringA(str.c_str());
		    OutputDebugStringA(" ************************************************\n");
	    }
    }
    std::string kernelName=getKernelName(m_fromFormat, m_toFormat);

    m_kernel=cl::Kernel(program, kernelName.c_str());

    m_kernelWorkGroupSize=m_kernel.getWorkGroupInfo<CL_KERNEL_WORK_GROUP_SIZE>(m_openCLDevice);

    if(m_blockSizeX > m_kernelWorkGroupSize)
    {
	    m_blockSizeX = m_kernelWorkGroupSize;
	    m_blockSizeY = 1;
    }
	
	m_openCLInitialized=true;
}
