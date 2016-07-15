#include "medialib/spectrogram.h"

#include <fftw3.h>

namespace medialib
{

//template<typename _BufferType>
//struct HiddenFftw
//{
//};

template<>
struct HiddenFftw<double>
{
	typedef fftw_complex ComplexType;

	fftw_plan fftwPlan(int n, double *in, ComplexType *out, unsigned flags)
	{
		return fftw_plan_dft_r2c_1d(n, in, out, flags);
	}

	ComplexType *mallocComplex(size_t n)
	{
		return (ComplexType *)fftw_malloc(n);
	}

	size_t sizeofComplex() { return sizeof(ComplexType); }

	void execute(fftw_plan plan, double *in, ComplexType *out)
	{
		fftw_execute_dft_r2c(plan, in, out);
	}

	fftw_plan m_plan;
	ComplexType *m_outBuffer;
};

template<>
struct HiddenFftw<float>
{
	typedef fftwf_complex ComplexType;

	fftwf_plan fftwPlan(int n, float *in, ComplexType *out, unsigned flags)
	{
		return fftwf_plan_dft_r2c_1d(n, in, out, flags);
	}

	ComplexType *mallocComplex(size_t n)
	{
		return (ComplexType *)fftw_malloc(n);
	}

	size_t sizeofComplex() { return sizeof(ComplexType); }

	void execute(fftwf_plan plan, float *in, ComplexType *out)
	{
		fftwf_execute_dft_r2c(plan, in, out);
	}

	fftwf_plan m_plan;
	ComplexType *m_outBuffer;
};

template HiddenFftw<float>;
template HiddenFftw<double>;

template<typename _DataType>
SpectrogramGenerator<_DataType>::SpectrogramGenerator(size_t windowSize, WindowFunctionType windowType, size_t overlap, bool logarithmic):
	m_overlap(overlap),
	m_windowType(windowType),
	m_windowSize(windowSize),
	m_logarithmic(logarithmic)
{
//	m_hiddenFftw.reset(new HiddenFftw<double>());
	init();
}

template<typename _DataType>
void SpectrogramGenerator<_DataType>::init()
{
	//
	m_windowFunction.set(m_windowType, m_windowSize);

	size_t outSamples=(m_windowSize/2)+1;

	m_inBufferSize=m_windowSize*sizeof(_DataType);
	m_inBuffer=(_DataType *)fftwf_malloc(m_inBufferSize);
//	m_outBufferSize=sizeof(fftwf_complex) * outSamples;
//	m_hiddenFftw.m_outBuffer=(fftwf_complex *)fftwf_malloc(m_outBufferSize);
	m_outBufferSize=m_hiddenFftw.sizeofComplex() * outSamples;
	m_hiddenFftw.m_outBuffer=m_hiddenFftw.mallocComplex(m_outBufferSize);

//	m_hiddenFftw->m_plan=fftwf_plan_dft_r2c_1d(m_windowSize, m_inBuffer, m_hiddenFftw->m_outBuffer, FFTW_ESTIMATE);
	m_hiddenFftw.m_plan=m_hiddenFftw.fftwPlan(m_windowSize, m_inBuffer, m_hiddenFftw.m_outBuffer, FFTW_ESTIMATE);
}

template<typename _DataType>
size_t SpectrogramGenerator<_DataType>::calculateBufferSize(unsigned int samples, size_t &width, size_t &height)
{
	size_t step=m_windowSize-m_overlap;
	width=floor((float)(samples-m_windowSize)/step)+1;
	height=(m_windowSize/2)+1;

	return width*height*sizeof(_DataType);
}

template<typename _DataType>
std::vector<_DataType> SpectrogramGenerator<_DataType>::process(const AudioBuffer &audioBuffer, unsigned int channel, size_t &width, size_t &height)
{
	size_t outputSize=calculateBufferSize(audioBuffer.getSamples(), width, height);
	std::vector<_DataType> output(width*height);

	process(audioBuffer, channel, output.data(), outputSize, width, height);
	return output;
}

template<typename _DataType>
bool SpectrogramGenerator<_DataType>::process(const AudioBuffer &audioBuffer, unsigned int channel, _DataType *output, size_t outputSize, size_t &width, size_t &height)
{
	switch(audioBuffer.getFormat())
	{
	case AudioFormat::UInt8:
	case AudioFormat::UInt8P:
		return process(audioBuffer.getBuffer(channel), audioBuffer.getSamples(), audioBuffer.getFormat(), output, outputSize, width, height);
		break;
	case AudioFormat::Int16:
	case AudioFormat::Int16P:
		return process((int16_t *)audioBuffer.getBuffer(channel), audioBuffer.getSamples(), audioBuffer.getFormat(), output, outputSize, width, height);
		break;
	case AudioFormat::Int32:
	case AudioFormat::Int32P:
		return process((int32_t *)audioBuffer.getBuffer(channel), audioBuffer.getSamples(), audioBuffer.getFormat(), output, outputSize, width, height);
		break;
	case AudioFormat::Float:
	case AudioFormat::FloatP:
		return process((float *)audioBuffer.getBuffer(channel), audioBuffer.getSamples(), audioBuffer.getFormat(), output, outputSize, width, height);
		break;
	case AudioFormat::Double:
	case AudioFormat::DoubleP:
		return process((double *)audioBuffer.getBuffer(channel), audioBuffer.getSamples(), audioBuffer.getFormat(), output, outputSize, width, height);
		break;
	}
	return false;
}

template<typename _DataType>
template<typename _BufferType>
bool SpectrogramGenerator<_DataType>::process(_BufferType *audioBuffer, size_t audioSamples, AudioFormat format, _DataType *output, size_t outputSize, size_t &width, size_t &height)
{
	width=0;
	height=0;

	if(audioSamples<m_windowSize)
		return false;

	size_t step=m_windowSize-m_overlap;
	size_t sampleBins=floor((float)(audioSamples-m_windowSize)/step);// +1;
	size_t frequencies=(m_windowSize/2)+1;
	size_t requiredOutputSize=sampleBins*frequencies*sizeof(_DataType);

	if(outputSize<requiredOutputSize)
		return false;

	_BufferType *data=audioBuffer;
	HiddenFftw<_DataType>::ComplexType *complexOut=m_hiddenFftw.m_outBuffer;
//	size_t stride=sampleBins*sizeof(_BufferType);

	if(m_logarithmic)
	{
		for(size_t i=0; i<sampleBins; ++i)
		{
			convert(data, m_windowSize, m_inBuffer, m_inBufferSize);

//			fftwf_execute_dft_r2c(m_hiddenFftw.m_plan, m_inBuffer, m_hiddenFftw.m_outBuffer);
			m_hiddenFftw.execute(m_hiddenFftw.m_plan, m_inBuffer, m_hiddenFftw.m_outBuffer);

			for(size_t j=0; j<frequencies; ++j)
			{
				//normaly db is 20*log10(sample), but since log(X^2) == 1/2*log(X), we skip the sqrt and just divide the 20 by 2
				output[j*sampleBins+i]=10*std::log10(m_hiddenFftw.m_outBuffer[j][0]*m_hiddenFftw.m_outBuffer[j][0]+m_hiddenFftw.m_outBuffer[j][1]*m_hiddenFftw.m_outBuffer[j][1]);
			}
			data+=sampleBins;
		}
	}
	else
	{
		for(size_t i=0; i<sampleBins; ++i)
		{
			convert(data, m_windowSize, m_inBuffer, m_inBufferSize);

//			fftwf_execute_dft_r2c(m_hiddenFftw.m_plan, m_inBuffer, m_hiddenFftw.m_outBuffer);
			m_hiddenFftw.execute(m_hiddenFftw.m_plan, m_inBuffer, m_hiddenFftw.m_outBuffer);

			for(size_t j=0; j<frequencies; ++j)
			{
				output[j*sampleBins+i]=sqrt(m_hiddenFftw.m_outBuffer[j][0]*m_hiddenFftw.m_outBuffer[j][0]+m_hiddenFftw.m_outBuffer[j][1]*m_hiddenFftw.m_outBuffer[j][1]);
			}
			data+=sampleBins;
		}
	}

	width=sampleBins;
	height=frequencies;

	return true;
}

//template SpectrogramGenerator<uint8_t>;
//template SpectrogramGenerator<int16_t>;
//template SpectrogramGenerator<int32_t>;
template SpectrogramGenerator<float>;
template SpectrogramGenerator<double>;



}//namespace medialib