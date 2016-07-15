#include "Media/MediaSourcePad.h"
#include "Media/MediaSinkPad.h"
#include "boost/foreach.hpp"

//bool MediaSourcePad::onAccept(SharedMediaFormat format)
//{
//	SharedMediaFormats mediaFormats=queryMediaFormats();
//
//	BOOST_FOREACH(SharedMediaFormat &mediaFormat, mediaFormats)
//	{
//		if(mediaFormat->match(format))
//			return true;
//	}
//	return false;
//}