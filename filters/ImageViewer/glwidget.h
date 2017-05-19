#ifndef GLWIDGET_H
#define GLWIDGET_H

#include <GL/glew.h>

#include <QtOpenGL/QGLWidget>
#include <QtGui/QImage>
//#include <QtCore/QEvent>
//#include <boost/atomic.hpp>
//#include "Media/MediaSample.h"
#include "Media/IImageSample.h"
#include "Media/GPUImageSample.h"
#include "Media/GPUContext.h"

#include <boost/thread.hpp>
#include <boost/atomic.hpp>
#include <atomic>

#include <glm/glm.hpp>

class Vector2f
{
public:
	Vector2f():x(0.0), y(0.0){};
	Vector2f(float x, float y):x(x), y(y){};
	~Vector2f(){};

	float x, y;
};

class Vector3f
{
public:
	Vector3f():x(0.0), y(0.0), z(0.0){};
	Vector3f(float x, float y, float z):x(x), y(y), z(z){};
	~Vector3f(){};

	union
	{
		struct
		{
			float x, y, z;
		};
		struct
		{
			float r, g, b;
		};
	};
};

class Coord
{
public:
	Coord(){};
	Coord(float x, float y, float z, float tx, float ty){point=Vector3f(x, y, z);texture=Vector2f(tx, ty);}
	Coord(Vector3f point, Vector2f texture):point(point), texture(texture){};

	Vector3f point;
	Vector2f texture;
};

class Quad
{
public:
	Quad(){}
	Quad(Coord coord1, Coord coord2, Coord coord3, Coord coord4){coords[0]=coord1; coords[1]=coord2; coords[2]=coord3; coords[3]=coord4;}
	Quad(std::vector<Coord> coords)
	{
		for(int i=0; i<coords.size() && i<4; ++i) 
			coords[i]=coords[i];
	};
	~Quad(){};

	Coord coords[4];
};
typedef std::vector<Quad> Quads;


class GLWidget : public QGLWidget
{
	Q_OBJECT

public:
	GLWidget(QWidget *parent, GLWidget *masterWidget=NULL);
	GLWidget(QGLContext *context, QWidget *parent);
	~GLWidget();

	enum DisplayMode
	{
		SINGLE,
		MULTIPLE,
		CYLINDRICAL
	};

	bool initialize();
	void setDisplayMode(DisplayMode mode);

	void displaySample(Limitless::SharedMediaSample sample);

	void attachViewer(GLWidget *glViewer);
	void removeViewer(GLWidget *glViewer);
	void notify();

	void startThread();
	void stopThread();
	void drawThread();

	void draw();
protected:
	bool updateSample();
	void setCurrentSample(Limitless::SharedMediaSample sample);
	void setMasterWidget(GLWidget *master);
	GLuint LoadShaderFiles(const char * vertex_file_path, const char * fragment_file_path);
	GLuint LoadShaders(std::string VertexShaderCode, std::string FragmentShaderCode);

//	bool draw(bool useCurrent=true);

	bool event(QEvent *e);

	virtual void glInit();
	virtual void glDraw();

	virtual void initializeGL();
	void initOpenGlFunctions();
	virtual void resizeGL(int width, int height);
	virtual void paintGL();

	void resizeEvent(QResizeEvent *evt);
//	void paintEvent(QPaintEvent *);
	void closeEvent(QCloseEvent *evt);

	void getMainGLHandle();
	void getOpenglThread();

private:
	boost::mutex &getMutex();
	Limitless::SharedDrawCallback m_drawCallback;

	bool checkErrorGL();

	void setupDisplayMode(DisplayMode mode, const std::vector<QImage> &images);
//	void setupSingleImage(const QImage &image);
	void calculateSingleMediaPos();
	void calculateMultipleMediaPos();
	void setupleMultipleImages(const std::vector<QImage> &images);
	void setupCylindrical();

	void fitToScreen();

	void resizeMedia();
	void calculateMediaPos();

	void drawImage(std::vector<QImage> &images);
	void drawSingleImage(QImage &image);

	void allocateTextures(int textureCount);

	void mousePressEvent(QMouseEvent* event);
	void mouseReleaseEvent(QMouseEvent* event);
	void mouseMoveEvent(QMouseEvent* event);

    static bool m_glewInit;

	QThread *getQThread();

	QThread *m_processQThread;
	GLWidget *m_masterWidget;
	std::vector<GLWidget *> m_glViewers;
	QThread *m_openglThread;

	const QGLContext *m_qtMainGlContext;

	boost::thread m_thread;
	boost::condition_variable m_event;
	boost::mutex m_mutex;
	boost::atomic<bool> m_threadStarted;
	boost::condition_variable m_threadStartedEvent;
	boost::atomic<bool> m_endThread;
	boost::atomic<bool> m_suspendThread;
	boost::atomic<bool> m_isSuspended;
	bool m_contextMoved;
	boost::condition_variable m_threadEvent;
	
	std::atomic<bool> m_sampleInUse;
	Limitless::SharedMediaSample m_currentSample;
	Limitless::SharedGpuImageSample m_currentGpuImageSample;
	Limitless::SharedMediaSample m_newSample;
	bool m_textureLoaded;
	GLuint m_currentTexture;
	
//	boost::mutex m_viewerMutex;

	bool m_readyToInit;
	bool m_init;
	bool m_resize;
	bool m_calculateMedia;

	int m_mediaCount;
	int m_mediaWidth;
	int m_mediaHeight;
	int m_mediaGridWidth;
	int m_mediaGridHeight;
	int m_mediaGridX;
	int m_mediaGridY;

	DisplayMode m_displayMode;
//	boost::atomic<bool> m_displayModeChanged;
	bool m_displayModeChanged;
//	std::vector<QImage> m_images;
	std::vector<Limitless::SharedIImageSample> m_images;
	int m_currentImage;
	

	Quads m_imageQuads;

	GLenum m_textureType;
	std::vector<unsigned int> m_textures;

	size_t m_imageInterfaceSampleId;
	size_t m_imageSampleSetInterfaceId;
	size_t m_imageSampleId;
	size_t m_imageSetSampleId;
	size_t m_gpuImageSampleId;
//	size_t m_GPUImageSampleSetId;
//	size_t m_GPUUploadSampleId;
//	size_t m_GPUImageGLSampleId;

	float m_fovX;
	float m_fovY;
	float m_clipNear;
	float m_clipFar;

	float m_imageQuadWidth;
	float m_imageQuadHeight;

	size_t m_textureWidth;
	size_t m_textureHeight;

	QPoint m_leftClick;
	QPoint m_rightClick;

	float m_transX;
	float m_transY;
	float m_transZ;
	float m_rotX;
	float m_rotY;
	float m_rotZ;

	GLuint m_programID;
	GLuint m_programOverlayID;
	GLuint m_textureSamplerID;
	GLuint m_textureID;
	GLuint m_matrixID;
	GLuint m_vertexArrayID;
	GLuint m_vertexBuffer;
	
	GLuint m_texCoordBuffer;
	glm::mat4 m_modelViewPerspective;

	std::vector<GLuint> m_vertexArrayIDs;
	std::vector<GLuint> m_vertexBuffers;

	std::vector<GLfloat> m_vertexBufferData;
	std::vector<std::vector<GLfloat>> m_vertexBuffersData;
	std::vector<GLfloat> m_texCoordBufferData;

	GLuint m_overlayBuffer;
	std::vector<GLfloat> m_overlayBufferData;
	bool m_updateOverlayBuffer;

//opengl functions
	PFNGLCOMPILESHADERPROC _glCompileShader;
	PFNGLCREATESHADERPROC _glCreateShader;
	PFNGLSHADERSOURCEPROC _glShaderSource;
	PFNGLGETSHADERIVPROC _glGetShaderiv;
	PFNGLCREATEPROGRAMPROC _glCreateProgram;
	PFNGLATTACHSHADERPROC _glAttachShader;
	PFNGLLINKPROGRAMPROC _glLinkProgram;
	PFNGLGETPROGRAMIVPROC _glGetProgramiv;
	PFNGLDELETESHADERPROC _glDeleteShader;
	PFNGLGETUNIFORMLOCATIONPROC _glGetUniformLocation;
	PFNGLGENVERTEXARRAYSPROC _glGenVertexArrays;
	PFNGLBINDVERTEXARRAYPROC _glBindVertexArray;
	PFNGLGENBUFFERSPROC _glGenBuffers;
	PFNGLBINDBUFFERPROC _glBindBuffer;
	PFNGLBUFFERDATAPROC _glBufferData;
	PFNGLACTIVETEXTUREPROC _glActiveTexture;
	PFNGLUSEPROGRAMPROC _glUseProgram;
	PFNGLUNIFORM1IPROC _glUniform1i;
	PFNGLENABLEVERTEXATTRIBARRAYPROC _glEnableVertexAttribArray;
	PFNGLVERTEXATTRIBPOINTERPROC _glVertexAttribPointer;
	PFNGLDISABLEVERTEXATTRIBARRAYPROC _glDisableVertexAttribArray;
};

typedef std::vector<GLWidget *> GLWidgets;

//GLuint LoadShaderFiles(const char * vertex_file_path, const char * fragment_file_path);
//GLuint LoadShaders(std::string VertexShaderCode, std::string FragmentShaderCode);

#endif // GLWIDGET_H
