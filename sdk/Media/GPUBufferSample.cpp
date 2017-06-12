#include "Media/GpuBufferSample.h"
#include "GPUContext.h"

namespace Limitless
{

GpuBufferSample::GpuBufferSample():
    m_flags(CL_MEM_READ_WRITE),
    m_size(0),
    m_actualSize(0),
    m_pbo(0)
{}

GpuBufferSample::GpuBufferSample(size_t size):
    m_flags(CL_MEM_READ_WRITE),
    m_pbo(0)
{
    resize(size);
}

GpuBufferSample::GpuBufferSample(unsigned char *buffer, size_t size):
    m_flags(CL_MEM_READ_WRITE),
    m_pbo(0)
{
    resize(size);

    cl::Event event;

    if(write(buffer, size, event))
        event.wait();
}

GpuBufferSample::~GpuBufferSample()
{}

void GpuBufferSample::resize(size_t bufferSize)
{
    if(size()!=bufferSize)
    {
        cl_int error=CL_SUCCESS;

//        GPUContext::makeOpenGLCurrent();
//
//        glGenBuffers(1, &m_pbo);
//        glBindBuffer(GL_ARRAY_BUFFER, m_pbo);
//        glBufferData(GL_ARRAY_BUFFER, bufferSize, NULL, GL_DYNAMIC_DRAW);
//        glBindBuffer(GL_PIXEL_UNPACK_BUFFER_ARB, m_texture);
//        glFinish();
        std::pair<GLuint, GLuint> buffers=GPUContext::createBuffer(bufferSize);

        m_pbo=buffers.first;
//        m_texture=buffers.second;

        m_buffer=cl::BufferGL(GPUContext::openCLContext(), m_flags, m_pbo, &error);
        m_size=bufferSize;
        m_actualSize=bufferSize;
        m_hostBuffer.clear();

        if(error!=CL_SUCCESS)
        {
            assert(false);
        }
    }
}

bool GpuBufferSample::write(unsigned char *buffer, size_t size, cl::Event &event)
{
    if(size==0)
        return false;

    m_hostBuffer.clear();
    if(m_size!=size)
    {
        cl_int error=CL_SUCCESS;

        m_buffer=cl::BufferGL(GPUContext::openCLContext(), m_flags, m_texture, &error);
        m_size=size;
    }

    cl::Event acquireEvent;

    if(acquireOpenCl(acquireEvent))
        acquireEvent.wait();

    cl_int status;

    status=GPUContext::openCLCommandQueue().enqueueWriteBuffer(m_buffer, CL_FALSE, 0, size, buffer, NULL, &event);

    if(status!=CL_SUCCESS)
    {
        return false;
        assert(false);
    }
    return true;
}

bool GpuBufferSample::read(unsigned char *buffer, size_t size, cl::Event &event)
{
    if(size==0)
        return false;

    cl::Event acquireEvent;

    if(acquireOpenCl(acquireEvent))
        acquireEvent.wait();

    cl_int status;

    status=GPUContext::openCLCommandQueue().enqueueReadBuffer(m_buffer, CL_FALSE, 0, size, buffer, NULL, &event);

    if(status!=CL_SUCCESS)
    {
        return false;
        assert(false);
    }
    return true;
}

unsigned char *GpuBufferSample::buffer()
{
    if(m_hostBuffer.size()==m_size)
        return m_hostBuffer.data();

    cl::Event event;

    m_hostBuffer.resize(m_size);

    if(!read(m_hostBuffer.data(), m_size, event))
        return NULL;

    event.wait();
    return m_hostBuffer.data();
}

bool GpuBufferSample::acquireOpenCl(cl::Event &event, std::vector<cl::Event> *waitEvents)
{
    if(m_owned==OpenCl)
        return false;

    std::vector<cl::Memory> glObjects;

    glObjects.push_back(m_buffer);
    cl_int error=GPUContext::openCLCommandQueue().enqueueAcquireGLObjects(&glObjects, waitEvents, &event);
    m_owned=OpenCl;
#ifdef DEBUG_OWNER
    OutputDebugStringA((boost::format("GpuImageSample acquireOpenCl (%08x, %08x) OpenCl\n")%m_texture%m_image()).str().c_str());
#endif //DEBUG_OWNER
    assert(error==CL_SUCCESS);
    return true;
}

bool GpuBufferSample::releaseOpenCl(cl::Event &event, std::vector<cl::Event> *waitEvents)
{
    if(m_owned!=OpenCl)
        return false;

    std::vector<cl::Memory> glObjects;

    glObjects.push_back(m_buffer);
    cl_int error=GPUContext::openCLCommandQueue().enqueueReleaseGLObjects(&glObjects, waitEvents, &event);
    m_owned=OpenGl;
#ifdef DEBUG_OWNER
    OutputDebugStringA((boost::format("GpuImageSample releaseOpenCl (%08x, %08x) OpenGl\n")%m_texture%m_image()).str().c_str());
#endif //DEBUG_OWNER
    assert(error==CL_SUCCESS);
    return true;
}

}//namespace Limitless