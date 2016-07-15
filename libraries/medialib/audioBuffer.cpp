#include "medialib/audioBuffer.h"

#include <cassert>

namespace medialib
{

size_t getTypeSize(const AudioFormat &format)
{
    switch(format)
    {
    case AudioFormat::UInt8:
    case AudioFormat::UInt8P:
        return sizeof(uint8_t);
        break;
    case AudioFormat::Int16:
    case AudioFormat::Int16P:
        return sizeof(uint16_t);
        break;
    case AudioFormat::Int32:
    case AudioFormat::Int32P:
        return sizeof(uint32_t);
        break;
    case AudioFormat::Float:
    case AudioFormat::FloatP:
        return sizeof(float);
        break;
    case AudioFormat::Double:
    case AudioFormat::DoubleP:
        return sizeof(double);
        break;
    }
    assert(false);
    return 0;
}

size_t calculateBufferSize(AudioFormat format, unsigned int channels, unsigned int samples)
{
    return getTypeSize(format)*channels*samples;
}

bool isPlanar(const AudioFormat &format)
{
    if(format>AudioFormat::Double)
        return true;
    return false;
}

bool isPlanar(const AudioBuffer &audioBuffer)
{
    return isPlanar(audioBuffer.getFormat());
}

//bool AudioBuffer::isPlanar() const
//{
//    if(format>AudioFormat::Double)
//        return true;
//    return false;
//}
//
//uint8_t *AudioBuffer::getBufferPos(size_t index)
//{
//    size_t typeSize=getTypeSize(getFormat());
//
//    return &buffer[index*typeSize];
//}
//
//uint8_t *AudioBuffer::getBufferPos(unsigned int channel, size_t sampleIndex)
//{
//    size_t typeSize=getTypeSize(getFormat());
//    bool planar=isPlanar();
//
//    if(planar)
//    {
//        return &buffer[(samples*channel+sampleIndex)*typeSize];
//    }
//    else
//    {
//        return &buffer[(sampleIndex*channel)*typeSize];
//    }
//}


}

