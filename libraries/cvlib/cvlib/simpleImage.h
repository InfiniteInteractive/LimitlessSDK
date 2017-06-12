#ifndef _cvlib_simpleImage_h_
#define _cvlib_simpleImage_h_

#include "cvlib/image.h"
#include "cvlib/cvlibDefine.h"

namespace cvlib
{

struct cvlib_EXPORT SimpleImage
{
	SimpleImage():width(0), stride(0), height(0), owned(false), data(nullptr), dataSize(0) {};
    SimpleImage(ImageFormat format, size_t width, size_t height);
	SimpleImage(ImageFormat format, size_t width, size_t height, uint8_t *data, size_t dataSize):
        format(format), width(width), stride(width), height(height), owned(false), data(data), dataSize(dataSize){}
	SimpleImage(ImageFormat format, size_t width, size_t stride, size_t height, uint8_t *data, size_t dataSize):
        format(format), width(width), stride(stride), height(height), owned(false), data(data), dataSize(dataSize)
	{}
    ~SimpleImage();

    void allocData(ImageFormat format, size_t width, size_t height);
    void freeData();

	size_t width;
	size_t height;
	size_t stride;

	ImageFormat format;

    bool owned;
	uint8_t *data;
	size_t dataSize;
};

cvlib_EXPORT bool resize(SimpleImage &image, ImageFormat format, size_t width, size_t height);
cvlib_EXPORT size_t width(const SimpleImage &image);
cvlib_EXPORT size_t height(const SimpleImage &image);
cvlib_EXPORT size_t stride(const SimpleImage &image);

cvlib_EXPORT ImageFormat format(const SimpleImage &image);
cvlib_EXPORT uint8_t *data(const SimpleImage &image);
cvlib_EXPORT size_t dataSize(const SimpleImage &image);

}//namespace cvlib

#endif //_cvlib_simpleImage_h_