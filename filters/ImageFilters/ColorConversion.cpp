#include "ColorConversion.h"

#include "Media/GPUContext.h"
#include "Media/MediaSampleFactory.h"
#include "Media/ImageSample.h"
#include "Media/ImageSampleSet.h"
#include "Media/GpuBufferSample.h"
#include "Media/GpuImageSample.h"
#include "Media/GpuImageSampleSet.h"

#include <boost/filesystem.hpp>
#include <boost/math/constants/constants.hpp>
#include <sstream>

using namespace Limitless;

std::string ColorFormat::toString(Type type)
{
	ColorFormatMap &formatMap=getFormatMap();

	ColorFormatMap::iterator iter=formatMap.find(type);

	if(iter != formatMap.end())
		return iter->second;
	return "Unknown";
}

ColorFormat::Type ColorFormat::toType(std::string type)
{
	ColorFormatMap &formatMap=getFormatMap();
	ColorFormatMap::iterator iter=std::find_if(formatMap.begin(), formatMap.end(),[&type](const ColorFormatMap::value_type& valueType){ return valueType.second == type;});

	if(iter != formatMap.end())
	{
		return iter->first;
	}
	return Type::Unknown;
}

bool ColorFormat::encoded(ColorFormat::Type type)
{
    switch(type)
    {
    case Type::RGB8:
    case Type::BGR8:
        return false;
        break;
    case Type::RGB10:
    case Type::RGB12:
    case Type::YUV4:
    case Type::YUV422:
    case Type::YUV420:
    case Type::YUV422P:
    case Type::YUV420P:
    case Type::YUVJ420P:
        return true;
        break;
    }
    return true;
}

ColorFormat::ColorFormatMap &ColorFormat::getFormatMap()
{
	static ColorFormatMap formatMap;

	if(formatMap.empty())
	{
		formatMap.insert(ColorFormatMap::value_type(Type::RGB8, "RGB8"));
		formatMap.insert(ColorFormatMap::value_type(Type::RGB10, "RGB10"));
		formatMap.insert(ColorFormatMap::value_type(Type::RGB12, "RGB12"));
        formatMap.insert(ColorFormatMap::value_type(Type::BGR8, "BGR8"));
		formatMap.insert(ColorFormatMap::value_type(Type::YUV4, "YUV4"));
		formatMap.insert(ColorFormatMap::value_type(Type::YUV422, "YUV422"));
		formatMap.insert(ColorFormatMap::value_type(Type::YUV420, "YUV420"));
        formatMap.insert(ColorFormatMap::value_type(Type::YUV422P, "YUV422P"));
        formatMap.insert(ColorFormatMap::value_type(Type::YUV420P, "YUV420P"));
        formatMap.insert(ColorFormatMap::value_type(Type::YUVJ420P, "YUVJ420P"));
	}

	return formatMap;
}

ColorConversion::ColorConversion(std::string name, Limitless::SharedMediaFilter parent):
MediaAutoRegister(name, parent),
m_passThrough(true),
m_fromFormat(ColorFormat::Type::Unknown),
m_toFormat(ColorFormat::Type::Unknown),
m_hasCommandQueue(false)
{
	TypeKernelMap typeYUV422Map;

    typeYUV422Map.insert(TypeKernelMap::value_type(ColorFormat::Type::RGB8, ColorKernelInfo("yuv422torgb", 2, 1)));
	m_kernelMap.insert(KernelNameMap::value_type(ColorFormat::Type::YUV422, typeYUV422Map));
    
//    TypeKernelMap typeYUV420Map;
//
//    typeYUV420Map.insert(TypeKernelMap::value_type(ColorFormat::Type::RGB8, "Yuv420ToRgb8"));
//    m_kernelMap.insert(KernelNameMap::value_type(ColorFormat::Type::YUV420, typeYUV420Map));

    TypeKernelMap typeYUV420PMap;
    
    typeYUV420PMap.insert(TypeKernelMap::value_type(ColorFormat::Type::RGB8, ColorKernelInfo("yuv420ptorgb", 2, 2)));
    m_kernelMap.insert(KernelNameMap::value_type(ColorFormat::Type::YUV420P, typeYUV420PMap));

    TypeKernelMap typeYUVJ420PMap;

    typeYUVJ420PMap.insert(TypeKernelMap::value_type(ColorFormat::Type::RGB8, ColorKernelInfo("yuvj420ptorgb", 2, 2)));
    m_kernelMap.insert(KernelNameMap::value_type(ColorFormat::Type::YUVJ420P, typeYUVJ420PMap));

    TypeKernelMap typeRGBMap;

    typeRGBMap.insert(TypeKernelMap::value_type(ColorFormat::Type::BGR8, ColorKernelInfo("rgbtobgr")));
    m_kernelMap.insert(KernelNameMap::value_type(ColorFormat::Type::RGB8, typeRGBMap));

    TypeKernelMap typeBGRMap;

    typeBGRMap.insert(TypeKernelMap::value_type(ColorFormat::Type::RGB8, ColorKernelInfo("bgrtorgb")));
    m_kernelMap.insert(KernelNameMap::value_type(ColorFormat::Type::BGR8, typeBGRMap));
}

ColorConversion::~ColorConversion()
{
}

bool ColorConversion::initialize(const Attributes &attributes)
{
	m_imageSampleId=MediaSampleFactory::getTypeId("IImageSample");
	m_imageSampleSetId=MediaSampleFactory::getTypeId("ImageSampleSet");
    m_gpuBufferSampleId=MediaSampleFactory::getTypeId("GpuBufferSample");
	m_gpuImageSampleId=MediaSampleFactory::getTypeId("GpuImageSample");
//	m_gpuImageSampleSetId=MediaSampleFactory::getTypeId("GpuImageSampleSet");

	std::vector<std::string> m_colorFormats;

	m_colorFormats.push_back(ColorFormat::toString(ColorFormat::Type::RGB8));

	m_toFormat=ColorFormat::Type::RGB8;
	addAttribute("conversionColor", m_colorFormats[0], m_colorFormats);
		
	std::string sinkPadDescription;

    sinkPadDescription+="[";
	sinkPadDescription+="{\"mime\":\"image/gpu\"}";
	sinkPadDescription+=", {\"mime\":\"image\"}";
    sinkPadDescription+=", {\"mime\":\"image/raw\"}";
	sinkPadDescription+="]";

	addSinkPad("Sink", sinkPadDescription);

    std::string sourcePadDescription;

    sourcePadDescription+="[";
    sourcePadDescription+="{\"mime\":\"image/gpu\"}";
    sourcePadDescription+="]";

    m_sourcePad=addSourcePad("Source", sourcePadDescription);

    m_openCLContext=GPUContext::openCLContext();
//	m_openCLComandQueue=GPUContext::openCLCommandQueue();
    m_openCLDevice=GPUContext::openCLDevice();

	return true;
}

SharedPluginView ColorConversion::getView()
{
	return SharedPluginView();
}

IMediaFilter::StateChange ColorConversion::onReady()
{
	return IMediaFilter::SUCCESS;
}

IMediaFilter::StateChange ColorConversion::onPaused()
{
	return IMediaFilter::SUCCESS;
}

IMediaFilter::StateChange ColorConversion::onPlaying()
{
	return IMediaFilter::SUCCESS;
}

bool ColorConversion::onAcceptMediaFormat(Limitless::SharedMediaPad pad, Limitless::SharedMediaFormat format)
{
    if(pad->type()==MediaPad::SOURCE)
    {
        if(format->exists("format"))
        {
            if(m_toFormat != ColorFormat::toType(format->attribute("format")->toString()))
                return false;
        }
        return true;
    }
    else
        return true;
}

void ColorConversion::onLinkFormatChanged(Limitless::SharedMediaPad mediaPad, Limitless::SharedMediaFormat format)
{
	if(mediaPad->type() == MediaPad::SINK)
	{
		if(format->exists("format"))
		{
			m_fromFormat=ColorFormat::toType(format->attribute("format")->toString());
            initOpenCL();
		}

        SharedMediaFormat originalFormat=boost::make_shared<MediaFormat>();
        SharedMediaFormat conversionFormat=boost::make_shared<MediaFormat>();

        if(format->exists("mime"))
            originalFormat->addAttribute("mime", format->attribute("mime")->toString());
        else
            originalFormat->addAttribute("mime", std::string("image/gpu"));

        if(format->exists("width"))
            originalFormat->addAttribute("width", format->attribute("width")->toInt());
        if(format->exists("height"))
            originalFormat->addAttribute("height", format->attribute("height")->toInt());
        if(format->exists("timeBaseNum"))
            originalFormat->addAttribute("timeBaseNum", format->attribute("timeBaseNum")->toInt());
        if(format->exists("timeBaseDen"))
            originalFormat->addAttribute("timeBaseDen", format->attribute("timeBaseDen")->toInt());

        conversionFormat=originalFormat;

        if(format->exists("format"))
            originalFormat->addAttribute("format", format->attribute("format")->toString());
        conversionFormat->addAttribute("format", ColorFormat::toString(m_toFormat));

        SharedMediaPads sourcePads=getSourcePads();

        for(SharedMediaPad &sourcePad:sourcePads)
        {
            sourcePad->setFormat(*conversionFormat.get());
        }
	}
//    else if(mediaPad->type()==MediaPad::SINK)
//    {
//        if(format->exists("format"))
//        {
//            m_toFormat=ColorFormat::toType(mediaFormat->attribute("format")->toString());
//            initOpenCL();
//        }
//    }
}

void ColorConversion::onAttributeChanged(std::string name, Limitless::SharedAttribute attribute)
{
	if(name == "conversionColor")
	{
		std::string colorFormat=attribute->toString();

		m_toFormat=ColorFormat::toType(colorFormat);
		initOpenCL();
	}	
}

//bool ColorConversion::process(GPUImageMedia *imageMedia1, GPUImageMedia *imageMedia2, GPUImageMedia *outputImageMedia, cl::Event &event)
bool ColorConversion::processSample(Limitless::SharedMediaPad sinkPad, Limitless::SharedMediaSample sample)
{
    if(!m_hasCommandQueue)
    {
        //commandqueue is maintained per thread, dont get it until it is from the thread its going to be used from
        m_openCLComandQueue=GPUContext::openCLCommandQueue();
        m_hasCommandQueue=true;
    }

    if(m_passThrough)
    {
        Limitless::SharedIImageSample imageSample=boost::dynamic_pointer_cast<Limitless::IImageSample>(sample);

        if(!imageSample)
            return true;

        if(imageSample->imageFormat() == ColorFormat::toString(m_toFormat))
            pushSample(m_sourcePad, sample);

        return true;
    }

	std::vector<IImageSample *> imageSamples;
	std::vector<GpuImageSample *> gpuImageSamples;
    std::vector<std::vector<cl::Event> > sampleEvents;

    struct GpuBuffer
    {
        GpuBuffer(GpuBufferSample *sample, size_t width, size_t height):sample(sample), width(width), height(height) {}

        size_t width, height;
        GpuBufferSample *sample;
    };
    std::vector<GpuBuffer> gpuBuffers;
	
	if(sample->isType(m_gpuImageSampleId))
	{
		SharedGpuImageSample gpuImageSample=boost::dynamic_pointer_cast<GpuImageSample>(sample);
				
		gpuImageSamples.push_back(gpuImageSample.get());
		sampleEvents.push_back(std::vector<cl::Event>());
	}
	else if(sample->isType(m_imageSampleId))
	{
		IImageSample *imageSample=dynamic_cast<IImageSample *>(sample.get());

		imageSamples.push_back(imageSample);
	}

	if(!imageSamples.empty())
	{
        ColorFormat::Type type=ColorFormat::toType(imageSamples[0]->imageFormat());

        if(ColorFormat::encoded(type))
        {//image has some type of encoding so they need to be used as buffers
            if(m_gpuCopyBuffers.size()<imageSamples.size())
                m_gpuCopyBuffers.resize(imageSamples.size());

            for(size_t i=0; i<imageSamples.size(); ++i)
            {
                IImageSample *imageSample=imageSamples[i];

                GpuBufferSample *gpuBufferSample=&m_gpuCopyBuffers[i];
                std::vector<cl::Event> events(1);

                gpuBufferSample->resize(imageSample->size());

                size_t bufferCount=imageSample->buffers();

                std::vector<unsigned char *> buffers(bufferCount);
                std::vector<size_t> sizes(bufferCount);

                for(size_t j=0; j<bufferCount; ++j)
                {
                    buffers[j]=imageSample->buffer(j);
                    sizes[j]=imageSample->bufferSize(j);
                }
                gpuBufferSample->write(buffers, sizes, events[0]);
//                gpuBufferSample->write(imageSample->buffer(), imageSample->size(), events[0]);
                gpuBuffers.push_back(GpuBuffer(gpuBufferSample, imageSample->width(), imageSample->height()));
                sampleEvents.push_back(events);
            }
        }
        else
        {
            if(m_gpuCopyImages.size()<imageSamples.size())
                m_gpuCopyImages.resize(imageSamples.size());

            for(size_t i=0; i<imageSamples.size(); ++i)
            {
                IImageSample *imageSample=imageSamples[i];

                Limitless::GpuImageSample *gpuImageSample=&m_gpuCopyImages[i];
                std::vector<cl::Event> events(1);

//                gpuImageSample->resize(imageSample->width(), imageSample->height(), imageSample->size());
                gpuImageSample->write(imageSample, events[0]);
                gpuImageSamples.push_back(gpuImageSample);
                sampleEvents.push_back(events);
            }
        }
	}

    if(gpuBuffers.empty()&&gpuImageSamples.empty())
    {
//        pushSample(m_originalSourcePad, newGpuImageSamples[0]);m
        return false;
    }
	
	std::vector<SharedGpuImageSample> newGpuImageSamples;
	std::vector<cl::Event> acquireEvents(gpuImageSamples.size());
	std::vector<cl::Memory> glImages(1);
	
    std::vector<cl::Event> kernelBufferEvents(gpuBuffers.size());

    for(size_t i=0; i<gpuBuffers.size(); ++i)
    {
        GpuBuffer &gpuBuffer=gpuBuffers[i];
        GpuBufferSample *gpuBufferSample=gpuBuffer.sample;
        SharedGpuImageSample newGpuImageSample=boost::dynamic_pointer_cast<GpuImageSample>(newSample(m_gpuImageSampleId));

        newGpuImageSample->resize(gpuBuffer.width, gpuBuffer.height, 4);
        newGpuImageSamples.push_back(newGpuImageSample);

        cl_int status;

        status=m_kernel.setArg(0, gpuBufferSample->glBuffer());
        status=m_kernel.setArg(1, (cl_uint)gpuBuffer.width);
        status=m_kernel.setArg(2, (cl_uint)gpuBuffer.height);
        status=m_kernel.setArg(3, newGpuImageSample->glImage());
        status=m_kernel.setArg(4, (cl_uint)gpuBuffer.width);
        status=m_kernel.setArg(5, (cl_uint)gpuBuffer.height);

        cl::NDRange globalThreads(gpuBuffer.width/m_kernerlDivisorX, gpuBuffer.height/m_kernerlDivisorY);

        status=m_openCLComandQueue.enqueueNDRangeKernel(m_kernel, cl::NullRange, globalThreads, cl::NullRange, &sampleEvents[i], &kernelBufferEvents[i]);
    }

	for(size_t i=0; i<gpuImageSamples.size(); ++i)
	{
		GpuImageSample *gpuImageSample=gpuImageSamples[i];
		cl::Event acquireEvent;

		glImages[0]=gpuImageSample->glImage();

		if(sampleEvents[i].empty())
			m_openCLComandQueue.enqueueAcquireGLObjects(&glImages, NULL, &acquireEvent);
		else
			m_openCLComandQueue.enqueueAcquireGLObjects(&glImages, &sampleEvents[i], &acquireEvent);
		sampleEvents[i].push_back(acquireEvent);
	}
	
	std::vector<cl::Event> kernelEvents(gpuImageSamples.size());
	cl_int status;

	for(size_t i=0; i<gpuImageSamples.size(); ++i)
	{
		GpuImageSample *gpuImageSample=gpuImageSamples[i];
		SharedGpuImageSample newGpuImageSample=boost::dynamic_pointer_cast<GpuImageSample>(newSample(m_gpuImageSampleId));
		
        newGpuImageSample->resize(gpuImageSample->width(), gpuImageSample->height(), 4);
		newGpuImageSamples.push_back(newGpuImageSample);

		status=m_kernel.setArg(0, gpuImageSample->glImage());
		status=m_kernel.setArg(1, gpuImageSample->width());
		status=m_kernel.setArg(2, gpuImageSample->height());
		status=m_kernel.setArg(3, newGpuImageSample->glImage());
		
		cl::NDRange globalThreads(gpuImageSample->width(), gpuImageSample->height());

		status=m_openCLComandQueue.enqueueNDRangeKernel(m_kernel, cl::NullRange, globalThreads, cl::NullRange, &sampleEvents[i], &kernelEvents[i]);
	}

	cl::WaitForEvents(kernelEvents);

	if(newGpuImageSamples.size() == 1)
	{
        pushSample(m_sourcePad, newGpuImageSamples[0]);
	}
	else
	{
//		SharedGpuImageSampleSet newGpuImageSampleSet=boost::dynamic_pointer_cast<GpuImageSampleSet>(newSample(m_gpuImageSampleSetId));
//
//		for(size_t i; i<newGpuImageSamples.size(); ++i)
//		{
//			newGpuImageSampleSet->addSample(newGpuImageSamples[i]);
//		}
//		pushSample(sinkPad->linkedPad(), newGpuImageSampleSet);
	}

	return true;
}

void ColorConversion::initOpenCL()
{
    if(m_fromFormat==m_toFormat)
    {
        m_passThrough=true;
        return;
    }

    m_passThrough=false;
//  	boost::filesystem::path kernelDirectory=boost::filesystem::current_path();
//
//	kernelDirectory/="kernels";
//	std::string kernelPath=kernelDirectory.string()+"/colorConversion.cl";
	extern std::string colorConversion_cl;
	
//	FILE *kernelFile=fopen(kernelPath.c_str(), "rb");
//	
//	if(kernelFile != NULL)
	{
//		size_t size;
//
//		fseek(kernelFile, 0L, SEEK_END);
//		size=ftell(kernelFile);
//		rewind(kernelFile);
//
//		std::string kernelSource;
//
//		kernelSource.resize(size);
//		fread((void *)kernelSource.data(), sizeof(char), size, kernelFile);
//		fclose(kernelFile);

//		cl::Program::Sources programSource(1, std::make_pair(kernelSource.data(), kernelSource.size()));
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
        ColorKernelInfo kernel=getKernelName(m_fromFormat, m_toFormat);

        if(kernel.name == "doNothing")
        {
            std::stringstream errorMsg;
            
            errorMsg<<"Conversion not not implemented for "<<ColorFormat::toString(m_fromFormat)<<" to "<<ColorFormat::toString(m_toFormat);
            Log::error("ColorConversion", errorMsg.str());
        }
		
        m_kernel=cl::Kernel(program, kernel.name.c_str());
        m_kernerlDivisorX=kernel.divisorX;
        m_kernerlDivisorY=kernel.divisorY;

		m_kernelWorkGroupSize=m_kernel.getWorkGroupInfo<CL_KERNEL_WORK_GROUP_SIZE>(m_openCLDevice);

		if(m_blockSizeX > m_kernelWorkGroupSize)
		{
			m_blockSizeX = m_kernelWorkGroupSize;
			m_blockSizeY = 1;
		}
	}
	
	m_openCLInitialized=true;
}

ColorKernelInfo ColorConversion::getKernelName(ColorFormat::Type from, ColorFormat::Type to)
{
	KernelNameMap::iterator iterKernelMap=m_kernelMap.find(from);

	if(iterKernelMap == m_kernelMap.end())
		return ColorKernelInfo("doNothing");

	TypeKernelMap &typeKernelMap=iterKernelMap->second;

	TypeKernelMap::iterator iterTypeKernelMap=typeKernelMap.find(to);
	
	if(iterTypeKernelMap == typeKernelMap.end())
		return ColorKernelInfo("doNothing");

	return iterTypeKernelMap->second;
}