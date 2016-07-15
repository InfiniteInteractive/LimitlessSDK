#include "Media/ImageSample.h"
#include "Utilities/utilitiesImage.h"

using namespace Limitless;

ImageSample::ImageSample():
m_buffer(NULL),
m_externalBuffer(false),
m_size(0),
m_width(0),
m_pitch(0),
m_height(0), 
m_channels(3),
m_channelBits(8)
{
}

ImageSample::ImageSample(unsigned int width, unsigned int height, unsigned int channels, unsigned int channelBits):
m_size(0)
{
	resize(width, height, channels, channelBits);
}

ImageSample::ImageSample(unsigned int width, unsigned int pitch, unsigned int height, unsigned int channels, unsigned int channelBits):
m_size(0)
{
	resize(width, pitch, height, channels, channelBits);
}

ImageSample::ImageSample(unsigned char *buffer, unsigned int width, unsigned int height, unsigned int channels, unsigned int channelBits):
m_size(0)
{
	setImage(buffer, width, height, channels, channelBits);
}

ImageSample::ImageSample(unsigned char *buffer, unsigned int width, unsigned int pitch, unsigned int height, unsigned int channels, unsigned int channelBits):
m_size(0)
{
	setImage(buffer, width, pitch, height, channels, channelBits);
}

ImageSample::~ImageSample()
{
	freeBuffer();
}

void ImageSample::setImage(unsigned char *buffer, unsigned int width, unsigned int height, unsigned int channels, unsigned int channelBits)
{
	setImage(buffer, width, width, height, channels, channelBits);
}

void ImageSample::setImage(unsigned char *buffer, unsigned int width, unsigned int pitch, unsigned int height, unsigned int channels, unsigned int channelBits)
{
	freeBuffer();
	
	int bytes=(int)ceil((float)(channels*channelBits)/8);

	m_buffer=buffer;
	m_externalBuffer=true;
	m_size=(pitch*bytes)*height;
	m_width=width;
	m_pitch=pitch;
	m_height=height;
	m_channels=channels;
	m_channelBits=channelBits;
}

void ImageSample::resize(unsigned int width, unsigned int height, unsigned int channels, unsigned int channelBits)
{
	resize(width, width, height, channels, channelBits);
}

void ImageSample::resize(unsigned int width, unsigned int pitch, unsigned int height, unsigned int channels, unsigned int channelBits)
{
//	freeBuffer();
	int bytes=(int)ceil((float)(channels*channelBits)/8);
	int size=(pitch*bytes)*height;

	if(size > m_size)
	{
		freeBuffer();
		m_buffer=(unsigned char *)malloc(size);
		m_externalBuffer=false;
		m_size=size;
	}

	m_width=width;
	m_pitch=pitch;
	m_height=height;
	m_channels=channels;
	m_channelBits=channelBits;
}

bool ImageSample::save(std::string fileName)
{
	if(m_channels==4)
		savePPM(fileName.c_str(), RGBA, m_buffer, m_pitch, m_height);
	else
		savePPM(fileName.c_str(), RGB, m_buffer, m_pitch, m_height);

	return true;
}

void ImageSample::freeBuffer()
{
	if((m_buffer != NULL) && (!m_externalBuffer))
		free(m_buffer);
	m_buffer=NULL;
}