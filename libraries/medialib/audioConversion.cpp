#include "medialib/audioConversion.h"

namespace medialib
{

size_t convertToFloat(const AudioBuffer &audioBuffer, std::vector<float *> buffer, size_t bufferSize)
{
    return convertToFloat(audioBuffer, 0, audioBuffer.getSamples(), buffer, bufferSize);
}

size_t convertToFloat(const AudioBuffer &audioBuffer, size_t sampleOffset, size_t samples, std::vector<float *> buffer, size_t bufferSize)
{
    switch(audioBuffer.getFormat())
    {
    case AudioFormat::UInt8:
        return convert<uint8_t>(audioBuffer, sampleOffset, samples, buffer, bufferSize);
        break;
    case AudioFormat::Int16:
        return convert<int16_t>(audioBuffer, sampleOffset, samples, buffer, bufferSize);
        break;
    case AudioFormat::Int32:
        return convert<int32_t>(audioBuffer, sampleOffset, samples, buffer, bufferSize);
        break;
    case AudioFormat::Float:
        return 0;
        break;
    case AudioFormat::Double:
        return convert<double>(audioBuffer, sampleOffset, samples, buffer, bufferSize);
        break;
    case AudioFormat::UInt8P:
        return convert<uint8_t>(audioBuffer, sampleOffset, samples, buffer, bufferSize);
        break;
    case AudioFormat::Int16P:
        return convert<int16_t>(audioBuffer, sampleOffset, samples, buffer, bufferSize);
        break;
    case AudioFormat::Int32P:
        return convert<int32_t>(audioBuffer, sampleOffset, samples, buffer, bufferSize);
        break;
    case AudioFormat::FloatP:
        return 0;
        break;
    case AudioFormat::DoubleP:
        return convert<double>(audioBuffer, sampleOffset, samples, buffer, bufferSize);
        break;
    }
    return 0;
}

size_t convertAudioBuffer(const AudioBuffer &audioBufferFrom, AudioBuffer &audioBufferTo)
{
	switch(audioBufferFrom.getFormat())
	{
	case AudioFormat::UInt8:
		return convertAudioBuffer<uint8_t>(audioBufferFrom, audioBufferTo);
		break;
	case AudioFormat::Int16:
		return convertAudioBuffer<uint16_t>(audioBufferFrom, audioBufferTo);
		break;
	case AudioFormat::Int32:
		return convertAudioBuffer<uint32_t>(audioBufferFrom, audioBufferTo);
		break;
	case AudioFormat::Float:
		return convertAudioBuffer<float>(audioBufferFrom, audioBufferTo);
		break;
	case AudioFormat::Double:
		return convertAudioBuffer<double>(audioBufferFrom, audioBufferTo);
		break;
	case AudioFormat::UInt8P:
		return convertAudioBuffer<uint8_t>(audioBufferFrom, audioBufferTo);
		break;
	case AudioFormat::Int16P:
		return convertAudioBuffer<uint16_t>(audioBufferFrom, audioBufferTo);
		break;
	case AudioFormat::Int32P:
		return convertAudioBuffer<uint32_t>(audioBufferFrom, audioBufferTo);
		break;
	case AudioFormat::FloatP:
		return convertAudioBuffer<float>(audioBufferFrom, audioBufferTo);
		break;
	case AudioFormat::DoubleP:
		return convertAudioBuffer<double>(audioBufferFrom, audioBufferTo);
		break;
	}
	return 0;
}

std::vector<uint8_t> toImage(AudioBuffer audioBuffer, size_t width, size_t height)
{
	size_t samples=audioBuffer.getSamples();
	size_t skip=samples/width;
	std::vector<uint8_t> data;
	std::vector<float> bufferVec;
	uint8_t *buffer;

//	if(audioBuffer.getFormat() != AudioFormat::Float)
//		bufferVec
//	uint8_t *buffer=audioBuffer.getBuffer();

	data.resize(width*height);
	for(size_t x=0; x<width; ++x)
	{
		auto values=std::minmax_element(&buffer[x], &buffer[x+skip]);
		float value;

		if(*values.first>abs(*values.second))
			value=convertToDb(*values.first);
		else
			value=convertToDb(*values.second);
	}
	return data;
}

}//namespace medialib

