#ifndef _audioOperations_h_
#define _audioOperations_h_

#include "medialib/medialibDefine.h"
#include "medialib/audioBuffer.h"

#include <algorithm>
#include <cassert>

namespace medialib
{

medialib_EXPORT void concat(AudioBuffer audioBufferDst, const AudioBuffer &audioBufferSrc);
//template<typename _AudioBufferType>
//void concat(_AudioBufferType &audioBuffer, const AudioBuffer &audioBufferSrc)
//{
//    AudioBuffer audioBufferDst(audioBuffer);
//
//    if((audioBufferSrc.getSize()==0)) //nothing to add
//        return;
//
//    if(audioBufferDst.getChannels()!=audioBufferSrc.getChannels())
//        return;
//
//    if(audioBufferDst.getSampleRate()!=audioBufferSrc.getSampleRate())
//    {
//        assert(false);//not implemented yet, need conversion
//        return;
//    }
//
//    if(audioBufferDst.getFormat()!=audioBufferSrc.getFormat())
//        return;
//
//    //get offset before we resize it
//    size_t dstOffset=audioBufferDst.getSamples()*getTypeSize(audioBufferDst.getFormat());
//
//    audioBufferDst.resize(audioBufferDst.getSamples()+audioBufferSrc.getSamples());
//
//    if(isPlanar(audioBufferDst))
//    {
//        for(unsigned int channel=0; channel<audioBufferDst.getChannels(); ++channel)
//        {
//            uint8_t *dataDst=audioBufferDst.getBuffer(channel)+dstOffset;
//            uint8_t *dataSrc=audioBufferSrc.getBuffer(channel);
//
//            memcpy(dataDst, dataSrc, audioBufferSrc.getSize(channel));
//        }
//    }
//    else
//    {
//        dstOffset*=audioBufferDst.getChannels();//increase offset by numver of channels
//        uint8_t *dataDst=audioBufferDst.getBuffer()+dstOffset;
//
//        memcpy(dataDst, audioBufferSrc.getBuffer(), audioBufferSrc.getSize());
//    }
//}

template<typename _AudioBufferType>
_AudioBufferType combine(const AudioBuffer &audioBuffer1, const AudioBuffer &audioBuffer2)
{
    AudioBuffer newBuffer((_AudioBufferType())); //extra params required, see "most vexing parse"

    if((audioBuffer1.getSize() == 0) || (audioBuffer2.getSize() == 0)) //if one empty just copy the non empty
    {
        AudioFormat format;
        unsigned int channels;
        unsigned int samples;
        unsigned int sampleRate;
        uint8_t *data=nullptr;
        size_t dataSize=0;

        if(audioBuffer1.getSize())
        {
            format=audioBuffer1.getFormat();
            channels=audioBuffer1.getChannels();
            samples=audioBuffer1.getSamples();
            sampleRate=audioBuffer1.getSampleRate();
            data=audioBuffer1.getBuffer();
            dataSize=audioBuffer1.getSize();
        }
        else if(audioBuffer2.getSize())
        {
            format=audioBuffer2.getFormat();
            channels=audioBuffer2.getChannels();
            samples=audioBuffer2.getSamples();
            sampleRate=audioBuffer2.getSampleRate();
            data=audioBuffer2.getBuffer();
            dataSize=audioBuffer2.getSize();
        }

        if(data!=nullptr)
        {
            
            size_t bufferSize2=calculateBufferSize(format, channels, samples);

            newBuffer.alloc(format, channels, samples, sampleRate);
            
            if(newBuffer.getSize()<dataSize) //something wrong as the size should be enough to hold the data
            {
                assert(false);
                return newBuffer.moveType<_AudioBufferType>();
            }
      
            memcpy(newBuffer.getBuffer(), audioBuffer2.getBuffer(), dataSize);
            return newBuffer.moveType<_AudioBufferType>();
        }

        //both empty, might want to check what you are doing
        assert(false);
        return newBuffer.moveType<_AudioBufferType>();
    }

    if(audioBuffer1.getChannels()!=audioBuffer2.getChannels()) //need to know which channel to put samples in, need different function
        return newBuffer.moveType<_AudioBufferType>();

    if(audioBuffer1.getSampleRate()!=audioBuffer2.getSampleRate())
    {
        assert(false);//not implemented yet, need conversion
        return newBuffer.moveType<_AudioBufferType>();
    }

    unsigned int samples=audioBuffer1.getSamples()+audioBuffer2.getSamples();
    unsigned int requiredBufferSize=calculateBufferSize(audioBuffer1.getFormat(), audioBuffer1.getChannels(), samples);
    
    newBuffer.alloc(audioBuffer1.getFormat(), audioBuffer1.getChannels(), samples, audioBuffer1.getSampleRate());
    
    if(newBuffer.getSize() < requiredBufferSize)//something wrong as the size should be enough to hold the data
    {
        assert(false);
        return newBuffer.moveType<_AudioBufferType>();
    }

    if(audioBuffer1.getFormat() == audioBuffer2.getFormat())
    {
        uint8_t *newData=newBuffer.getBuffer();

        if(isPlanar(audioBuffer1))
        {
            uint8_t *data1=audioBuffer1.getBuffer();
            uint8_t *data2=audioBuffer2.getBuffer();

            size_t bufferSize1=audioBuffer1.getSamples()*getTypeSize(audioBuffer1.getFormat());
            size_t bufferSize2=audioBuffer2.getSamples()*getTypeSize(audioBuffer2.getFormat());

            for(unsigned int channel=0; channel<audioBuffer1.getChannels(); ++channel)
            {
                memcpy(newData, data1, bufferSize1);
                newData+=bufferSize1;
                data1+=bufferSize1;
                memcpy(newData, data2, bufferSize2);
                newData+=bufferSize2;
                data2+=bufferSize2;
            }
        }
        else
        {
            size_t bufferSize1=calculateBufferSize(audioBuffer1.getFormat(), audioBuffer1.getChannels(), audioBuffer1.getSamples());
            size_t bufferSize2=calculateBufferSize(audioBuffer2.getFormat(), audioBuffer2.getChannels(), audioBuffer2.getSamples());

            memcpy(newData, audioBuffer1.getBuffer(), bufferSize1);
            memcpy(&newData[bufferSize1], audioBuffer2.getBuffer(), bufferSize2);
        }
    }
    else
    {
        assert(false);//not implemented
    }

    return newBuffer.moveType<_AudioBufferType>();
}

}
#endif //_audioOperations_h_
