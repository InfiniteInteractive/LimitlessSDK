#include "cvlib/simpleImage.h"

namespace cvlib
{

SimpleImage::SimpleImage(ImageFormat format, size_t width, size_t height):
    format(format), width(width), stride(width), height(height), owned(false)
{
    allocData(format, width, height);
}

SimpleImage::~SimpleImage()
{
    freeData();
}

void SimpleImage::allocData(ImageFormat format, size_t width, size_t height)
{
    dataSize=sizeOfImageFormat(format)*width*height;
    data=(uint8_t *)malloc(dataSize);
    owned=true;
}

void SimpleImage::freeData()
{
    if((owned)&&(data!=nullptr))
    {
        free(data);
        data=nullptr;
        owned=false;
    }
}

bool resize(SimpleImage &image, ImageFormat format, size_t width, size_t height)
{
    image.format=format;
    image.width=width;
    image.stride=width;
    image.height=height;

    image.freeData();
    
    image.allocData(format, width, height);
    return true;
}

size_t width(const SimpleImage &image) { return image.width; }
size_t height(const SimpleImage &image) { return image.height; }
size_t stride(const SimpleImage &image) { return image.stride; }

ImageFormat format(const SimpleImage &image) { return image.format; }
uint8_t *data(const SimpleImage &image) { return image.data; }
size_t dataSize(const SimpleImage &image) { return image.dataSize; }

}//namespace cvlib
