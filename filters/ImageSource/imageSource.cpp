#include "ImageSource.h"

#include "Base/Log.h"
#include "Media/MediaPad.h"
#include "Media/MediaSampleFactory.h"
#include "Media/ImageSample.h"
#include "Media/BufferSample.h"
#include "Media/EventSample.h"

#include "ImageSourceAttributeView.h"

#include <boost/foreach.hpp>
#include <boost/format.hpp>

#include <png.h>

using namespace Limitless;

ImageSource::ImageSource(std::string name, SharedMediaFilter parent):
MediaAutoRegister(name, parent)
{
    m_imageLoaded=false;
}

ImageSource::~ImageSource()
{

}

bool ImageSource::initialize(const Attributes &attributes)
{
	addAttribute("format", "");
	addAttribute("location", "");
	addAttribute("directory", "");

    m_imageSampleId=Limitless::MediaSampleFactory::getTypeId("ImageSample");

	return true;
}

SharedPluginView ImageSource::getView()
{
	return SharedPluginView();
}

bool ImageSource::processSample(SharedMediaPad sinkPad, SharedMediaSample sample)
{
    if(!m_imageLoaded)
    {
        std::string location=attribute("location")->toString();

        if(!exists("location"))
            loadImage(location);
    }

    if(sample->isType(m_imageSampleId))
    {
        Limitless::SharedImageSample imageSample=boost::dynamic_pointer_cast<Limitless::ImageSample>(sample);

        imageSample->copy(&m_sourceImage);
    }
    return true;
}

IMediaFilter::StateChange ImageSource::onReady()
{
	if(!exists("location"))
		return FAILED;

	std::string location=attribute("location")->toString();

//    SharedMediaFormat mediaFormat(new MediaFormat());
//    std::string mimeType=(boost::format("image/%s")%codec->name).str();
//    
//    mediaFormat->addAttribute("mime", mimeType);
//    mediaFormat->addAttribute("width", codecContext->width);
//    mediaFormat->addAttribute("height", codecContext->height);
//    mediaFormat->addAttribute("format", FfmpegResources::getAvPixelFormatName(codecContext->pix_fmt));
//
//    if(m_streamPads.size() < i+1)
//        m_streamPads.resize(i+1);
//    
//    SharedMediaPad mediaPad=addSourcePad((boost::format("Source%d")%i).str(), mediaFormat);

    return SUCCESS;
}

IMediaFilter::StateChange ImageSource::onPaused()
{
    return SUCCESS;
}

IMediaFilter::StateChange ImageSource::onPlaying()
{
    return SUCCESS;
}

void ImageSource::onLinkFormatChanged(SharedMediaPad pad, SharedMediaFormat format)
{
    if(pad->type() == MediaPad::SOURCE)
    {
    }
}

bool ImageSource::onAcceptMediaFormat(SharedMediaPad pad, SharedMediaFormat format)
{
	if(pad->type() == MediaPad::SOURCE)
	{
		SharedMediaPads pads=getSourcePads();
		SharedMediaPads::iterator iter=std::find(pads.begin(), pads.end(), pad);

		if(iter != pads.end())
		{
			if(format->exists("mime"))
			{
				std::string mime=format->attribute("mime")->toString();

				if(mime.compare(0, 5, "video") == 0)
					return true;
				if(mime.compare(0, 5, "image") == 0)
					return true;
			}
		}
	}
	return false;
}

SharedPluginView ImageSource::getAttributeView()
{
	SharedQtPluginView currentView=m_attributeView.lock();

	if(currentView == SharedQtPluginView())
	{
		ImageSourceAttributeView *attributeView=new ImageSourceAttributeView(this);

		currentView.reset(new QtPluginView(attributeView));
		m_attributeView=currentView;
	}
	return currentView;
}

void ImageSource::onAttributeChanged(std::string name, SharedAttribute attribute)
{
//	if(name == "loop")
//		m_loop=attribute->toBool();
}

void ImageSource::loadImage(std::string location)
{
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
//    std::vector<png_byte> imageData(width*height*channels);

    m_sourceImage.resize(width, height, channels, bit_depth);
    png_byte *imageData=(png_byte *)m_sourceImage.buffer();

    size_t pos=0;
    size_t stride=width*channels;

    for(int y=0; y<height; y++)
    {
        row_pointers[y]=&imageData[pos];
        pos+=stride;
    }

    png_read_image(png_ptr, row_pointers.data());
}