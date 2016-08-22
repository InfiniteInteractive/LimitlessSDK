#include "medialib/simpleAudioBuffer.h"

#include <cassert>

namespace medialib
{

SimpleAudioBuffer::SimpleAudioBuffer():
format(AudioFormat::Unknown),
channels(0),
samples(0),
sampleRate(0),
channelStride(0)
{

}

SimpleAudioBuffer::SimpleAudioBuffer(AudioFormat format, unsigned int channels, unsigned int samples, unsigned int sampleRate)//:
//format(format),
//channels(channels),
//samples(samples),
//sampleRate(sampleRate)
{
    alloc(format, channels, samples, sampleRate);
}

void SimpleAudioBuffer::alloc(AudioFormat format, unsigned int channels, unsigned int samples, unsigned int sampleRate)
{
    this->format=format;
    this->channels=channels;
    this->samples=0;
    this->sampleRate=sampleRate;

    if(isPlanar(format))
        channelStride=getTypeSize(format)*samples;
    else
        channelStride=getTypeSize(format);

//    reserve(samples);
	resize(samples);
}

void SimpleAudioBuffer::resize(unsigned int samples)
{
//    if(isPlanar(format))
//        channelStride=getTypeSize(format)*samples;
//    else
//        channelStride=getTypeSize(format);
//
//    size_t size=channels*channelStride;
//    size_t size=0;

//    if(channels>0)
//        size=(channels-1)*channelStride+getTypeSize(format)*samples; //need to include channelStride in buffer size
//    data.resize(size);

	size_t size=channels*getTypeSize(format)*samples;

	data.resize(size);
    this->samples=samples;
}

void SimpleAudioBuffer::reserve(unsigned int samples)
{
    size_t size=channels*getTypeSize(format)*samples;

    data.reserve(size);
}

uint8_t *SimpleAudioBuffer::getBuffer(size_t index)
{
    if(index>=channels)
        return 0;

    return &data[channelStride*index];
}

size_t SimpleAudioBuffer::getSize(size_t index) const
{
    if(index>=channels)
        return 0;
    return channelStride;    
}

unsigned int getSampleRate(const SimpleAudioBuffer &audioBuffer) { return audioBuffer.sampleRate; }
AudioFormat getFormat(const SimpleAudioBuffer &audioBuffer) { return audioBuffer.format; }
unsigned int getChannels(const SimpleAudioBuffer &audioBuffer) { return audioBuffer.channels; }

unsigned int getSamples(const SimpleAudioBuffer &audioBuffer) { return audioBuffer.samples; }
void alloc(SimpleAudioBuffer &audioBuffer, AudioFormat format, unsigned int channels, unsigned int samples, unsigned int sampleRate) { audioBuffer.alloc(format, channels, samples, sampleRate); }
void resize(SimpleAudioBuffer &audioBuffer, unsigned int samples) { audioBuffer.resize(samples); }
void reserve(SimpleAudioBuffer &audioBuffer, unsigned int samples) {  audioBuffer.reserve(samples); }
unsigned int getCapacity(const SimpleAudioBuffer &audioBuffer) { return audioBuffer.data.capacity()/getTypeSize(audioBuffer.format); }

uint8_t *getBuffer(SimpleAudioBuffer &audioBuffer, size_t index) { return audioBuffer.getBuffer(index); }
size_t getSize(const SimpleAudioBuffer &audioBuffer, size_t index) { return audioBuffer.getSize(index); }
medialib_EXPORT size_t getChannelStride(const SimpleAudioBuffer &audioBuffer) { return audioBuffer.channelStride; }






}

