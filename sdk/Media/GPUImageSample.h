#ifndef _GpuImageSample_h_
#define _GpuImageSample_h_

#include "Media/media_define.h"
#include "Media/IImageSample.h"
#include "Media/MediaSampleFactory.h"

#ifdef Media_EXPORTS
#include <GL/glew.h>
#else
#ifdef _WINDOWS
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#endif //_WINDOWS
#include <gl/gl.h>
#endif
#include "CL/cl.hpp"

namespace Limitless
{

class MEDIA_EXPORT GpuImageSample:public AutoRegisterMediaSample<GpuImageSample, IImageSample>
{
public:
	GpuImageSample();
	GpuImageSample(unsigned int width, unsigned int height, unsigned int channles=4, unsigned int channelBits=8);
	GpuImageSample(unsigned char *buffer, unsigned int width, unsigned int height, unsigned int channles=4, unsigned int channelBits=8);
	virtual ~GpuImageSample();

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

	virtual bool save(std::string fileName);

	bool write(IImageSample *imageSample, cl::Event &event, std::vector<cl::Event> *events=nullptr);
	bool write(unsigned char *buffer, unsigned int width, unsigned int height, unsigned int channels, cl::Event &event, std::vector<cl::Event> *events=nullptr);
	bool read(IImageSample *imageSample, cl::Event &event, std::vector<cl::Event> *events=nullptr);
	bool read(unsigned char *buffer, unsigned int width, unsigned int height, cl::Event &event, std::vector<cl::Event> *events=nullptr);
	bool copy(GpuImageSample *sample, cl::Event &event, std::vector<cl::Event> *waitEvents=nullptr);

	bool resize(unsigned int width, unsigned int height, unsigned int channles=3, unsigned int channelBits=8);

	GLuint texture() { return m_texture; }
	cl::ImageGL glImage(){return m_image;}

	static bool acquireMultipleOpenCl(std::vector<GpuImageSample *> samples, cl::Event &event, std::vector<cl::Event> *waitEvents=nullptr);
	static bool releaseMultipleOpenCl(std::vector<GpuImageSample *> samples, cl::Event &event, std::vector<cl::Event> *waitEvents=nullptr);

	bool acquireOpenCl(cl::Event &event, std::vector<cl::Event> *waitEvents=nullptr);
	bool releaseOpenCl(cl::Event &event, std::vector<cl::Event> *waitEvents=nullptr);
	bool acquireOpenGl();
	bool releaseOpenGl();

private:
	cl::ImageGL m_image;
	GLuint m_texture;
	cl_mem_flags m_flags;

	enum APIOwned
	{
		None,
		OpenGl,
		OpenCl
	};

	APIOwned m_owned;
		
	std::vector<unsigned char> m_hostBuffer;

	unsigned int m_width;
	unsigned int m_height;
	size_t m_size;

	int m_channels;
	int m_channelBits;
};
typedef boost::shared_ptr<GpuImageSample> SharedGpuImageSample;
typedef std::vector<SharedGpuImageSample> SharedGpuImageSamples;

}//namespace Limitless

#endif //_GpuImageSample_h_