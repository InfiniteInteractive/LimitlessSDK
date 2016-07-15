#ifndef GRAPHVIEW_GLOBAL_H
#define GRAPHVIEW_GLOBAL_H

#include <QtCore/qglobal.h>

#ifdef GRAPHVIEW_LIB
# define GRAPHVIEW_EXPORT Q_DECL_EXPORT
#else
# define GRAPHVIEW_EXPORT Q_DECL_IMPORT
#endif

#endif // GRAPHVIEW_GLOBAL_H
