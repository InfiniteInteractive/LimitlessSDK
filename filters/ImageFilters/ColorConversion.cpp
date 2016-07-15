#include "ColorConversion.h"

#include "Media/GPUContext.h"
#include "Media/MediaSampleFactory.h"
#include "Media/ImageSample.h"
#include "Media/ImageSampleSet.h"
#include "Media/GpuImageSample.h"
#include "Media/GpuImageSampleSet.h"

#include <boost/filesystem.hpp>
#include <boost/math/constants/constants.hpp>

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

	if(iter == formatMap.end())
	{
		return iter->first;
	}
	return Type::Unknown;
}

ColorFormat::ColorFormatMap &ColorFormat::getFormatMap()
{
	static ColorFormatMap formatMap;

	if(formatMap.empty())
	{
		formatMap.insert(ColorFormatMap::value_type(Type::RGB8, "RGB8"));
		formatMap.insert(ColorFormatMap::value_type(Type::RGB10, "RGB10"));
		formatMap.insert(ColorFormatMap::value_type(Type::RGB12, "RGB12"));
		formatMap.insert(ColorFormatMap::value_type(Type::YUV4, "YUV4"));
		formatMap.insert(ColorFormatMap::value_type(Type::YUV422, "YUV422"));
		formatMap.insert(ColorFormatMap::value_type(Type::YUV420, "YUV420"));	
	}

	return formatMap;
}

ColorConversion::ColorConversion(std::string name, Limitless::SharedMediaFilter parent):
MediaAutoRegister(name, parent),
m_fromFormat(ColorFormat::Type::Unknown),
m_toFormat(ColorFormat::Type::Unknown)
{
	TypeKernelMap typeMap;

	typeMap.insert(TypeKernelMap::value_type(ColorFormat::Type::RGB8, "Yuv422ToRgb8"));
	
	m_kernelMap.insert(KernelNameMap::value_type(ColorFormat::Type::YUV422, typeMap));
}

ColorConversion::~ColorConversion()
{
}

bool ColorConversion::initialize(const Attributes &attributes)
{
	m_imageSampleId=MediaSampleFactory::getTypeId("ImageSample");
	m_imageSampleSetId=MediaSampleFactory::getTypeId("ImageSampleSet");
	m_gpuImageSampleId=MediaSampleFactory::getTypeId("GPUImageSample");
	m_gpuImageSampleSetId=MediaSampleFactory::getTypeId("GPUImageSampleSet");

	std::vector<std::string> m_colorFormats;

	m_colorFormats.push_back(ColorFormat::toString(ColorFormat::Type::RGB8));

	m_toFormat=ColorFormat::Type::RGB8;
	addAttribute("conversionColor", m_colorFormats[0], m_colorFormats);
		
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
	return true;
}

void ColorConversion::onLinkFormatChanged(Limitless::SharedMediaPad mediaPad, Limitless::SharedMediaFormat format)
{
	if(mediaPad->type() == MediaPad::SOURCE)
	{
		SharedMediaFormat mediaFormat=mediaPad->format();

		if(mediaFormat->exists("colorFormat"))
		{
			m_fromFormat=ColorFormat::toType(mediaFormat->attribute("colorFormat")->toString());
		}
	}
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
	std::vector<ImageSample *> imageSamples;
	std::vector<SharedGpuImageSample> gpuImageSamples;
	std::vector<std::vector<cl::Event> > sampleEvents;
	
	if(sample->isType(m_gpuImageSampleId))
	{
		SharedGpuImageSample gpuImageSample=boost::dynamic_pointer_cast<GpuImageSample>(sample);
				
		gpuImageSamples.push_back(gpuImageSample);
		sampleEvents.push_back(std::vector<cl::Event>());
	}
	else if(sample->isType(m_gpuImageSampleSetId))
	{
//		GpuImageSampleSet *imageSampleSet=dynamic_cast<GpuImageSampleSet *>(sample.get());
//
//		for(size_t i=0; i<imageSampleSet->sampleSetSize(); ++i)
//		{
//		}
	}
	else if(sample->isType(m_imageSampleId))
	{
		ImageSample *imageSample=dynamic_cast<ImageSample *>(sample.get());

		imageSamples.push_back(imageSample);
	}
	else if(sample->isType(m_imageSampleSetId))
	{
		ImageSampleSet *imageSampleSet=dynamic_cast<ImageSampleSet *>(sample.get());

		for(size_t i=0; i<imageSampleSet->sampleSetSize(); ++i)
		{
			SharedIImageSample iImageSample=imageSampleSet->sample(i);

			if(iImageSample->isType(m_imageSampleId))
			{
				ImageSample *imageSample=dynamic_cast<ImageSample *>(iImageSample.get());

				imageSamples.push_back(imageSample);
			}
			else if(iImageSample->isType(m_gpuImageSampleId))
			{
				SharedGpuImageSample gpuImageSample=boost::dynamic_pointer_cast<GpuImageSample>(iImageSample);
				
				gpuImageSamples.push_back(gpuImageSample);
				sampleEvents.push_back(std::vector<cl::Event>());
			}
		}
	}

	if(!imageSamples.empty())
	{
		for(size_t i=0; i<imageSamples.size(); ++i)
		{
			ImageSample *imageSample=imageSamples[i];
			SharedGpuImageSample gpuImageSample=boost::dynamic_pointer_cast<GpuImageSample>(newSample(m_gpuImageSampleId));

			std::vector<cl::Event> events(1);

			gpuImageSample->write(imageSample->buffer(), imageSample->width(), imageSample->height(), events[0], nullptr);
			gpuImageSamples.push_back(gpuImageSample);
			sampleEvents.push_back(events);

		}
	}

	if(gpuImageSamples.empty())
		return false;
	
	std::vector<SharedGpuImageSample> newGpuImageSamples;
	std::vector<cl::Event> acquireEvents(gpuImageSamples.size());
	std::vector<cl::Memory> glImages(1);
	
	for(size_t i; i<gpuImageSamples.size(); ++i)
	{
		GpuImageSample *gpuImageSample=gpuImageSamples[i].get();
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

	for(size_t i; i<gpuImageSamples.size(); ++i)
	{
		GpuImageSample *gpuImageSample=gpuImageSamples[i].get();
		SharedGpuImageSample newGpuImageSample=boost::dynamic_pointer_cast<GpuImageSample>(newSample(m_gpuImageSampleId));
		
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
		pushSample(sinkPad->linkedPad(), newGpuImageSamples[0]);
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
		std::string kernelName=getKernelName(m_fromFormat, m_toFormat);

		m_kernel=cl::Kernel(program, kernelName.c_str());

		m_kernelWorkGroupSize=m_kernel.getWorkGroupInfo<CL_KERNEL_WORK_GROUP_SIZE>(m_openCLDevice);

		if(m_blockSizeX > m_kernelWorkGroupSize)
		{
			m_blockSizeX = m_kernelWorkGroupSize;
			m_blockSizeY = 1;
		}
	}
	
	m_openCLInitialized=true;
}

std::string ColorConversion::getKernelName(ColorFormat::Type from, ColorFormat::Type to)
{
	KernelNameMap::iterator iterKernelMap=m_kernelMap.find(from);

	if(iterKernelMap == m_kernelMap.end())
		return "doNothing";

	TypeKernelMap &typeKernelMap=iterKernelMap->second;

	TypeKernelMap::iterator iterTypeKernelMap=typeKernelMap.find(from);
	
	if(iterTypeKernelMap == typeKernelMap.end())
		return "doNothing";

	return iterTypeKernelMap->second;
}