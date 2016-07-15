#ifndef _Limitless_MoveToCpu_h_
#define _Limitless_MoveToCpu_h_

#include "Media/MediaPluginFactory.h"
#include "Media/IMediaFilter.h"
#include "Media/ImageSample.h"
#include "Media/ImageSampleSet.h"
#include "Media/GPUImageSample.h"

class MoveToCpu:public Limitless::MediaAutoRegister<MoveToCpu, Limitless::IMediaFilter>
{
public:
	MoveToCpu(std::string name, Limitless::SharedMediaFilter parent);
	~MoveToCpu();

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
	size_t m_interfaceImageSampleId;
	size_t m_imageSampleSetId;
	size_t m_gpuImageSampleId;
	size_t m_imageSampleId;

	size_t m_sampleSetBin;
};

#endif //_Limitless_MoveToCpu_h_
