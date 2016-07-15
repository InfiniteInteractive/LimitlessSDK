#include "glwidget.h"
#include <boost/foreach.hpp>
#include <boost/format.hpp>
#include <boost/numeric/ublas/vector.hpp>
#include "Base/Log.h"
#include <QtGui/QImage>
#include "Media/ImageSampleSet.h"
#include "Media/GpuImageSampleSet.h"
#include <QtGui/QMouseEvent>
#include <QtCore/QThread>
#include <QtGui/QOpenGlContext>
#include <QtPlatformHeaders/QWGLNativeContext>
#include <QtGui/QWindow>

#include "ControlStructures/gpuUploadSample.h"

#include <iostream>
#include <fstream>
#include <functional>

#define _USE_MATH_DEFINES
#include <math.h>
#include <GL/glu.h>

#include <glm/gtc/matrix_transform.hpp>

#define TEXTURE_RECTANGLE_ARB 0x84F5
#ifndef GL_BGR 
#define GL_BGR 0x80E0 
#endif //GL_BGR 
#ifndef GL_BGRA
#define GL_BGRA 0x80E1
#endif //GL_BGRA

using namespace boost::numeric;
using namespace Limitless;

std::string vertexShader="\
#version 330\n\
\n\
layout(location = 0) in vec2 vertex;\n\
layout(location = 1) in vec2 vertexTexCoord;\n\
out vec2 texCoord;\n\
\n\
void main()\n\
{\n\
	texCoord=vertexTexCoord;\n\
	gl_Position=vec4(vertex, 0.0, 1.0);\n\
}";

std::string fragmentShader="\
#version 330\n\
\n\
in vec2 texCoord;\n\
out vec3 color;\n\
uniform usampler2D textureSampler;\n\
\n\
void main()\n\
{\n\
	uvec4 texel=texture(textureSampler, texCoord);\n\
	vec4 normalizedColor=vec4(texel)/255.0;\n\
	color=normalizedColor.rgb;\n\
//	color=vec3(normalizedColor.r, 1.0, texel.b);\n\
//	color=vec3(texCoord.x, texCoord.y, 0.0);\n\
}";

std::string vertexOverlayShader="\
#version 330\n\
\n\
layout(location = 0) in vec3 vertex;\n\
\n\
void main()\n\
{\n\
	gl_Position=vec4(vertex, 1);\n\
}";

std::string fragmentOverlayShader="\
#version 330\n\
\n\
out vec3 color;\n\
\n\
void main()\n\
{\n\
	color=vec3(1,0,0);\n\
}";


GLWidget::GLWidget(QWidget *parent, GLWidget *masterWidget):
QGLWidget(parent),
m_masterWidget(masterWidget),
m_displayMode(SINGLE),
m_displayModeChanged(true),
m_currentImage(0),
m_clipNear(1.0f),
m_clipFar(1000.0f),
m_transX(0.0f),
m_transY(0.0f),
m_transZ(-10.0f),
m_rotX(0.0f),
m_rotY(0.0f),
m_rotZ(0.0f),
m_init(false),
m_endThread(false),
m_readyToInit(false)
{
	m_sampleInUse=false;
	m_textureLoaded=false;

	m_imageInterfaceSampleId=MediaSampleFactory::getTypeId("IImageSample");
	m_imageSampleId=MediaSampleFactory::getTypeId("ImageSample");
	m_imageSampleSetInterfaceId=MediaSampleFactory::getTypeId("IImageSampleSet");
	m_imageSetSampleId=MediaSampleFactory::getTypeId("ImageSampleSet");
	m_gpuImageSampleId=MediaSampleFactory::getTypeId("GpuImageSample");
//	m_GPUImageSampleSetId=MediaSampleFactory::getTypeId("GPUImageSampleSet");
//	m_GPUUploadSampleId=MediaSampleFactory::getTypeId("GPUUploadSample");

	doneCurrent();

//	GPUContext::callback(std::bind(&GLWidget::getOpenglThread, this));

//	getMainGLHandle();

	m_drawCallback.reset(new Limitless::DrawCallback(std::bind(&GLWidget::draw, this)));
	GPUContext::addDrawCallback(m_drawCallback);
//	if(m_masterWidget == NULL)
//	{
//		m_contextMoved=false;
//		startThread();
//
////		QGLContext *glContext=new QGLContext(QGLFormat::defaultFormat(), this);
////		QThread *thread=getQThread();
////
////		setContext(glContext);
////		glContext->moveToThread(thread);
////
////		m_contextMoved=true;
////		m_event.notify_all();
//	}

//	m_event.notify_one();
}

GLWidget::GLWidget(QGLContext *context, QWidget *parent):
QGLWidget(context, parent),
m_masterWidget(NULL),
m_displayMode(SINGLE),
m_displayModeChanged(true),
m_currentImage(0),
m_clipNear(1.0f),
m_clipFar(1000.0f),
m_transX(0.0f),
m_transY(0.0f),
m_transZ(-10.0f),
m_rotX(0.0f),
m_rotY(0.0f),
m_rotZ(0.0f),
m_init(false),
m_endThread(false),
m_readyToInit(false)
{
	m_sampleInUse=false;
	m_textureLoaded=false;

	m_imageInterfaceSampleId=MediaSampleFactory::getTypeId("IImageSample");
	m_imageSampleId=MediaSampleFactory::getTypeId("ImageSample");
	m_imageSampleSetInterfaceId=MediaSampleFactory::getTypeId("IImageSampleSet");
	m_imageSetSampleId=MediaSampleFactory::getTypeId("ImageSampleSet");
	m_gpuImageSampleId=MediaSampleFactory::getTypeId("GpuImageSample");

	GPUContext::callback(std::bind(&GLWidget::getOpenglThread, this));

	m_drawCallback.reset(new Limitless::DrawCallback(std::bind(&GLWidget::draw, this)));
	GPUContext::addDrawCallback(m_drawCallback);

//	doneCurrent();
//	context->moveToThread(m_openglThread);
}

GLWidget::~GLWidget()
{
	GPUContext::removeDrawCallback(m_drawCallback);
}

void GLWidget::getOpenglThread()
{
	m_openglThread=QThread::currentThread();
}

void GLWidget::attachViewer(GLWidget *glViewer)
{
	if(glViewer == this)
		return;

	boost::unique_lock<boost::mutex> lock(m_mutex);

	GLWidgets::iterator iter=std::find(m_glViewers.begin(), m_glViewers.end(), glViewer);

	if(iter == m_glViewers.end())
	{
		Limitless::Log::debug("GLWidget", (boost::format("0x%08x drawThread request suspend")%this).str());
		//halt thread
		m_suspendThread=true;
		while(!m_isSuspended)
		{
			m_event.notify_one();
			m_threadEvent.wait(lock);
		}
		Limitless::Log::debug("GLWidget", (boost::format("0x%08x drawThread request suspend finished")%this).str());
		
		m_glViewers.push_back(glViewer);
		glViewer->setMasterWidget(this);

		//run thread
		Limitless::Log::debug("GLWidget", (boost::format("0x%08x drawThread request unsuspend")%this).str());
		m_suspendThread=false;
		while(m_isSuspended)
		{
			m_event.notify_one();
			m_threadEvent.wait(lock);
		}
		Limitless::Log::debug("GLWidget", (boost::format("0x%08x drawThread request unsuspend finished")%this).str());
	}
}

void GLWidget::removeViewer(GLWidget *glViewer)
{
	boost::unique_lock<boost::mutex> lock(m_mutex);

	GLWidgets::iterator iter=std::find(m_glViewers.begin(), m_glViewers.end(), glViewer);

	if(iter != m_glViewers.end())
	{
		m_glViewers.erase(iter);
		glViewer->setMasterWidget(nullptr);
	}
}

void GLWidget::setMasterWidget(GLWidget *master)
{
	if(m_thread.joinable())
		stopThread();

	QGLContext *glContext=new QGLContext(master->format(), this);
	QThread *masterThread=master->getQThread();

	glContext->create(master->context());
	setContext(glContext);

	glContext->moveToThread(masterThread);

	m_masterWidget=master;
	m_init=false;
}

void GLWidget::startThread()
{
	Limitless::Log::debug("GLWidget", (boost::format("0x%08x drawThread request start")%this).str());
	
	m_threadStarted=false;
	m_endThread=false;
	m_suspendThread=false;
	
	m_thread=boost::thread(boost::bind(&GLWidget::drawThread, this));

	boost::unique_lock<boost::mutex> lock(m_mutex);
	
	while(!m_threadStarted)
		m_threadEvent.wait(lock);
	Limitless::Log::debug("GLWidget", (boost::format("0x%08x drawThread request start finished")%this).str());
}

QThread *GLWidget::getQThread()
{
	return m_processQThread;
}

void GLWidget::stopThread()
{
	Limitless::Log::debug("GLWidget", (boost::format("0x%08x drawThread request stop")%this).str());

	{
		boost::unique_lock<boost::mutex> lock(m_mutex);
	
		m_endThread=true;
		m_suspendThread=false;
	}
	m_event.notify_all();
//	notify();
	m_thread.join();

	Limitless::Log::debug("GLWidget", (boost::format("0x%08x drawThread request finished")%this).str());
}

void GLWidget::notify()
{
//	if(m_masterWidget != NULL)
//		m_masterWidget->notify();
//	else
//		m_event.notify_one();
	GPUContext::requestRedraw();
}

boost::mutex &GLWidget::getMutex()
{
	if(m_masterWidget != NULL)
		return m_masterWidget->m_mutex;
	return m_mutex;
}

void GLWidget::getMainGLHandle()
{
//	QOpenGLContext mainContext;
//	QVariant glContextVariant;
//	
//	glContextVariant.setValue<void *>(GPUContext::openGLContext());
//
//	mainContext.setNativeHandle(glContextVariant);
//	mainContext.create();
//
//	m_qtMainGlContext=QGLContext::fromOpenGLContext(&mainContext);
//
////	QOpenGLContext *glContext=QOpenGLContext::currentContext();
//
//	//
//	QGLContext *glContext=new QGLContext(QGLFormat::defaultFormat());
//
//	HGLRC glHandle=wglGetCurrentContext();
//	HDC hdc=wglGetCurrentDC();
//
//	glContext->create(m_qtMainGlContext);
//
//	setContext(glContext);

	QOpenGLContext *nativeOpenglContext=new QOpenGLContext();
	QWGLNativeContext nativeContext((HGLRC)Limitless::GPUContext::openGLContext(), (HWND)Limitless::GPUContext::nativeWindow());

	nativeOpenglContext->setNativeHandle(QVariant::fromValue(nativeContext));
	nativeOpenglContext->create();

	QOpenGLContext *openglContext=new QOpenGLContext();

	openglContext->setShareContext(nativeOpenglContext);
	openglContext->setFormat(nativeOpenglContext->format());
	openglContext->create();

	QGLContext *glContext=QGLContext::fromOpenGLContext(openglContext);

	setContext(glContext);

//	doneCurrent();
//	glContext->moveToThread(m_openglThread);
}

bool GLWidget::initialize()
{
	getMainGLHandle();
//	GLenum glewError;
//
//	if(m_masterWidget != NULL)
//	{
////		QGLContext *glContext=new QGLContext(m_masterWidget->format(), this);
////
////		glContext->create(m_masterWidget->context());
////		setContext(glContext);
////
////		m_init=true;
//	}
//	else
//	{
////		QGLContext *glContext=new QGLContext(QGLFormat::defaultFormat(), this);
////
////		setContext(glContext);
////		
////		makeCurrent();
////		glewError=glewInit();
//
////		GPUContext::makeOpenGLCurrent();
//
//		GPUContext::callback(std::bind(&GLWidget::getMainGLHandle, this));
//		
////		const QGLContext *glCurrentContext=QGLContext::currentContext();
//
////		QGLContext *glContext=new QGLContext(QGLFormat::defaultFormat());
////
//////		glContext->create(glCurrentContext);
////		glContext->create(m_qtMainGlContext);
////
////		setContext(glContext);
////		makeCurrent();
//	}

	makeCurrent();

	initOpenGlFunctions();

	setAutoBufferSwap(false);
//	glShadeModel(GL_SMOOTH);
	glClearColor(0.2, 0.2, 0.2, 1.0f);
	glClearDepth(1.0f);
//	glEnable(GL_CULL_FACE);
//	glFrontFace(GL_CCW);
//	glCullFace(GL_BACK);
//	glEnable(GL_DEPTH_TEST);
//	glDepthFunc(GL_LEQUAL);
//	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);

	m_programID=LoadShaders(vertexShader, fragmentShader);
//	m_programOverlayID=LoadShaders(vertexOverlayShader, fragmentOverlayShader);
	checkErrorGL();

	m_textureSamplerID=_glGetUniformLocation(m_programID, "textureSampler");
	checkErrorGL();

	_glGenVertexArrays(1, &m_vertexArrayID);
	_glBindVertexArray(m_vertexArrayID);
	checkErrorGL();

	m_textureType=GL_TEXTURE_2D;
	glEnable(m_textureType);
	checkErrorGL();

	GLboolean textureSet;
	glGetBooleanv(m_textureType, &textureSet);

	m_vertexBufferData.resize(12);

	m_vertexBufferData[0]=-1.0f;
	m_vertexBufferData[1]=1.0f;
	m_vertexBufferData[2]=-1.0f;
	m_vertexBufferData[3]=1.0f;
	m_vertexBufferData[4]=-1.0f;
	m_vertexBufferData[5]=-1.0f;
	m_vertexBufferData[6]=1.0f;
	m_vertexBufferData[7]=1.0f;
	m_vertexBufferData[8]=1.0f;
	m_vertexBufferData[9]=-1.0f;
	m_vertexBufferData[10]=-1.0f;
	m_vertexBufferData[11]=-1.0f;

	checkErrorGL();

	_glGenBuffers(1, &m_vertexBuffer);
	_glBindBuffer(GL_ARRAY_BUFFER, m_vertexBuffer);
	_glBufferData(GL_ARRAY_BUFFER, m_vertexBufferData.size()*sizeof(GLfloat), m_vertexBufferData.data(), GL_STATIC_DRAW);
	checkErrorGL();
	
	m_texCoordBufferData.resize(12);
	m_texCoordBufferData[0]=0.0;
	m_texCoordBufferData[1]=0.0;
	m_texCoordBufferData[2]=1.0;
	m_texCoordBufferData[3]=0.0;
	m_texCoordBufferData[4]=0.0;
	m_texCoordBufferData[5]=1.0;
	m_texCoordBufferData[6]=1.0;
	m_texCoordBufferData[7]=0.0;
	m_texCoordBufferData[8]=1.0;
	m_texCoordBufferData[9]=1.0;
	m_texCoordBufferData[10]=0.0;
	m_texCoordBufferData[11]=1.0;

	_glGenBuffers(1, &m_texCoordBuffer);
	_glBindBuffer(GL_ARRAY_BUFFER, m_texCoordBuffer);
	_glBufferData(GL_ARRAY_BUFFER, m_texCoordBufferData.size()*sizeof(GLfloat), m_texCoordBufferData.data(), GL_STATIC_DRAW);
	checkErrorGL();

//	doneCurrent();

	m_init=true;
	return true;
}

void GLWidget::initOpenGlFunctions()
{
	getOpenGlExtension("glCompileShader", _glCompileShader);
	getOpenGlExtension("glCreateShader", _glCreateShader);
	getOpenGlExtension("glShaderSource", _glShaderSource);
	getOpenGlExtension("glGetShaderiv", _glGetShaderiv);
	getOpenGlExtension("glCreateProgram", _glCreateProgram);
	getOpenGlExtension("glAttachShader", _glAttachShader);
	getOpenGlExtension("glLinkProgram", _glLinkProgram);
	getOpenGlExtension("glGetProgramiv", _glGetProgramiv);
	getOpenGlExtension("glDeleteShader", _glDeleteShader);
	getOpenGlExtension("glGetUniformLocation", _glGetUniformLocation);
	getOpenGlExtension("glGenVertexArrays", _glGenVertexArrays);
	getOpenGlExtension("glBindVertexArray", _glBindVertexArray);
	getOpenGlExtension("glGenBuffers", _glGenBuffers);
	getOpenGlExtension("glBindBuffer", _glBindBuffer);
	getOpenGlExtension("glBufferData", _glBufferData);
	getOpenGlExtension("glActiveTexture", _glActiveTexture);
	getOpenGlExtension("glUseProgram", _glUseProgram);
	getOpenGlExtension("glUniform1i", _glUniform1i);
	getOpenGlExtension("glEnableVertexAttribArray", _glEnableVertexAttribArray);
	getOpenGlExtension("glVertexAttribPointer", _glVertexAttribPointer);
	getOpenGlExtension("glDisableVertexAttribArray", _glDisableVertexAttribArray);
}

void GLWidget::glInit()
{
//	QGLWidget::glInit();
}

void GLWidget::glDraw()
{
//	QGLWidget::glDraw();
//	m_event.notify_one();
	notify();
}

void GLWidget::initializeGL()
{
//	initialize();
//	m_event.notify_one();
//	m_readyToInit=true;
}

void GLWidget::resizeGL(int width, int height)
{
//	glViewport(0, 0, width, height);
//	glMatrixMode(GL_PROJECTION);
//	glLoadIdentity();
//
//	double ratio=(double)width/height;
//	m_fovY=25.0f*(M_PI/180.0f);
//	m_fovX=2.0f*atan(ratio*tan(m_fovY/2));
//	gluPerspective(m_fovY*(180.0f/M_PI), ratio, m_clipNear, m_clipFar);
//
//	glMatrixMode(GL_MODELVIEW);
//	checkErrorGL();

//	m_resize=true;
//	m_event.notify_one();
}

void GLWidget::paintGL()
{
//	drawImage(m_images);
//	m_event.notify_one();
}

void GLWidget::fitToScreen()
{
	float transX, transY;

	m_transX=0.0f;
	m_transY=0.0f;
	
	transX=-m_imageQuadWidth/(2.0f*tan(m_fovX/2.0f));
	transY=-m_imageQuadHeight/(2.0f*tan(m_fovY/2.0f));
	
	if(transX < transY)
		m_transZ=transX;
	else
		m_transZ=transY;
}

void GLWidget::setDisplayMode(DisplayMode mode)
{
	m_displayMode=mode;
	m_displayModeChanged=true;
}

void GLWidget::displaySample(SharedMediaSample sample)
{
//	boost::unique_lock<boost::mutex> lock(getMutex());
	if(!m_sampleInUse)
	{
		m_newSample=sample;
		m_sampleInUse=true;
		notify();
	}
}

bool GLWidget::updateSample()
{
	bool update=false;

	{
//		boost::unique_lock<boost::mutex> lock(getMutex());
		if(m_sampleInUse)
		{
			if(m_newSample != m_currentSample)
			{
				m_currentSample=m_newSample;
				update=true;
			}
		}
	}

	if(update)
		setCurrentSample(m_newSample);

	return update;
}

void GLWidget::setCurrentSample(SharedMediaSample sample)
{
//	std::vector<QImage> images;
	bool initImages=true;

	//	OutputDebugStringA((boost::format("Sample (0x%08x:%02d) - displaySample\n")%sample.get()%sample.use_count()).str().c_str());
	if(sample->isType(m_gpuImageSampleId))
	{
		SharedGpuImageSample gpuImageSample=boost::dynamic_pointer_cast<GpuImageSample>(sample);

		if(!gpuImageSample)
			return;

		if(m_images.size() != 1)
			m_images.resize(1);

		m_images[0]=gpuImageSample;
	}
	if(sample->isType(m_imageInterfaceSampleId))
	{
		SharedIImageSample imageSample=boost::dynamic_pointer_cast<IImageSample>(sample);

		if(imageSample != SharedIImageSample())
		{
//			boost::unique_lock<boost::mutex> lock(getMutex());
			
			if(m_images.size() != 1)
				m_images.resize(1);

//			OutputDebugStringA((boost::format("Sample (0x%08x:%02d) - swap\n")%m_images[0].get()%m_images[0].use_count()).str().c_str());
//			QImage saveImage((const uchar *)imageSample->buffer(), imageSample->width(), imageSample->height(), QImage::Format_RGB32);
//
//			saveImage.save("capture.png");

			m_images[0]=imageSample;
		}

		if(m_images.size() != m_mediaCount)
			m_calculateMedia=true;

//		notify();
//		m_event.notify_one();
	}
	else if(sample->isType(m_imageSampleSetInterfaceId))
	{
		SharedIImageSampleSet imageSampleSet=boost::dynamic_pointer_cast<IImageSampleSet>(sample);

		if(imageSampleSet != SharedIImageSampleSet())
		{
//			boost::unique_lock<boost::mutex> lock(getMutex());

			if(m_images.size() != imageSampleSet->sampleSetSize())
				m_images.resize(imageSampleSet->sampleSetSize());

			for(size_t i=0; i<imageSampleSet->sampleSetSize(); ++i)
			{
				m_images[i]=imageSampleSet->sample(i);
			}
		}

		if(m_images.size() != m_mediaCount)
			m_calculateMedia=true;

//		notify();
	}
	
}

void GLWidget::resizeEvent(QResizeEvent *evt)
{
	m_resize=true;

	notify();
}

void GLWidget::closeEvent(QCloseEvent *evt)
{
	if(m_masterWidget == NULL)
	{
		m_endThread=true;
		notify();
	}
	QGLWidget::closeEvent(evt);
}

bool GLWidget::event(QEvent *e)
{
	if(e->type() == QEvent::Show)
		notify();
	else if(e->type() == QEvent::ParentChange) //The glContext will be changed, need to reinit openGl
	{
		boost::unique_lock<boost::mutex> lock(m_mutex);

		//need to init
		m_init=false;
		bool ret=QGLWidget::event(e);
		
		notify();
		return ret;
	}
	else if(e->type() == QEvent::Resize)
	{
		return QGLWidget::event(e);
	}
	return QGLWidget::event(e);
}


void GLWidget::drawThread()
{
	Limitless::Log::debug("GLWidget", (boost::format("0x%08x drawThread started")%this).str());

	m_processQThread=QThread::currentThread();

//	{ //wait for context to be moved
//		boost::unique_lock<boost::mutex> lock(m_mutex);
//
//		m_threadStarted=true;
//		m_threadEvent.notify_all();
//
//		Limitless::Log::write((boost::format("0x%08x drawThread wait for context")%this).str());
//		while(!m_contextMoved)
//			m_event.wait(lock);
//		Limitless::Log::write((boost::format("0x%08x drawThread wait for context finished")%this).str());
//	}

	if(!m_init)
		initialize();
		
	makeCurrent();

	boost::unique_lock<boost::mutex> lock(m_mutex);

	m_threadStarted=true;
	m_isSuspended=false;

	if(m_endThread)
		return;

	m_threadEvent.notify_all();

	while(true)
	{
//		Limitless::Log::write((boost::format("0x%08x drawThread wait event")%this).str());
		m_event.wait(lock); //wait for event
//		Limitless::Log::write((boost::format("0x%08x drawThread wait event finished")%this).str());

		if(m_endThread)
			break;

		if(m_suspendThread)
		{
			Limitless::Log::debug("GLWidget", (boost::format("0x%08x drawThread suspended")%this).str());

			while(m_suspendThread)
			{
				m_isSuspended=true;
				m_threadEvent.notify_all();
				m_event.wait(lock);
			}
			Limitless::Log::debug("GLWidget", (boost::format("0x%08x drawThread unsuspended")%this).str());
			m_isSuspended=false;
			m_threadEvent.notify_all();
		}


		bool swap=false;

//		Limitless::Log::write((boost::format("0x%08x drawThread draw")%this).str());
		if(!m_glViewers.empty())
		{
			for(GLWidget *viewer:m_glViewers)
			{
//				if(viewer->draw())
//					swap=true;
			}
		}

//			makeCurrent();
//		if(draw())
//			swap=true;

//			if(swap)
//				swapBuffers();
//			doneCurrent();
//		Limitless::Log::write((boost::format("0x%08x drawThread draw finished")%this).str());
	}

	Limitless::Log::debug("GLWidget", (boost::format("0x%08x drawThread exit")%this).str());
}

//bool GLWidget::draw(bool useCurrent)
void GLWidget::draw()
{
	if(!isVisible())
		return;// false;

//	QWidget *parent=parentWidget();
//
//	while(parent != NULL)
//	{
//		if(parent->parentWidget() != NULL)
//			parent=parent->parentWidget();
//	}
//
//	if(parent == NULL)
//		return;
//
//	QWindow *widnow=mainWindow->windowHandle();
//
//	if(widnow
	if(!windowHandle()->isExposed())
		return;

	if(!m_init)
		initialize();

	//	if(useCurrent)
		makeCurrent();

	bool drawTexture=false;
	bool updateTexture=updateSample();

	if(m_resize)
		resizeMedia();

	if(m_calculateMedia)
		calculateMediaPos();

	GLenum errCode=glGetError();
	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
	glDepthRange(0.0, 1.0);

	bool textureEnbled=false;
	{
		{
			if(updateTexture)
			{
				glActiveTexture(GL_TEXTURE0);
				assert(checkErrorGL());

				textureEnbled=true;

				if(!m_images.empty())
				{
					if(m_images[0])
					{
						if(m_images[0]->isType(m_gpuImageSampleId))
						{
							SharedGpuImageSample gpuImageSample=boost::dynamic_pointer_cast<GpuImageSample>(m_images[0]);

							m_currentTexture=gpuImageSample->texture();
	//						glUniform1i(m_textureSamplerID, gpuImageSample->texture());
							m_textureLoaded=true;

							//need to hold onto the sample as the texture is not owned by this filter
							m_currentGpuImageSample=gpuImageSample;
							m_sampleInUse=false;
						}
						else
						{
							if(m_textures.size() < m_images.size())
								allocateTextures(m_images.size());

							if((m_textureWidth != m_mediaWidth) || (m_textureHeight != m_mediaHeight))
							{
								//reset memory size
								glTexImage2D(m_textureType, 0, GL_RGBA8UI, m_mediaWidth, m_mediaHeight, 0, GL_RGBA_INTEGER, GL_UNSIGNED_BYTE, NULL);
	
								m_textureWidth=m_mediaWidth;
								m_textureHeight=m_mediaHeight;
							}

							for(size_t i=0; i<m_images.size(); ++i)
							{
								Limitless::IImageSample *image=m_images[i].get();

								glBindTexture(m_textureType, m_textures[i]);

								if(image->channels() == 4)
									glTexSubImage2D(m_textureType, 0, 0, 0, image->width(), image->height(), GL_RGBA_INTEGER, GL_UNSIGNED_BYTE, image->buffer());
								else
									glTexSubImage2D(m_textureType, 0, 0, 0, image->width(), image->height(), GL_RGB_INTEGER, GL_UNSIGNED_BYTE, image->buffer());
							}
							m_textureLoaded=true;
	//						glUniform1i(m_textureSamplerID, 0);
							m_currentTexture=m_textures[0];
							m_sampleInUse=false;//texture updated no longer need sample
						}
					}
				}
			}
			checkErrorGL();

			if(m_textureLoaded)
			{
				glUseProgram(m_programID);
				assert(checkErrorGL());

				if(!textureEnbled)
				{
					glActiveTexture(GL_TEXTURE0);
					assert(checkErrorGL());
				}

				glUniform1i(m_textureSamplerID, 0);
				glBindTexture(m_textureType, m_currentTexture);

				for(size_t i=0; i<m_vertexBuffers.size(); ++i)
				{
					glEnableVertexAttribArray(0);
					glBindBuffer(GL_ARRAY_BUFFER, m_vertexBuffers[i]);
					glVertexAttribPointer(
						0,                  // attribute. No particular reason for 0, but must match the layout in the shader.
						2,                  // size
						GL_FLOAT,           // type
						GL_FALSE,           // normalized?
						0,                  // stride
						(void*)0            // array buffer offset
						);
					assert(checkErrorGL());
					
					glEnableVertexAttribArray(1);
					glBindBuffer(GL_ARRAY_BUFFER, m_texCoordBuffer);
					assert(checkErrorGL());
					glVertexAttribPointer(
						1,                                // attribute. No particular reason for 1, but must match the layout in the shader.
						2,                                // size : U+V => 2
						GL_FLOAT,                         // type
						GL_FALSE,                         // normalized?
						0,                                // stride
						(void*)0                          // array buffer offset
						);
					glDrawArrays(GL_TRIANGLES, 0, 6); // 3 indices starting at 0 -> 1 triangle
					assert(checkErrorGL());

					glDisableVertexAttribArray(0);
					glDisableVertexAttribArray(1);
				}
			}
		}
	}

//	drawOverlay();

	swapBuffers();
//	if(useCurrent)
//		doneCurrent();

//	return drawTexture;
}

void GLWidget::resizeMedia()
{
//	makeCurrent();
	GLenum errCode=glGetError();//clear errors

	int localWidth=width();
	int localHeight=height();

	m_resize=false;
	glViewport(0, 0, width(), height());
	checkErrorGL();
	float ratio=(float)width()/height();

	m_fovY=25.0f*(M_PI/180.0f);
	m_fovX=2.0f*atan(ratio*tan(m_fovY/2));
	float fovYRads=m_fovY*(180.0f/M_PI);

	calculateMediaPos();
}

void GLWidget::calculateMediaPos()
{
	int mediaWidth;
	int mediaHeight;
	bool calculatePos=false;
	int maxWidth=0;
	int maxHeight=0;
	int imagesX=0;
	int imagesY=0;

	if(!m_images.empty())
	{
		if(m_textures.size() < m_images.size())
			allocateTextures(m_images.size());

		imagesX=sqrt((float)m_images.size());

		if(m_images.size() <= (imagesX*imagesX))
			imagesY=imagesX;
		else if(m_images.size() <= (imagesX*(imagesX+1)))
			imagesY=imagesX+1;
		else
		{
			imagesX++;
			imagesY=imagesX;
		}

		for(Limitless::SharedIImageSample &image:m_images)
		{
			if(!image)
				continue;

			if(image->width() > maxWidth)
				maxWidth=image->width();
			if(image->height() > maxHeight)
				maxHeight=image->height();
		}

		mediaWidth=maxWidth*imagesX;
		mediaHeight=maxHeight*imagesY;

		calculatePos=true;
	}

	if(calculatePos)
	{
		m_calculateMedia=false;

		m_mediaCount=m_images.size();
		m_mediaWidth=mediaWidth;
		m_mediaHeight=mediaHeight;
		m_mediaGridWidth=maxWidth;
		m_mediaGridHeight=maxHeight;
		m_mediaGridX=imagesX;
		m_mediaGridY=imagesY;

		int localWidth=width();
		int localHeight=height();

		float aspect;
		float positionX, positionY;

		aspect=(float)mediaHeight/mediaWidth;

		if(localWidth*aspect >localHeight)
		{
			positionY=1.0;
			positionX=localHeight/(aspect*localWidth);
		}
		else
		{
			positionX=1.0;
			positionY=(localWidth*aspect)/localHeight;
		}

		m_imageQuadWidth=2*positionX;
		m_imageQuadHeight=2*positionY;

		m_vertexBuffersData.resize(m_images.size());
		for(size_t i=0; i<m_images.size(); ++i)
			m_vertexBuffersData[i].resize(12);

//		m_vertexBufferData.resize(12*m_images.size());
//		m_texCoordBufferData.resize(12*m_images.size());

		float quadWidth=m_imageQuadWidth/imagesX;
		float quadHeight=m_imageQuadHeight/imagesY;
		size_t pos=0;
		size_t texPos=0;

		int count=0;

		float texWidth=1.0/imagesX;
		float texHeight=1.0/imagesY;

		for(int x=0; x<imagesX && count<m_images.size(); ++x)
		{
			float xPos=(quadWidth*x)-positionX;
			float texXPos=texWidth*x;

			for(int y=0; y<imagesY && count<m_images.size(); ++y)
			{
				float yPos=positionY-(quadHeight*y);

				std::vector<GLfloat> &vertexBuffer=m_vertexBuffersData[count];

				vertexBuffer[0]=xPos;
				vertexBuffer[1]=yPos;
				vertexBuffer[2]=xPos+quadWidth;
				vertexBuffer[3]=yPos;
				vertexBuffer[4]=xPos;
				vertexBuffer[5]=yPos-quadHeight;
				vertexBuffer[6]=xPos+quadWidth;
				vertexBuffer[7]=yPos;
				vertexBuffer[8]=xPos+quadWidth;
				vertexBuffer[9]=yPos-quadHeight;
				vertexBuffer[10]=xPos;
				vertexBuffer[11]=yPos-quadHeight;

				glBindBuffer(GL_ARRAY_BUFFER, m_vertexBuffers[count]);
				glBufferData(GL_ARRAY_BUFFER, vertexBuffer.size()*sizeof(GLfloat), vertexBuffer.data(), GL_STATIC_DRAW);

//				m_vertexBufferData[pos++]=xPos;
//				m_vertexBufferData[pos++]=yPos;
//				m_vertexBufferData[pos++]=xPos+quadWidth;
//				m_vertexBufferData[pos++]=yPos;
//				m_vertexBufferData[pos++]=xPos;
//				m_vertexBufferData[pos++]=yPos-quadHeight;
//				m_vertexBufferData[pos++]=xPos+quadWidth;
//				m_vertexBufferData[pos++]=yPos;
//				m_vertexBufferData[pos++]=xPos+quadWidth;
//				m_vertexBufferData[pos++]=yPos-quadHeight;
//				m_vertexBufferData[pos++]=xPos;
//				m_vertexBufferData[pos++]=yPos-quadHeight;
//
//				float texYPos=texHeight*y;
//
//				m_texCoordBufferData[texPos++]=texXPos;
//				m_texCoordBufferData[texPos++]=texYPos;
//				m_texCoordBufferData[texPos++]=texXPos+texWidth;
//				m_texCoordBufferData[texPos++]=texYPos;
//				m_texCoordBufferData[texPos++]=texXPos;
//				m_texCoordBufferData[texPos++]=texYPos+texHeight;
//				m_texCoordBufferData[texPos++]=texXPos+texWidth;
//				m_texCoordBufferData[texPos++]=texYPos;
//				m_texCoordBufferData[texPos++]=texXPos+texWidth;
//				m_texCoordBufferData[texPos++]=texYPos+texHeight;
//				m_texCoordBufferData[texPos++]=texXPos;
//				m_texCoordBufferData[texPos++]=texYPos+texHeight;

				count++;
			}
		}

//		glBindBuffer(GL_ARRAY_BUFFER, m_vertexBuffer);
//		glBufferData(GL_ARRAY_BUFFER, m_vertexBufferData.size()*sizeof(GLfloat), m_vertexBufferData.data(), GL_STATIC_DRAW);
//
//		glBindBuffer(GL_ARRAY_BUFFER, m_texCoordBuffer);
//		glBufferData(GL_ARRAY_BUFFER, m_texCoordBufferData.size()*sizeof(GLfloat), m_texCoordBufferData.data(), GL_STATIC_DRAW);
	}
}

void GLWidget::setupleMultipleImages(const std::vector<QImage> &images)
{
	int imagesX=sqrt((float)images.size());
	int imagesY;

	if(images.size() <= (imagesX*imagesX))
		imagesY=imagesX;
	else if(images.size() <= (imagesX*(imagesX+1)))
		imagesY=imagesX+1;
	else
	{
		imagesX++;
		imagesY=imagesX;
	}

	int maxWidth=0;
	int maxHeight=0;
	
	BOOST_FOREACH(const QImage &image, images)
	{
		if(image.width() > maxWidth)
			maxWidth=image.width();
		if(image.height() > maxHeight)
			maxHeight=image.height();
	}

	int totalWidth=maxWidth*imagesX;
	int totalHeight=maxHeight*imagesY;
	
	float positionX, positionY;
	float aspect;

	aspect=totalHeight/totalWidth;
	if(totalWidth > totalHeight)
	{
		positionX=-1.0f;
		positionY=-positionX/aspect;
	}
	else
	{
		positionY=1.0f;
		positionX=positionY*aspect;
	}

	m_displayModeChanged=false;
}

void GLWidget::setupCylindrical()
{
	m_displayModeChanged=false;
}

void GLWidget::drawImage(std::vector<QImage> &images)
{
//	makeCurrent();
	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();

	glDisable(GL_LIGHTING);

	glTranslatef(m_transX, m_transY, m_transZ);
	glRotatef(m_rotX, 1.0f, 0.0f, 0.0f);
	glRotatef(m_rotY, 0.0f, 1.0f, 0.0f);
	glRotatef(m_rotZ, 0.0f, 0.0f, 1.0f);

	switch(m_displayMode)
	{
	case SINGLE:
		{
			if((m_currentImage >= 0) && (m_currentImage < images.size()))
				drawSingleImage(images[m_currentImage]);
		}
		break;
	case MULTIPLE:
		break;
	case CYLINDRICAL:
		break;
	}

//	glBegin(GL_TRIANGLES);
//	{
//		glColor3f(1.0f, 0.0f, 0.0f);
//		glVertex3f(-2.0f, 2.0f, -1.0f);
//		glColor3f(0.0f, 1.0f, 0.0f);
//		glVertex3f(-2.0f, -2.0f, -1.0f);
//		glColor3f(0.0f, 0.0f, 1.0f);
//		glVertex3f(2.0f, -2.0f, -1.0f);
//	}
//	glEnd();

//	swapBuffers();
//	doneCurrent();
}

void GLWidget::drawSingleImage(QImage &image)
{
	if((image.width() <= 0) || (image.height() <= 0))
		return;

	int width, height;
	unsigned char *imageData;

	imageData=image.bits();
	width=image.width();
	height=image.height();

//	image.save("test.jpg");

	if(imageData == NULL)
		return;

	if(m_textures.size() < 1)
		allocateTextures(1);

	checkErrorGL();
	glBindTexture(m_textureType, m_textures[0]);
//	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	checkErrorGL();
//	if(m_textureType == TEXTURE_RECTANGLE_ARB)
	glColor4f(1.0, 1.0, 1.0, 1.0);

	if(image.format() == QImage::Format_RGB888)
		glTexImage2D(m_textureType, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, imageData); 
	else if(image.format() == QImage::Format_ARGB32)
		glTexImage2D(m_textureType, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, imageData); 
	else
		return;
	glTexParameteri(m_textureType, GL_TEXTURE_MAG_FILTER, GL_LINEAR); 
	glTexParameteri(m_textureType, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

//		glTexImage2D(m_textureType, 0, GL_RGBA, width, height, 0, GL_BGRA, GL_UNSIGNED_BYTE, imageData); 
	checkErrorGL();
//		glTexImage2D(m_textureType, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, imageData); 
//	else
//		gluBuild2DMipmaps(m_textureType, GL_RGB, width, height, GL_RGB, GL_UNSIGNED_BYTE, imageData);
	checkErrorGL();

	glBegin(GL_QUADS);
	{
		for(Quads::iterator iter=m_imageQuads.begin(); iter!=m_imageQuads.end(); ++iter)
		{
			for(int i=0; i<4; ++i)
			{
				Coord &coord=iter->coords[i];
				
				glTexCoord2f(coord.texture.x, coord.texture.y);
				glVertex3f(coord.point.x, coord.point.y, coord.point.z);
			}
		}
	}
	glEnd();
	checkErrorGL();
}

void GLWidget::allocateTextures(int textureCount)
{
	if(m_textures.size() < textureCount)
	{
		if(!m_textures.empty())
			glDeleteTextures(m_textures.size(), &m_textures[0]);
		m_textures.clear();
	}

	m_textures.resize(textureCount);
	glGenTextures(textureCount, &m_textures[0]);

	for(size_t i=0; i < m_textures.size(); ++i)
	{
		glBindTexture(m_textureType, m_textures[i]);

		glTexParameteri(m_textureType, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(m_textureType, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(m_textureType, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(m_textureType, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	}

	if(m_vertexArrayIDs.size() < textureCount)
	{
		if(!m_vertexArrayIDs.empty())
			glDeleteVertexArrays(m_vertexArrayIDs.size(), m_vertexArrayIDs.data());
		m_vertexArrayIDs.clear();
	}
	
	m_vertexArrayIDs.resize(textureCount);
	_glGenVertexArrays(textureCount, m_vertexArrayIDs.data());

	if(m_vertexBuffers.size() < textureCount)
	{
		if(!m_vertexBuffers.empty())
			glDeleteBuffers(m_vertexBuffers.size(), m_vertexBuffers.data());
		m_vertexBuffers.clear();
	}

	m_vertexBuffers.resize(textureCount);
	_glGenBuffers(textureCount, m_vertexBuffers.data());

	checkErrorGL();
}

void GLWidget::mousePressEvent(QMouseEvent* event)
{
	Qt::MouseButton button=event->button();

	if(Qt::LeftButton == button)
		m_leftClick=event->globalPos();
	if(Qt::RightButton == button)
		m_rightClick=event->globalPos();
}

void GLWidget::mouseReleaseEvent(QMouseEvent* event)
{
	this->setCursor(QCursor(Qt::OpenHandCursor));
}

void GLWidget::mouseMoveEvent(QMouseEvent* event)
{
	Qt::MouseButtons buttons=event->buttons();

	if(Qt::LeftButton&buttons)
	{
		bool rightButton=Qt::RightButton&buttons;

		if(!rightButton)
		{
			Qt::KeyboardModifiers keys=event->modifiers();

			if(Qt::AltModifier&keys)
			{
			}
			else
			{
				m_rotY += (float)(event->globalX()-m_leftClick.x())*(360.0f/width());
				m_rotX += (float)(m_leftClick.y()-event->globalY())*(360.0f/height());
				notify();
			}
		}
		m_leftClick = event->globalPos();
	}
}

bool GLWidget::checkErrorGL()
{
	GLenum errCode;
	const GLubyte *errString;

	if ((errCode = glGetError()) != GL_NO_ERROR)
	{
		errString=gluErrorString(errCode);
		
		std::string error=(boost::format("GL error: %s\n")%errString).str();
		
		OutputDebugStringA((LPSTR)error.c_str());
		return false;
	}
	return true;
}

GLuint GLWidget::LoadShaderFiles(const char * vertex_file_path, const char * fragment_file_path)
{

	// Read the Vertex Shader code from the file
	std::string VertexShaderCode;
	std::ifstream VertexShaderStream(vertex_file_path, std::ios::in);
	if(VertexShaderStream.is_open())
	{
		std::string Line="";
		while(getline(VertexShaderStream, Line))
			VertexShaderCode+="\n" + Line;
		VertexShaderStream.close();
	}
	else
	{
		//		printf("Impossible to open %s. Are you in the right directory ? Don't forget to read the FAQ !\n", vertex_file_path);
		//		getchar();
		assert(false);
		return 0;
	}

	// Read the Fragment Shader code from the file
	std::string FragmentShaderCode;
	std::ifstream FragmentShaderStream(fragment_file_path, std::ios::in);
	if(FragmentShaderStream.is_open())
	{
		std::string Line="";
		while(getline(FragmentShaderStream, Line))
			FragmentShaderCode+="\n" + Line;
		FragmentShaderStream.close();
	}

	return LoadShaders(VertexShaderCode, FragmentShaderCode);
}

GLuint GLWidget::LoadShaders(std::string VertexShaderCode, std::string FragmentShaderCode)
{
	GLint Result=GL_FALSE;
	int InfoLogLength;

	// Create the shaders
	GLuint VertexShaderID=_glCreateShader(GL_VERTEX_SHADER);
	GLuint FragmentShaderID=_glCreateShader(GL_FRAGMENT_SHADER);

	// Compile Vertex Shader
	//	printf("Compiling shader : %s\n", vertex_file_path);
	char const * VertexSourcePointer=VertexShaderCode.c_str();
	_glShaderSource(VertexShaderID, 1, &VertexSourcePointer, NULL);
	_glCompileShader(VertexShaderID);

	// Check Vertex Shader
	_glGetShaderiv(VertexShaderID, GL_COMPILE_STATUS, &Result);
	_glGetShaderiv(VertexShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
	if(InfoLogLength > 0)
	{
		std::vector<char> VertexShaderErrorMessage(InfoLogLength+1);
		glGetShaderInfoLog(VertexShaderID, InfoLogLength, NULL, &VertexShaderErrorMessage[0]);
		//printf("%s\n", &VertexShaderErrorMessage[0]);
		OutputDebugStringA((boost::format("%s\n")%VertexShaderErrorMessage[0]).str().c_str());
		assert(Result);
	}



	// Compile Fragment Shader
	//	printf("Compiling shader : %s\n", fragment_file_path);
	char const * FragmentSourcePointer=FragmentShaderCode.c_str();
	_glShaderSource(FragmentShaderID, 1, &FragmentSourcePointer, NULL);
	_glCompileShader(FragmentShaderID);

	// Check Fragment Shader
	_glGetShaderiv(FragmentShaderID, GL_COMPILE_STATUS, &Result);
	_glGetShaderiv(FragmentShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
	if(InfoLogLength > 0)
	{
		std::vector<char> FragmentShaderErrorMessage(InfoLogLength+1);
		glGetShaderInfoLog(FragmentShaderID, InfoLogLength, NULL, &FragmentShaderErrorMessage[0]);
		OutputDebugStringA((boost::format("%s\n")%FragmentShaderErrorMessage[0]).str().c_str());
		//		printf("%s\n", &FragmentShaderErrorMessage[0]);
		assert(Result);
	}



	// Link the program
	//	printf("Linking program\n");
	GLuint ProgramID=_glCreateProgram();
	_glAttachShader(ProgramID, VertexShaderID);
	_glAttachShader(ProgramID, FragmentShaderID);
	_glLinkProgram(ProgramID);

	// Check the program
	_glGetProgramiv(ProgramID, GL_LINK_STATUS, &Result);
	_glGetProgramiv(ProgramID, GL_INFO_LOG_LENGTH, &InfoLogLength);
	if(InfoLogLength > 0)
	{
		std::vector<char> ProgramErrorMessage(InfoLogLength+1);
		glGetProgramInfoLog(ProgramID, InfoLogLength, NULL, &ProgramErrorMessage[0]);
		OutputDebugStringA((boost::format("%s\n")%ProgramErrorMessage[0]).str().c_str());
		//		printf("%s\n", &ProgramErrorMessage[0]);
		assert(Result);
	}

	_glDeleteShader(VertexShaderID);
	_glDeleteShader(FragmentShaderID);

	return ProgramID;
}
