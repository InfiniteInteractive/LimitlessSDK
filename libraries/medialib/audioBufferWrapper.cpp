#include "medialib/audioBufferWrapper.h"

#include <cassert>

namespace medialib
{

AudioBufferWrapper::AudioBufferWrapper(AudioFormat format, unsigned int channels, unsigned int samples, unsigned int sampleRate, uint8_t *remoteBuffer, size_t remoteBufferSize):
format(format),
channels(channels),
samples(samples),
sampleRate(sampleRate),
remoteBuffer(remoteBuffer),
remoteBufferSize(remoteBufferSize)
{
    if(isPlanar(format))
        channelStride=getTypeSize(format)*samples;
    else
        channelStride=getTypeSize(format);
}

uint8_t *AudioBufferWrapper::getBuffer(size_t index)
{
    if(index>=channels)
        return 0;

    return &remoteBuffer[channelStride*index];
}

size_t AudioBufferWrapper::getSize(size_t index) const
{
    if(index>=channels)
        return 0;
    return channelStride;
}


unsigned int getSampleRate(const AudioBufferWrapper &audioBuffer) { return audioBuffer.sampleRate; }
AudioFormat getFormat(const AudioBufferWrapper &audioBuffer) { return audioBuffer.format; }
unsigned int getChannels(const AudioBufferWrapper &audioBuffer) { return audioBuffer.channels; }

unsigned int getSamples(const AudioBufferWrapper &audioBuffer) { return audioBuffer.samples; }
void alloc(AudioBufferWrapper &audioBuffer, AudioFormat format, unsigned int channels, unsigned int samples, unsigned int sampleRate) {}
void resize(AudioBufferWrapper &audioBuffer, unsigned int size) {}
void reserve(AudioBufferWrapper &audioBuffer, unsigned int size) {}
unsigned int getCapacity(const AudioBufferWrapper &audioBuffer) { return audioBuffer.samples; }

uint8_t *getBuffer(AudioBufferWrapper &audioBuffer, size_t index) { return audioBuffer.getBuffer(index); }
size_t getSize(const AudioBufferWrapper &audioBuffer, size_t index) { return audioBuffer.getSize(index); }
medialib_EXPORT size_t getChannelStride(const AudioBufferWrapper &audioBuffer) { return audioBuffer.channelStride; }





}

