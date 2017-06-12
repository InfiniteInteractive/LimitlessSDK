#include "medialib/synchronize.h"

#include "medialib/audioConversion.h"

#include <fftw3.h>
#include <unordered_map>
#include <cassert>

#include "cvlib/imageDLib.h"

#include "cvlib/slidingWindow.h"
#include "cvlib/match.h"
#include "cvlib/ppm.h"
#include "medialib/spectrogram.h"

#include "dlib/image_transforms/spatial_filtering_abstract.h"
#include <dlib/image_processing.h>

namespace medialib
{

std::vector<float> synchronizeAudio(AudioBuffers &audioBuffers, float searchRange)
{
//	AudioBuffers audioBuffers;

//	for(size_t i=0; i<buffers.size(); ++i)
//		audioBuffers.push_back(AudioBuffer(buffers[i]));

	std::vector<float> timeDiff;

	if(audioBuffers.size()<2)
		return timeDiff;

	int maxSamples=std::ceil((float)audioBuffers[0].getSampleRate()*searchRange);
	//   size_t maxChannels=audioBuffers[0].getChannels();

	//get maximum segment size and maxium channels of all the audio buffers
	for(size_t i=1; i<audioBuffers.size(); ++i)
	{
		const AudioBuffer &audioBuffer=audioBuffers[i];

		//       int localSegementSize=std::ceil((float)audioBuffer.getSampleRate()*searchRange);

		maxSamples=std::max(maxSamples, (int)std::ceil((float)audioBuffer.getSampleRate()*searchRange));
		//       if(localSegementSize!=maxSegmentSize)
		//       {
		//           if(localSegementSize>maxSamples)
		//               maxSamples=localSegementSize;
		//       }

		//        if(audioBuffer.getChannels() > maxChannels)
		//            maxChannels=audioBuffer.getChannels();
	}

	size_t windowSize=1024;
	SpectrogramGenerator<float> spectrogramGenerator(windowSize, WindowFunctionType::Hamming, 0, true);
	size_t outputSize=spectrogramGenerator.calculateBufferSize(maxSamples);
	std::vector<float> spectrogram(outputSize);
	std::vector<float> maximum(outputSize);
	std::vector<uint8_t> binaryImage(outputSize);
	size_t width, height;

	for(size_t i=0; i<audioBuffers.size(); ++i)
	{
		const AudioBuffer &audioBuffer=audioBuffers[i];

		if(spectrogramGenerator.process(audioBuffer, 0, spectrogram.data(), outputSize, width, height))
//		if(spectrogramGenerator.process(audioBuffer.getBuffer(), audioBuffer.getSamples(), audioBuffer.getFormat(), spectrogram.data(), outputSize, width, height))
		{
			cvlib::SimpleImage imageSpectrogram(cvlib::ImageFormat::GreyScale32f, width, height, (uint8_t *)spectrogram.data(), outputSize*sizeof(float));

			std::string fileName="spectrogram"+std::to_string(i)+".ppm";
			cvlib::savePpm(imageSpectrogram, fileName);

//			cvlib::maximumSlidingWindow(spectrogram.data(), maximum.data(), width, height, 20);
//
//			cvlib::ImageStruct imageMaximum(width, height, cvlib::ImageFormat::GreyScale32f, (uint8_t *)maximum.data(), outputSize*sizeof(float));
//			cvlib::ImageStruct imageBinary(width, height, cvlib::ImageFormat::Binary, binaryImage.data(), outputSize*sizeof(uint8_t));
//
//			cvlib::match(imageSpectrogram, imageMaximum, imageBinary);
//
//			std::vector<std::pair<double, dlib::point>> maxPoints;
//			dlib::find_points_above_thresh(maxPoints, imageBinary, 0, std::numeric_limits<unsigned long>::max());
		}
	}

	return timeDiff;
}

//struct SampleMagnitude
//{
//	float magnitude;
//	size_t sampleIndex;
//};
//
//typedef std::unordered_map<size_t, SampleMagnitude> FrequencyBinMap;
//
//std::vector<float> synchronizeAudio(AudioBuffers audioBuffers, float searchRange, float segmentSeconds)
//{
//    std::vector<float> timeDiff;
//
//    if(audioBuffers.size()<2)
//        return timeDiff;
//
//    std::vector<int> segmentSizes;
//    typedef std::unordered_map<int, fftwf_plan> PlanMap;
//    PlanMap segmentPlans;
//    int maxSegmentSize=std::ceil((float)audioBuffers[0].sampleRate*segmentSeconds);
//	size_t maxChannels=audioBuffers[0].channels;
//
//	typedef std::unordered_map<int, > PlanMap;
//
//    segmentSizes.push_back(maxSegmentSize);
//
//    //get maximum segment size and maxium channels of all the audio buffers
//    for(size_t i=1; i<audioBuffers.size(); ++i)
//    {
//        AudioBuffer &audioBuffer=audioBuffers[i];
//
//        int localSegementSize=std::ceil((float)audioBuffer.sampleRate*segmentSeconds);
//
//        if(localSegementSize!=maxSegmentSize)
//        {
//            if(localSegementSize>maxSegmentSize)
//                maxSegmentSize=localSegementSize;
//
//            if(std::find(segmentSizes.begin(), segmentSizes.end(), localSegementSize) == segmentSizes.end())
//                segmentSizes.push_back(localSegementSize);
//        }
//
//		if(audioBuffer.channels > maxChannels)
//			maxChannels=audioBuffer.channels;
//    }
//
//	if(maxChannels <= 0)
//		return timeDiff;
//
//    int outSamples=(maxSegmentSize/2)+1;
//    size_t inBufferSize=sizeof(float) * maxSegmentSize;
//	std::vector<float *> inBuffers(maxChannels);
//
//	for(size_t i=0; i<maxChannels; ++i)
//		inBuffers[i]=(float *)fftwf_malloc(inBufferSize);
//	fftwf_complex *outBuffer=(fftwf_complex *)fftwf_malloc(sizeof(fftwf_complex) * outSamples);
//
//    for(int &segmentSize:segmentSizes)
//    {
//        segmentPlans.insert(PlanMap::value_type(segmentSize, fftwf_plan_dft_r2c_1d(segmentSize, inBuffers[0], outBuffer, FFTW_ESTIMATE)));
//    }
//
//    for(size_t i=0; i<audioBuffers.size(); ++i)
//    {
//        AudioBuffer &audioBuffer=audioBuffers[i];
//
//        int localSegementSize=std::ceil((float)audioBuffer.sampleRate*segmentSeconds);
//        fftwf_plan &plan=segmentPlans[localSegementSize];
//        int segments=((float)audioBuffer.sampleRate*searchRange)/localSegementSize;
//
//		size_t offset=0;
//
//		for(size_t bufferIndex=0; bufferIndex<segments; ++bufferIndex)
//		{
//			if(audioBuffer.format == AudioFormat::Float) //in native float format no need to copy
//			{
//				assert(false); //need to implement this
//
////				fftwf_execute_dft_r2c(plan, in, out);
//			}
//			else
//			{
//				//move to in buffer for fft
//				convertToFloat(audioBuffer, offset, localSegementSize, inBuffers, inBufferSize);
//
//				for(size_t channel=0; channel<audioBuffer.channels; ++channel)
//				{
//					fftwf_execute_dft_r2c(plan, inBuffers[channel], outBuffer, localSegementSize);
//
//					pushFrequencies(frequencyMap, outBuffer);
//				}
//			}
//			offset+=localSegementSize;
//		}
//    }
//}
//
//void pushFrequencies(FrequencyBinMap &frequencyBinMap, fftwf_complex *buffer, size_t size)
//{
//	for(size_t i=0; i<size; ++i)
//	{
//		fftwf_complex &complex=buffer[i];
//		float sqrMagnitude=complex[0]*complex[0]+complex[1]*complex[1]; //using relative difference so sqrt not needed for actual magnitude
//
//
//		frequencyBinMap.insert(FrequencyBinMap)
//
//	}
//}

}//namespace medialib