#ifndef _medialib_audioChannels_h_
#define _medialib_audioChannels_h_

#include "medialib/medialibDefine.h"
#include "medialib/accumulatorType.h"

#include "audioBuffer.h"
#include <algorithm>

#include <cassert>

namespace medialib
{

medialib_EXPORT void combineAudioBufferChannels(std::vector<AudioBuffer> const &audioBuffers, AudioBuffer combinedBuffer);
medialib_EXPORT void splitAudioBufferChannels(AudioBuffer combinedBuffer, std::vector<AudioBuffer> const &audioBuffers);

template<typename _Type>
void combineChannels(std::vector<_Type *> const &buffers, _Type *combinedBuffer, unsigned int samples)
{
	size_t index=0;
	for(size_t i=0; i<samples; ++i)
	{
		for(size_t j=0; j<buffers.size(); ++j)
			combinedBuffer[index++]=buffers[j][i];
	}
}

template<typename _Type>
void combineChannelsPlanar(std::vector<_Type *> const &buffers, _Type *combinedBuffer, unsigned int samples)
{
	size_t index=0;
	size_t stride=samples*sizeof(_Type);

	for(size_t i=0; i<buffers.size(); ++i)
	{
		memcpy(buffers[i], &combinedBuffer[index], stride);
		index+=stride;
	}
}

}//namespace medialib
#endif //_medialib_audioChannels_h_
