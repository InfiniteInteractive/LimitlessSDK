#ifndef _cvlib_define_h_
#define _cvlib_define_h_

#ifdef cvlib_EXPORTS
# define cvlib_EXPORT __declspec(dllexport)
#else
# define cvlib_EXPORT __declspec(dllimport)
#endif

#endif // _cvlib_define_h_
