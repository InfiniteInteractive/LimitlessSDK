#ifndef _audioBuffer_h_
#define _audioBuffer_h_

#include "medialib/medialibDefine.h"
#include <vector>
#include <memory>

namespace medialib
{

enum class AudioFormat
{
    Unknown=0,
    UInt8=1, ///< enum value unsigned 8 bit sample
    Int16=2, ///< enum value 16 bit sample
    Int32=3, ///< enum value 32 bit sample
    Float=4, ///< enum value float sample
    Double=5, ///< enum value double sample
 //Planar
    UInt8P=6, ///< enum value unsigned 8 bit sample, planar
    Int16P=7, ///< enum value 16 bit sample, planar
    Int32P=8, ///< enum value 32 bit sample, planar
    FloatP=9, ///< enum value float sample, planar
    DoubleP=10 ///< enum value double sample, planar
};

class AudioBuffer
{
public:
    template<typename _Type>
    AudioBuffer(_Type &value):m_self(new AudioBufferModel<_Type>(value)) {}//std::move(value))) {}

//Format Information
    unsigned int getSampleRate() const { return m_self->getSampleRate(); }
    AudioFormat getFormat() const { return m_self->getFormat(); }
    unsigned int getChannels() const { return m_self->getChannels(); }
 //Sample information
    unsigned int getSamples() const { return m_self->getSamples(); }
    void alloc(AudioFormat format, unsigned int channels, unsigned int samples, unsigned int sampleRate) { return m_self->alloc(format, channels, samples, sampleRate); }
    void resize(unsigned int size) { return m_self->resize(size); }
    void reserve(unsigned int size) { return m_self->reserve(size); }
    unsigned int getCapacity() const { return m_self->getCapacity(); }
 //Raw Buffer information
    uint8_t *getBuffer(size_t index=0) const { return m_self->getBuffer(index); }
    size_t getSize(size_t index=0) const { return m_self->getSize(index); }
//    size_t getChannelStride() const { return m_self->getChannelStride(); }

    template<typename _Type>
    _Type &&moveType() { return std::move(static_cast<AudioBufferModel<_Type> *>(m_self.get())->m_data); }


private:
    struct AudioBufferConcept
    {
        virtual ~AudioBufferConcept()=default;
        
//Format Information
        virtual unsigned int getSampleRate() const=0;
        virtual AudioFormat getFormat() const=0;
        virtual unsigned int getChannels() const=0;
//Sample information
        virtual unsigned int getSamples() const=0;
        virtual void alloc(AudioFormat format, unsigned int channels, unsigned int samples, unsigned int sampleRate)=0;
        virtual void resize(unsigned int samples)=0;
        virtual void reserve(unsigned int samples)=0;
        virtual unsigned int getCapacity() const=0;
//Raw Buffer information
        virtual uint8_t *getBuffer(size_t index)=0;
        virtual size_t getSize(size_t index) const=0;
//        virtual size_t getChannelStride() const=0;
    };

    template<typename _Type>
    struct AudioBufferModel:AudioBufferConcept
    {
        AudioBufferModel(_Type &value):m_data(value){}//std::move(value)) {}

        virtual unsigned int getSampleRate() const { return medialib::getSampleRate(m_data); }
        virtual AudioFormat getFormat() const { return medialib::getFormat(m_data); }
        virtual unsigned int getChannels() const { return medialib::getChannels(m_data); }

        virtual unsigned int getSamples() const { return medialib::getSamples(m_data); }
        virtual void alloc(AudioFormat format, unsigned int channels, unsigned int samples, unsigned int sampleRate) { medialib::alloc(m_data, format, channels, samples, sampleRate); }
        virtual void resize(unsigned int samples) { return medialib::resize(m_data, samples); }
        virtual void reserve(unsigned int samples) { return medialib::reserve(m_data, samples); }
        virtual unsigned int getCapacity() const { return medialib::getCapacity(m_data); }

        virtual uint8_t *getBuffer(size_t index) { return medialib::getBuffer(m_data, index); }
        virtual size_t getSize(size_t index) const { return medialib::getSize(m_data, index); }
//        virtual size_t getChannelStride() const{ return medialib::getChannelStride(m_data);}

        _Type &m_data;
    };

 //   std::shared_ptr<const AudioBufferConcept> m_self;
    std::unique_ptr<AudioBufferConcept> m_self;
};

medialib_EXPORT size_t getTypeSize(const AudioFormat &format);
medialib_EXPORT bool isPlanar(const AudioFormat &format);
medialib_EXPORT bool isPlanar(const AudioBuffer &audioBuffer);
medialib_EXPORT size_t calculateBufferSize(AudioFormat format, unsigned int channels, unsigned int samples);

//struct AudioBuffer
//{
//    bool isPlanar() const;
//    size_t getTypeSize() const;
//    
//    uint8_t *getBufferPos(size_t index);
//    uint8_t *getBufferPos(unsigned int channel, size_t sampleIndex);
//
//    uint8_t *buffer;
//    size_t size; //buffer size in bytes
//
//    unsigned int channels;
//    unsigned int samples;
//    AudioFormat format;
//    unsigned int sampleRate;
//};

typedef std::vector<AudioBuffer> AudioBuffers;

template<typename _Type> uint8_t *bufferPosByIndex(uint8_t *buffer, size_t index)
{
    return ((_Type *)buffer)[index];
}

}
#endif //_audioBuffer_h_
