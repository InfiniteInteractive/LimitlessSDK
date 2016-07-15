#ifndef _Limitless_MediaFormat_h_
#define _Limitless_MediaFormat_h_

#include "Media/media_define.h"

#include "Base/Attributes.h"
#include "Base/AttributeContainer.h"
#include "Media/MediaFormat.h"
#include "boost/shared_ptr.hpp"
#include <boost/unordered_map.hpp>
#include <boost/format.hpp>
#include <vector>

namespace Limitless
{

class MEDIA_EXPORT MediaAttribute:public AttributeString
{
public:
	enum Flags
	{
		NONE,
		OPTIONAL_FLAG
	};

	MediaAttribute(IChangeNotify *parent, std::string name, std::string value, Flags flags=NONE);
//	MediaAttribute(SharedAttributeString attribute, Flags flags=NONE);
	virtual ~MediaAttribute(){};

//	bool operator==(const MediaAttribute &mediaAttribute) const{return (*m_attribute == *mediaAttribute.m_attribute);}
	bool operator==(const MediaAttribute &mediaAttribute) const{return true;}

//	std::string name() const{return m_attribute->name();}
//
//	std::string toString() const{return m_attribute->toString();}
//	int toInt() const{return m_attribute->toInt();}
//	float toFloat() const{return m_attribute->toFloat();}
//	bool toBool() const{return m_attribute->toBool();}
//
	bool optional() const{return ((m_flags&OPTIONAL_FLAG) > 0);}
	void setOptional(){m_flags|=OPTIONAL_FLAG;}
	Flags flags() const{return (Flags)m_flags;}
	void setFlags(Flags flags){m_flags=(int)flags;}

private:
//	SharedAttribute m_attribute;
	int m_flags;

};
typedef boost::shared_ptr<MediaAttribute> SharedMediaAttribute;
typedef boost::unordered_map<std::string, SharedMediaAttribute> SharedMediaAttributes;

class MediaFormat;
typedef boost::shared_ptr<MediaFormat> SharedMediaFormat;

class MEDIA_EXPORT MediaFormat:public AttributeContainer
{
public:
	MediaFormat(){};
	virtual ~MediaFormat(){};

	void addMediaAttribute(SharedMediaAttribute mediaAttribute);
	void addMediaAttributes(std::string jsonAttributes);
    void addMediaAttributes(Unserializer *unSerializer);
//	void addMediaAttributes(QVariant *jsonVariant);
//	bool exists(std::string name) const;
//	SharedMediaAttribute attribute(std::string name) const;
//	
//	void setAttribute(std::string name, std::string value);
//	MediaAttribute &MediaFormat::attribute(std::string name);

//	Strings attributeKeys() const;

	bool match(SharedMediaFormat mediaFormat);
	bool operator==(const MediaFormat &mediaFormat) const;
	
	bool isVideo() { return (attribute("mime")->toString().compare(0, 5, "video") == 0); }
	bool isAudio() { return (attribute("mime")->toString().compare(0, 5, "audio") == 0); }

private:
//	SharedMediaAttributes m_mediaAttributes;
};

typedef std::vector<SharedMediaFormat> SharedMediaFormats;

}//namespace Limitless

#ifdef _DEBUG
#define WINDOWS_LEAN_AND_MEAN
#include "windows.h"
#define DEBUG_MEDIA_FORMAT(FORMAT) {std::vector<std::string> keys=FORMAT->keys();for(std::vector<std::string>::iterator iter=keys.begin(); iter!=keys.end(); ++iter){OutputDebugStringA((LPCSTR)(boost::format("%s - %s\n")%*iter%FORMAT->attribute(*iter)->toString()).str().c_str());}}
#else
#define DEBUG_MEDIA_FORMAT(MAP) 0
#endif

#endif //_Limitless_MediaFormat_h_
