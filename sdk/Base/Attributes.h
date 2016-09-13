#ifndef _Attributes_h_
#define _Attributes_h_

#include "common.h"
#include <boost/shared_ptr.hpp>
#include <boost/unordered_map.hpp>
#include "base_define.h"

#include "Base/Attribute.h"

#pragma warning(push)
#pragma warning(disable:4800)
#include <boost/lexical_cast.hpp>
#pragma warning(pop)

#pragma warning(push)
#pragma warning(disable:4251)

namespace Limitless
{

class BASE_EXPORT Attributes
{
public:
	void addAttribute(SharedAttribute attribute);
	template <typename T> void addAttribute(std::string name, T value) { m_attributes[name].reset(new AttributeTemplate<T>(name, value)); }

	typedef SharedAttributeMap::value_type value_type;
	typedef SharedAttributeMap::iterator iterator;
	typedef SharedAttributeMap::const_iterator const_iterator;

	size_t size() const{return m_attributes.size();}
	std::vector<std::string> keys() const;
	bool exists(std::string name) const{return (m_attributes.find(name) != m_attributes.end());}
	SharedAttribute find(const char *key) const;
	SharedAttribute find(const std::string &key) const;
	SharedAttribute &operator[](const char *key){const std::string constKey(key); return m_attributes[constKey];}
	SharedAttribute &operator[](const std::string &key){return m_attributes[key];}
	SharedAttribute const &operator[](const char *key) const { const std::string constKey(key); return m_attributes.at(constKey); }
	SharedAttribute const &operator[](const std::string &key) const { return m_attributes.at(key); }

	SharedAttribute const&at(const std::string key) const{return m_attributes.at(key);}

	iterator begin(){return m_attributes.begin();}
    const_iterator begin() const{return m_attributes.begin();}
    iterator end(){return m_attributes.end();}
    const_iterator end() const{return m_attributes.end();}

	iterator erase(const_iterator position){return m_attributes.erase(position);}

	void serialize(Serializer *serializer);
	void unserialize(Unserializer *serializer);

private:
	SharedAttributeMap m_attributes;
};

#pragma warning(pop)

void BASE_EXPORT merge(Attributes &dst, const Attributes &src);

}//namespace Limitless

#endif //_Attributes_h_