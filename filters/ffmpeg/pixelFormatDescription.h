#ifndef ffmpeg_pixelFormatDescription_h_
#define ffmpeg_pixelFormatDescription_h_

#include <string>
#include <vector>
#include <unordered_map>

extern "C"
{
    #include <libavformat/avformat.h>
}

class PixelFormatDescription
{
public:
    PixelFormatDescription():planes(0) {}
    PixelFormatDescription(std::string name, size_t planes, std::vector<int> divisors):name(name), planes(planes), divisors(divisors){};
    ~PixelFormatDescription(){}

    std::string name;
    size_t planes;
    std::vector<int> divisors;
};

typedef std::unordered_map<AVPixelFormat, PixelFormatDescription> PixelFormatDescriptions;

const PixelFormatDescription &getPixelFormatDescription(AVPixelFormat pixelFormat);

#endif // ffmpeg_pixelFormatDescription_h_
