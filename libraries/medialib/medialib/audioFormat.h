#ifndef _medialib_audioFormat_h_
#define _medialib_audioFormat_h_

#include "medialib/medialibDefine.h"

namespace medialib
{

enum class AudioFormat
{
    Unknown=0,
    UInt8=1, ///< enum value unsigned 8 bit sample
    Int16=2, ///< enum value 16 bit sample
    Int32=3, ///< enum value 32 bit sample
    Float=4, ///< enum value float sample
    Double=5, ///< enum value double sample
 //Planar
    UInt8P=6, ///< enum value unsigned 8 bit sample, planar
    Int16P=7, ///< enum value 16 bit sample, planar
    Int32P=8, ///< enum value 32 bit sample, planar
    FloatP=9, ///< enum value float sample, planar
    DoubleP=10 ///< enum value double sample, planar
};

//switch()
//{
//case AudioFormat::UInt8:
//	break;
//case AudioFormat::Int16:
//	break;
//case AudioFormat::Int32:
//	break;
//case AudioFormat::Float:
//	break;
//case AudioFormat::Double:
//	break;
//case AudioFormat::UInt8P:
//	break;
//case AudioFormat::Int16P:
//	break;
//case AudioFormat::Int32P:
//	break;
//case AudioFormat::FloatP:
//	break;
//case AudioFormat::DoubleP:
//	break;
//}

}
#endif //_medialib_audioFormat_h_
