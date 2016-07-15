#include "medialib/audioOperations.h"
#include <cassert>

namespace medialib
{

void concat(AudioBuffer audioBufferDst, const AudioBuffer &audioBufferSrc)
{
    if((audioBufferSrc.getSize()==0)) //nothing to add
        return;

    if(audioBufferDst.getChannels()!=audioBufferSrc.getChannels())
        return;

    if(audioBufferDst.getSampleRate()!=audioBufferSrc.getSampleRate())
    {
        assert(false);//not implemented yet, need conversion
        return;
    }

    if(audioBufferDst.getFormat()!=audioBufferSrc.getFormat())
        return;

    //get offset before we resize it
    size_t dstOffset=audioBufferDst.getSamples()*getTypeSize(audioBufferDst.getFormat());

    audioBufferDst.resize(audioBufferDst.getSamples()+audioBufferSrc.getSamples());

    if(isPlanar(audioBufferDst))
    {
        for(unsigned int channel=0; channel<audioBufferDst.getChannels(); ++channel)
        {
            uint8_t *dataDst=audioBufferDst.getBuffer(channel)+dstOffset;
            uint8_t *dataSrc=audioBufferSrc.getBuffer(channel);

            memcpy(dataDst, dataSrc, audioBufferSrc.getSize(channel));
        }
    }
    else
    {
        dstOffset*=audioBufferDst.getChannels();//increase offset by numver of channels
        uint8_t *dataDst=audioBufferDst.getBuffer()+dstOffset;

        memcpy(dataDst, audioBufferSrc.getBuffer(), audioBufferSrc.getSize());
    }
}

}//namespace medialib

