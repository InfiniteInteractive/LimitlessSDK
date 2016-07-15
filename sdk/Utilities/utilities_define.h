#ifndef UTILITIES_DEFINE_H
#define UTILITIES_DEFINE_H

#ifdef Utilities_EXPORTS
# define UTILITIES_EXPORT __declspec(dllexport)
#else
# define UTILITIES_EXPORT __declspec(dllimport)
#endif

#endif // UTILITIES_DEFINE_H
