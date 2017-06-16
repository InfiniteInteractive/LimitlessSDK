#ifndef _cvlib_png_h_
#define _cvlib_png_h

#include "cvlib/cvlibDefine.h"
#include "cvlib/image.h"

#include <string>

namespace cvlib
{

cvlib_EXPORT bool loadPng(Image image, std::string filename);
cvlib_EXPORT bool savePng(Image image, std::string filename);

}//namespace cvlib

#endif //_cvlib_png_h