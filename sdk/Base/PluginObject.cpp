#include "PluginObject.h"

using namespace Limitless;
//bool PluginObject::hasAttribute(const std::string &name)
//{
//	return m_attributes.exists(name);
//}
//
//SharedAttribute PluginObject::attribute(const std::string &name)
//{
//	SharedAttribute attribute;
//
//	attribute=m_attributes.find(name);
//	if(attribute != SharedAttribute())
//		return m_attributes[name];
//	return SharedAttribute();
//}
//
//void PluginObject::setAttribute(std::string name, std::string value)
//{
//	SharedAttribute attribute;
//
//	attribute=m_attributes.find(name);
//	if(attribute != SharedAttribute())
//	{
//		attribute->fromString(value);
//		onAttributeChanged(name, attribute);
//	}
//}
//
//void PluginObject::setAttribute(std::string name, std::string value, const Strings &values)
//{
//	SharedAttribute attribute;
//
//	attribute=m_attributes.find(name);
//	if(attribute != SharedAttribute())
//	{
//		SharedAttributeStringEnum stringEnum=boost::dynamic_pointer_cast<AttributeStringEnum>(attribute);
//
//		if(stringEnum != SharedAttributeStringEnum())
//		{
//			stringEnum->fromString(value);
//			stringEnum->setEnums(values);
//			onAttributeChanged(name, attribute);
//		}
//	}
//}
//
//void PluginObject::addAttribute(std::string name, bool value)
//{
//	m_attributes[name].reset(new AttributeBool(name, value));
//}
//
//void PluginObject::addAttribute(std::string name, int value)
//{
//	m_attributes[name].reset(new AttributeInt(name, value));
//}
//
//void PluginObject::addAttribute(std::string name, __int64 value)
//{
//	m_attributes[name].reset(new AttributeInt64(name, value));
//}
//
//void PluginObject::addAttribute(std::string name, float value)
//{
//	m_attributes[name].reset(new AttributeFloat(name, value));
//}
//
//void PluginObject::addAttribute(std::string name, double value)
//{
//	m_attributes[name].reset(new AttributeDouble(name, value));
//}
//
//void PluginObject::addAttribute(std::string name, char *value)
//{
//	m_attributes[name].reset(new AttributeString(name, value));
//}
//
//void PluginObject::addAttribute(std::string name, std::string value)
//{
//	m_attributes[name].reset(new AttributeString(name, value));
//}
//
//void PluginObject::addAttribute(std::string name, std::string value, const Strings &values)
//{
//	m_attributes[name].reset(new AttributeStringEnum(name, value, values));
//}

void PluginObject::serialize(Serializer *serializer)
{
	serializer->startObject();

	serializer->addKey("type");
	serializer->addString(typeName());
	serializer->addKey("instance");
	serializer->addString(instance());

	AttributeContainer::serialize(serializer);

	serializer->endObject();
}

void PluginObject::unserialize(Unserializer *unserializer)
{
	if(unserializer->key("attributes"))
		AttributeContainer::unserialize(unserializer);
}