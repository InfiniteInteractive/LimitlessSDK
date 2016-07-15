#include "OpenClThread.h"
#include "Base/Log.h"

#include <boost/format.hpp>
#include "OpenClTask.h"

namespace Limitless
{

OpenClThread::OpenClThread()
{
}

void OpenClThread::start(cl::Context openCLContext, cl::CommandQueue openCLComandQueue)
{
	m_openCLContext=openCLContext;
	m_openCLComandQueue=openCLComandQueue;

	startThread();
}

void OpenClThread::stop()
{
	stopThread();
}

cl_int OpenClThread::enqueueWriteImage(cl::Image &image, const cl::size_t<3> &origin, const cl::size_t<3> &region, size_t rowPitch, size_t slicePitch, void *ptr,
        const std::vector<cl::Event> *events, cl::Event *event)
{
	WriteImageTask *task=new WriteImageTask();
	SharedITask sharedTask(task);

	task->commandQueue=m_openCLComandQueue;
	task->image=&image;
    task->blocking=CL_FALSE;
    task->origin=origin;
    task->region=region;
    task->row_pitch=rowPitch;
    task->slice_pitch=slicePitch;
    task->ptr=ptr;
    task->events=events;
    task->event=event;

	addTask(sharedTask);

	task->wait();

	return task->result;
}

}//namespace Limitless
