#ifndef _cvlib_match_h_
#define _cvlib_match_h_

#include "cvlib/cvlibdefine.h"
#include "cvlib/image.h"

#include <algorithm>

namespace cvlib
{

template<ImageFormat _PixelType>
bool matchByType(const Image &src1, const Image &src2, Image &dest)
{
    size_t height=std::min(src1.height(), src2.height());
    size_t width=std::min(src1.width(), src2.width());

    uint8_t *src1Data=src1.data();
    uint8_t *src2Data=src2.data();
    uint8_t *dstData=dest.data();

    size_t src1Stride=src1.stride()-(width*traits::sizeOf<_PixelType>());
    size_t src2Stride=src2.stride()-(width*traits::sizeOf<_PixelType>());
    size_t dstStride=dest.stride()-(width*traits::sizeOf<ImageFormat::Binary>());

    for(size_t y=0; y<height; ++y)
    {
        for(size_t x=0; x<width; ++x)
        {
            (*dstData)=utils::comparePixel<_PixelType>(src1Data, src2Data)?1:0;

            src1Data+=traits::sizeOf<_PixelType>();
            src2Data+=traits::sizeOf<_PixelType>();
            dstData+=traits::sizeOf<ImageFormat::Binary>();
        }
        src1Data+=src1Stride;
        src2Data+=src2Stride;
        dstData+=dstStride;
    }

    return true;
}

//cvlib_EXPORT bool match(const Image &src1, const Image &src2, Image dest);
inline bool match(Image src1, Image src2, Image dest)
{
	if(src1.format()!=src2.format())
		return false;
	if(dest.format()!=ImageFormat::Binary)
		return false;

	switch(src1.format())
	{
	case ImageFormat::Binary:
		matchByType<ImageFormat::Binary>(src1, src2, dest);
		break;
	case ImageFormat::GreyScale:
		matchByType<ImageFormat::GreyScale>(src1, src2, dest);
		break;
	case ImageFormat::GreyScale32f:
		matchByType<ImageFormat::GreyScale32f>(src1, src2, dest);
		break;
	case ImageFormat::Rgb:
		matchByType<ImageFormat::Rgb>(src1, src2, dest);
		break;
	case ImageFormat::Rgba:
		matchByType<ImageFormat::Rgba>(src1, src2, dest);
		break;
	}

	return true;
}

}//namespace cvlib

#endif //_cvlib_match_h_