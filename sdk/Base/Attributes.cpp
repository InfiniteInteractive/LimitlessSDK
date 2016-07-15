#include "Attributes.h"
#include "Attribute.h"
//#include "json.h"
#include <boost/foreach.hpp>

namespace Limitless
{

void Attributes::addAttribute(SharedAttribute attribute)
{
    m_attributes[attribute->name()]=attribute;
}

std::vector<std::string> Attributes::keys() const
{
    std::vector<std::string> localKeys;

    for(SharedAttributeMap::const_iterator iter=m_attributes.begin(); iter!=m_attributes.end(); ++iter)
        localKeys.push_back(iter->first);
    return localKeys;
}

void merge(Attributes &dst, const Attributes &src)
{
    for(const Attributes::value_type &value:src)
    {
        dst[value.first]=value.second;
    }
}

SharedAttribute Attributes::find(const char *key) const
{
    const std::string stringKey(key);

    return find(stringKey);
}

SharedAttribute Attributes::find(const std::string &key) const
{
    SharedAttributeMap::const_iterator iter=m_attributes.find(key);

    if(iter!=m_attributes.end())
        return iter->second;
    return SharedAttribute();
}

void Attributes::serialize(Serializer *serializer)
{
    serializer->addKey("attributes");
    serializer->startObject();

    for(value_type &attribute:m_attributes)
        attribute.second->serialize(serializer);

    serializer->endObject();
}

void Attributes::unserialize(Unserializer *unserializer)
{
    if(unserializer->key("attributes"))
    {
        if(unserializer->type()==Type::OBJECT)
        {
            unserializer->openObject();

            for(value_type &attribute:m_attributes)
                attribute.second->unserialize(unserializer);

            unserializer->closeObject();
        }
    }
}

}//namespace Limitless