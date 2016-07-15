#ifndef _Limitless_Define_h_
#define _Limitless_Define_h_

#ifdef Media_EXPORTS
# define MEDIA_EXPORT __declspec(dllexport)
#else
# define MEDIA_EXPORT __declspec(dllimport)
#endif

#endif // _Limitless_Define_h_
