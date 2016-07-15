#ifndef _medialib_define_h_
#define _medialib_define_h_

#ifdef medialib_EXPORTS
# define medialib_EXPORT __declspec(dllexport)
#else
# define medialib_EXPORT __declspec(dllimport)
#endif

#endif // _medialib_define_h_
