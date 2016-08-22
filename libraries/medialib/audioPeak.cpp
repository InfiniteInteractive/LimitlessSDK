#include "medialib/audioPeak.h"
#include "medialib/accumulatorType.h"

namespace medialib
{

template<typename _SampleType>
std::vector<float> callPeakNormalize(AudioBuffer &audioBuffer)
{
	std::vector<_SampleType> peakValues=peak<_SampleType>((_SampleType *)audioBuffer.getBuffer(), audioBuffer.getChannels(), audioBuffer.getSamples());
	return normalize<_SampleType>(peakValues.data(), peakValues.size());
}

template<typename _SampleType>
std::vector<float> callPeakNormalizePlanar(AudioBuffer &audioBuffer)
{
	std::vector<_SampleType> peakValues=peakPlanar<_SampleType>((_SampleType *)audioBuffer.getBuffer(), audioBuffer.getChannels(), audioBuffer.getSamples());
	return normalize<_SampleType>(peakValues.data(), peakValues.size());
}

std::vector<float> peakAudioBuffer(AudioBuffer audioBuffer)
{
	std::vector<float> peakValues;

	switch(audioBuffer.getFormat())
	{
	case AudioFormat::UInt8:
		peakValues=callPeakNormalize<uint8_t>(audioBuffer);
		break;
	case AudioFormat::Int16:
		peakValues=callPeakNormalize<int16_t>(audioBuffer);
		break;
	case AudioFormat::Int32:
		peakValues=callPeakNormalize<int32_t>(audioBuffer);
		break;
	case AudioFormat::Float:
		peakValues=callPeakNormalize<float>(audioBuffer);
		break;
	case AudioFormat::Double:
		peakValues=callPeakNormalize<double>(audioBuffer);
		break;
	case AudioFormat::UInt8P:
		peakValues=callPeakNormalizePlanar<uint8_t>(audioBuffer);
		break;
	case AudioFormat::Int16P:
		peakValues=callPeakNormalizePlanar<int16_t>(audioBuffer);
		break;
	case AudioFormat::Int32P:
		peakValues=callPeakNormalizePlanar<int32_t>(audioBuffer);
		break;
	case AudioFormat::FloatP:
		peakValues=callPeakNormalizePlanar<float>(audioBuffer);
		break;
	case AudioFormat::DoubleP:
		peakValues=callPeakNormalizePlanar<double>(audioBuffer);
		break;
	}

	return peakValues;
}

}//namespace medialib

