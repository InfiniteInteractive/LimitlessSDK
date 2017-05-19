#ifndef _GpuBufferSample_h_
#define _GpuBufferSample_h_

#include "Media/media_define.h"
#include "Media/MediaSample.h"
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

class MEDIA_EXPORT GpuBufferSample:public AutoRegisterMediaSample<GpuBufferSample, MediaSample>
{
public:
	GpuBufferSample();
	GpuBufferSample(size_t size);
	GpuBufferSample(unsigned char *buffer, size_t size);
	virtual ~GpuBufferSample();

	void resize(size_t size);
	bool write(unsigned char *buffer, size_t size, cl::Event &event);
	bool read(unsigned char *buffer, size_t size, cl::Event &event);

	GLuint texture(){return m_texture;}
	GLuint pbo(){return m_pbo;}

    cl::BufferGL glBuffer() { return m_buffer; }

	//MediaSample
	virtual unsigned char *buffer();
	virtual size_t size() const{return m_size;}
	virtual size_t actualSize() const{return m_actualSize;}

private:
	cl::Context m_openCLContext;
	cl::CommandQueue m_openCLComandQueue;
	cl::BufferGL m_buffer;
	GLuint m_texture;
	GLuint m_pbo;
	cl_mem_flags m_flags;

	std::vector<unsigned char> m_hostBuffer;

	size_t m_size;
	size_t m_actualSize;
};

typedef boost::shared_ptr<GpuBufferSample> SharedGpuBufferSample;

}//namespace Limitless

#endif //_GpuBufferSample_h_