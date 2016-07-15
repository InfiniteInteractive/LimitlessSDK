#ifndef _IIMSDK_PluginObject_h_
#define _IIMSDK_PluginObject_h_

#include "Base/base_define.h"
#include <vector>
#include "Base/Attributes.h"
#include "Base/PluginView.h"
#include "Base/AttributeContainer.h"

#pragma warning(push)
#pragma warning(disable:4251)

namespace Limitless
{

class BASE_EXPORT PluginObject:public AttributeContainer
{
public:
	PluginObject(std::string instance):m_instance(instance){m_baseClasses.push_back("PluginObject");};
	virtual ~PluginObject(){};

	virtual bool initialize(const Attributes &attributes)=0;
	virtual bool shutdown()=0;

	std::string instance(){return m_instance;}
//	std::string type(){return m_type;}
	virtual std::string typeName(){return std::string("Unknown");}
	bool isType(std::string type){return (std::find(m_baseClasses.begin(), m_baseClasses.end(), type) != m_baseClasses.end());}

	virtual SharedPluginView getView() { return SharedPluginView(); }
	virtual SharedPluginView getAttributeView(){return SharedPluginView();}

	virtual void serialize(Serializer *serializer);
	virtual void unserialize(Unserializer *unserializer);

//	bool hasAttribute(const std::string &name);
//	SharedAttribute attribute(const std::string &name);
//	void setAttribute(std::string name, std::string value);
//	void setAttribute(std::string name, std::string value, const Strings &values);
//	Attributes &attributes(){return m_attributes;}
//
protected:
//	void addAttribute(std::string name, bool value);
//	void addAttribute(std::string name, int value);
//	void addAttribute(std::string name, __int64 value);
//	void addAttribute(std::string name, float value);
//	void addAttribute(std::string name, double value);
//	void addAttribute(std::string name, char *value);
//	void addAttribute(std::string name, std::string value);
//	void addAttribute(std::string name, std::string value, const Strings &values);
//	
//	virtual void onAttributeChanged(std::string name, SharedAttribute attribute){};

	std::string m_type;
	std::string m_instance;
	std::vector<std::string> m_baseClasses;
//	Attributes m_attributes;
};

}//namespace Limitless

#pragma warning(pop)

#endif //_IIMSDK_PluginObject_h_