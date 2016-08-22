#ifndef _medialib_accumulatorType_h_
#define _medialib_accumulatorType_h_

#include <stdint.h>

namespace medialib
{

template<typename _Type>
struct accumulatorType
{
	typedef _Type Type;
};

template<>
struct accumulatorType<uint8_t>
{
	typedef uint16_t Type;
};

template<>
struct accumulatorType<int16_t>
{
	typedef int32_t Type;
};

template<>
struct accumulatorType<float>
{
	typedef double Type;
};

template<>
struct accumulatorType<double>
{
	typedef double Type;
};

}//namespace medialib

#endif //_medialib_accumulatorType_h_

