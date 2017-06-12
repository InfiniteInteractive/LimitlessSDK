#ifndef _GLImageView_h_
#define _GLImageView_h_

#include "qtcomponents_define.h"

#ifndef Q_MOC_RUN
#include <GL/glew.h>
#endif //Q_MOC_RUN

#include <QtOpenGL/QGLWidget>
#include <QtGui/QImage>
//#include <QtCore/QEvent>
//#include <boost/atomic.hpp>
//#include "Media/MediaSample.h"

#ifndef Q_MOC_RUN
#include "opengl_util/program.h"
#include "Media/IImageSample.h"
#include "Media/GPUImageSample.h"
#include "Media/GPUContext.h"

#include <boost/thread.hpp>
#include <boost/atomic.hpp>
#include <atomic>

#include <glm/glm.hpp>
#endif //Q_MOC_RUN

namespace Limitless
{

class AxisAlignedQuad
{
public:
    AxisAlignedQuad():verticies(6), textureCoords(6)
    {
        glGenBuffers(1, &vertexBuffer);
    }
    ~AxisAlignedQuad()
    {
        glDeleteBuffers(1, &vertexBuffer);
    };

    GLuint vertexBuffer;

    glm::vec2 start;
    glm::vec2 end;
    std::vector<glm::vec2> verticies;
    std::vector<glm::vec2> textureCoords;
};
typedef std::vector<AxisAlignedQuad> AxisAlignedQuads;


class QTCOMPONENTS_EXPORT GLImageView: public QGLWidget
{
    Q_OBJECT

public:
    GLImageView(QWidget *parent, GLImageView *masterWidget=NULL, bool manualDraw=false);
    GLImageView(QGLContext *context, QWidget *parent, bool manualDraw=false);
    ~GLImageView();

    enum DisplayMode
    {
        SINGLE,
        MULTIPLE,
        CYLINDRICAL
    };

    bool initialize();
    void setDisplayMode(DisplayMode mode);

    void displaySample(Limitless::SharedMediaSample sample);
    void notify();

    void draw();

    void fitToScreen();
    void fitOneToOne();

protected:
    virtual void drawOverlay() {};
    virtual void onUpdateZoom(float zoom) {};

    bool updateSample();
    void setCurrentSample(Limitless::SharedMediaSample sample);
    GLuint LoadShaderFiles(const char * vertex_file_path, const char * fragment_file_path);
    GLuint LoadShaders(std::string VertexShaderCode, std::string FragmentShaderCode);

    bool event(QEvent *e);

    virtual void glInit();
    virtual void glDraw();

    virtual void initializeGL();
    virtual void resizeGL(int width, int height);
    virtual void paintGL();

    virtual void mousePressEvent(QMouseEvent* event);
    virtual void mouseReleaseEvent(QMouseEvent* event);
    virtual void mouseMoveEvent(QMouseEvent* event);
    virtual void wheelEvent(QWheelEvent *event);

    void resizeEvent(QResizeEvent *evt);
    //	void paintEvent(QPaintEvent *);
    void closeEvent(QCloseEvent *evt);

    void getMainGLHandle();
//    void getOpenglThread();

//    const glm::mat4 &modelView() { return m_modelViewPerspective; }

    glm::ivec2 getMediaPos(glm::ivec2 windowPos);
    glm::vec2 getNormalizedMediaPos(glm::ivec2 mediaPos);
    glm::ivec2 getWindowPos(glm::ivec2 mediaPos);

    bool checkErrorGL();

private:
//    static bool m_glewInit;
//    bool m_glewInit;
    bool m_manualDraw;
//    boost::mutex &getMutex();
    Limitless::SharedDrawCallback m_drawCallback;

//    void setupDisplayMode(DisplayMode mode, const std::vector<QImage> &images);
//    //	void setupSingleImage(const QImage &image);
//    void calculateSingleMediaPos();
//    void calculateMultipleMediaPos();
//    void setupleMultipleImages(const std::vector<QImage> &images);
//    void setupCylindrical();

//    void fitToScreen();
    void resizeMedia();
    void calculateMediaPos();

    void allocateTextures(int textureCount);
//    QThread *getQThread();
//
//    QThread *m_processQThread;
//    GLImageView *m_masterWidget;
//    std::vector<GLImageView *> m_glViewers;
//    QThread *m_openglThread;
//
//    const QGLContext *m_qtMainGlContext;
//
//    boost::thread m_thread;
//    boost::condition_variable m_event;
//    boost::mutex m_mutex;
//    boost::atomic<bool> m_threadStarted;
//    boost::condition_variable m_threadStartedEvent;
//    boost::atomic<bool> m_endThread;
//    boost::atomic<bool> m_suspendThread;
//    boost::atomic<bool> m_isSuspended;
//    bool m_contextMoved;
//    boost::condition_variable m_threadEvent;

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


    static unsigned int invalidTexture;
    GLenum m_textureType;
    std::vector<unsigned int> m_textures;
    unsigned int m_texture;

    size_t m_imageInterfaceSampleId;
    size_t m_imageSampleSetInterfaceId;
    size_t m_imageSampleId;
    size_t m_imageSetSampleId;
    size_t m_gpuImageSampleId;
    //	size_t m_GPUImageSampleSetId;
    //	size_t m_GPUUploadSampleId;
    //	size_t m_GPUImageGLSampleId;

    int m_currentImageWidth;
    int m_currentImageHeight;
    float m_zoom;
    float m_centerPosX;
    float m_centerPosY;

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

    GLuint m_logoTextureID;
    size_t m_logoWidth;
    size_t m_logoHeight;

    opengl_util::Program m_program;
    opengl_util::UniformBuffer *m_imagePosUniform;
    GLuint m_programID;
    GLuint m_programOverlayID;
    GLuint m_textureSamplerID;
    GLuint m_textureID;
    GLuint m_matrixID;
    GLuint m_vertexArrayID;
    GLuint m_vertexBuffer;

    GLuint m_texCoordBuffer;
    glm::mat4 m_modelViewPerspective;

    AxisAlignedQuads m_imageQuads;

    std::vector<GLuint> m_vertexArrayIDs;
    std::vector<GLuint> m_vertexBuffers;

    std::vector<GLfloat> m_vertexBufferData;
    std::vector<std::vector<GLfloat>> m_vertexBuffersData;
    std::vector<GLfloat> m_texCoordBufferData;

    GLuint m_overlayBuffer;
    std::vector<GLfloat> m_overlayBufferData;
    bool m_updateOverlayBuffer;

//    //opengl functions
//    PFNGLCOMPILESHADERPROC _glCompileShader;
//    PFNGLCREATESHADERPROC _glCreateShader;
//    PFNGLSHADERSOURCEPROC _glShaderSource;
//    PFNGLGETSHADERIVPROC _glGetShaderiv;
//    PFNGLCREATEPROGRAMPROC _glCreateProgram;
//    PFNGLATTACHSHADERPROC _glAttachShader;
//    PFNGLLINKPROGRAMPROC _glLinkProgram;
//    PFNGLGETPROGRAMIVPROC _glGetProgramiv;
//    PFNGLDELETESHADERPROC _glDeleteShader;
//    PFNGLGETUNIFORMLOCATIONPROC _glGetUniformLocation;
//    PFNGLGENVERTEXARRAYSPROC _glGenVertexArrays;
//    PFNGLBINDVERTEXARRAYPROC _glBindVertexArray;
//    PFNGLGENBUFFERSPROC _glGenBuffers;
//    PFNGLBINDBUFFERPROC _glBindBuffer;
//    PFNGLBUFFERDATAPROC _glBufferData;
//    PFNGLACTIVETEXTUREPROC _glActiveTexture;
//    PFNGLUSEPROGRAMPROC _glUseProgram;
//    PFNGLUNIFORM1IPROC _glUniform1i;
//    PFNGLENABLEVERTEXATTRIBARRAYPROC _glEnableVertexAttribArray;
//    PFNGLVERTEXATTRIBPOINTERPROC _glVertexAttribPointer;
//    PFNGLDISABLEVERTEXATTRIBARRAYPROC _glDisableVertexAttribArray;
};

typedef std::vector<GLImageView *> GLImageViews;

} //namesapce Limitless

//GLuint LoadShaderFiles(const char * vertex_file_path, const char * fragment_file_path);
//GLuint LoadShaders(std::string VertexShaderCode, std::string FragmentShaderCode);

#endif //_GLImageView_h_
