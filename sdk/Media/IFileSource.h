#ifndef _Limitless_IFileSource_h_
#define _Limitless_IFileSource_h_

#include "Media/media_define.h"

#include "Base/common.h"
#include "Media/IMediaSource.h"

namespace Limitless
{

class MEDIA_EXPORT IFileSource:public IMediaSource
{
public:
	IFileSource(std::string name, SharedMediaFilter parent):IMediaSource(name, parent){m_baseClasses.push_back("IFileSource");}
	virtual ~IFileSource(){};

	enum FileSourceType
	{
		SINGLE_FILE,
		MULTI_FILE,
		DIRECTORY
	};

	virtual FileSourceType sourceType()=0;
	virtual Strings sourceTypes()=0;
	
	virtual void open(std::string source)=0;
	virtual void close()=0;

	virtual void seek(int position)=0;
//	virtual bool readSample(SharedMediaSample sample)=0;
};

}//namespace Limitless

#endif //_Limitless_IFileSource_h_