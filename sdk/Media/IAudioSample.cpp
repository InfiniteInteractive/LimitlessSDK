#include "Media/IAudioSample.h"

namespace Limitless
{

size_t sampleSize(AudioSampleFormat format)
{
	switch(format)
	{
	case AudioSampleFormat::Unknown:
		return 0;
		break;
	case AudioSampleFormat::UInt8:
	case AudioSampleFormat::UInt8P:
		return sizeof(uint8_t);
		break;
	case AudioSampleFormat::Int16:
	case AudioSampleFormat::Int16P:
		return sizeof(uint16_t);
		break;
	case AudioSampleFormat::Int32:
	case AudioSampleFormat::Int32P:
		return sizeof(uint32_t);
		break;
	case AudioSampleFormat::Float:
	case AudioSampleFormat::FloatP:
		return sizeof(float);
		break;
	case AudioSampleFormat::Double:
	case AudioSampleFormat::DoubleP:
		return sizeof(double);
		break;
	}
}

size_t calculateAudioBufferSize(AudioSampleFormat format, unsigned int channels, unsigned int samples)
{
	size_t typeSize=sampleSize(format);

	return typeSize*channels*samples;
}

}//namespace Limitless