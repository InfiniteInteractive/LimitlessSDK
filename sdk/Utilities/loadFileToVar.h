#ifndef _utilities_loadFilteToVar_h_
#define _utilities_loadFilteToVar_h_

#include "utilities_define.h"
#include <string>

namespace Limitless
{
	UTILITIES_EXPORT std::string loadFileToString(std::string path, std::string fileName);
	UTILITIES_EXPORT std::string loadResource(std::string externVariable, std::string fileName);
}//namespace Limitless

#endif //_utilities_loadFilteToVar_h_