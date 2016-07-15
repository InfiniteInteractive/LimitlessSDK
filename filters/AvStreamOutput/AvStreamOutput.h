#ifndef _AvStreamOutput_h_
#define _AvStreamOutput_h_

#include "Media/MediaPluginFactory.h"
#include "Media/IMediaFilter.h"
//#include "imageviewer_global.h"
#include "AvStream/AvStreamServer.h"

class AvStreamOutput:public Limitless::MediaAutoRegister<AvStreamOutput, Limitless::IMediaFilter>
{
public:
	AvStreamOutput(std::string name, Limitless::SharedMediaFilter parent);
	~AvStreamOutput();

	virtual bool initialize(const Limitless::Attributes &attributes);
	virtual bool shutdown(){return true;}

	virtual Limitless::SharedPluginView getView();

	virtual bool processSample(Limitless::SharedMediaPad sinkPad, Limitless::SharedMediaSample sample);

protected:
	//IMediaFilter
	virtual StateChange onReady();
	virtual StateChange onPaused();
	virtual StateChange onPlaying();

	bool onAcceptMediaFormat(Limitless::SharedMediaPad pad, Limitless::SharedMediaFormat format);
	void onLinkFormatChanged(Limitless::SharedMediaPad pad, Limitless::SharedMediaFormat format);

private:
	Limitless::AvStreamServer m_streamServer;

	int m_width;
	int m_height;

	unsigned int m_imageSampleId;
	unsigned int m_imageSetSampleId;

	std::string m_fmleName;
};

namespace Limitless{namespace traits
{
	template<> struct category<AvStreamOutput>
	{
		static std::string get()
		{return "sink";}
	};
}}//Limitless::traits

#endif // _AvStreamOutput_h_
