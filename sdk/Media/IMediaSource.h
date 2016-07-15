#ifndef _Limitless_IMediaSource_h_
#define _Limitless_IMediaSource_h_

#include "Media/media_define.h"

#include <boost/shared_ptr.hpp>
#include "Media/IMediaFilter.h"
#include "Media/MediaSample.h"
#include "MediaPipeline/IPipelineCallback.h"

#pragma warning(push)
#pragma warning(disable:4251)

namespace Limitless
{

class MEDIA_EXPORT MediaDevice
{
public:
	MediaDevice(){}
	MediaDevice(std::string name, std::string id):name(name), id(id){}
	~MediaDevice(){}

	std::string name;
	std::string id;
};
typedef std::vector<MediaDevice> MediaDevices;

class MEDIA_EXPORT IMediaSource:public IMediaFilter
{
public:
	IMediaSource(std::string instance, SharedMediaFilter parent):IMediaFilter(instance, parent){m_baseClasses.push_back("IMediaSource");}
	virtual ~IMediaSource(){};

	virtual MediaDevices devices()=0;
	virtual bool connect(MediaDevice mediaDevice)=0;
	virtual bool disconnect()=0;
	virtual bool connected()=0;

//	virtual bool readSample(SharedMediaSample sample)=0;

//	virtual bool startPush(IPipelineCallback *callback, MediaSample *sample)=0;
//	virtual bool stopPush()=0;
//	virtual bool pushSample(MediaSample *sample)=0;
};

typedef boost::shared_ptr<IMediaSource> SharedMediaSource;

}//namespace Limitless

#pragma warning(pop)

#endif //_Limitless_IMediaSource_h_
