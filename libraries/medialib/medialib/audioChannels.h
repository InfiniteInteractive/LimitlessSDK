#ifndef _medialib_audioChannels_h_
#define _medialib_audioChannels_h_

#include "medialib/medialibDefine.h"
#include "medialib/accumulatorType.h"
#include "medialib/audioConversion.h"

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

medialib_EXPORT void copyAudioBufferChannel(AudioBuffer srcBuffer, size_t srcChannel, AudioBuffer dstBuffer, size_t dstChannel);

template<typename _SrcType, typename _DstType>
void copyChannel(_SrcType *srcBuffer, size_t srcChannels, size_t srcChannel, _DstType *dstBuffer, size_t dstChannels,  size_t dstChannel, unsigned int samples)
{
	_SrcType *srcBufferPos=&srcBuffer[srcChannel];
	_DstType *dstBufferPos=&dstBuffer[dstChannel];

	size_t srcIndex=0;
	size_t dstIndex=0;

	for(size_t i=0; i<samples; ++i)
	{
		dstBufferPos[dstIndex]=ConvertSample<_SrcType, _DstType>::value(srcBufferPos[srcIndex]);
		srcIndex+=srcChannels;
		dstIndex+=dstChannels;
	}
}

template<typename _SrcType, typename _DstType>
void copyChannelPlanar(_SrcType *srcBuffer, size_t srcChannels, size_t srcChannel, _DstType *dstBuffer, size_t dstChannels, size_t dstChannel, unsigned int samples)
{
	assert(false);
}

template<typename _SrcType, typename _DstType>
void copyChannelNonToPlanar(_SrcType *srcBuffer, size_t srcChannels, size_t srcChannel, _DstType *dstBuffer, size_t dstChannels, size_t dstChannel, unsigned int samples)
{
	assert(false);
}

template<typename _SrcType, typename _DstType>
void copyChannelPlanarToNon(_SrcType *srcBuffer, size_t srcChannels, size_t srcChannel, _DstType *dstBuffer, size_t dstChannels, size_t dstChannel, unsigned int samples)
{
	assert(false);
}

}//namespace medialib
#endif //_medialib_audioChannels_h_
