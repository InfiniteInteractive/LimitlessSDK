#ifndef _ColorConversion_h_
#define _ColorConversion_h_

//#include <GL/glew.h>
//#include "CL/cl.hpp"

#include "Media/MediaPluginFactory.h"
#include "Media/IMediaFilter.h"
#include "Media/ImageSample.h"
#include "Media/GPUImageSample.h"

#include <unordered_map>


class ColorFormat
{
public:
	enum class Type
	{
		Unknown,
		RGB8,
		RGB10,
		RGB12,
		YUV4,
		YUV422,
		YUV420
	};
	typedef std::unordered_map<Type, std::string> ColorFormatMap;

	static std::string toString(Type type);
	static Type toType(std::string type);

private:
	static ColorFormatMap &ColorFormat::getFormatMap();
};

class ColorConversion:public Limitless::MediaAutoRegister<ColorConversion, Limitless::IMediaFilter>
{
public:
	ColorConversion(std::string name, Limitless::SharedMediaFilter parent);
	~ColorConversion();

//PluginObject interface
	virtual bool initialize(const Limitless::Attributes &attributes);
	virtual bool shutdown() { return true; }

	virtual Limitless::SharedPluginView getView();

//IMediaFilter interfacce
	virtual bool processSample(Limitless::SharedMediaPad sinkPad, Limitless::SharedMediaSample sample);

protected:
//IMediaFilter interfacce
	virtual StateChange onReady();
	virtual StateChange onPaused();
	virtual StateChange onPlaying();

	virtual bool onAcceptMediaFormat(Limitless::SharedMediaPad pad, Limitless::SharedMediaFormat format);
	virtual void onLinkFormatChanged(Limitless::SharedMediaPad mediaPad, Limitless::SharedMediaFormat format);

//PluginObject interfacce
	virtual void onAttributeChanged(std::string name, Limitless::SharedAttribute attribute);

	typedef std::unordered_map<ColorFormat::Type, std::string> TypeKernelMap;
	typedef std::unordered_map<ColorFormat::Type, TypeKernelMap> KernelNameMap;

	void initOpenCL();

private:
	std::string getKernelName(ColorFormat::Type from, ColorFormat::Type to);
	KernelNameMap m_kernelMap;

	ColorFormat::Type m_fromFormat;
	ColorFormat::Type m_toFormat;

	size_t m_imageSampleId;
	size_t m_imageSampleSetId;
	size_t m_gpuImageSampleId;
	size_t m_gpuImageSampleSetId;

	bool m_openCLInitialized;
	cl::Device m_openCLDevice;
	cl::Context m_openCLContext;
	cl::CommandQueue m_openCLComandQueue;

	size_t m_kernelWorkGroupSize;
	size_t m_blockSizeX;
	size_t m_blockSizeY;
	cl::Kernel m_kernel;
};

#endif //_ColorConversion_h_
