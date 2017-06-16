#ifndef _limitless_openglUtils_h_
#define _limitless_openglUtils_h_

#include "qtcomponents_define.h"

#include <QtGui/QOpenGlContext>
#include <QtOpenGL/QGlContext>

namespace Limitless
{

QTCOMPONENTS_EXPORT QOpenGLContext *getNativeQOpenGLContext();
QTCOMPONENTS_EXPORT QGLContext *createSharedGLContext(QOpenGLContext *glContext);

}//namespace Limitless

#endif // _limitless_openglUtils_h_
