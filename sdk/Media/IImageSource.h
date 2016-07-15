#ifndef _Limitless_IImageSource_h_
#define _Limitless_IImageSource_h_

#include "Media/media_define.h"

#include "Media/MediaSample.h"
#include "Media/IMediaSource.h"

namespace Limitless
{

class MEDIA_EXPORT IImageSource:public IMediaSource
{
public:
	IImageSource(std::string name, SharedMediaFilter parent):IMediaSource(name, parent){m_baseClasses.push_back("IImageSource");}
	virtual ~IImageSource(){};

//	virtual bool connect(std::string name, std::string device)=0;
//	virtual bool disconnect()=0;
//
//	virtual bool readSample(SharedMediaSample sample)=0;

//	virtual bool startPush(IPipelineCallback *callback, IMediaSample *sample)=0;
//	virtual bool stopPush()=0;
//	virtual bool pushSample(MediaSample *sample)=0;
};

}//namespace Limitless

#endif //_Limitless_IImageSource_h_