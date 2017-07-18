#include "pixelFormatDescription.h"
#include "ffmpegResources.h"

const PixelFormatDescription &getPixelFormatDescription(AVPixelFormat pixelFormat)
{
    static PixelFormatDescriptions descriptions;

    if(descriptions.empty())
    {
        descriptions.insert(PixelFormatDescriptions::value_type(AV_PIX_FMT_NONE, PixelFormatDescription("none", 1, {1})));
        descriptions.insert(PixelFormatDescriptions::value_type(AV_PIX_FMT_RGB24, PixelFormatDescription("rgb24", 1, {1})));
        descriptions.insert(PixelFormatDescriptions::value_type(AV_PIX_FMT_BGR24, PixelFormatDescription("bgr24", 1, {1})));

        descriptions.insert(PixelFormatDescriptions::value_type(AV_PIX_FMT_YUYV422, PixelFormatDescription("yuyv422", 1, {1})));
        
        descriptions.insert(PixelFormatDescriptions::value_type(AV_PIX_FMT_YUV410P, PixelFormatDescription("yuv410p", 3, {1, 4, 4})));
        descriptions.insert(PixelFormatDescriptions::value_type(AV_PIX_FMT_YUV411P, PixelFormatDescription("yuv411p", 3, {1, 1, 1})));
        descriptions.insert(PixelFormatDescriptions::value_type(AV_PIX_FMT_YUV420P, PixelFormatDescription("yuv420p", 3, {1, 2, 2})));
        descriptions.insert(PixelFormatDescriptions::value_type(AV_PIX_FMT_YUV422P, PixelFormatDescription("yuv422p", 3, {1, 1, 1})));
        descriptions.insert(PixelFormatDescriptions::value_type(AV_PIX_FMT_YUV440P, PixelFormatDescription("yuv440p", 3, {1, 4, 4,})));
        descriptions.insert(PixelFormatDescriptions::value_type(AV_PIX_FMT_YUV444P, PixelFormatDescription("yuv444p", 3, {1, 1, 1})));
        
        descriptions.insert(PixelFormatDescriptions::value_type(AV_PIX_FMT_YUVJ420P, PixelFormatDescription("yuvj420p", 3, {1, 2, 2})));
        descriptions.insert(PixelFormatDescriptions::value_type(AV_PIX_FMT_YUVJ422P, PixelFormatDescription("yuvj422p", 3, {1, 1, 1})));
        descriptions.insert(PixelFormatDescriptions::value_type(AV_PIX_FMT_YUVJ440P, PixelFormatDescription("yuvj440p", 3, {1, 4, 4})));
        descriptions.insert(PixelFormatDescriptions::value_type(AV_PIX_FMT_YUVJ444P, PixelFormatDescription("yuvj444p", 3, {1, 1, 1})));
    }

    auto &iter=descriptions.find(pixelFormat);

    if(iter==descriptions.end())
        return descriptions[AV_PIX_FMT_NONE];

    return iter->second;
}