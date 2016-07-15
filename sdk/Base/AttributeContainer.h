#ifndef _AttributeContainer_h_
#define _AttributeContainer_h_

#include "Base/base_define.h"
//#include "Base/Attributes.h"
#include <vector>
#include <boost/shared_ptr.hpp>
#include <boost/unordered_map.hpp>

#include "Base/Attributes.h"

namespace Limitless
{
class AttributeContainer;

class BASE_EXPORT ChangeListener
{
public:
    ChangeListener() {}
    virtual ~ChangeListener() {}

    virtual void attributeChanged(AttributeContainer *parent, std::string name)=0;
};


class BASE_EXPORT AttributeContainer:public IChangeNotify
{
public:
	AttributeContainer(){}
	AttributeContainer(const AttributeContainer &container);
	virtual ~AttributeContainer(){}

	std::vector<std::string> keys() const;
	bool exists(const std::string &name) const;
	SharedAttribute attribute(const std::string &name) const;
	
	template<typename T> void setAttribute(std::string name, T value)
	{
		SharedAttribute attribute;

		attribute=m_attributes.find(name);
		if(attribute != SharedAttribute())
		{
			attribute->from(value);
			onAttributeChanged(name, attribute);
		}
	}
	template<> void Limitless::AttributeContainer::setAttribute(std::string name, const char *value)
	{
		setAttribute(name, std::string(value));
	}
	template<typename T> void setAttribute(std::string name, T value, const std::vector<T> &values);

	Attributes &attributes(){return m_attributes;}

	void addAttribute(SharedAttribute attribute);
	template <typename T> void addAttribute(std::string name, T value){m_attributes[name].reset(new AttributeTemplate<T>(this, name, value));}
	template<> void addAttribute<const char *>(std::string name, const char * value){addAttribute(name, std::string(value));}
	template<> void addAttribute<char *>(std::string name, char * value){addAttribute(name, std::string(value));}
	template <typename T> void addAttribute(std::string name, T value, const std::vector<T> &values){m_attributes[name].reset(new AttributeEnum<T>(this, name, value, values));}

	virtual void attributeChanged(std::string name);

    void addChangeListener(ChangeListener *listener)
    {
        if(std::find(m_changeListners.begin(), m_changeListners.end(), listener) == m_changeListners.end())
            m_changeListners.push_back(listener);
    }
    void removeChangeListener(ChangeListener *listener)
    {
        auto iter=std::find(m_changeListners.begin(), m_changeListners.end(), listener);

        if(iter != m_changeListners.end())
            m_changeListners.erase(iter);
    }

	virtual void serialize(Serializer *serializer);
	virtual void unserialize(Unserializer *unserializer);
	
protected:
	virtual void onAttributeChanged(std::string name, SharedAttribute attribute){};

private:
	Attributes m_attributes;

    std::vector<ChangeListener *> m_changeListners;
};

}//namespace Limitless

#endif //_AttributeContainer_h_