#ifndef _Limitless_Overlay_h_
#define _Limitless_Overlay_h_

#include "Media/MediaPluginFactory.h"
#include "Media/IMediaFilter.h"
#include "Media/ImageSample.h"
#include "Media/GPUImageSample.h"

class Overlay:public Limitless::MediaAutoRegister<Overlay, Limitless::IMediaFilter>
{
public:
	Overlay(std::string name, Limitless::SharedMediaFilter parent);
	virtual ~Overlay();

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
	void loadImage(std::string location);

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
	Limitless::SharedGpuImageSample m_overlayImage;
};

#endif //_Limitless_Overlay_h_
