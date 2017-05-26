#include "GPUContext.h"
#include "Base/Log.h"
#include <sstream>
#include <GL/glew.h>
//#include <GL/glut.h>

#include <boost/format.hpp>

extern "C"
{
    __declspec(dllexport) DWORD NvOptimusEnablement=0x00000001;
    __declspec(dllexport) int AmdPowerXpressRequestHighPerformance=1;
}

enum ComputeVendor
{
    Unknown=0,
    Intel=1,
    nVidia=2,
    AMD=3
};

ComputeVendor getVendor(std::string vendor)
{
    if(vendor.compare(0, 5, "Intel"))
        return ComputeVendor::Intel;
    else if(vendor.compare(0, 6, "nVidia"))
        return ComputeVendor::nVidia;
    else if(vendor.compare(0, 3, "ATI"))
        return ComputeVendor::AMD;
    else if(vendor.compare(0, 22, "Advanced Micro Devices"))
        return ComputeVendor::AMD;
    return ComputeVendor::Unknown;
}

bool isSameVendor(std::string vendorName1, std::string vendorName2)
{
    ComputeVendor vendor1=getVendor(vendorName1);

    if(vendor1==ComputeVendor::Unknown)
        return false;

    return (vendor1==getVendor(vendorName2));
}

using namespace Limitless;

boost::mutex GPUContext::m_contextMutex;

bool GPUContext::m_openGLInit=false;
Limitless::DisplayHandle GPUContext::m_displayHandle=NULL;
#ifdef WIN32
HWND GPUContext::m_nativeWindow=NULL;
HDC GPUContext::m_hdc=NULL;
HGLRC GPUContext::m_glHandle=NULL;
#else// WIN32
#endif// WIN32

std::string GPUContext::m_openglVendor;
std::string GPUContext::m_name;
bool GPUContext::m_openCLInit=false;
cl::Platform GPUContext::m_openCLPlatform;
cl::Device GPUContext::m_openCLDevice;
cl::Context GPUContext::m_openCLContext;
cl::CommandQueue GPUContext::m_openCLComandQueue;

std::thread GPUContext::m_thread;
std::condition_variable GPUContext::m_event;
std::mutex GPUContext::m_mutex;
std::atomic<size_t> GPUContext::m_requestRedraw=0;
GpuTasks GPUContext::m_gpuTasks;

bool GPUContext::m_threadRunning;
bool GPUContext::m_stopThread;

SharedDrawCallbackSet GPUContext::m_drawCallbacks;
clGetGLContextInfoKHR_fn GPUContext::clGetGLContextInfoKHR=NULL;

OpenClThread GPUContext::m_openClThread;

bool  GPUContext::initOpenGL(Limitless::DisplayHandle displayHandle)
{
	boost::unique_lock<boost::mutex> lock(m_contextMutex);

	if(m_openGLInit)
		return true;

//	glutInit(NULL, NULL);
//	glutCreateWindow("Context Window");
//	glutHideWindow();
//
//	GLenum glError=glewInit();

#ifdef WIN32
	m_nativeWindow=(HWND)displayHandle;
	m_hdc=GetDC((HWND)displayHandle);
	m_displayHandle=displayHandle;//wglGetCurrentDC();
//	m_glHandle=wglGetCurrentContext();
	
	PIXELFORMATDESCRIPTOR pfd = {};

    pfd.nSize=sizeof(PIXELFORMATDESCRIPTOR);
    pfd.nVersion = 1;
    pfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
    pfd.iPixelType = PFD_TYPE_RGBA;
    pfd.cColorBits = 24;
    pfd.cAlphaBits = 8;
    pfd.cDepthBits = 32;
    pfd.iLayerType = PFD_MAIN_PLANE;

    int pixelFormat = ChoosePixelFormat(m_hdc, &pfd);
    
	if(pixelFormat == 0)
    {
		std::ostringstream errorString;

		errorString<<"ChoosePixelFormat() failed - returned "<<pixelFormat;
		Log::error("GPUContext", errorString.str());
		return false;
    }

    if(!SetPixelFormat(m_hdc, pixelFormat, &pfd))
	{
		std::ostringstream errorString;

		errorString<<"SetPixelFormat() failed";
		Log::error("GPUContext", errorString.str());
		return false;
	}

	m_glHandle=wglCreateContext(m_hdc);

	wglMakeCurrent(m_hdc, m_glHandle);
//	makeCurrent();
#else //WIN32
	assert(false);
#endif //WIN32
//	GLenum glError=glewInit();
    const GLubyte *glVendor=glGetString(GL_VENDOR);

    m_openglVendor=std::string((char *)glVendor);

    const GLubyte *glVersion=glGetString(GL_VERSION);
    const GLubyte *glExtensions=glGetString(GL_EXTENSIONS);

#ifdef WIN32
	wglMakeCurrent(NULL, NULL); //let go of context so thread can take it
#else //WIN32
#endif //WIN32

	startDrawThread();
	m_openGLInit=true;

	initOpenCL();

	return true;
}

void GPUContext::close()
{
    if(m_openGLInit)
    {
        m_openClThread.stop();
        stopThread();
    }
}

//void GPUContext::openGLContext()
//{
//	if(!m_openGLInit)
//		initOpenGL();
//}

void GPUContext::makeOpenGLCurrent()
{
//	if(!m_openGLInit)
//	{
//		assert(false);
//		return;
//		//initOpenGL();
//	}
//		
//
//#ifdef WIN32
//	wglMakeCurrent(m_hdc, m_glHandle);
//#else //WIN32
//#endif //WIN32
}

void *GPUContext::openGLContext()
{
	return (void *)m_glHandle;
}

void *GPUContext::nativeWindow()
{
	return (void *)m_nativeWindow;
}

void GPUContext::setOpenCLPlatform(cl::Platform platform, cl::Device device)
{
	m_openCLPlatform=platform;
	m_openCLDevice=device;
}

void GPUContext::initOpenCL(DisplayHandle displayHandle)
{
	boost::unique_lock<boost::mutex> lock(m_contextMutex);

	if(m_openCLInit)
		return;

	//assume opengl interop
	if(!m_openGLInit)
	{
		lock.unlock(); //release for opengl to lock for its init
		initOpenGL(displayHandle);
		lock.lock();
	}

	makeOpenGLCurrent();
	initOpenCL();
}

void GPUContext::initOpenCL()
{
	InitOpenClTask task;

	executeTask(&task);

	m_openClThread.start(m_openCLContext, m_openCLComandQueue);
}

void GPUContext::internalInitOpenCL()
{
#ifdef WIN32
	std::vector<cl::Platform> platforms;
	cl::Platform::get(&platforms);
	cl_device_id openGLDevice;
	bool found=false;

	assert(wglMakeCurrent(m_hdc, m_glHandle));

	for(size_t i=0; i<platforms.size(); ++i)
	{

        std::string vendor;

        platforms[i].getInfo(CL_PLATFORM_VENDOR, &m_name);

        //Intel openCL will return the CPU as a valid openCL device for a GPU
        //so lets make sure platforms match before selecting device
        if(!isSameVendor(m_openglVendor, m_name))
            continue;
//        if(m_name!=m_openglVendor)
//            continue;

		cl_int error;

		cl_context_properties clContextProps[]={ 
			CL_CONTEXT_PLATFORM, (cl_context_properties)platforms[i](),
			CL_WGL_HDC_KHR, (intptr_t)m_hdc,
			CL_GL_CONTEXT_KHR, (intptr_t)m_glHandle, 0
		};

//		clGetGLContextInfoKHR=(clGetGLContextInfoKHR_fn)clGetExtensionFunctionAddress("clGetGLContextInfoKHR");
		clGetGLContextInfoKHR=(clGetGLContextInfoKHR_fn)clGetExtensionFunctionAddressForPlatform(platforms[i](), "clGetGLContextInfoKHR");
		size_t deviceSize=sizeof(cl_device_id);
        size_t deviceSizeRet;

		error=clGetGLContextInfoKHR(clContextProps, CL_CURRENT_DEVICE_FOR_GL_CONTEXT_KHR, deviceSize, &openGLDevice, &deviceSizeRet);

		if(error == CL_SUCCESS)
		{
            if(deviceSizeRet!=0)
            {
                m_openCLPlatform=platforms[i];
                found=true;
                break;
            }
		}
	}

	if(!found)
		return;
//
//
	cl_context_properties clContextProps[]={ 
		CL_CONTEXT_PLATFORM, (cl_context_properties)m_openCLPlatform(),
		CL_WGL_HDC_KHR, (intptr_t)m_hdc,
		CL_GL_CONTEXT_KHR, (intptr_t)m_glHandle, 0
	};

	cl_int error;
//
//	cl_device_id openGLDevice;
//	size_t deviceSize=sizeof(cl_device_id);
//
//	if(clGetGLContextInfoKHR == NULL)
//		clGetGLContextInfoKHR=(clGetGLContextInfoKHR_fn)clGetExtensionFunctionAddress("clGetGLContextInfoKHR");
//
//	error=clGetGLContextInfoKHR(clContextProps, CL_CURRENT_DEVICE_FOR_GL_CONTEXT_KHR, deviceSize, &openGLDevice, &deviceSize);

	m_openCLDevice=cl::Device(openGLDevice);

	std::string extensions;
	std::string vendor;
	std::string profile;
	std::string version;
    std::string languages;

	m_openCLDevice.getInfo(CL_DEVICE_NAME, &m_name);
	m_openCLDevice.getInfo(CL_DEVICE_EXTENSIONS, &extensions);
	m_openCLDevice.getInfo(CL_DEVICE_VENDOR, &vendor);
	m_openCLDevice.getInfo(CL_DEVICE_PROFILE, &profile);
//	m_openCLDevice.getInfo(CL_DEVICE_IL_VERSION, &languages);

	m_openCLContext=cl::Context(m_openCLDevice, clContextProps, NULL, NULL, &error);
	if(error != CL_SUCCESS)
		assert(false);

	m_openCLComandQueue=cl::CommandQueue(m_openCLContext, m_openCLDevice, 0, &error);
	if(error != CL_SUCCESS)
		assert(false);
#else //WIN32
	assert(false);
#endif //WIN32

	m_openCLInit=true;
}

cl::Context GPUContext::openCLContext()
{
	if(!m_openCLInit)
		initOpenCL(m_displayHandle);

	return m_openCLContext;
}

#ifdef WIN32
__declspec(thread) cl::CommandQueue *perThread_clComandQueue=nullptr;
#else
thread_local cl::CommandQueue *perThread_clComandQueue=nullptr;
#endif

cl::CommandQueue &GPUContext::openCLCommandQueue()
{
//	openCLContext();
//	return m_openCLComandQueue;
	if(perThread_clComandQueue == nullptr)
	{
		cl_int error;

		perThread_clComandQueue=new cl::CommandQueue(m_openCLContext, m_openCLDevice, 0, &error);

		assert(error == CL_SUCCESS);
	}
	return *perThread_clComandQueue;
}

cl::Device &GPUContext::openCLDevice()
{
	return m_openCLDevice;
}

void GPUContext::addDrawCallback(SharedDrawCallback callback)
{
	m_drawCallbacks.insert(m_drawCallbacks.end(), callback);
}

void GPUContext::removeDrawCallback(SharedDrawCallback callback)
{
	SharedDrawCallbackSet::iterator iter=std::find(m_drawCallbacks.begin(), m_drawCallbacks.end(), callback);

	if(iter != m_drawCallbacks.end())
		m_drawCallbacks.erase(iter);
}

void GPUContext::requestRedraw()
{
	m_requestRedraw++;
	m_event.notify_all();
}

void GPUContext::startDrawThread()
{
	Limitless::Log::debug("GPUContext", "drawThread request start");
	
	m_threadRunning=false;
	m_stopThread=false;
	m_thread=std::thread(std::bind(&GPUContext::drawThread));

	{//wait for thread to start
		std::unique_lock<std::mutex> lock(m_mutex);
	
		while(!m_threadRunning)
			m_event.wait(lock);
	}

	Limitless::Log::debug("GPUContext", "drawThread request start finished");
}

void GPUContext::stopThread()
{
	Limitless::Log::debug("GPUContext", "drawThread request stop");

	{
		std::unique_lock<std::mutex> lock(m_mutex);
	
		m_stopThread=true;

	}
	m_event.notify_all();
	m_thread.join();

	Limitless::Log::debug("GPUContext", "drawThread request finished");
}

void GPUContext::drawThread()
{
	Limitless::Log::debug("GPUContext", "drawThread started");

//	m_processQThread=QThread::currentThread();
//	makeCurrent();
	std::unique_lock<std::mutex> lock(m_mutex);

//	makeOpenGLCurrent();
#ifdef WIN32
	wglMakeCurrent(m_hdc, m_glHandle);
#else //WIN32
#endif //WIN32
	GLenum glewError;

	glewError=glewInit();

	if(glewError != GLEW_OK)
		assert(false);

	HGLRC glHandle=wglGetCurrentContext();
	HDC hdc=wglGetCurrentDC();

	m_threadRunning=true;
	m_event.notify_all();

	while(!m_stopThread)
	{
		glHandle=wglGetCurrentContext();
		hdc=wglGetCurrentDC();
//		OutputDebugStringA((boost::format("Process Task gl:0x%016x  hdc:0x%016x\n")%glHandle%hdc).str().c_str());

		if(m_gpuTasks.empty() && (m_requestRedraw==0))
			m_event.wait(lock);

		if(!m_gpuTasks.empty())
		{
			processTasks(lock);
		}

		if(m_requestRedraw > 0)
		{
//			glHandle=wglGetCurrentContext();
//			hdc=wglGetCurrentDC();
//			OutputDebugStringA((boost::format("Draw gl:0x%016x  hdc:0x%016x\n")%glHandle%hdc).str().c_str());

			size_t requests=m_requestRedraw;

			m_requestRedraw-=requests; //clear all we currently see
			//call all rendering items
			for(const SharedDrawCallback &callback:m_drawCallbacks)
			{
				(*callback)();
			}
		}

		glHandle=wglGetCurrentContext();
		hdc=wglGetCurrentDC();
//		OutputDebugStringA((boost::format("End gl:0x%016x  hdc:0x%016x\n")%glHandle%hdc).str().c_str());
	}

    //need to release items we have or deconstructor will handle it in another thread with fault
    m_openCLComandQueue=cl::CommandQueue();
    m_openCLContext=cl::Context();
    m_openCLDevice=cl::Device();
    m_openCLPlatform=cl::Platform();

	m_threadRunning=false;
	Limitless::Log::debug("GPUContext", "drawThread stopped");
}

void GPUContext::executeTask(GpuWaitTask *task)
{
	std::unique_lock<std::mutex> lock(m_mutex);

	m_gpuTasks.push(task);
	m_event.notify_all();

	while(!task->complete)
		task->event.wait(lock);
}

void GPUContext::processTasks(std::unique_lock<std::mutex> &lock)
{
	GpuTask *gpuTask=m_gpuTasks.front();
	m_gpuTasks.pop();

	switch(gpuTask->task)
	{
	case GpuTask::Callback:
		threadCallback(lock, gpuTask);
		break;
	case GpuTask::InitOpenCl:
		threadInitOpenCl(lock, gpuTask);
		break;
    case GpuTask::CreateBuffer:
        threadCreateBuffer(lock, gpuTask);
        break;
	case GpuTask::CreateTexture:
		threadCreateTexture(lock, gpuTask);
		break;
	case GpuTask::UploadTexture:
		threadUploadTexture(lock, gpuTask);
		break;
	case GpuTask::CreateClImage:
		threadCreateClImage(lock, gpuTask);
		break;
    case GpuTask::GlFinish:
        threadCreateClImage(lock, gpuTask);
        break;
	}
}

void GPUContext::callback(std::function<void()> localCallback)
{
	GpuCallbackTask task;

	task.callback=localCallback;
	
	executeTask(&task);
}

void GPUContext::threadCallback(std::unique_lock<std::mutex> &lock, GpuTask *gpuTask)
{
	//unlock so we can process while tasks are added
	lock.unlock();

	GpuCallbackTask *task=(GpuCallbackTask *)gpuTask;

	task->callback();
	
	//need to lock back before setting complete
	lock.lock();

	task->complete=true;
	task->event.notify_all();
}

void GPUContext::threadInitOpenCl(std::unique_lock<std::mutex> &lock, GpuTask *gpuTask)
{
	//unlock so we can process while tasks are added
	lock.unlock();

	InitOpenClTask *intOpenClTask=(InitOpenClTask *)gpuTask;

	internalInitOpenCL();
	
	//need to lock back before setting complete
	lock.lock();

	intOpenClTask->complete=true;
	intOpenClTask->event.notify_all();
}

std::pair<GLuint, GLuint> GPUContext::createBuffer()
{
    CreateBufferTask task;

    task.alloc=false;

    executeTask(&task);

    return std::pair<GLuint, GLuint>(task.pbo, task.texture);
}

std::pair<GLuint, GLuint> GPUContext::createBuffer(GLsizei size)
{
    CreateBufferTask task;

    task.alloc=true;
    task.size=size;

    executeTask(&task);

    return std::pair<GLuint, GLuint>(task.pbo, task.texture);
}

void GPUContext::threadCreateBuffer(std::unique_lock<std::mutex> &lock, GpuTask *gpuTask)
{
    //unlock so we can process while tasks are added
    lock.unlock();

    CreateBufferTask *task=(CreateBufferTask *)gpuTask;

    glGenBuffers(1, &task->pbo);
//    glGenTextures(1, &task->texture);

    if(task->alloc)
    {
        glBindBuffer(GL_ARRAY_BUFFER, task->pbo);
        glBufferData(GL_ARRAY_BUFFER, task->size, NULL, GL_STATIC_DRAW);// GL_DYNAMIC_DRAW);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
//        glBindBuffer(GL_PIXEL_UNPACK_BUFFER_ARB, task->texture);

        glFinish();
    }

    //need to lock back before setting complete
    lock.lock();

    task->complete=true;
    task->event.notify_all();
}

GLuint GPUContext::createTexture()
{
	CreateTextureTask createTextureTask;

	createTextureTask.alloc=false;
	
	executeTask(&createTextureTask);

	return createTextureTask.texture;
}

GLuint GPUContext::createTexture(GLenum target, GLint internalFormat, GLsizei width, GLsizei height, GLenum format, GLenum type)
{
	CreateTextureTask createTextureTask;

	createTextureTask.alloc=true;
	createTextureTask.target=target;
	createTextureTask.internalFormat=internalFormat;
	createTextureTask.width=width;
	createTextureTask.height=height;
	createTextureTask.format=format;
	createTextureTask.type=type;

	executeTask(&createTextureTask);

	return createTextureTask.texture;
}

void GPUContext::threadCreateTexture(std::unique_lock<std::mutex> &lock, GpuTask *gpuTask)
{
	//unlock so we can process while tasks are added
	lock.unlock();

	CreateTextureTask *task=(CreateTextureTask *)gpuTask;

	glGenTextures(1, &task->texture);

	if(task->alloc)
	{
		glBindTexture(GL_TEXTURE_2D, task->texture);
		glTexImage2D(GL_TEXTURE_2D, 0, task->internalFormat, task->width, task->height, 0, task->format, task->type, NULL); 
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR); 
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glBindTexture(GL_TEXTURE_2D, 0);

		glFinish();
	}

	//need to lock back before setting complete
	lock.lock();

	task->complete=true;
	task->event.notify_all();
}

void GPUContext::uploadTexture(GLuint texture, GLenum target, GLint internalFormat, GLsizei width, GLsizei height, GLenum format, GLenum type, const GLvoid *data)
{
	UploadTextureTask task;

	task.texture=texture;
	task.target=target;
	task.internalFormat=internalFormat;
	task.width=width;
	task.height=height;
	task.format=format;
	task.type=type;
	task.data=data;

	executeTask(&task);
}

void GPUContext::threadUploadTexture(std::unique_lock<std::mutex> &lock, GpuTask *gpuTask)
{
	//unlock so we can process while tasks are added
	lock.unlock();

	UploadTextureTask *task=(UploadTextureTask *)gpuTask;

	glBindTexture(GL_TEXTURE_2D, task->texture);
	glTexImage2D(GL_TEXTURE_2D, 0, task->internalFormat, task->width, task->height, 0, task->format, task->type, task->data); 

	//need to lock back before setting complete
	lock.lock();
	
	task->complete=true;
	task->event.notify_all();
}

cl::ImageGL GPUContext::createClImage(cl_mem_flags flags, GLenum target, GLint internalFormat, GLsizei width, GLsizei height, GLenum format, GLenum type, GLuint &texture)
{
	CreateClImageTask task;
        
	task.flags=flags;
	task.target=target;
	task.internalFormat=internalFormat;
	task.width=width;
	task.height=height;
	task.format=format;
	task.type=type;
	task.texture=texture;

	executeTask(&task);

	texture=task.texture;
	return task.image;
}

void GPUContext::threadCreateClImage(std::unique_lock<std::mutex> &lock, GpuTask *gpuTask)
{
	//unlock so we can process while tasks are added
	lock.unlock();

	CreateClImageTask *task=(CreateClImageTask *)gpuTask;

	HGLRC glHandle=wglGetCurrentContext();
	HDC hdc=wglGetCurrentDC();

	if(task->texture == 0)
	{
		glGenTextures(1, &task->texture);

		glBindTexture(task->target, task->texture);
		glTexImage2D(task->target, 0, task->internalFormat, task->width, task->height, 0, task->format, task->type, NULL); 
		glTexParameteri(task->target, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(task->target, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(task->target, GL_TEXTURE_MAG_FILTER, GL_LINEAR); 
		glTexParameteri(task->target, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glBindTexture(task->target, 0);

		glFinish();
	}

	cl_int error = CL_SUCCESS;

	task->image=cl::ImageGL(GPUContext::openCLContext(), task->flags, task->target, 0, task->texture, &error);

	if(error != CL_SUCCESS)
		assert(false);
	//need to lock back before setting complete
	lock.lock();

	task->complete=true;
	task->event.notify_all();
}

cl_int GPUContext::enqueueWriteImage(cl::Image &image, const cl::size_t<3> &origin, const cl::size_t<3> &region, size_t rowPitch, size_t slicePitch, void *ptr,
        const std::vector<cl::Event> *events, cl::Event *event)
{
	return m_openClThread.enqueueWriteImage(image, origin, region, rowPitch, slicePitch, ptr, events, event);
}

void *getOpenGlExtension(std::string funcName)
{
    void *funcPtr=NULL;
#ifdef _WINDOWS
    funcPtr=(void *)wglGetProcAddress(funcName.c_str());

    if((funcPtr==0)||(funcPtr==(void *)0x1)||(funcPtr==(void *)0x2)||(funcPtr==(void *)0x3)||(funcPtr==(void *)-1))
    {
        return NULL;
    }
#else
#endif
    return funcPtr;
}