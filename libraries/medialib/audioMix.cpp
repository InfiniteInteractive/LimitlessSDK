#include "medialib/audioMix.h"
#include "medialib/accumulatorType.h"

namespace medialib
{

void mixAudioBufferClipping(std::vector<AudioBuffer> const &audioBuffers, AudioBuffer &mixBuffer, float threshold)
{
	assert(false);
}

template<typename _Type> 
void callMixLinearAttenuation(std::vector<AudioBuffer> const &audioBuffers, AudioBuffer &mixBuffer, unsigned int samples)
{
	std::vector<_Type *> buffers(audioBuffers.size());

	for(size_t i=0; i<audioBuffers.size(); ++i)
		buffers[i]=(_Type *)audioBuffers[i].getBuffer();

	mixLinearAttenuation<_Type>(buffers, (_Type *)mixBuffer.getBuffer(), samples);
}

void mixAudioBufferLinearAttenuation(std::vector<AudioBuffer> const &audioBuffers, AudioBuffer &mixBuffer)
{
	if(audioBuffers.empty())
		return;

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

	samples=samples*channels;

	switch(format)
	{
	case AudioFormat::UInt8:
	case AudioFormat::UInt8P:
		callMixLinearAttenuation<uint8_t>(audioBuffers, mixBuffer, samples);
		break;
	case AudioFormat::Int16:
	case AudioFormat::Int16P:
		callMixLinearAttenuation<int16_t>(audioBuffers, mixBuffer, samples);
		break;
	case AudioFormat::Int32:
	case AudioFormat::Int32P:
		callMixLinearAttenuation<int32_t>(audioBuffers, mixBuffer, samples);
		break;
	case AudioFormat::Float:
	case AudioFormat::FloatP:
		callMixLinearAttenuation<float>(audioBuffers, mixBuffer, samples);
		break;
	case AudioFormat::Double:
	case AudioFormat::DoubleP:
		callMixLinearAttenuation<double>(audioBuffers, mixBuffer, samples);
		break;
	}
}

void mixAudioBufferLinearDRC(std::vector<AudioBuffer> const &audioBuffers, AudioBuffer &mixBuffer, float threshold)
{
	assert(false);
}

template<typename _Type>
void callMixAudioBufferLogrithmicDRC(std::vector<AudioBuffer> const &audioBuffers, AudioBuffer &mixBuffer, unsigned int samples, float threshold)
{
	std::vector<_Type *> buffers(audioBuffers.size());

	for(size_t i=0; i<audioBuffers.size(); ++i)
		buffers[i]=(_Type *)audioBuffers[i].getBuffer();

	mixLogrithmicDRC<_Type>(buffers, (_Type *)mixBuffer.getBuffer(), samples, threshold);
}

void mixAudioBufferLogrithmicDRC(std::vector<AudioBuffer> const &audioBuffers, AudioBuffer &mixBuffer, float threshold)
{
	if(audioBuffers.empty())
		return;

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

	samples=samples*channels;

	switch(format)
	{
	case AudioFormat::UInt8:
	case AudioFormat::UInt8P:
		callMixAudioBufferLogrithmicDRC<uint8_t>(audioBuffers, mixBuffer, samples, threshold);
		break;
	case AudioFormat::Int16:
	case AudioFormat::Int16P:
		callMixAudioBufferLogrithmicDRC<int16_t>(audioBuffers, mixBuffer, samples, threshold);
		break;
	case AudioFormat::Int32:
	case AudioFormat::Int32P:
		callMixAudioBufferLogrithmicDRC<int32_t>(audioBuffers, mixBuffer, samples, threshold);
		break;
	case AudioFormat::Float:
	case AudioFormat::FloatP:
		callMixAudioBufferLogrithmicDRC<float>(audioBuffers, mixBuffer, samples, threshold);
		break;
	case AudioFormat::Double:
	case AudioFormat::DoubleP:
		callMixAudioBufferLogrithmicDRC<double>(audioBuffers, mixBuffer, samples, threshold);
		break;
	}
}

void mixAudioBuffer(std::vector<AudioBuffer> const &audioBuffers, AudioBuffer mixBuffer, MixMethod method, float threshold)
{
	switch(method)
	{
	case MixMethod::Clipping:
		mixAudioBufferClipping(audioBuffers, mixBuffer, threshold);
		break;
	default:
	case MixMethod::LinearAttenuation:
		mixAudioBufferLinearAttenuation(audioBuffers, mixBuffer);
		break;
	case MixMethod::LinearDRC:
		mixAudioBufferLinearDRC(audioBuffers, mixBuffer, threshold);
		break;
	case MixMethod::LogrithmicDRC:
		mixAudioBufferLogrithmicDRC(audioBuffers, mixBuffer, threshold);
		break;
	}
}

/////////////////////////////////////////////////////////////////////////////////
//mixChannels
void mixAudioBufferChannelsClipping(AudioBuffer &audioBuffer, std::vector<float> &channelLevels, AudioBuffer &mixBuffer, float threshold)
{
	assert(false);
}

void mixAudioBufferChannelsLinearAttenuation(AudioBuffer &audioBuffer, std::vector<float> &channelLevels, AudioBuffer &mixBuffer)
{
	if(audioBuffer.getChannels() == 0)
		return;

	AudioFormat format=audioBuffer.getFormat();
	unsigned int samples=audioBuffer.getSamples();

	switch(format)
	{
	case AudioFormat::UInt8:
	case AudioFormat::UInt8P:
		mixChannelsLinearAttenuation<uint8_t>((uint8_t *)audioBuffer.getBuffer(), channelLevels, (uint8_t *)mixBuffer.getBuffer(), samples);
		break;
	case AudioFormat::Int16:
	case AudioFormat::Int16P:
		mixChannelsLinearAttenuation<int16_t>((int16_t *)audioBuffer.getBuffer(), channelLevels, (int16_t *)mixBuffer.getBuffer(), samples);
		break;
	case AudioFormat::Int32:
	case AudioFormat::Int32P:
		mixChannelsLinearAttenuation<int32_t>((int32_t *)audioBuffer.getBuffer(), channelLevels, (int32_t *)mixBuffer.getBuffer(), samples);
		break;
	case AudioFormat::Float:
	case AudioFormat::FloatP:
		mixChannelsLinearAttenuation<float>((float *)audioBuffer.getBuffer(), channelLevels, (float *)mixBuffer.getBuffer(), samples);
		break;
	case AudioFormat::Double:
	case AudioFormat::DoubleP:
		mixChannelsLinearAttenuation<double>((double *)audioBuffer.getBuffer(), channelLevels, (double *)mixBuffer.getBuffer(), samples);
		break;
	}
}

void mixAudioBufferChannelsLinearDRC(AudioBuffer &audioBuffer, std::vector<float> &channelLevels, AudioBuffer &mixBuffer, float threshold)
{
	assert(false);
}

void mixAudioBufferChannelsLogrithmicDRC(AudioBuffer &audioBuffer, std::vector<float> &channelLevels, AudioBuffer &mixBuffer, float threshold)
{
	if(audioBuffer.getChannels() == 0)
		return;

	AudioFormat format=audioBuffer.getFormat();
	unsigned int samples=audioBuffer.getSamples();

	switch(format)
	{
	case AudioFormat::UInt8:
	case AudioFormat::UInt8P:
		mixChannelsLogrithmicDRC<uint8_t>((uint8_t *)audioBuffer.getBuffer(), channelLevels, (uint8_t *)mixBuffer.getBuffer(), samples, threshold);
		break;
	case AudioFormat::Int16:
	case AudioFormat::Int16P:
		mixChannelsLogrithmicDRC<int16_t>((int16_t *)audioBuffer.getBuffer(), channelLevels, (int16_t *)mixBuffer.getBuffer(), samples, threshold);
		break;
	case AudioFormat::Int32:
	case AudioFormat::Int32P:
		mixChannelsLogrithmicDRC<int32_t>((int32_t *)audioBuffer.getBuffer(), channelLevels, (int32_t *)mixBuffer.getBuffer(), samples, threshold);
		break;
	case AudioFormat::Float:
	case AudioFormat::FloatP:
		mixChannelsLogrithmicDRC<float>((float *)audioBuffer.getBuffer(), channelLevels, (float *)mixBuffer.getBuffer(), samples, threshold);
		break;
	case AudioFormat::Double:
	case AudioFormat::DoubleP:
		mixChannelsLogrithmicDRC<double>((double *)audioBuffer.getBuffer(), channelLevels, (double *)mixBuffer.getBuffer(), samples, threshold);
		break;
	}
}

void mixAudioBufferChannels(AudioBuffer audioBuffer, std::vector<float> &channelLevels, AudioBuffer mixBuffer, MixMethod method, float threshold)
{
	switch(method)
	{
	case MixMethod::Clipping:
		mixAudioBufferChannelsClipping(audioBuffer, channelLevels, mixBuffer, threshold);
		break;
	default:
	case MixMethod::LinearAttenuation:
		mixAudioBufferChannelsLinearAttenuation(audioBuffer, channelLevels, mixBuffer);
		break;
	case MixMethod::LinearDRC:
		mixAudioBufferChannelsLinearDRC(audioBuffer, channelLevels, mixBuffer, threshold);
		break;
	case MixMethod::LogrithmicDRC:
		mixAudioBufferChannelsLogrithmicDRC(audioBuffer, channelLevels, mixBuffer, threshold);
		break;
	}
}


struct AlphaTable
{
	AlphaTable():lastThreshold(-1.0){}

	float calculateAlpha(float threshold, size_t inputs, size_t percision=2, size_t iterations=10)
	{
		float alphaLower=2.5f;
		float alphaUpper=10.0f;

		float lastValidLower=0.0f;
		float lastValidUpper=100.0f;
		float leftLower=pow((1.0f+alphaLower), 1.0f/alphaLower);
		float leftUpper=pow((1.0f+alphaUpper), 1.0f/alphaUpper);
		float right=exp((1.0f-threshold)/((float)inputs-threshold));

		double truncValue=pow(10.0, percision);

		for(size_t i=0; i<iterations; ++i)
		{
			if(right<leftLower)
			{
				alphaUpper=alphaLower;
				alphaLower=lastValidLower;
			}
			else if(right>leftUpper)
			{
				alphaLower=alphaUpper;
				alphaUpper=lastValidUpper;
			}
			else
			{
				lastValidLower=alphaLower;
				lastValidUpper=alphaUpper;
				alphaLower=(alphaUpper-alphaLower)/2.0f+alphaLower;
			}

			leftLower=pow((1.0f+alphaLower), 1.0f/alphaLower);
			leftUpper=pow((1.0f+alphaUpper), 1.0f/alphaUpper);

			if(trunc(truncValue*leftLower)==trunc(truncValue*leftUpper))
				return leftLower;
		}

		return leftLower;
	}

//	float get(float threshold, size_t inputs)
//	{
//		size_t alphIndex=(size_t)trunc(threshold*100.0f);
//
//		if(inputs>alphaTable.size())
//			alphaTable.resize(inputs);
//
//		std::vector<float> &table=alphaTable[inputs];
//
//		if(table.empty())
//			table.resize(1000, 0.0f);
//
//		if(table[alphIndex] == 0.0f)
//			table[alphIndex]=calculateAlpha(threshold);
//		return table[alphIndex];
//	}
//	
//	std::vector<std::vector<float>> alphaTable;
	float get(float threshold, size_t inputs)
	{
		if((lastThreshold!=threshold)||(lastInputs!=inputs))
		{
			alpha=calculateAlpha(threshold, inputs);
			lastThreshold=threshold;
			lastInputs=inputs;
		}
		return alpha;
	}

	float alpha;
	float lastThreshold;
	size_t lastInputs;
};

float getAlpha(float threshold, size_t inputs)
{
	static AlphaTable alphaTable;

	return alphaTable.get(threshold, inputs);
}

}//namespace medialib

