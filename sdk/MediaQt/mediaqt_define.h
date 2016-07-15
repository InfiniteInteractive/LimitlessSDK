#ifndef MEDIAQT_DEFINE_H
#define MEDIAQT_DEFINE_H

#ifdef MediaQt_EXPORTS
# define MEDIAQT_EXPORT __declspec(dllexport)
#else
# define MEDIAQT_EXPORT __declspec(dllimport)
#endif

#endif // MEDIAQT_DEFINE_H
