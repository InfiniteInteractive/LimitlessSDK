#include "Media/AudioSample.h"

namespace Limitless
{

unsigned char *AudioSample::buffer()
{
	return nullptr;
}

size_t AudioSample::size() const
{
	return 0;
}

AudioSampleFormat AudioSample::format() const
{
	return AudioSampleFormat::Unknown;
}

unsigned int AudioSample::channels() const
{
	return 0;
}

unsigned int AudioSample::samples() const
{
	return 0;
}

}//namespace Limitless