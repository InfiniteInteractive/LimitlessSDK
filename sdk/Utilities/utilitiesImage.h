#ifndef _utilitiesImage_h_
#define _utilitiesImage_h_

#include "utilities_define.h"

namespace Limitless
{
	enum ImageFormat
	{
		RGB,
		RGBA,
		GREY
	};
extern "C"
{
	UTILITIES_EXPORT void savePPM(const char *const strFile, ImageFormat format, unsigned char *data, int width, int height);
	UTILITIES_EXPORT void savePGM(const char* const strFile, ImageFormat format, unsigned char *data, int width, int height);
}

}//namespace Limitless

#endif //_utilitiesImage_h_