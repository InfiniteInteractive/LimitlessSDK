#ifndef _GPUContext_h_
#define _GPUContext_h_

#include "Media/media_define.h"
#include "Media/IImageSample.h"
#include "Media/MediaSampleFactory.h"
#include "Media/MediaInterface.h"

#ifdef _WINDOWS
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#endif //_WINDOWS

#include <GL/glew.h>
#include "CL/cl.hpp"

#include "Media/OpenClThread.h"

#include <boost/thread.hpp>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <atomic>
#include <queue>
#include <set>
#include <functional>

namespace Limitless
{

struct GpuTask
{
	enum Task
	{
		Callback,
		InitOpenCl,
		CreateTexture,
		UploadTexture,
		CreateClImage
	};

	GpuTask(Task task):task(task){}

	Task task;
};

struct GpuWaitTask:GpuTask
{
	GpuWaitTask(Task task):GpuTask(task), complete(false){}

	std::condition_variable event;
	bool complete;
};

struct GpuCallbackTask:GpuWaitTask
{
	GpuCallbackTask():GpuWaitTask(GpuTask::Callback){}

	std::function<void()> callback;
};

struct InitOpenClTask:GpuWaitTask
{
	InitOpenClTask():GpuWaitTask(GpuTask::InitOpenCl){}
};

struct CreateTextureTask:GpuWaitTask
{
	CreateTextureTask():GpuWaitTask(GpuTask::CreateTexture){}

//input
	bool alloc;
	GLenum target;
	GLint internalFormat;
	GLsizei width;
	GLsizei height;
	GLenum format;
	GLenum type;

//output
	GLuint texture;
};

struct UploadTextureTask:GpuWaitTask
{
	UploadTextureTask():GpuWaitTask(GpuTask::UploadTexture){}

	GLuint texture;
	GLenum target;
	GLint internalFormat;
	GLsizei width;
	GLsizei height;
	GLenum format;
	GLenum type;
	const GLvoid *data;
};

struct CreateClImageTask:GpuWaitTask
{
	CreateClImageTask():GpuWaitTask(GpuTask::CreateClImage){}

//input
	cl_mem_flags flags;
	GLenum target;
	GLint internalFormat;
	GLsizei width;
	GLsizei height;
	GLenum format;
	GLenum type;

//output
	GLuint texture;
	cl::ImageGL image;
};

typedef std::queue<GpuTask *> GpuTasks;

typedef std::function<void()> DrawCallback;
typedef std::shared_ptr<DrawCallback> SharedDrawCallback;
typedef std::set<SharedDrawCallback> SharedDrawCallbackSet;

class MEDIA_EXPORT GPUContext
{
public:
	GPUContext();

//	static void openGLContext();
	static void makeOpenGLCurrent();
	static void *openGLContext();
	static void *nativeWindow();

	static void setOpenCLPlatform(cl::Platform platform, cl::Device device);
	static cl::Context openCLContext();
	static cl::CommandQueue openCLCommandQueue();
	static cl::Device openCLDevice();

	static void callback(std::function<void()> localCallback);
	static void initOpenCL(DisplayHandle hdc);
	static void initOpenCL();
	static bool initOpenGL(DisplayHandle hdc);
	static void close();

	static GLuint createTexture();
	static GLuint createTexture(GLenum target, GLint internalFormat, GLsizei width, GLsizei height, GLenum format, GLenum type);
	static void uploadTexture(GLuint texture, GLenum target, GLint internalFormat, GLsizei width, GLsizei height, GLenum format, GLenum type, const GLvoid *data);
	static cl::ImageGL createClImage(cl_mem_flags flags, GLenum target, GLint internalFormat, GLsizei width, GLsizei height, GLenum format, GLenum type, GLuint &texture);

	static void addDrawCallback(SharedDrawCallback callback);
	static void removeDrawCallback(SharedDrawCallback callback);

	static void requestRedraw();

//opencl
	static cl_int enqueueWriteImage(cl::Image &image, const cl::size_t<3> &origin, const cl::size_t<3> &region, size_t rowPitch, size_t slicePitch, void *ptr,
        const std::vector<cl::Event> *events, cl::Event *event);

private:
	static void startDrawThread();
	static void stopThread();
	static void drawThread();
	static void processTasks(std::unique_lock<std::mutex> &lock);
	static void executeTask(GpuWaitTask *task);

	static void threadCallback(std::unique_lock<std::mutex> &lock, GpuTask *gpuTask);
	static void internalInitOpenCL();
	static void threadInitOpenCl(std::unique_lock<std::mutex> &lock, GpuTask *gpuTask);
	static void threadCreateTexture(std::unique_lock<std::mutex> &lock, GpuTask *gpuTask);
	static void threadUploadTexture(std::unique_lock<std::mutex> &lock, GpuTask *gpuTask);
	static void threadCreateClImage(std::unique_lock<std::mutex> &lock, GpuTask *gpuTask);

	static boost::mutex m_contextMutex;

	static bool m_openGLInit;
	static DisplayHandle m_displayHandle;

#ifdef WIN32
	static HWND m_nativeWindow;
	static HDC m_hdc;
	static HGLRC m_glHandle;
#else// WIN32
#endif// WIN32
	static std::string m_name;
	static bool m_openCLInit;
	static cl::Platform m_openCLPlatform;
	static cl::Device m_openCLDevice;
	static cl::Context m_openCLContext;
	static cl::CommandQueue m_openCLComandQueue;

	static clGetGLContextInfoKHR_fn clGetGLContextInfoKHR;

	static std::thread m_thread;
	static std::condition_variable m_event;
	static std::mutex m_mutex;

	static std::atomic<size_t> m_requestRedraw;
	static GpuTasks m_gpuTasks;

	static bool m_threadRunning;
	static bool m_stopThread;

	static SharedDrawCallbackSet m_drawCallbacks;

	static OpenClThread m_openClThread;
};

template<typename _Func>
bool getOpenGlExtension(std::string funcName, _Func &function)
{
	function=NULL;
#ifdef _WINDOWS
    function=(_Func)wglGetProcAddress(funcName.c_str());

    if((function==0)||(function==(void *)0x1)||(function==(void *)0x2)||(function==(void *)0x3)||(function==(void *)-1))
    {
        function=NULL;
		return false;
    }
#else
#endif
    return true;
}

}//namespace Limitless

#endif //_GPUContext_h_