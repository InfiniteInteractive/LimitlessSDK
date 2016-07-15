#ifndef _cvlib_simpleImage_h_
#define _cvlib_simpleImage_h_

#include "cvlib/image.h"
#include "cvlib/cvlibDefine.h"

namespace cvlib
{

struct cvlib_EXPORT SimpleImage
{
	SimpleImage():width(0), stride(0), height(0), data(nullptr), dataSize(0) {};
	SimpleImage(size_t width, size_t height, ImageFormat format, uint8_t *data, size_t dataSize):
		width(width), stride(width), height(height), format(format), data(data), dataSize(dataSize){}
	SimpleImage(size_t width, size_t stride, size_t height, ImageFormat format, uint8_t *data, size_t dataSize):
		width(width), stride(stride), height(height), format(format), data(data), dataSize(dataSize)
	{}

	size_t width;
	size_t height;
	size_t stride;

	ImageFormat format;
	uint8_t *data;
	size_t dataSize;
};

cvlib_EXPORT size_t width(const SimpleImage &image);
cvlib_EXPORT size_t height(const SimpleImage &image);
cvlib_EXPORT size_t stride(const SimpleImage &image);

cvlib_EXPORT ImageFormat format(const SimpleImage &image);
cvlib_EXPORT uint8_t *data(const SimpleImage &image);
cvlib_EXPORT size_t dataSize(const SimpleImage &image);

}//namespace cvlib

#endif //_cvlib_simpleImage_h_