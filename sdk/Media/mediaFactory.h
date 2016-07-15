#ifndef _Limitless_Media_MediaFactory_h_
#define _Limitless_Media_MediaFactory_h_

#include "Media/media_define.h"
#include "Media/IMedia.h"

//#include <boost/shared_ptr.hpp>
//#include <boost/unordered_map.hpp>
#include <string>
#include <vector>
#include <map>
#include <boost/algorithm/string.hpp> 

namespace Limitless
{

template <typename T> struct TypeName
{	
	static std::string get()
	{
		const char *fullName=typeid(T).name();
		const char *name=strstr(fullName, "class");
		return (name)? name+6 : fullName;
	}
};

template<typename CLASS, typename INTERFACE>
class AutoRegisterMedia:public INTERFACE
{
public:
	AutoRegisterMedia():INTERFACE(){&s_typeName;}

	static IMedia *create(){return dynamic_cast<IMedia *>(new CLASS());}
	virtual std::string typeName(){return s_typeName;}
	static std::vector<std::string> getFileExtensions(){return CLASS::fileExtensions();}

private:
	static std::string s_typeName;
};

class MEDIA_EXPORT MediaFactory
{
public:
	typedef IMedia *(*MediaFactoryMethod)();
	typedef std::map<std::string, MediaFactoryMethod> MediaFactoryMethods;
	typedef std::multimap<std::string, std::string> MediaExtensions;
	typedef std::pair<std::string, std::string> MediaExtensionPair;
	typedef std::pair<MediaExtensions::iterator, MediaExtensions::iterator> MediaExtensionsRange;
	
	static MediaFactory &instance(){if(s_instance == nullptr) s_instance=new MediaFactory(); return *s_instance;}
	SharedMedia create(std::string typeName)
	{
		MediaFactoryMethods::iterator iter=m_mediaMap.find(typeName);

		SharedMedia media;

		if(iter != m_mediaMap.end())
			media.reset(iter->second());
		return media;
	}

	std::vector<std::string> names()
	{
		std::vector<std::string> names;

		for(MediaFactoryMethods::iterator iter=m_mediaMap.begin(); iter!=m_mediaMap.end(); ++iter)
			names.push_back(iter->first);
		return names;
	}

	std::string registerMedia(std::string typeName, MediaFactoryMethod mediaFunc, std::vector<std::string> extentions)
	{
		m_mediaMap[typeName]=mediaFunc;

		for(size_t i=0; i<extentions.size(); ++i)
		{
			boost::algorithm::to_lower(extentions[i]);
			m_mediaExtensionMap.insert(MediaExtensionPair(extentions[i], typeName));
		}
		return typeName;
	}

	std::vector<std::string> extensions(std::string typeName)
	{
		std::vector<std::string> mediaExtensions;

		for(MediaExtensions::iterator iter=m_mediaExtensionMap.begin(); iter!=m_mediaExtensionMap.end(); ++iter)
		{
			if(iter->second == typeName)
				mediaExtensions.push_back(iter->first);
		}

		return mediaExtensions;
	}

	std::vector<std::string> handlesExtension(std::string extension)
	{
		boost::algorithm::to_lower(extension);

		MediaExtensionsRange range=m_mediaExtensionMap.equal_range(extension);
		std::vector<std::string> mediaNames;

		for(MediaExtensions::iterator iter=range.first; iter!=range.second; ++iter)
			mediaNames.push_back(iter->second);
		return mediaNames;
	}

private:
	MediaFactory(){};

	static MediaFactory *s_instance;
	MediaFactoryMethods m_mediaMap;
	MediaExtensions m_mediaExtensionMap;
};

template<typename CLASS, typename INTERFACE> std::string AutoRegisterMedia<CLASS, INTERFACE>::s_typeName=\
	MediaFactory::instance().registerMedia(TypeName<CLASS>::get(), &AutoRegisterMedia<CLASS, INTERFACE>::create, AutoRegisterMedia<CLASS, INTERFACE>::getFileExtensions());


} //Limitless

#endif // _Limitless_Media_MediaFactory_h_
