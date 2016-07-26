#include "Overlay.h"

#include "Media/GPUContext.h"
#include "Media/MediaSampleFactory.h"
#include "Media/ImageSample.h"
#include "Media/ImageSampleSet.h"
#include "Media/GpuImageSample.h"
#include "Media/GpuImageSampleSet.h"

#include <boost/filesystem.hpp>
#include <boost/math/constants/constants.hpp>

#include <png.h>

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
	m_gpuImageSampleId=MediaSampleFactory::getTypeId("GpuImageSample");

    addAttribute("enable", m_enabled);
	addAttribute("location", "");
		
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
	m_overlayImage.reset(new Limitless::GpuImageSample());

	FILE *file=fopen(location.c_str(), "rb");

	if(!file)
		return;

	char header[8];
	
	fread(header, 1, 8, file);

	if(png_sig_cmp((png_const_bytep)header, 0, 8))
		return;

	png_structp png_ptr;

	/* initialize stuff */
	png_ptr=png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);

	if(!png_ptr)
		return;

	png_infop info_ptr;

	info_ptr=png_create_info_struct(png_ptr);
	if(!info_ptr)
		return;

	if(setjmp(png_jmpbuf(png_ptr)))
		return;

	int width, height;
	png_byte color_type;
	png_byte bit_depth;
	int number_of_passes;
	
	png_init_io(png_ptr, file);
	png_set_sig_bytes(png_ptr, 8);
	png_read_info(png_ptr, info_ptr);

	width=png_get_image_width(png_ptr, info_ptr);
	height=png_get_image_height(png_ptr, info_ptr);
	color_type=png_get_color_type(png_ptr, info_ptr);
	bit_depth=png_get_bit_depth(png_ptr, info_ptr);

	number_of_passes=png_set_interlace_handling(png_ptr);
	png_read_update_info(png_ptr, info_ptr);

	if(setjmp(png_jmpbuf(png_ptr)))
		return;

	int channels;

	if(color_type==PNG_COLOR_TYPE_GRAY)
		channels=1;
	else if(color_type==PNG_COLOR_TYPE_GRAY_ALPHA)
		channels=2;
	else if(color_type==PNG_COLOR_TYPE_RGB)
		channels=3;
	else if(color_type==PNG_COLOR_TYPE_RGB_ALPHA)
		channels=4;

	std::vector<png_bytep> row_pointers(height);
	std::vector<png_byte> imageData(width*height*channels);

	m_overlayImage->resize(width, height, channels, bit_depth);
		
	size_t pos=0;
	size_t stride=width*channels;

	for(int y=0; y<height; y++)
	{
		row_pointers[y]=&imageData[pos];
		pos+=stride;
	}

	png_read_image(png_ptr, row_pointers.data());

	cl::Event event;

	m_overlayImage->write((unsigned char *)imageData.data(), width, height, channels, event);
	event.wait();

	fclose(file);
	
}

bool Overlay::processSample(Limitless::SharedMediaPad sinkPad, Limitless::SharedMediaSample sample)
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

		cl_float xScale=(float)m_overlayImage->width()/gpuImageSample->width();
		cl_float yScale=(float)m_overlayImage->height()/gpuImageSample->height();
		
		std::vector<Limitless::GpuImageSample *> images(3);
		std::vector<cl::Event> imageAcquireEvent;
		cl::Event kernelEvent;

		images[0]=gpuImageSample.get();
		images[1]=dstSample.get();
		images[2]=m_overlayImage.get();

        cl::Event event;
		
        if(Limitless::GpuImageSample::acquireMultipleOpenCl(images, event))
            imageAcquireEvent.push_back(event);

        status=m_kernel.setArg(index++, gpuImageSample->glImage());
        status=m_kernel.setArg(index++, m_overlayImage->glImage());
		status=m_kernel.setArg(index++, dstSample->glImage());
		status=m_kernel.setArg(index++, xScale);
		status=m_kernel.setArg(index++, yScale);
		
        cl::NDRange globalThreads(gpuImageSample->width(), gpuImageSample->height());

		status=m_openCLComandQueue.enqueueNDRangeKernel(m_kernel, cl::NullRange, globalThreads, cl::NullRange, &imageAcquireEvent, &kernelEvent);

        if(status!=CL_SUCCESS)
        {
            assert(false);
            return false;
        }

		kernelEvent.wait();

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

void Overlay::initOpenCl()
{
	extern std::string overlay_cl;

    cl::Program::Sources programSource(1, std::make_pair(overlay_cl.data(), overlay_cl.size()));
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
    m_kernel=cl::Kernel(program, "overlay");

    m_kernelWorkGroupSize=m_kernel.getWorkGroupInfo<CL_KERNEL_WORK_GROUP_SIZE>(m_openCLDevice);

    if(m_blockSizeX > m_kernelWorkGroupSize)
    {
	    m_blockSizeX = m_kernelWorkGroupSize;
	    m_blockSizeY = 1;
    }
	
	m_openCLInitialized=true;
}
