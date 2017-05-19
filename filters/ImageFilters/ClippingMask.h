#ifndef _Limitless_ClippingMask_h_
#define _Limitless_ClippingMask_h_

#include "Media/MediaPluginFactory.h"
#include "Media/IMediaFilter.h"
#include "Media/ImageSample.h"
#include "Media/GPUImageSample.h"

class ClippingMask:public Limitless::MediaAutoRegister<ClippingMask, Limitless::IMediaFilter>
{
public:
	ClippingMask(std::string name, Limitless::SharedMediaFilter parent);
	virtual ~ClippingMask();

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

private:
    void initOpenCl();

    Limitless::ImageSample m_clippingMask;

	size_t m_imageSampleId;
	size_t m_gpuImageSampleId;
	
	bool m_openCLInitialized;
	cl::Device m_openCLDevice;
	cl::Context m_openCLContext;
	cl::CommandQueue m_openCLComandQueue;

	size_t m_kernelWorkGroupSize;
	size_t m_blockSizeX;
	size_t m_blockSizeY;
	cl::Kernel m_kernel;

    bool m_enabled;
};

#endif //_Limitless_ClippingMask_h_
