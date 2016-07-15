#ifndef _Limitless_MediaSampleFlags_h_
#define _Limitless_MediaSampleFlags_h_

#include "Media/media_define.h"

#include <string>

namespace Limitless
{

MEDIA_EXPORT unsigned int registerMediaSampleFlag(std::string name);
MEDIA_EXPORT unsigned int getMediaSampleFlag(std::string name);

}//namespace Limitless

#endif //_Limitless_MediaSampleFlags_h_