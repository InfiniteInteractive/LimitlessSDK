#ifndef _simpleAudioBuffer_h_
#define _simpleAudioBuffer_h_

#include "medialib/audioBuffer.h"
#include <vector>
#include <memory>

namespace medialib
{

struct medialib_EXPORT SimpleAudioBuffer
{
    SimpleAudioBuffer();
    SimpleAudioBuffer(AudioFormat format, unsigned int channels, unsigned int samples, unsigned int sampleRate);

    void alloc(AudioFormat format, unsigned int channels, unsigned int samples, unsigned int sampleRate);
    void reserve(unsigned int samples);
    void resize(unsigned int samples);

    uint8_t *getBuffer(size_t index);
    size_t getSize(size_t index) const;

	std::vector<uint8_t> data;
    size_t channelStride;
	
	unsigned int samples;
	unsigned int sampleRate;
	AudioFormat format;
	unsigned int channels;
};

medialib_EXPORT unsigned int getSampleRate(const SimpleAudioBuffer &audioBuffer);
medialib_EXPORT AudioFormat getFormat(const SimpleAudioBuffer &audioBuffer);
medialib_EXPORT unsigned int getChannels(const SimpleAudioBuffer &audioBuffer);

medialib_EXPORT unsigned int getSamples(const SimpleAudioBuffer &audioBuffer);
medialib_EXPORT void alloc(SimpleAudioBuffer &audioBuffer, AudioFormat format, unsigned int channels, unsigned int samples, unsigned int sampleRate);
medialib_EXPORT void resize(SimpleAudioBuffer &audioBuffer, unsigned int samples);
medialib_EXPORT void reserve(SimpleAudioBuffer &audioBuffer, unsigned int samples);
medialib_EXPORT unsigned int getCapacity(const SimpleAudioBuffer &audioBuffer);

medialib_EXPORT uint8_t *getBuffer(SimpleAudioBuffer &audioBuffer, size_t index);
medialib_EXPORT size_t getSize(const SimpleAudioBuffer &audioBuffer, size_t index);
medialib_EXPORT size_t getChannelStride(const SimpleAudioBuffer &audioBuffer);

}
#endif //_simpleAudioBuffer_h_
