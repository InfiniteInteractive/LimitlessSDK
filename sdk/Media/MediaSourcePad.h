#ifndef _Limitless_MediaSourcePad_h_
#define _Limitless_MediaSourcePad_h_

#include "Media/media_define.h"

#include "Media/MediaPad.h"
#include "boost/shared_ptr.hpp"
#include <vector>

namespace Limitless
{

class MEDIA_EXPORT MediaSourcePad:public MediaPad
{
public:
	MediaSourcePad(std::string name, SharedMediaFilter parent):MediaPad(name, parent){};
	virtual ~MediaSourcePad(){};

	Type type(){return SOURCE;}

protected:
//	virtual bool onAccept(SharedMediaFormat format);

private:
};
typedef boost::shared_ptr<MediaSourcePad> SharedMediaSourcePad;
typedef std::vector<SharedMediaSourcePad> SharedMediaSourcePads;

}//namespace Limitless

#endif //_Limitless_MediaSourcePad_h_
