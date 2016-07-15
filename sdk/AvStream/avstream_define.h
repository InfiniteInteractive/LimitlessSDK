#ifndef _avstream_define_h_
#define _avstream_define_h_



#ifdef AvStream_EXPORTS
# define AVSTREAM_EXPORT __declspec(dllexport)
#else
# define AVSTREAM_EXPORT __declspec(dllimport)
#endif

#endif // _avstream_define_h_
