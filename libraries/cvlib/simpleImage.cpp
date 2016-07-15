#include "cvlib/simpleImage.h"

namespace cvlib
{

size_t width(const SimpleImage &image) { return image.width; }
size_t height(const SimpleImage &image) { return image.height; }
size_t stride(const SimpleImage &image) { return image.stride; }

ImageFormat format(const SimpleImage &image) { return image.format; }
uint8_t *data(const SimpleImage &image) { return image.data; }
size_t dataSize(const SimpleImage &image) { return image.dataSize; }

}//namespace cvlib
