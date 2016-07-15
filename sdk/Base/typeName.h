#ifndef _Limitless_TypeName_h_
#define _Limitless_TypeName_h_

#include <string>

namespace Limitless
{

template <typename _Class> 
struct TypeName
{	
	static std::string get()
	{
		std::string fullName=typeid(_Class).name();

		size_t beginPos=0;
		size_t classPos=fullName.find("class");
		size_t nameSpacePos=fullName.find_last_of("::");

		if(classPos != std::string::npos)
			beginPos=classPos+6;
		if(nameSpacePos != std::string::npos)
		{
			if(nameSpacePos+1 > beginPos)
				beginPos=nameSpacePos+1;
		}
		return fullName.substr(beginPos);
	}
};


}//namespace Limitless

#endif //_Limitless_TypeName_h_