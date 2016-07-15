#ifndef _IImageSample_h_
#define _IImageSample_h_

#include "Media/media_define.h"
#include "Media/MediaSample.h"
#include "Media/MediaSampleFactory.h"
#include <boost/shared_ptr.hpp>
#include <deque>

namespace Limitless
{

class MEDIA_EXPORT IImageSample:public AutoRegisterMediaSample<IImageSample, MediaSample, true>
{
public:
	IImageSample(){};
	virtual ~IImageSample(){};

	virtual std::string imageFormat() const=0;
	virtual unsigned int width() const=0;
	virtual unsigned int pitch() const=0;
	virtual unsigned int height() const=0;
	virtual unsigned int channels() const=0;
	virtual unsigned int channelBits() const=0;

	virtual bool save(std::string fileName)=0;
};

typedef boost::shared_ptr<IImageSample> SharedIImageSample;
typedef std::deque<SharedIImageSample> SharedIImageSampleQueue;

}//namespace Limitless

#endif //_IImageSample_h_