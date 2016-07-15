#include "AttributeContainer.h"
#include "Attribute.h"

namespace Limitless
{
//bool AttributeContainer::hasAttribute(const std::string &name)
//{
//	return m_attributes.exists(name);
//}
AttributeContainer::AttributeContainer(const AttributeContainer &container)
{
    std::vector<std::string> keys=container.keys();

    for(size_t i=0; i<keys.size(); ++i)
    {
        addAttribute(container.attribute(keys[i]));
    }
}

std::vector<std::string> AttributeContainer::keys() const
{
    return m_attributes.keys();
}

bool AttributeContainer::exists(const std::string &name) const
{ return m_attributes.exists(name); }

SharedAttribute AttributeContainer::attribute(const std::string &name) const
{
    SharedAttribute attribute;

    attribute=m_attributes.find(name);
    if(attribute!=SharedAttribute())
        return m_attributes.at(name);
    return SharedAttribute();
}

//void AttributeContainer::setAttribute(std::string name, std::string value)
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
//void AttributeContainer::setAttribute(std::string name, std::string value, const Strings &values)
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

void AttributeContainer::addAttribute(SharedAttribute attribute)
{
    Attribute *newAttribute=NULL;
    switch(attribute->type())
    {
    case Type::BOOL:
        newAttribute=new AttributeBool(this, *(AttributeBool *)attribute.get());
        break;
    case Type::INT:
        newAttribute=new AttributeInt(this, *(AttributeInt *)attribute.get());
        break;
    case Type::UINT:
        newAttribute=new AttributeUInt(this, *(AttributeUInt *)attribute.get());
        break;
    case Type::INT64:
        newAttribute=new AttributeInt64(this, *(AttributeInt64 *)attribute.get());
        break;
    case Type::UINT64:
        newAttribute=new AttributeUInt64(this, *(AttributeUInt64 *)attribute.get());
        break;
    case Type::FLOAT:
        newAttribute=new AttributeFloat(this, *(AttributeFloat *)attribute.get());
        break;
    case Type::DOUBLE:
        newAttribute=new AttributeDouble(this, *(AttributeDouble *)attribute.get());
        break;
    case Type::STRING:
        newAttribute=new AttributeString(this, *(AttributeString *)attribute.get());
        break;
    case Type::STRING_ENUM:
        newAttribute=new AttributeStringEnum(this, *(AttributeStringEnum *)attribute.get());
        break;
    }

    if(newAttribute!=NULL)
        m_attributes[attribute->name()].reset(newAttribute);
}

//void AttributeContainer::addAttribute(std::string name, bool value)
//{
//	m_attributes[name].reset(new AttributeBool(name, value));
//}
//
//void AttributeContainer::addAttribute(std::string name, int value)
//{
//	m_attributes[name].reset(new AttributeInt(name, value));
//}
//
//void AttributeContainer::addAttribute(std::string name, __int64 value)
//{
//	m_attributes[name].reset(new AttributeInt64(name, value));
//}
//
//void AttributeContainer::addAttribute(std::string name, float value)
//{
//	m_attributes[name].reset(new AttributeFloat(name, value));
//}
//
//void AttributeContainer::addAttribute(std::string name, double value)
//{
//	m_attributes[name].reset(new AttributeDouble(name, value));
//}
//
//void AttributeContainer::addAttribute(std::string name, char *value)
//{
//	m_attributes[name].reset(new AttributeString(name, value));
//}
//
//void AttributeContainer::addAttribute(std::string name, std::string value)
//{
//	m_attributes[name].reset(new AttributeString(name, value));
//}

//void AttributeContainer::addAttribute(std::string name, std::string value, const Strings &values)
//{
//	m_attributes[name].reset(new AttributeStringEnum(name, value, values));
//}

void AttributeContainer::attributeChanged(std::string name)
{
    SharedAttribute localAttribute=attribute(name);

    if(localAttribute)
    {
        onAttributeChanged(name, localAttribute);
        if(!m_changeListners.empty())
        {
            for(auto &listener:m_changeListners)
                listener->attributeChanged(this, name);
        }
    }
}

void AttributeContainer::serialize(Serializer *serializer)
{
    m_attributes.serialize(serializer);
}

void AttributeContainer::unserialize(Unserializer *unserializer)
{
    //	m_attributes.unserialize(unserializer);
    if(unserializer->key("attributes")) //need to fire off callbacks
    {
        if(unserializer->type()==Type::OBJECT)
        {
            unserializer->openObject();

            std::vector<std::string> keys=m_attributes.keys();

            for(auto &pair:m_attributes)
            {
                SharedAttribute &attribute=pair.second;

                if(unserializer->key(attribute->name()))
                {
                    attribute->unserialize(unserializer);
                    onAttributeChanged(attribute->name(), attribute);
                }
            }

            unserializer->closeObject();
        }
    }

}

}//namespace Limitless