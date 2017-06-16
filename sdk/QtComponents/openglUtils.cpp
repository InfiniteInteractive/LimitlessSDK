#include "openglUtils.h"

#include "Media/GPUContext.h"
#include <QtPlatformHeaders/QWGLNativeContext>

namespace Limitless
{

QOpenGLContext *getNativeQOpenGLContext()
{
    QOpenGLContext *nativeOpenglContext=new QOpenGLContext();
    QWGLNativeContext nativeContext((HGLRC)Limitless::GPUContext::openGLContext(), (HWND)Limitless::GPUContext::nativeWindow());

    nativeOpenglContext->setNativeHandle(QVariant::fromValue(nativeContext));
    nativeOpenglContext->create();

    return nativeOpenglContext;
}


QGLContext *createSharedGLContext(QOpenGLContext *glContext)
{
    QOpenGLContext *openglContext=new QOpenGLContext();

    openglContext->setShareContext(glContext);
    openglContext->setFormat(glContext->format());
    openglContext->create();

    return QGLContext::fromOpenGLContext(openglContext);
}

}//namespace Limitless
