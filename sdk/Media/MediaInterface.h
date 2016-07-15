#ifndef _MediaInterface_h_
#define _MediaInterface_h_

#include "Media/media_define.h"

namespace Limitless
{
	typedef void * DisplayHandle;

	void MEDIA_EXPORT initOpenGl(DisplayHandle displayHandle);
	void MEDIA_EXPORT initOpenCl();

	void MEDIA_EXPORT closeOpenGl();
}//namespace Limitless

#endif //_MediaInterface_h_