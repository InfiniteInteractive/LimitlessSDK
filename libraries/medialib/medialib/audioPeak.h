#ifndef _medialib_audioPeak_h_
#define _medialib_audioPeak_h_

#include "medialib/medialibDefine.h"
#include "medialib/accumulatorType.h"

#include "audioBuffer.h"
#include <algorithm>

#include <cassert>

namespace medialib
{

medialib_EXPORT std::vector<float> peakAudioBuffer(AudioBuffer audioBuffer);

template<typename _SampleType>
std::vector<float> normalize(_SampleType *buffer, int samples)
{
	std::vector<float> peakValues(samples);

	_SampleType max=std::numeric_limits<_SampleType>::max();

	for(size_t index=0; index<samples; ++index)
		peakValues[index]=(float)buffer[index]/max;

	return peakValues;
}

template<typename _SampleType>
std::vector<_SampleType> peak(_SampleType *buffer, int channels, int samples)
{
	std::vector<_SampleType> peakValues(channels);
	std::vector<_SampleType> min(channels);
	std::vector<_SampleType> max(channels);
	std::vector<int> direction(channels);

	_SampleType *data=(_SampleType *)buffer;
	_SampleType value;

	for(size_t channel=0; channel<channels; ++channel)
	{
		peakValues[channel]=0;// std::numeric_limits<_SampleType>::min();
		direction[channel]=0;

		min[channel]=(*data);
		max[channel]=(*data);
		data++;
	}

	for(size_t sample=1; sample<samples; ++sample)
	{
		for(size_t channel=0; channel<channels; ++channel)
		{
			value=abs((*data));

			if(direction[channel])
			{
				if(value>=max[channel])
					max[channel]=value;
				else
				{
					value=max[channel]-min[channel];
					if(value>peakValues[channel])
						peakValues[channel]=value;
					min[channel]=(*data);
					direction[channel]=0;
				}
			}
			else
			{
				if(value<=min[channel])
					min[channel]=value;
				else
				{
					value=max[channel]-min[channel];
					if(value>peakValues[channel])
						peakValues[channel]=value;
					max[channel]=(*data);
					direction[channel]=1;
				}
			}

			data++;
		}
	}

	return peakValues;
}

template<typename _SampleType>
std::vector<_SampleType> peakPlanar(_SampleType *buffer, int channels, int samples, bool normalize=true)
{
	std::vector<_SampleType> peakValues(channels);
	_SampleType *data=(_SampleType *)buffer;
	_SampleType value;

	for(size_t channel=0; channel<channels; ++channel)
	{
		peakValues[channel]=std::numeric_limits<_SampleType>::min();
		for(size_t sample=0; sample<samples; ++sample)
		{
			value=abs((*data));

			if(value > peakValues[channel])
				peakValues[channel]=value;
			data++;
		}
	}

	return peakValues;
}

}//namespace medialib

#endif //_medialib_audioPeak_h_
