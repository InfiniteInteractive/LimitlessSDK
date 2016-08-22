#ifndef _medialib_audioMix_h_
#define _medialib_audioMix_h_

#include "medialib/medialibDefine.h"
#include "medialib/accumulatorType.h"

#include "audioBuffer.h"
#include <algorithm>

#include <cassert>

namespace medialib
{

enum class MixMethod
{
	Clipping,
	LinearAttenuation,
	LinearDRC,
	LogrithmicDRC
};

medialib_EXPORT void mixAudioBufferClipping(std::vector<AudioBuffer> const &audioBuffers, AudioBuffer &mix, float threshold=0.6);
medialib_EXPORT void mixAudioBufferLinearAttenuation(std::vector<AudioBuffer> const &audioBuffers, AudioBuffer &mix);
medialib_EXPORT void mixAudioBufferLinearDRC(std::vector<AudioBuffer> const &audioBuffers, AudioBuffer &mix, float threshold=0.6);
medialib_EXPORT void mixAudioBufferLogrithmicDRC(std::vector<AudioBuffer> const &audioBuffers, AudioBuffer &mix, float threshold=0.6);
medialib_EXPORT void mixAudioBuffer(std::vector<AudioBuffer> const &audioBuffers, AudioBuffer mix, MixMethod method=MixMethod::LinearAttenuation, float threshold=0.6);

medialib_EXPORT void mixAudioBufferChannels(AudioBuffer audioBuffer, std::vector<float> &channelLevels, AudioBuffer mix, MixMethod method=MixMethod::LinearAttenuation, float threshold=0.6);


template<typename _Type>
void mixLinearAttenuation(std::vector<_Type *> const &buffers, _Type *mixBuffer, unsigned int samples)
{
	accumulatorType<_Type>::Type accumulator;
	size_t inputs=buffers.size();

	for(size_t i=0; i<samples; ++i)
	{
		accumulator=0;

		for(size_t j=0; j<buffers.size(); ++j)
		{
			accumulator+=buffers[j][i];
		}

		mixBuffer[i]=(_Type)(accumulator/inputs);
	}
}

template<typename _Type>
void mixChannelsLinearAttenuation(_Type *buffer, std::vector<float> &channelLevels, _Type *mixBuffer, unsigned int samples)
{
	accumulatorType<_Type>::Type accumulator;
	size_t inputs=0;

	for(size_t j=0; j<channelLevels.size(); ++j)
	{
		if(channelLevels[j]>0.0f)
			++inputs;
	}

	if(inputs<=0)
	{
		for(size_t i=0; i<samples; ++i)
		{
			mixBuffer[i]=TypeMiddle<_Type>::value;
		}
	}
	else
	{
		size_t index=0;
		for(size_t i=0; i<samples; ++i)
		{
			accumulator=0;

			for(size_t j=0; j<channelLevels.size(); ++j)
			{
				if(channelLevels[j]>0.0f)
					accumulator+=buffer[index];
				index++;
			}

			mixBuffer[i]=(_Type)(accumulator/inputs);
		}
	}
}

template<typename _Type>
void mixChannelsLinearAttenuationPlanar(_Type *buffer, std::vector<float> &channelLevels, _Type *mixBuffer, unsigned int samples)
{
	accumulatorType<_Type>::Type accumulator;
	size_t inputs=0;
	std::vector<_Type *>channelBuffers(channelLevels.size());

	size_t channelIndex=0;
	for(size_t j=0; j<channelLevels.size(); ++j)
	{
		channelBuffers[j]=buffer[channelIndex];

		channelIndex+=samples;
		if(channelLevels[j]>0.0f)
			++inputs;
	}

	for(size_t i=0; i<samples; ++i)
	{
		accumulator=0;

		for(size_t j=0; j<channelLevels.size(); ++j)
		{
			if(channelLevels[j]>0.0f)
				accumulator+=channelBuffers[j][i];
		}

		mixBuffer[i]=(_Type)(accumulator/inputs);
	}
}

template<typename _Type, typename _Flag=void>
struct TypeMiddle
{
	constexpr static _Type value=(std::numeric_limits<_Type>::max()-std::numeric_limits<_Type>::min())/(_Type)2;
};

template<typename _Type>
struct TypeMiddle<_Type, typename std::enable_if<!std::is_unsigned<_Type>::value>::type>
{
	constexpr static _Type value=(_Type)0;
};

float getAlpha(float threshold, size_t inputs);

template<typename _Type>
void mixLogrithmicDRC(std::vector<_Type *> const &buffers, _Type *mixBuffer, unsigned int samples, float threshold)
{
	accumulatorType<_Type>::Type accumulator;
	
	//scale threshold with the number of samples
	threshold=pow(threshold, buffers.size()-1);

	float alpha=getAlpha(threshold, buffers.size());
	float inputs=(float)buffers.size();
	accumulatorType<_Type>::Type maxSignal=std::numeric_limits<_Type>::max()*inputs;
	_Type middle=TypeMiddle<_Type>::value;
	float value;

	if(middle == (_Type)0)
	{
		for(size_t i=0; i<samples; ++i)
		{
			accumulator=(_Type)0;

			for(size_t j=0; j<buffers.size(); ++j)
				accumulator+=buffers[j][i];

			value=accumulator/maxSignal;

//			if(accumulator>typeThreshold)
			if(value >= threshold)
			{
				value=threshold+(1-threshold)*(log(1.0f-alpha*(value-threshold)/(inputs-threshold))/log(1.0-alpha*threshold));
				mixBuffer[i]=value*std::numeric_limits<_Type>::max();
			}
			else
				mixBuffer[i]=accumulator;
		}
	}
	else
	{
		maxSignal/=(_Type)2;

		for(size_t i=0; i<samples; ++i)
		{
			accumulator=0;

			for(size_t j=0; j<buffers.size(); ++j)
			{
				accumulator+=(buffers[j][i]-middle);
			}

			value=accumulator/maxSignal;

//			if(accumulator>typeThreshold)
			if(value>threshold)
			{
				value=threshold+(1-threshold)*(log(1.0f-alpha*(value-threshold)/(inputs-threshold))/log(1.0-alpha*threshold));
				mixBuffer[i]=value*std::numeric_limits<_Type>::max();
			}
			else
				mixBuffer[i]=accumulator;
		}
	}
}

template<typename _Type>
void mixChannelsLogrithmicDRC(_Type *buffer, std::vector<float> &channelLevels, _Type *mixBuffer, unsigned int samples, float threshold)
{
	assert(false);
}

template<typename _Type>
void mixChannelsLogrithmicDRCPlanar(_Type *buffer, std::vector<float> &channelLevels, _Type *mixBuffer, unsigned int samples, float threshold)
{
	assert(false);
}

}
#endif //_medialib_audioMix_h_
