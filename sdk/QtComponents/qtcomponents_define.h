#ifndef QTCOMPONENTS_DEFINE_H
#define QTCOMPONENTS_DEFINE_H

#ifdef QtComponents_EXPORTS
# define QTCOMPONENTS_EXPORT __declspec(dllexport)
#else
# define QTCOMPONENTS_EXPORT __declspec(dllimport)
#endif

#endif // QTCOMPONENTS_DEFINE_H
