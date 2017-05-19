#ifndef _ImageSource_h_
#define _ImageSource_h_

#include "Base/common.h"
#include "Media/MediaPluginFactory.h"
#include "Media/IMediaFilter.h"
#include "Media/ImageSample.h"
#include "QtComponents/QtPluginView.h"

class ImageSource:public Limitless::MediaAutoRegister<ImageSource, Limitless::IMediaFilter>
{
public:
	ImageSource(std::string name, Limitless::SharedMediaFilter parent);
	~ImageSource();

	virtual bool initialize(const Limitless::Attributes &attributes);
	virtual bool shutdown(){return true;}

	//PluginObject
	virtual Limitless::SharedPluginView getView();
	virtual Limitless::SharedPluginView getAttributeView();

	virtual bool processSample(Limitless::SharedMediaPad sinkPad, Limitless::SharedMediaSample sample);

protected:
	//IMediaFilter
	virtual StateChange onReady();
	virtual StateChange onPaused();
	virtual StateChange onPlaying();

	virtual void onLinkFormatChanged(Limitless::SharedMediaPad mediaPad, Limitless::SharedMediaFormat format);
	virtual bool onAcceptMediaFormat(Limitless::SharedMediaPad pad, Limitless::SharedMediaFormat format);
	
	//PluginObject
	virtual void onAttributeChanged(std::string name, Limitless::SharedAttribute attribute);

private:
    void loadImage(std::string location);

    Limitless::WeakQtPluginView m_attributeView;
    size_t m_imageSampleId;

    bool m_imageLoaded;
    Limitless::ImageSample m_sourceImage;
};

namespace Limitless{namespace traits
{
	template<> struct type<ImageSource>
	{
		static FilterType get()
		{return Source;}
	};
	template<> struct category<ImageSource>
	{
		static std::string get()
		{return "source";}
	};

}}//Limitless::traits
#endif // _ImageSource_h_
