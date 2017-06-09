#include "glImageView.h"

#include <boost/foreach.hpp>
#include <boost/format.hpp>
#include <boost/numeric/ublas/vector.hpp>
#include "Base/Log.h"
#include "Media/ImageSampleSet.h"
#include "Media/GpuImageSampleSet.h"

#include <QtGui/QImage>
#include <QtGui/QMouseEvent>
#include <QtCore/QThread>
#include <QtGui/QOpenGlContext>
#include <QtPlatformHeaders/QWGLNativeContext>
#include <QtGui/QWindow>


//#include "ControlStructures/gpuUploadSample.h"

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

namespace Limitless
{

//std::string vertexShader="\
//#version 330\n\
//\n\
//layout(location = 0) in vec2 vertex;\n\
//layout(location = 1) in vec2 vertexTexCoord;\n\
//out vec2 texCoord;\n\
//\n\
//void main()\n\
//{\n\
//	texCoord=vertexTexCoord;\n\
//	gl_Position=vec4(vertex, 0.0, 1.0);\n\
//}";
//
//std::string fragmentShader="\
//#version 330\n\
//\n\
//in vec2 texCoord;\n\
//out vec3 color;\n\
//uniform usampler2D textureSampler;\n\
//\n\
//void main()\n\
//{\n\
//	uvec4 texel=texture(textureSampler, texCoord);\n\
//	vec4 normalizedColor=vec4(texel)/255.0;\n\
//	color=normalizedColor.rgb;\n\
////	color=vec3(normalizedColor.r, 1.0, texel.b);\n\
////	color=vec3(texCoord.x, texCoord.y, 0.0);\n\
//}";

std::string vertexShader="\
#version 330\n\
\n\
layout(location = 0) in vec2 vertex;\n\
uniform ImagePos\
{\
    ivec2 window;\
    ivec2 image;\
    vec2 center;\
    float zoom;\
};\
out vec2 texCoord;\n\
\n\
void main()\n\
{\n\
    vec2 wndCoord;\
    wndCoord.x=float(window.x)/2.0f*vertex.x;\
    wndCoord.y=float(window.y)/2.0f*-vertex.y;\
    wndCoord*=zoom;\
    texCoord=(center+wndCoord)/image;\
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
    if((texCoord.x<0.0) || (texCoord.x>1.0))\n\
        color=vec3(0.8, 0.8, 0.8);\n\
    else if((texCoord.y<0.0) || (texCoord.y>1.0))\n\
        color=vec3(0.8, 0.8, 0.8);\n\
    else\n\
    {\
//        color=vec3(texCoord.x, texCoord.y, 0.0);\n\
        uvec4 texel=texture(textureSampler, texCoord);\n\
        vec4 normalizedColor=vec4(texel)/255.0;\n\
        color=normalizedColor.rgb;\n\
    }\
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

void initGlew()
{
    static bool _initGlew=false;

    if(!_initGlew)
    {
        glewInit();
        _initGlew=true;
    }
}

unsigned int GLImageView::invalidTexture=std::numeric_limits<unsigned int>::max();

GLImageView::GLImageView(QWidget *parent, GLImageView *masterWidget, bool manualDraw):
QGLWidget(parent),
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
m_readyToInit(false),
m_manualDraw(manualDraw),
m_texture(invalidTexture),
m_currentImageWidth(0),
m_currentImageHeight(0)
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
    if(!m_manualDraw)
    {
        m_drawCallback.reset(new Limitless::DrawCallback(std::bind(&GLImageView::draw, this)));
        GPUContext::addDrawCallback(m_drawCallback);
    }
}

GLImageView::GLImageView(QGLContext *context, QWidget *parent, bool manualDraw):
QGLWidget(context, parent),
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
m_readyToInit(false),
m_manualDraw(manualDraw)
{
	m_sampleInUse=false;
	m_textureLoaded=false;

	m_imageInterfaceSampleId=MediaSampleFactory::getTypeId("IImageSample");
	m_imageSampleId=MediaSampleFactory::getTypeId("ImageSample");
	m_imageSampleSetInterfaceId=MediaSampleFactory::getTypeId("IImageSampleSet");
	m_imageSetSampleId=MediaSampleFactory::getTypeId("ImageSampleSet");
	m_gpuImageSampleId=MediaSampleFactory::getTypeId("GpuImageSample");

    if(!m_manualDraw)
    {
        m_drawCallback.reset(new Limitless::DrawCallback(std::bind(&GLImageView::draw, this)));
        GPUContext::addDrawCallback(m_drawCallback);
    }
}

GLImageView::~GLImageView()
{
	GPUContext::removeDrawCallback(m_drawCallback);
}

void GLImageView::notify()
{
	GPUContext::requestRedraw();
}

void GLImageView::getMainGLHandle()
{
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
}

bool GLImageView::initialize()
{
	getMainGLHandle();
    initGlew();

	makeCurrent();

	setAutoBufferSwap(false);
	glClearColor(0.2, 0.2, 0.2, 1.0f);
	glClearDepth(1.0f);

	m_programID=LoadShaders(vertexShader, fragmentShader);
	checkErrorGL();

	m_textureSamplerID=glGetUniformLocation(m_programID, "textureSampler");
	checkErrorGL();

	m_textureType=GL_TEXTURE_2D;
	glEnable(m_textureType);
	checkErrorGL();

	GLboolean textureSet;
	glGetBooleanv(m_textureType, &textureSet);
	
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

	glGenBuffers(1, &m_texCoordBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, m_texCoordBuffer);
	glBufferData(GL_ARRAY_BUFFER, m_texCoordBufferData.size()*sizeof(GLfloat), m_texCoordBufferData.data(), GL_STATIC_DRAW);
	checkErrorGL();

//	doneCurrent();

     std::vector<glm::vec2> verticies(6);

    verticies[0].x=-1.0f;
    verticies[0].y=1.0f;
    verticies[1].x=1.0f;
    verticies[1].y=1.0f;
    verticies[2].x=-1.0f;
    verticies[2].y=-1.0f;
    verticies[3].x=1.0f;
    verticies[3].y=1.0f;
    verticies[4].x=1.0f;
    verticies[4].y=-1.0f;
    verticies[5].x=-1.0f;
    verticies[5].y=-1.0f;

    glGenBuffers(1, &m_vertexBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, m_vertexBuffer);
    glBufferData(GL_ARRAY_BUFFER, verticies.size()*sizeof(glm::vec2), verticies.data(), GL_STATIC_DRAW);

    glGenVertexArrays(1, &m_vertexArrayID);
    glBindVertexArray(m_vertexArrayID);
    checkErrorGL();
    
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, m_vertexBuffer);
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
    assert(checkErrorGL());

    glBindVertexArray(0);

    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);


	m_init=true;
	return true;
}

void GLImageView::glInit()
{
}

void GLImageView::glDraw()
{
	notify();
}

void GLImageView::initializeGL()
{
}

void GLImageView::resizeGL(int width, int height)
{
}

void GLImageView::paintGL()
{
}

//void GLImageView::fitToScreen()
//{
//	float transX, transY;
//
//	m_transX=0.0f;
//	m_transY=0.0f;
//	
//	transX=-m_imageQuadWidth/(2.0f*tan(m_fovX/2.0f));
//	transY=-m_imageQuadHeight/(2.0f*tan(m_fovY/2.0f));
//	
//	if(transX < transY)
//		m_transZ=transX;
//	else
//		m_transZ=transY;
//}

void GLImageView::setDisplayMode(DisplayMode mode)
{
	m_displayMode=mode;
	m_displayModeChanged=true;
}

void GLImageView::displaySample(SharedMediaSample sample)
{
//	boost::unique_lock<boost::mutex> lock(getMutex());
	if(!m_sampleInUse)
	{
		m_newSample=sample;
		m_sampleInUse=true;
        if(!m_manualDraw)
		    notify();
	}
}

bool GLImageView::updateSample()
{
	bool update=false;

	{
//		boost::unique_lock<boost::mutex> lock(getMutex());
		if(m_sampleInUse)
		{
//			if(m_newSample != m_currentSample)
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

void GLImageView::setCurrentSample(SharedMediaSample sample)
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

void GLImageView::resizeEvent(QResizeEvent *evt)
{
	m_resize=true;

	notify();
}

void GLImageView::closeEvent(QCloseEvent *evt)
{
	QGLWidget::closeEvent(evt);
}

bool GLImageView::event(QEvent *e)
{
	if(e->type() == QEvent::Show)
		notify();
	else if(e->type() == QEvent::ParentChange) //The glContext will be changed, need to reinit openGl
	{
//		boost::unique_lock<boost::mutex> lock(m_mutex);

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



void GLImageView::draw()
{
    //glew init needs to be called for the dll otherwise functions will be null
	if(!isVisible())
		return;// false;

	if(!windowHandle()->isExposed())
		return;

	if(!m_init)
		initialize();

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
		if(updateTexture)
		{
			glActiveTexture(GL_TEXTURE0);
			assert(checkErrorGL());

			textureEnbled=true;

			if(!m_images.empty())
			{
				if(m_images[0])
				{
                    Limitless::SharedIImageSample image=m_images[0];

                    if((image->width()!=m_currentImageWidth)||(image->height()!=m_currentImageHeight))
                    {
                        m_currentImageWidth=image->width();
                        m_currentImageHeight=image->height();
                        m_centerPosX=(float)m_currentImageWidth/2.0f;
                        m_centerPosY=(float)(float)m_currentImageHeight/2.0f;
                        m_imagePosUniform->uniform("image")=glm::ivec2(m_currentImageWidth, m_currentImageHeight);
                        m_imagePosUniform->uniform("center")=glm::vec2(m_centerPosX, m_centerPosY);
                    }

					if(image->isType(m_gpuImageSampleId))
					{
						SharedGpuImageSample gpuImageSample=boost::dynamic_pointer_cast<GpuImageSample>(image);

						m_currentTexture=gpuImageSample->texture();
//						glUniform1i(m_textureSamplerID, gpuImageSample->texture());
						m_textureLoaded=true;

						//need to hold onto the sample as the texture is not owned by this filter
						m_currentGpuImageSample=gpuImageSample;
						m_sampleInUse=false;
					}
					else
					{
                        if(m_texture == invalidTexture)
                            glGenTextures(1, &m_texture);
                            
                        glBindTexture(m_textureType, m_texture);

                        //reset memory size
                        glTexImage2D(m_textureType, 0, GL_RGBA8UI, image->width(), image->height(), 0, GL_RGBA_INTEGER, GL_UNSIGNED_BYTE, image->buffer());
                        m_textureLoaded=true;

//							if(m_textures.size() < m_images.size())
//								allocateTextures(m_images.size());
//
//							if((m_textureWidth != m_mediaWidth) || (m_textureHeight != m_mediaHeight))
//							{
//								//reset memory size
//								glTexImage2D(m_textureType, 0, GL_RGBA8UI, m_mediaWidth, m_mediaHeight, 0, GL_RGBA_INTEGER, GL_UNSIGNED_BYTE, NULL);
//	
//								m_textureWidth=m_mediaWidth;
//								m_textureHeight=m_mediaHeight;
//							}
//
//							for(size_t i=0; i<m_images.size(); ++i)
//							{
//								Limitless::IImageSample *image=m_images[i].get();
//
//								glBindTexture(m_textureType, m_textures[i]);
//
//								if(image->channels() == 4)
//									glTexSubImage2D(m_textureType, 0, 0, 0, image->width(), image->height(), GL_RGBA_INTEGER, GL_UNSIGNED_BYTE, image->buffer());
//								else
//									glTexSubImage2D(m_textureType, 0, 0, 0, image->width(), image->height(), GL_RGB_INTEGER, GL_UNSIGNED_BYTE, image->buffer());
//							}
//							m_textureLoaded=true;
//	//						glUniform1i(m_textureSamplerID, 0);
//							m_currentTexture=m_textures[0];
//							m_sampleInUse=false;//texture updated no longer need sample
					}
				}
			}
		}
		checkErrorGL();

		if(m_textureLoaded)
		{
//			glUseProgram(m_programID);
            m_program.use();

            m_imagePosUniform->bind();
			assert(checkErrorGL());

			if(!textureEnbled)
			{
				glActiveTexture(GL_TEXTURE0);
				assert(checkErrorGL());
			}

			glUniform1i(m_textureSamplerID, 0);
			glBindTexture(m_textureType, m_currentTexture);

            glBindVertexArray(m_vertexArrayID);
//                for(size_t i=0; i<m_imageQuads.size(); ++i)
//                {
//                    AxisAlignedQuad &quad=m_imageQuads[i];
//
//                    glEnableVertexAttribArray(0);
//                    glBindBuffer(GL_ARRAY_BUFFER, quad.vertexBuffer);
//                    glVertexAttribPointer(
//                        0,                  // attribute. No particular reason for 0, but must match the layout in the shader.
//                        2,                  // size
//                        GL_FLOAT,           // type
//                        GL_FALSE,           // normalized?
//                        0,                  // stride
//                        (void*)0            // array buffer offset
//                    );
//                    assert(checkErrorGL());
//
//                    glEnableVertexAttribArray(1);
//                    glBindBuffer(GL_ARRAY_BUFFER, m_texCoordBuffer);
//                    assert(checkErrorGL());
//                    glVertexAttribPointer(
//                        1,                                // attribute. No particular reason for 1, but must match the layout in the shader.
//                        2,                                // size : U+V => 2
//                        GL_FLOAT,                         // type
//                        GL_FALSE,                         // normalized?
//                        0,                                // stride
//                        (void*)0                          // array buffer offset
//                    );
            glDrawArrays(GL_TRIANGLES, 0, 6); // 3 indices starting at 0 -> 1 triangle
            assert(checkErrorGL());

//                    glDisableVertexAttribArray(0);
//                    glDisableVertexAttribArray(1);
        }
	}
		
	
    
    drawOverlay();

	swapBuffers();
}

void GLImageView::resizeMedia()
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

    m_imagePosUniform->uniform("window")=glm::ivec2(width(), height());

	calculateMediaPos();
}

glm::ivec2 GLImageView::getMediaPos(glm::ivec2 windowPos)
{
    glm::ivec2 imagePos;
    glm::vec2 glWindowPos;

    glWindowPos.x=((float)windowPos.x/width()*2.0f)-1.0f;
    glWindowPos.y=((float)windowPos.y/height()*2.0f)-1.0f;

    if(!m_imageQuads.empty())
    {
        AxisAlignedQuad &quad=m_imageQuads[0];

        glm::vec2 size=quad.end-quad.start;
        
        imagePos.x=(glWindowPos.x-quad.start.x)/size.x*m_mediaWidth;
        imagePos.y=(glWindowPos.y-quad.start.y)/size.y*m_mediaHeight;
        
    }
    return imagePos;
}

glm::vec2 GLImageView::getNormalizedMediaPos(glm::ivec2 mediaPos)
{
    glm::vec2 imagePos;

    imagePos.x=((float)mediaPos.x/m_mediaWidth*m_imageQuadWidth)-(m_imageQuadWidth/2.0f);
    imagePos.y=(m_imageQuadHeight/2.0f)-((float)mediaPos.y/m_mediaHeight*m_imageQuadHeight);
    return imagePos;
}

glm::ivec2 GLImageView::getWindowPos(glm::ivec2 imagePos)
{
    glm::ivec2 windowPos;

    windowPos.x=(int)((float)imagePos.x/m_mediaWidth*width());
    windowPos.y=(int)((float)imagePos.y/m_mediaHeight*height());
    return imagePos;
}

void GLImageView::calculateMediaPos()
{
//	int mediaWidth;
//	int mediaHeight;
//	bool calculatePos=false;
//	int maxWidth=0;
//	int maxHeight=0;
//	int imagesX=0;
//	int imagesY=0;
//
//	if(!m_images.empty())
//	{
//		if(m_textures.size() < m_images.size())
//			allocateTextures(m_images.size());
//
//		imagesX=sqrt((float)m_images.size());
//
//		if(m_images.size() <= (imagesX*imagesX))
//			imagesY=imagesX;
//		else if(m_images.size() <= (imagesX*(imagesX+1)))
//			imagesY=imagesX+1;
//		else
//		{
//			imagesX++;
//			imagesY=imagesX;
//		}
//
//		for(Limitless::SharedIImageSample &image:m_images)
//		{
//			if(!image)
//				continue;
//
//			if(image->width() > maxWidth)
//				maxWidth=image->width();
//			if(image->height() > maxHeight)
//				maxHeight=image->height();
//		}
//
//		mediaWidth=maxWidth*imagesX;
//		mediaHeight=maxHeight*imagesY;
//
//		calculatePos=true;
//	}
//
//    if(calculatePos)
//    {
//        m_calculateMedia=false;
//
//        m_mediaCount=m_images.size();
//        m_mediaWidth=mediaWidth;
//        m_mediaHeight=mediaHeight;
//        m_mediaGridWidth=maxWidth;
//        m_mediaGridHeight=maxHeight;
//        m_mediaGridX=imagesX;
//        m_mediaGridY=imagesY;
//
//        int localWidth=width();
//        int localHeight=height();
//
//        float aspect;
//        float positionX, positionY;
//
//        aspect=(float)mediaHeight/mediaWidth;
//
//        if(localWidth*aspect>localHeight)
//        {
//            positionY=1.0;
//            positionX=localHeight/(aspect*localWidth);
//        }
//        else
//        {
//            positionX=1.0;
//            positionY=(localWidth*aspect)/localHeight;
//        }
//
//        m_imageQuadWidth=2*positionX;
//        m_imageQuadHeight=2*positionY;
//
//        m_imageQuads.resize(m_images.size());
//
//        float quadWidth=m_imageQuadWidth/imagesX;
//        float quadHeight=m_imageQuadHeight/imagesY;
//        size_t pos=0;
//        size_t texPos=0;
//
//        int count=0;
//
//        float texWidth=1.0/imagesX;
//        float texHeight=1.0/imagesY;
//
//        for(int x=0; x<imagesX && count<m_images.size(); ++x)
//        {
//            float xPos=(quadWidth*x)-positionX;
//            float texXPos=texWidth*x;
//
//            for(int y=0; y<imagesY && count<m_images.size(); ++y)
//            {
//                float yPos=positionY-(quadHeight*y);
//
//                AxisAlignedQuad &quad=m_imageQuads[count];
//
//                quad.start=glm::vec2(xPos, yPos-quadHeight);
//                quad.end=glm::vec2(xPos+quadWidth, yPos);
//
//                quad.verticies[0].x=xPos;
//                quad.verticies[0].y=yPos;
//                quad.verticies[1].x=xPos+quadWidth;
//                quad.verticies[1].y=yPos;
//                quad.verticies[2].x=xPos;
//                quad.verticies[2].y=yPos-quadHeight;
//                quad.verticies[3].x=xPos+quadWidth;
//                quad.verticies[3].y=yPos;
//                quad.verticies[4].x=xPos+quadWidth;
//                quad.verticies[4].y=yPos-quadHeight;
//                quad.verticies[5].x=xPos;
//                quad.verticies[5].y=yPos-quadHeight;
//
//                glBindBuffer(GL_ARRAY_BUFFER, quad.vertexBuffer);
//                glBufferData(GL_ARRAY_BUFFER, quad.verticies.size()*sizeof(glm::vec2), quad.verticies.data(), GL_STATIC_DRAW);
//            }
//        }
//    }
}

void GLImageView::fitToScreen()
{
    m_centerPosX=(float)m_mediaWidth/2.0f;
    m_centerPosY=(float)m_mediaHeight/2.0f;
}

void GLImageView::fitOneToOne()
{
    width();
}

void GLImageView::allocateTextures(int textureCount)
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
	glGenVertexArrays(textureCount, m_vertexArrayIDs.data());

	if(m_vertexBuffers.size() < textureCount)
	{
		if(!m_vertexBuffers.empty())
			glDeleteBuffers(m_vertexBuffers.size(), m_vertexBuffers.data());
		m_vertexBuffers.clear();
	}

	m_vertexBuffers.resize(textureCount);
	glGenBuffers(textureCount, m_vertexBuffers.data());

	checkErrorGL();
}

void GLImageView::mousePressEvent(QMouseEvent* event)
{
	Qt::MouseButton button=event->button();

	if(Qt::LeftButton == button)
		m_leftClick=event->globalPos();
	if(Qt::RightButton == button)
		m_rightClick=event->globalPos();
}

void GLImageView::mouseReleaseEvent(QMouseEvent* event)
{
	this->setCursor(QCursor(Qt::OpenHandCursor));
}

void GLImageView::mouseMoveEvent(QMouseEvent* event)
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

bool GLImageView::checkErrorGL()
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

GLuint GLImageView::LoadShaderFiles(const char * vertex_file_path, const char * fragment_file_path)
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

GLuint GLImageView::LoadShaders(std::string VertexShaderCode, std::string FragmentShaderCode)
{
    std::string error;

    if(!m_program.attachLoadAndCompileShaders(VertexShaderCode, FragmentShaderCode, error))
    {
        assert(false);
        return 0;
    }

    m_imagePosUniform=m_program.createUniformBuffer("ImagePos");

    m_imagePosUniform->uniform("window")=glm::ivec2(width(), height());
    m_imagePosUniform->uniform("image")=glm::ivec2(1, 1);
    m_imagePosUniform->uniform("center")=glm::vec2(1.0f, 1.0f);
    m_imagePosUniform->uniform("zoom")=1.0f;

    return m_program.id();
}

//GLuint GLImageView::LoadShaders(std::string VertexShaderCode, std::string FragmentShaderCode)
//{
//	GLint Result=GL_FALSE;
//	int InfoLogLength;
//
//	// Create the shaders
//	GLuint VertexShaderID=glCreateShader(GL_VERTEX_SHADER);
//	GLuint FragmentShaderID=glCreateShader(GL_FRAGMENT_SHADER);
//
//	// Compile Vertex Shader
//	//	printf("Compiling shader : %s\n", vertex_file_path);
//	char const * VertexSourcePointer=VertexShaderCode.c_str();
//	glShaderSource(VertexShaderID, 1, &VertexSourcePointer, NULL);
//	glCompileShader(VertexShaderID);
//
//	// Check Vertex Shader
//	glGetShaderiv(VertexShaderID, GL_COMPILE_STATUS, &Result);
//	glGetShaderiv(VertexShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
//	if(InfoLogLength > 0)
//	{
//		std::vector<char> VertexShaderErrorMessage(InfoLogLength+1);
//		glGetShaderInfoLog(VertexShaderID, InfoLogLength, NULL, &VertexShaderErrorMessage[0]);
//		//printf("%s\n", &VertexShaderErrorMessage[0]);
//		OutputDebugStringA((boost::format("%s\n")%VertexShaderErrorMessage[0]).str().c_str());
//		assert(Result);
//	}
//
//
//
//	// Compile Fragment Shader
//	//	printf("Compiling shader : %s\n", fragment_file_path);
//	char const * FragmentSourcePointer=FragmentShaderCode.c_str();
//	glShaderSource(FragmentShaderID, 1, &FragmentSourcePointer, NULL);
//	glCompileShader(FragmentShaderID);
//
//	// Check Fragment Shader
//	glGetShaderiv(FragmentShaderID, GL_COMPILE_STATUS, &Result);
//	glGetShaderiv(FragmentShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
//	if(InfoLogLength > 0)
//	{
//		std::vector<char> FragmentShaderErrorMessage(InfoLogLength+1);
//		glGetShaderInfoLog(FragmentShaderID, InfoLogLength, NULL, &FragmentShaderErrorMessage[0]);
//		OutputDebugStringA((boost::format("%s\n")%FragmentShaderErrorMessage[0]).str().c_str());
//		//		printf("%s\n", &FragmentShaderErrorMessage[0]);
//		assert(Result);
//	}
//
//
//
//	// Link the program
//	//	printf("Linking program\n");
//	GLuint ProgramID=glCreateProgram();
//	glAttachShader(ProgramID, VertexShaderID);
//	glAttachShader(ProgramID, FragmentShaderID);
//	glLinkProgram(ProgramID);
//
//	// Check the program
//	glGetProgramiv(ProgramID, GL_LINK_STATUS, &Result);
//	glGetProgramiv(ProgramID, GL_INFO_LOG_LENGTH, &InfoLogLength);
//	if(InfoLogLength > 0)
//	{
//		std::vector<char> ProgramErrorMessage(InfoLogLength+1);
//		glGetProgramInfoLog(ProgramID, InfoLogLength, NULL, &ProgramErrorMessage[0]);
//		OutputDebugStringA((boost::format("%s\n")%ProgramErrorMessage[0]).str().c_str());
//		//		printf("%s\n", &ProgramErrorMessage[0]);
//		assert(Result);
//	}
//
//	glDeleteShader(VertexShaderID);
//	glDeleteShader(FragmentShaderID);
//
//	return ProgramID;
//}

}//namespace Limitless