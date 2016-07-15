#ifndef IMAGEVIEWER_H
#define IMAGEVIEWER_H

#include "Media/MediaPluginFactory.h"
#include "Media/IMediaFilter.h"
//#include "imageviewer_global.h"

#include <queue>

class GlView;
class GLWidget;

#ifdef ImageViewer_EXPORTS
# define ImageViewer_EXPORT __declspec(dllexport)
#else
# define ImageViewer_EXPORT __declspec(dllimport)
#endif

class ImageViewer_EXPORT ImageViewer:public Limitless::MediaAutoRegister<ImageViewer, Limitless::IMediaFilter>
{
public:
	ImageViewer(std::string name, Limitless::SharedMediaFilter parent);
	~ImageViewer();

	void attachViewer(ImageViewer *imageViewer);
	void removeViewer(ImageViewer *imageViewer);

	virtual bool initialize(const Limitless::Attributes &attributes);
	virtual bool shutdown(){return true;}

	virtual Limitless::SharedPluginView getView();

	void setSample(Limitless::SharedMediaSample sample);
	virtual bool processSample(Limitless::SharedMediaPad sinkPad, Limitless::SharedMediaSample sample);

	void showControls(bool show);

protected:
	GLWidget *getGlWidget();

	//IMediaFilter
	virtual StateChange onReady();
	virtual StateChange onPaused();
	virtual StateChange onPlaying();

	bool onAcceptMediaFormat(Limitless::SharedMediaPad pad, Limitless::SharedMediaFormat format);
	void onLinkFormatChanged(Limitless::SharedMediaPad pad, Limitless::SharedMediaFormat format);

private:
	Limitless::SharedPluginView m_view;
	GlView *m_glView;

	size_t m_gpuImageSampleId;

	std::queue<Limitless::MediaTime> m_timeStamps;
	int frameCount;
};

namespace Limitless{namespace traits
{
	template<> struct type<ImageViewer>
	{
		static FilterType get()
		{return Sink;}
	};
	template<> struct category<ImageViewer>
	{
		static std::string get()
		{return "sink";}
	};
}}//Limitless::traits

#endif // IMAGEVIEWER_H
