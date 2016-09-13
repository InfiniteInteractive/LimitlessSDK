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

template<typename _Type>
void callCopyChannel(const AudioBuffer &srcBuffer, size_t srcChannel, const AudioBuffer &dstBuffer, size_t dstChannel)
{
	unsigned int samples=srcBuffer.getSamples();

	if(dstBuffer.getSamples()<samples)
		samples=dstBuffer.getSamples();

	switch(dstBuffer.getFormat())
	{
	case AudioFormat::UInt8:
		copyChannel<_Type, uint8_t>((_Type *)srcBuffer.getBuffer(), srcBuffer.getChannels(), srcChannel, (uint8_t *)dstBuffer.getBuffer(), dstBuffer.getChannels(), dstChannel, samples);
		break;
	case AudioFormat::Int16:
		copyChannel<_Type, int16_t>((_Type *)srcBuffer.getBuffer(), srcBuffer.getChannels(), srcChannel, (int16_t *)dstBuffer.getBuffer(), dstBuffer.getChannels(), dstChannel, samples);
		break;
	case AudioFormat::Int32:
		copyChannel<_Type, int32_t>((_Type *)srcBuffer.getBuffer(), srcBuffer.getChannels(), srcChannel, (int32_t *)dstBuffer.getBuffer(), dstBuffer.getChannels(), dstChannel, samples);
		break;
	case AudioFormat::Float:
		copyChannel<_Type, float>((_Type *)srcBuffer.getBuffer(), srcBuffer.getChannels(), srcChannel, (float *)dstBuffer.getBuffer(), dstBuffer.getChannels(), dstChannel, samples);
		break;
	case AudioFormat::Double:
		copyChannel<_Type, double>((_Type *)srcBuffer.getBuffer(), srcBuffer.getChannels(), srcChannel, (double *)dstBuffer.getBuffer(), dstBuffer.getChannels(), dstChannel, samples);
		break;
	case AudioFormat::UInt8P:
		copyChannelNonToPlanar<_Type, uint8_t>((_Type *)srcBuffer.getBuffer(), srcBuffer.getChannels(), srcChannel, (uint8_t *)dstBuffer.getBuffer(), dstBuffer.getChannels(), dstChannel, samples);
		break;
	case AudioFormat::Int16P:
		copyChannelNonToPlanar<_Type, int16_t>((_Type *)srcBuffer.getBuffer(), srcBuffer.getChannels(), srcChannel, (int16_t *)dstBuffer.getBuffer(), dstBuffer.getChannels(), dstChannel, samples);
		break;
	case AudioFormat::Int32P:
		copyChannelNonToPlanar<_Type, int32_t>((_Type *)srcBuffer.getBuffer(), srcBuffer.getChannels(), srcChannel, (int32_t *)dstBuffer.getBuffer(), dstBuffer.getChannels(), dstChannel, samples);
		break;
	case AudioFormat::FloatP:
		copyChannelNonToPlanar<_Type, float>((_Type *)srcBuffer.getBuffer(), srcBuffer.getChannels(), srcChannel, (float *)dstBuffer.getBuffer(), dstBuffer.getChannels(), dstChannel, samples);
		break;
	case AudioFormat::DoubleP:
		copyChannelNonToPlanar<_Type, double>((_Type *)srcBuffer.getBuffer(), srcBuffer.getChannels(), srcChannel, (double *)dstBuffer.getBuffer(), dstBuffer.getChannels(), dstChannel, samples);
		break;
	}
}

template<typename _Type>
void callCopyChannelPlanar(const AudioBuffer &srcBuffer, size_t srcChannel, const AudioBuffer &dstBuffer, size_t dstChannel)
{
	unsigned int samples=srcBuffer.getSamples();

	if(dstBuffer.getSamples()<samples)
		samples=dstBuffer.getSamples();

	switch(dstBuffer.getFormat())
	{
	case AudioFormat::UInt8:
		copyChannelPlanarToNon<_Type, uint8_t>((_Type *)srcBuffer.getBuffer(), srcBuffer.getChannels(), srcChannel, (uint8_t *)dstBuffer.getBuffer(), dstBuffer.getChannels(), dstChannel, samples);
		break;
	case AudioFormat::Int16:
		copyChannelPlanarToNon<_Type, int16_t>((_Type *)srcBuffer.getBuffer(), srcBuffer.getChannels(), srcChannel, (int16_t *)dstBuffer.getBuffer(), dstBuffer.getChannels(), dstChannel, samples);
		break;
	case AudioFormat::Int32:
		copyChannelPlanarToNon<_Type, int32_t>((_Type *)srcBuffer.getBuffer(), srcBuffer.getChannels(), srcChannel, (int32_t *)dstBuffer.getBuffer(), dstBuffer.getChannels(), dstChannel, samples);
		break;
	case AudioFormat::Float:
		copyChannelPlanarToNon<_Type, float>((_Type *)srcBuffer.getBuffer(), srcBuffer.getChannels(), srcChannel, (float *)dstBuffer.getBuffer(), dstBuffer.getChannels(), dstChannel, samples);
		break;
	case AudioFormat::Double:
		copyChannelPlanarToNon<_Type, double>((_Type *)srcBuffer.getBuffer(), srcBuffer.getChannels(), srcChannel, (double *)dstBuffer.getBuffer(), dstBuffer.getChannels(), dstChannel, samples);
		break;
	case AudioFormat::UInt8P:
		copyChannelPlanar<_Type, uint8_t>((_Type *)srcBuffer.getBuffer(), srcBuffer.getChannels(), srcChannel, (uint8_t *)dstBuffer.getBuffer(), dstBuffer.getChannels(), dstChannel, samples);
		break;
	case AudioFormat::Int16P:
		copyChannelPlanar<_Type, int16_t>((_Type *)srcBuffer.getBuffer(), srcBuffer.getChannels(), srcChannel, (int16_t *)dstBuffer.getBuffer(), dstBuffer.getChannels(), dstChannel, samples);
		break;
	case AudioFormat::Int32P:
		copyChannelPlanar<_Type, int32_t>((_Type *)srcBuffer.getBuffer(), srcBuffer.getChannels(), srcChannel, (int32_t *)dstBuffer.getBuffer(), dstBuffer.getChannels(), dstChannel, samples);
		break;
	case AudioFormat::FloatP:
		copyChannelPlanar<_Type, float>((_Type *)srcBuffer.getBuffer(), srcBuffer.getChannels(), srcChannel, (float *)dstBuffer.getBuffer(), dstBuffer.getChannels(), dstChannel, samples);
		break;
	case AudioFormat::DoubleP:
		copyChannelPlanar<_Type, double>((_Type *)srcBuffer.getBuffer(), srcBuffer.getChannels(), srcChannel, (double *)dstBuffer.getBuffer(), dstBuffer.getChannels(), dstChannel, samples);
		break;
	}
}

void copyAudioBufferChannel(AudioBuffer srcBuffer, size_t srcChannel, AudioBuffer dstBuffer, size_t dstChannel)
{
	switch(srcBuffer.getFormat())
	{
	case AudioFormat::UInt8:
		callCopyChannel<uint8_t>(srcBuffer, srcChannel, dstBuffer, dstChannel);
		break;
	case AudioFormat::Int16:
		callCopyChannel<int16_t>(srcBuffer, srcChannel, dstBuffer, dstChannel);
		break;
	case AudioFormat::Int32:
		callCopyChannel<int32_t>(srcBuffer, srcChannel, dstBuffer, dstChannel);
		break;
	case AudioFormat::Float:
		callCopyChannel<float>(srcBuffer, srcChannel, dstBuffer, dstChannel);
		break;
	case AudioFormat::Double:
		callCopyChannel<double>(srcBuffer, srcChannel, dstBuffer, dstChannel);
		break;
	case AudioFormat::UInt8P:
		callCopyChannelPlanar<uint8_t>(srcBuffer, srcChannel, dstBuffer, dstChannel);
		break;
	case AudioFormat::Int16P:
		callCopyChannelPlanar<int16_t>(srcBuffer, srcChannel, dstBuffer, dstChannel);
		break;
	case AudioFormat::Int32P:
		callCopyChannelPlanar<int32_t>(srcBuffer, srcChannel, dstBuffer, dstChannel);
		break;
	case AudioFormat::FloatP:
		callCopyChannelPlanar<float>(srcBuffer, srcChannel, dstBuffer, dstChannel);
		break;
	case AudioFormat::DoubleP:
		callCopyChannelPlanar<double>(srcBuffer, srcChannel, dstBuffer, dstChannel);
		break;
	}
}

}//namespace medialib

