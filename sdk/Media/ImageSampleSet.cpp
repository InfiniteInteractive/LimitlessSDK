#include "ImageSampleSet.h"

using namespace Limitless;

ImageSampleSet::ImageSampleSet()//:
//m_buffer(NULL),
//m_size(0)
{
}

ImageSampleSet::~ImageSampleSet()
{
//	freeBuffer();
}

void ImageSampleSet::addSample(SharedIImageSample sample)
{
	m_imageSamples.push_back(sample);
}

SharedIImageSample &ImageSampleSet::sample(size_t index)
{
	return m_imageSamples[index];
}

size_t ImageSampleSet::sampleSetSize()
{
	return m_imageSamples.size();
}

bool ImageSampleSet::sampleSetEmpty()
{
	return m_imageSamples.empty();
}

//void ImageSampleSet::resize(unsigned int count, int width, int height, int channels, int channelBits)
//{
//	resize(count, width, width, height, channels, channelBits);
//}
//
//void ImageSampleSet::resize(unsigned int count, int width, int pitch, int height, int channels, int channelBits)
//{
//	//only resize if needed
//	if((m_imageSamples.size() != count) || (m_pitch != pitch) || (m_height != height) || (m_channels != channels) || (m_channelBits != channelBits))
//	{
//		clear();
//
//		int bytes=(int)ceil((float)(channels*channelBits)/8);
//		unsigned char *buffer=(unsigned char *)malloc(count*(pitch*bytes)*height);
//		unsigned char *pos=buffer;
//
//		for(unsigned int i=0; i<count; ++i)
//		{
//			SharedImageSample imageSample(new ImageSample(pos, width, pitch, height, channels, channelBits));
//			
//			m_imageSamples.push_back(imageSample);
//			pos+=(pitch*bytes)*height;
//		}
//
//		m_buffer=buffer;
//		m_size=count*(pitch*bytes)*height;
//		m_pitch=pitch;
//		m_width=width;
//		m_height=height;
//		m_channels=channels;
//		m_channelBits=channelBits;
//	}
//}

void ImageSampleSet::setSampleSetSize(size_t size)
{
	m_imageSamples.resize(size);
}

void ImageSampleSet::clearSampleSet()
{
	m_imageSamples.clear();
}

//void ImageSampleSet::clear()
//{
//	m_imageSamples.clear();
////	freeBuffer();
//}

//void ImageSampleSet::freeBuffer()
//{
//	if(m_buffer != NULL)
//		free(m_buffer);
//	m_buffer=NULL;
//}