#ifndef _synchronize_h_
#define _synchronize_h_

#include "medialib/medialibDefine.h"
#include "medialib/audioBuffer.h"
#include "medialib/spectrogram.h"

//#include "cvlib/slidingWindow.h"
//#include "cvlib/match.h"

namespace medialib
{

/// \brief searches audio buffers for matching events and outputs time defferences
/// Searches audio buffers for matching events, output timestamp that can be used to synchronize the audio
/// \param audioBuffers vector of AusioBuffers
/// \param searchRange float value representsing seconds to search
template<typename _AudioBufferTpe>
std::vector<float> synchronizeAudio(std::vector<_AudioBufferTpe> buffers, float searchRange)
{
	AudioBuffers audioBuffers;

    for(size_t i=0; i<buffers.size(); ++i)
        audioBuffers.push_back(AudioBuffer(buffers[i]));

	return synchronizeAudio(audioBuffers, searchRange);
}

medialib_EXPORT std::vector<float> synchronizeAudio(AudioBuffers &audioBuffers, float searchRange);

//template<typename _AudioBufferType>
//std::vector<float> synchronizeAudio(std::vector<_AudioBufferType> &buffers, float searchRange)
//{
//    AudioBuffers audioBuffers;
//
//    for(size_t i=0; i<buffers.size(); ++i)
//        audioBuffers.push_back(AudioBuffer(buffers[i]));
//
//    std::vector<float> timeDiff;
//
//    if(audioBuffers.size()<2)
//        return timeDiff;
//
//    int maxSamples=std::ceil((float)audioBuffers[0].getSampleRate()*searchRange);
//    //   size_t maxChannels=audioBuffers[0].getChannels();
//
//    //get maximum segment size and maxium channels of all the audio buffers
//    for(size_t i=1; i<audioBuffers.size(); ++i)
//    {
//        const AudioBuffer &audioBuffer=audioBuffers[i];
//
//        //       int localSegementSize=std::ceil((float)audioBuffer.getSampleRate()*searchRange);
//
//        maxSamples=std::max(maxSamples, (int)std::ceil((float)audioBuffer.getSampleRate()*searchRange));
//        //       if(localSegementSize!=maxSegmentSize)
//        //       {
//        //           if(localSegementSize>maxSamples)
//        //               maxSamples=localSegementSize;
//        //       }
//
//        //        if(audioBuffer.getChannels() > maxChannels)
//        //            maxChannels=audioBuffer.getChannels();
//    }
//
//    SpectrogramGenerator<float> spectrogramGenerator;
//    //   size_t outputSize=audioBuffer.getSamples()*((audioBuffer.getSamples()/2)+1);
//    size_t outputSize=maxSamples*((maxSamples/2)+1);
//    std::vector<float> spectrogram(outputSize);
//    std::vector<float> maximum(outputSize);
//    std::vector<uint8_t> binaryImage(outputSize);
//    size_t width, height;
//
//    for(size_t i=0; i<audioBuffers.size(); ++i)
//    {
//        const AudioBuffer &audioBuffer=audioBuffers[i];
//
//        if(spectrogramGenerator.process(audioBuffer.getBuffer(), audioBuffer.getSamples(), audioBuffer.getFormat(), spectrogram.data(), outputSize, width, height))
//        {
//            cvlib::maximumSlidingWindow(spectrogram.data(), maximum.data(), width, height, 20);
//
//            cvlib::ImageStruct imageSpectrogram(width, height, cvlib::ImageFormat::GreyScale32f, (uint8_t *)spectrogram.data(), outputSize*sizeof(float));
//            cvlib::ImageStruct imageMaximum(width, height, cvlib::ImageFormat::GreyScale32f, (uint8_t *)maximum.data(), outputSize*sizeof(float));
//            cvlib::ImageStruct imageBinary(width, height, cvlib::ImageFormat::Binary, binaryImage.data(), outputSize*sizeof(uint8_t));
//
//            cvlib::match(imageSpectrogram, imageMaximum, imageBinary);
//
//            std::vector<std::pair<double, dlib::point>> maxPoints;
//            dlib::find_points_above_thresh(maxPoints, imageBinary, 0, std::numeric_limits<unsigned long>::max());
//        }
//    }
//
//    return timeDiff;
//}
}
#endif //_synchronize_h_
