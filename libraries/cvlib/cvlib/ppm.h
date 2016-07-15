#ifndef _cvlib_ppm_h_
#define _cvlib_ppm_h

#include "cvlib/cvlibDefine.h"
#include "cvlib/image.h"

#include <string>

namespace cvlib
{

cvlib_EXPORT bool savePpm(Image image, std::string filename);

}//namespace cvlib

#endif //_cvlib_ppm_h