#ifndef BASE_DEFINE_H
#define BASE_DEFINE_H

#ifdef Base_EXPORTS
# define BASE_EXPORT __declspec(dllexport)
#else
# define BASE_EXPORT __declspec(dllimport)
#endif

#endif // BASE_DEFINE_H
