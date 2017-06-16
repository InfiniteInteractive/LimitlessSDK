#include "cvlib/image.h"

namespace cvlib
{

size_t sizeOfImageFormat(ImageFormat format)
{
    switch(format)
    {
    case ImageFormat::Binary:
        return traits::sizeOf<ImageFormat::Binary>();
        break;
    case ImageFormat::GreyScale:
        return traits::sizeOf<ImageFormat::GreyScale>();
        break;
    case ImageFormat::GreyScale32f:
        return traits::sizeOf<ImageFormat::GreyScale32f>();
        break;
    case ImageFormat::Ra:
        return traits::sizeOf<ImageFormat::Ra>();
        break;
    case ImageFormat::Rgb:
        return traits::sizeOf<ImageFormat::Rgb>();
        break;
    case ImageFormat::Rgba:
        return traits::sizeOf<ImageFormat::Rgba>();
        break;
    }
    return 0;
}

}//namespace cvlib
