#ifndef _spectrogram_h_
#define _spectrogram_h_

#include "medialib/medialibDefine.h"
#include "audioBuffer.h"

#include "audioConversion.h"

#include <unordered_map>
#include <cassert>
#include <memory.h>

#define _USE_MATH_DEFINES
#include <cmath>

#ifndef M_PI
#define M_PI       3.14159265358979323846
#endif //M_PI

namespace medialib
{

enum class medialib_EXPORT WindowFunctionType
{
    Rectangular,
    Hann,
    Hamming
};

template<typename _DataType>
class WindowFunction
{
public:
    WindowFunction(WindowFunctionType type=WindowFunctionType::Hamming, size_t size=10):type(type) { set(type, size); }
    
    void set(WindowFunctionType type=WindowFunctionType::Hamming, size_t size=10);

    WindowFunctionType type;
    std::vector<_DataType> data;
};

template<typename _DataType>
void WindowFunction<_DataType>::set(WindowFunctionType type, size_t size)
{
    data.resize(size);

    switch(type)
    {
    case WindowFunctionType::Rectangular:
        for(size_t i=0; i<size; i++)
            data[i]=1.0;
        break;
    case WindowFunctionType::Hann:
        for(size_t i=0; i<size; i++)
            data[i]=0.5 * (1-std::cos((2.0*M_PI*static_cast<_DataType>(i))/static_cast<_DataType>(size-1)));
        break;
    case WindowFunctionType::Hamming:
        for(size_t i=0; i <size; i++)
            data[i]=0.54-0.46 * std::cos((2.0*M_PI*static_cast<_DataType>(i))/static_cast<_DataType>(size-1));
        break;
    }
}

template<typename _DataType>
struct HiddenFftw
{};

template<typename _DataType>
class SpectrogramGenerator
{
public:
    SpectrogramGenerator(size_t windowSize=4096, WindowFunctionType windowType=WindowFunctionType::Hamming, size_t overlap=0, bool logarithmic=true);

    std::vector<_DataType> process(const AudioBuffer &audioBuffers, unsigned int channel, size_t &width, size_t &height);
	bool process(const AudioBuffer &audioBuffer, unsigned int channel, _DataType *output, size_t outputSize, size_t &width, size_t &height);

	template<typename _BufferType>
	bool process(_BufferType *audioBuffer, size_t audioSamples, AudioFormat format, _DataType *output, size_t outputSize, size_t &width, size_t &height);

	size_t calculateBufferSize(unsigned int samples) { size_t width, height;  return calculateBufferSize(samples, width, height); }
	size_t calculateBufferSize(unsigned int samples, size_t &width, size_t &height);

private:
    void init();

    size_t m_overlap;
    WindowFunctionType m_windowType;
    size_t m_windowSize;
    WindowFunction<_DataType> m_windowFunction;
    bool m_logarithmic;
   
//    fftwf_plan m_plan;
    _DataType *m_inBuffer;
    size_t m_inBufferSize;
//    fftwf_complex *m_outBuffer;
    size_t m_outBufferSize;

//	std::unique_ptr<HiddenFftw> m_hiddenFftw;
	HiddenFftw<_DataType> m_hiddenFftw;
};

/// Builds spectrogram for audio sample
/// \param audioBuffer AudioBuffer to build spectrogram from
/// \param windowSize size of window in secs to calculate the frequency
/// \param windowType size of window in secs to calculate the frequency

//std::vector<float> spectrogram(const AudioBuffer &audioBuffers, size_t windowSize, SpectroGramWindow windowType=SpectroGramWindow::Hamming, bool logarithmic=true);

}
#endif //_synchronize_h_
