#ifndef _GPUImageGLSample_h_
#define _GPUImageGLSample_h_

#include "Media/media_define.h"
#include "Media/GpuImageSample.h"
#include "Media/MediaSampleFactory.h"

#include <GL/glew.h>
#include "CL/cl.hpp"

namespace Limitless
{

class MEDIA_EXPORT GPUImageGLSample:public AutoRegisterMediaSample<GPUImageGLSample, GPUImageSample>
{
public:
	GPUImageGLSample();
	GPUImageGLSample(unsigned int width, unsigned int height, unsigned int channles=4, unsigned int channelBits=8);
	GPUImageGLSample(unsigned char *buffer, unsigned int width, unsigned int height, unsigned int channles=4, unsigned int channelBits=8);
	virtual ~GPUImageGLSample();

//MediaSample interface
	virtual unsigned char *buffer();
	virtual size_t size() const{return m_size;}

//IImageSample interface
	virtual std::string imageFormat() const{return "RGBA";}
	virtual unsigned int width() const{return m_width;}
	virtual unsigned int pitch() const{return m_width;}
	virtual unsigned int height() const{return m_height;}
	virtual unsigned int channels() const{return m_channels;}
	virtual unsigned int channelBits() const{return m_channelBits;}

	bool write(unsigned char *buffer, unsigned int width, unsigned int height, cl::Event &event);
	bool read(unsigned char *buffer, unsigned int width, unsigned int height, cl::Event &event);

	bool resize(unsigned int width, unsigned int height, unsigned int channles=3, unsigned int channelBits=8);

private:
	cl::ImageGL m_image;
	GLuint m_texture;
	cl_mem_flags m_flags;
		
	std::vector<unsigned char> m_hostBuffer;

	unsigned int m_width;
	unsigned int m_height;
	size_t m_size;

	int m_channels;
	int m_channelBits;
};
typedef boost::shared_ptr<GPUImageGLSample> SharedGPUImageGLSample;

}//namespace Limitless

#endif //_GPUImageGLSample_h_