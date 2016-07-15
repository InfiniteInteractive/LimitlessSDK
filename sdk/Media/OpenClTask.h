#ifndef _Limitless_OpenClTask_h_
#define _Limitless_OpenClTask_h_

#include "Media/media_define.h"
#include "Utilities/taskThread.h"

#include "CL/cl.hpp"

namespace Limitless
{

struct WriteImageTask:WaitTask<WriteImageTask>
{
	WriteImageTask(){};

	cl::CommandQueue commandQueue;
	const cl::Image *image;
    cl_bool blocking;
    cl::size_t<3> origin;
    cl::size_t<3> region;
    size_t row_pitch;
    size_t slice_pitch;
    void *ptr;
    const std::vector<cl::Event> *events;
    cl::Event *event;

	cl_int result;

	virtual void process();
};


}//namespace Limitless

#endif //_Limitless_OpenClTask_h_