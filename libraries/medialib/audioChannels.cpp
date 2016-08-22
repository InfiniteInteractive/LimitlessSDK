#include "medialib/audioChannels.h"
#include "medialib/accumulatorType.h"

namespace medialib
{

template<typename _Type>
void callCombineChannels(std::vector<AudioBuffer> const &audioBuffers, AudioBuffer &combinedBuffer, unsigned int samples)
{
	std::vector<_Type *> buffers(audioBuffers.size());

	for(size_t i=0; i<audioBuffers.size(); ++i)
		buffers[i]=(_Type *)audioBuffers[i].getBuffer();

	combineChannels<_Type>(buffers, (_Type *)combinedBuffer.getBuffer(), samples);
}

template<typename _Type>
void callCombineChannelsPlanar(std::vector<AudioBuffer> const &audioBuffers, AudioBuffer &combinedBuffer, unsigned int samples)
{
	std::vector<_Type *> buffers(audioBuffers.size());

	for(size_t i=0; i<audioBuffers.size(); ++i)
		buffers[i]=(_Type *)audioBuffers[i].getBuffer();

	combineChannelsPlanar<_Type>(buffers, (_Type *)combinedBuffer.getBuffer(), samples);
}

void combineAudioBufferChannels(std::vector<AudioBuffer> const &audioBuffers, AudioBuffer combinedBuffer)
{
	AudioFormat format=audioBuffers[0].getFormat();
	unsigned int samples=audioBuffers[0].getSamples();
	unsigned int channels=audioBuffers[0].getChannels();

	for(size_t i=1; i<audioBuffers.size(); ++i)
	{
		if(audioBuffers[i].getFormat()!=format)
			return;
		if(audioBuffers[i].getSamples()!=samples)
			return;
		if(audioBuffers[i].getChannels()!=channels)
			return;
	}

	switch(format)
	{
	case AudioFormat::UInt8:
		callCombineChannels<uint8_t>(audioBuffers, combinedBuffer, samples);
		break;
	case AudioFormat::Int16:
		callCombineChannels<int16_t>(audioBuffers, combinedBuffer, samples);
		break;
	case AudioFormat::Int32:
		callCombineChannels<int32_t>(audioBuffers, combinedBuffer, samples);
		break;
	case AudioFormat::Float:
		callCombineChannels<float>(audioBuffers, combinedBuffer, samples);
		break;
	case AudioFormat::Double:
		callCombineChannels<double>(audioBuffers, combinedBuffer, samples);
		break;
	case AudioFormat::UInt8P:
		callCombineChannelsPlanar<uint8_t>(audioBuffers, combinedBuffer, samples);
		break;
	case AudioFormat::Int16P:
		callCombineChannelsPlanar<int16_t>(audioBuffers, combinedBuffer, samples);
		break;
	case AudioFormat::Int32P:
		callCombineChannelsPlanar<int32_t>(audioBuffers, combinedBuffer, samples);
		break;
	case AudioFormat::FloatP:
		callCombineChannelsPlanar<float>(audioBuffers, combinedBuffer, samples);
		break;
	case AudioFormat::DoubleP:
		callCombineChannelsPlanar<double>(audioBuffers, combinedBuffer, samples);
		break;
	}
}

void splitAudioBufferChannels(AudioBuffer combinedBuffer, std::vector<AudioBuffer> const &audioBuffers)
{
	assert(false);
}

}//namespace medialib

