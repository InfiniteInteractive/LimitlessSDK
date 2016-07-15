#ifndef _SampleType_h_
#define _SampleType_h_

#include "Media/media_define.h"

#include <vector>

namespace Limitless
{
struct SampleType
{
	SampleType():id(-1), name("") {}

	unsigned int id;
	std::vector<unsigned int> idInheritance;
	std::string name;
	std::vector<unsigned int> nameInheritance;
};

}//namespace Limitless

#endif //_SampleType_h_