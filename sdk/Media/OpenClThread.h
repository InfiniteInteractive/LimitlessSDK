#ifndef _Limitless_OpenClThread_h_
#define _Limitless_OpenClThread_h_

#include "Media/media_define.h"
#include "Utilities/taskThread.h"

#include "CL/cl.hpp"

namespace Limitless
{

class MEDIA_EXPORT OpenClThread:public TaskThread
{
public:
	OpenClThread();

	void start(cl::Context openCLContext, cl::CommandQueue openCLComandQueue);
	void stop();

	cl_int OpenClThread::enqueueWriteImage(cl::Image &image, const cl::size_t<3> &origin, const cl::size_t<3> &region, size_t rowPitch, size_t slicePitch, void *ptr,
        const std::vector<cl::Event> *events, cl::Event *event);

private:
	cl::Context m_openCLContext;
	cl::CommandQueue m_openCLComandQueue;
};

}//namespace Limitless

#endif //_Limitless_OpenClThread_h_