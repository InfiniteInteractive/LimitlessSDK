#ifndef _audioBufferWrapper_h_
#define _audioBufferWrapper_h_

#include "medialib/audioBuffer.h"
#include <vector>
#include <memory>

namespace medialib
{

struct medialib_EXPORT AudioBufferWrapper
{
    AudioBufferWrapper(AudioFormat format, unsigned int channels, unsigned int samples, unsigned int sampleRate, uint8_t *remoteBuffer, size_t remoteBufferSize);

    uint8_t *getBuffer(size_t index);
    size_t getSize(size_t index) const;

    uint8_t *remoteBuffer;
    size_t remoteBufferSize;
    size_t channelStride;
	
	unsigned int samples;
	unsigned int sampleRate;
	AudioFormat format;
	unsigned int channels;
};

medialib_EXPORT unsigned int getSampleRate(const AudioBufferWrapper &audioBuffer);
medialib_EXPORT AudioFormat getFormat(const AudioBufferWrapper &audioBuffer);
medialib_EXPORT unsigned int getChannels(const AudioBufferWrapper &audioBuffer);

medialib_EXPORT unsigned int getSamples(const AudioBufferWrapper &audioBuffer);
medialib_EXPORT void alloc(AudioBufferWrapper &audioBuffer, AudioFormat format, unsigned int channels, unsigned int samples, unsigned int sampleRate);
medialib_EXPORT void resize(AudioBufferWrapper &audioBuffer, unsigned int size);
medialib_EXPORT void reserve(AudioBufferWrapper &audioBuffer, unsigned int size);
medialib_EXPORT unsigned int getCapacity(const AudioBufferWrapper &audioBuffer);

medialib_EXPORT uint8_t *getBuffer(AudioBufferWrapper &audioBuffer, size_t index);
medialib_EXPORT size_t getSize(const AudioBufferWrapper &audioBuffer, size_t index);
medialib_EXPORT size_t getChannelStride(const AudioBufferWrapper &audioBuffer);

}
#endif //_audioBufferWrapper_h_
