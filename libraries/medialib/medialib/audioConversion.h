#ifndef _audioConversion_h_
#define _audioConversion_h_

#include "medialib/medialibDefine.h"
#include "audioBuffer.h"
#include <algorithm>

namespace medialib
{

/// Converts audio buffer into float buffer
/// 
/// \param audioBuffer AudioBuffer to convert
/// \param buffer buffer to copy conversion to
/// \param buffer size of buffer
size_t convertToFloat(const AudioBuffer &audioBuffer, std::vector<float *> buffer, size_t bufferSize);
size_t convertToFloat(const AudioBuffer &audioBuffer, size_t sampleOffset, size_t samples, std::vector<float *> buffer, size_t bufferSize);

//template<typename _From> void convert(const AudioBuffer &audioBuffer, _To *buffer, size_t size)
//{
//    std::static_assert(false, "Function not implemented");
//}

template<typename _From, typename _To>
using EnableIfDifferentAndSigned=typename std::enable_if<std::is_unsigned<_From>::value && !std::is_same<_From, _To>::value>::type;
template<typename _From, typename _To>
using EnableIfDifferentAndUnsigned=typename std::enable_if<!std::is_unsigned<_From>::value && !std::is_same<_From, _To>::value>::type;
template<typename _From, typename _To>
using EnableIfSame=typename std::enable_if<std::is_same<_From, _To>::value>::type;


template<typename _From, typename _To, typename _Flag=void >
struct ConvertSample
{
    static _To value(const _From &sample) { std::static_assert(false, "Not implemented"); }

};

template<typename _From, typename _To>
struct ConvertSample<_From, _To, EnableIfDifferentAndSigned<_From, _To>>
{
    static _To value(const _From &sample) { return (_To)abs((float)sample-middle)/(middle); }

    static constexpr _From middle=(std::numeric_limits<_From>::max()-std::numeric_limits<_From>::min())/2;
};

template<typename _From, typename _To>
struct ConvertSample<_From, _To, EnableIfDifferentAndUnsigned<_From, _To>>
{
    static _To value(const _From &sample) { return (_To)abs(sample)/(range);}

    static constexpr _From range=std::numeric_limits<_From>::max();
};

template<typename _From, typename _To>
struct ConvertSample<_From, _To, EnableIfSame<_From, _To>>
{
	static _To value(const _From &sample) { return sample; }
};

size_t convertAudioBuffer(const AudioBuffer &audioBufferFrom, AudioBuffer &audioBufferTo);

template<typename _FromType>
size_t convertAudioBuffer(const AudioBuffer &audioBufferFrom, AudioBuffer &audioBufferTo)
{
	switch(audioBufferTo.getFormat())
	{
	case AudioFormat::UInt8:
	{
		std::vector<uint8_t *> buffer;

		buffer.push_back(audioBufferTo.getBuffer());
		return convert<_FromType>(audioBufferFrom, 0, audioBufferFrom.getSamples(), buffer, audioBufferTo.getSize());
	}
	break;
	case AudioFormat::Int16:
	{
		std::vector<uint16_t *> buffer;

		buffer.push_back((uint16_t *)audioBufferTo.getBuffer());
		return convert<_FromType>(audioBufferFrom, 0, audioBufferFrom.getSamples(), buffer, audioBufferTo.getSize());
	}
	break;
	case AudioFormat::Int32:
	{
		std::vector<uint32_t *> buffer;

		buffer.push_back((uint32_t *)audioBufferTo.getBuffer());
		return convert<_FromType>(audioBufferFrom, 0, audioBufferFrom.getSamples(), buffer, audioBufferTo.getSize());
	}
	break;
	case AudioFormat::Float:
	{
		std::vector<float *> buffer;

		buffer.push_back((float *)audioBufferTo.getBuffer());
		return convert<_FromType>(audioBufferFrom, 0, audioBufferFrom.getSamples(), buffer, audioBufferTo.getSize());
	}
	break;
	case AudioFormat::Double:
	{
		std::vector<double *> buffer;

		buffer.push_back((double *)audioBufferTo.getBuffer());
		return convert<_FromType>(audioBufferFrom, 0, audioBufferFrom.getSamples(), buffer, audioBufferTo.getSize());
	}
	break;
	case AudioFormat::UInt8P:
	{
		std::vector<uint8_t *> buffer;

		buffer.push_back((uint8_t *)audioBufferTo.getBuffer());
		return convert<_FromType>(audioBufferFrom, 0, audioBufferFrom.getSamples(), buffer, audioBufferTo.getSize());
	}
	break;
	case AudioFormat::Int16P:
	{
		std::vector<uint16_t *> buffer;

		buffer.push_back((uint16_t *)audioBufferTo.getBuffer());
		return convert<_FromType>(audioBufferFrom, 0, audioBufferFrom.getSamples(), buffer, audioBufferTo.getSize());
	}
	break;
	case AudioFormat::Int32P:
	{
		std::vector<uint32_t *> buffer;

		buffer.push_back((uint32_t *)audioBufferTo.getBuffer());
		return convert<_FromType>(audioBufferFrom, 0, audioBufferFrom.getSamples(), buffer, audioBufferTo.getSize());
	}
	break;
	case AudioFormat::FloatP:
	{
		std::vector<float *> buffer;

		buffer.push_back((float *)audioBufferTo.getBuffer());
		return convert<_FromType>(audioBufferFrom, 0, audioBufferFrom.getSamples(), buffer, audioBufferTo.getSize());
	}
	break;
	case AudioFormat::DoubleP:
	{
		std::vector<double *> buffer;

		buffer.push_back((double *)audioBufferTo.getBuffer());
		return convert<_FromType>(audioBufferFrom, 0, audioBufferFrom.getSamples(), buffer, audioBufferTo.getSize());
	}
	break;
	}
	return 0;
}

template<typename _From, typename _To> size_t convert(const AudioBuffer &audioBuffer, size_t sampleOffset, size_t samples, std::vector<_To *> buffers, size_t size)
{
    _From min=std::numeric_limits<_From>::min();
    _From max=std::numeric_limits<_From>::max();
    _From middle=(max-min)/2;
    _From *fromBuffer=(_From *)audioBuffer.getBuffer();

    size_t audioBufferOffset=sampleOffset*sizeof(_From);
    size_t audioBufferSize=samples*sizeof(_From);

    size_t loopSize=std::min(audioBufferSize, size/sizeof(_To));
    size_t channels=std::min((size_t)audioBuffer.getChannels(), buffers.size());
    size_t index=audioBufferOffset;

    if(isPlanar(audioBuffer))
    {
        size_t stride=audioBuffer.getSamples();

        for(size_t i=0; i<channels; ++i)
        {
            _To *buffer=buffers[i];

            for(size_t j=0; j<loopSize; ++j)
            {
                //               buffer[j]=(float)abs(fromBuffer[index]-middle)/(middle);
                buffer[j]=ConvertSample<_From, _To>::value(fromBuffer[index]);
                ++index;
            }

            index=audioBufferOffset+(i*stride);
        }
    }
    else
    {
        for(size_t i=0; i<loopSize; ++i)
        {
            for(size_t j=0; j<channels; ++j)
            {
                //                buffers[j][i]=(float)abs(fromBuffer[index]-middle)/(middle);
                buffers[j][i]=ConvertSample<_From, _To>::value(fromBuffer[index]);
                ++index;
            }
        }
    }

    return loopSize;
}

template<typename _From, typename _To> size_t convert(_From *fromBuffer, size_t samples, _To *toBuffer, size_t toBufferSize)
{
    size_t requriedBufferSize=samples*sizeof(_To);
    size_t loopSize=std::min(requriedBufferSize, toBufferSize/sizeof(_To))/sizeof(_To);
    
    for(size_t i=0; i<loopSize; ++i)
    {
        toBuffer[i]=ConvertSample<_From, _To>::value(fromBuffer[i]);
    }

    return loopSize;
}

//template<typename _From> typename std::enable_if<!std::is_unsigned<_From>::value, size_t>::type convert(const AudioBuffer &audioBuffer, size_t sampleOffset, size_t samples, std::vector<float *> buffer, size_t size)
//{
//    _From range=std::numeric_limits<_From>::max();
//    _From *fromBuffer=(_From *)audioBuffer.buffer;
//
//    size_t audioBufferOffset=sampleOffset*sizeof(_From);
//    size_t audioBufferSize=samples*sizeof(_From);
//
//    size_t loopSize=std::min(audioBufferSize, size/sizeof(float));
//    size_t channels=std::min(audioBuffer.channels, buffers.size());
//    size_t index=audioBufferOffset;
//
//    if(audioBuffer.isPlanar())
//    {
//        size_t stride=audioBuffer.samples;
//
//        for(size_t i=0; i<channels; ++i)
//        {
//            float *buffer=buffers[i];
//
//            for(size_t j=0; j<loopSize; ++j)
//            {
//                buffer[j]=(float)abs(fromBuffer[index])/(range);
//                ++index;
//            }
//
//            index=audioBufferOffset+(i*stride);
//        }
//    }
//    else
//    {
//        for(size_t i=0; i<loopSize; ++i)
//        {
//            for(size_t j=0; j<channels; ++j)
//            {
//                buffers[j][i]=(float)abs(fromBuffer[index])/(range);
//                ++index;
//            }
//        }
//    }
//
//    return loopSize;
//}

//template<> size_t convert<double>(const AudioBuffer &audioBuffer, size_t sampleOffset, size_t samples, std::vector<float *> buffer, size_t size)
//{
//    double *fromBuffer=(double *)audioBuffer.buffer;
//
//    size_t audioBufferOffset=sampleOffset*sizeof(double);
//    size_t audioBufferSize=samples*sizeof(double);
//
//    size_t loopSize=std::min(audioBufferSize, size/sizeof(float));
//    size_t channels=std::min(audioBuffer.channels, buffers.size());
//    size_t index=audioBufferOffset;
//
//    if(audioBuffer.isPlanar())
//    {
//        size_t stride=audioBuffer.samples;
//
//        for(size_t i=0; i<channels; ++i)
//        {
//            float *buffer=buffers[i];
//
//            for(size_t j=0; j<loopSize; ++j)
//            {
//                buffer[j]=(float)fromBuffer[index];
//                ++index;
//            }
//
//            index=audioBufferOffset+(i*stride);
//        }
//    }
//    else
//    {
//        for(size_t i=0; i<loopSize; ++i)
//        {
//            for(size_t j=0; j<channels; ++j)
//            {
//                buffers[j][i]=fromBuffer[index];
//                ++index;
//            }
//        }
//    }
//
//    return loopSize;
//}

template<typename _SampleType> typename std::enable_if<std::is_unsigned<_SampleType>::value, float>::type convertToDb(_SampleType sample)
{
    _SampleType min=std::numeric_limits<_SampleType>::min();
    _SampleType max=std::numeric_limits<_SampleType>::max();
    _SampleType middle=(max-min)/2;

    float normalizedSample=(float)abs(sample-middle)/(middle);

    return 20.0f*std::log10(normalizedSample);   
}

template<typename _SampleType> typename std::enable_if<!std::is_unsigned<_SampleType>::value, float>::type convertToDb(_SampleType sample)
{
    _SampleType min=std::numeric_limits<_SampleType>::min();
    _SampleType max=std::numeric_limits<_SampleType>::max();

    float normalizedSample=(float)abs(sample)/(max);

    return 20.0f*std::log10(normalizedSample);
}

template<> inline float convertToDb<float>(float sample)
{
    //float db calculated between -1.0 and 1.0
    return 20.0f*std::log10(abs(sample));
}

template<> inline float convertToDb<double>(double sample)
{
    //double db calculated between -1.0 and 1.0
    return 20.0f*std::log10(abs((float)sample));
}

medialib_EXPORT std::vector<uint8_t> toImage(AudioBuffer audioBuffer, size_t width, size_t height);

}
#endif //_audioConversion_h_
