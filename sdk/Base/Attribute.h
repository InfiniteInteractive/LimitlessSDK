#ifndef _Limitless_Attribute_h_
#define _Limitless_Attribute_h_

#include "common.h"
#include <boost/shared_ptr.hpp>
//#include <boost/unordered_map.hpp>
#include <unordered_map>
#include "base_define.h"

#pragma warning(push)
#pragma warning(disable:4800)
#include <boost/lexical_cast.hpp>
#pragma warning(pop)

#pragma warning(push)
#pragma warning(disable:4251)

#include "Base/types.h"
#include "Base/serializer.h"

template<typename classType> class AttributeTemplate;

namespace Limitless
{

class BASE_EXPORT IChangeNotify
{
public:
	IChangeNotify(){}
	virtual ~IChangeNotify(){}

	virtual void attributeChanged(std::string name)=0;
};

class Attribute;

typedef boost::shared_ptr<Attribute> SharedAttribute;
typedef std::vector<SharedAttribute> SharedAttributes;

class BASE_EXPORT Attribute
{
public:
//	enum Type
//	{
//		UNKNOWN,
//		BOOL,
//		INT,
//		UINT,
//		INT64,
//		UINT64,
//		FLOAT,
//		DOUBLE,
//		STRING,
//		STRING_ENUM,
//		ARRAY,
//		OBJECT
//	};

	Attribute(Type type, std::string name):m_type(type), m_name(name){}
	virtual ~Attribute(){};

	Type type(){return m_type;}
	std::string name(){return m_name;}

	template <typename T> void from(T value){AttributeTemplate<T> *d=dynamic_cast<AttributeTemplate<T> *>(this);d?d->from(value):assert(false);}
	template <typename T> T to() const { const AttributeTemplate<T> *d=dynamic_cast<const AttributeTemplate<T> *>(this); return d?d->to<T>():assert(false); }

	virtual void fromString(std::string value)=0;
	virtual void fromInt(int value)=0;
	virtual void fromUInt(unsigned int value)=0;
	virtual void fromInt64(__int64 value)=0;
	virtual void fromUInt64(unsigned __int64 value)=0;
	virtual void fromFloat(float value)=0;
	virtual void fromDouble(double value)=0;
	virtual void fromBool(bool value)=0;

	virtual std::string toString() const=0;
	virtual int toInt() const=0;
	virtual unsigned int toUInt() const=0;
	virtual __int64 toInt64() const=0;
	virtual unsigned __int64 toUInt64() const=0;
	virtual float toFloat() const=0;
	virtual double toDouble() const=0;
	virtual bool toBool() const=0;

	virtual SharedAttributes values(){return SharedAttributes();}

//	template<typename T> Attribute &operator=(const T &value) { from(value); return *this; }

	virtual bool operator==(const Attribute &attribute) const{return (toString()==attribute.toString());}
	virtual bool equal(boost::shared_ptr<Attribute> attribute) const{return (toString()==attribute->toString());}

	virtual void serialize(Serializer *serializer)=0;
	virtual void unserialize(Unserializer *serializer)=0;

protected:
	Type m_type;
	std::string m_name;
};
typedef std::unordered_map<std::string, SharedAttribute> SharedAttributeMap;

template<typename classType>
class AttributeTemplate:public Attribute
{
public:
	AttributeTemplate(const AttributeTemplate<classType> &attribute):Attribute(attribute.m_type, attribute.m_name), m_parent(nullptr) { m_value=attribute.m_value; };
	AttributeTemplate(const std::string name):Attribute(Type::UNKNOWN, name), m_parent(nullptr){assert(false);};
	AttributeTemplate(const std::string name, classType value):Attribute(Type::UNKNOWN, name), m_parent(nullptr), m_value(value) { assert(false); };
	AttributeTemplate(IChangeNotify *parent, const std::string name):Attribute(Type::UNKNOWN, name), m_parent(parent){assert(false);};
	AttributeTemplate(IChangeNotify *parent, const std::string name, classType value):Attribute(Type::UNKNOWN, name), m_parent(parent), m_value(value){assert(false);};
	AttributeTemplate(IChangeNotify *parent, const AttributeTemplate<classType> &attribute):Attribute(attribute.m_type, attribute.m_name), m_parent(parent){m_value=attribute.m_value;};
	virtual ~AttributeTemplate(){};

	classType &value(){return m_value;}

	template<typename T> void from(const T &value){m_value=boost::lexical_cast<classType>(value);}
	template<typename T> void to() const{boost::lexical_cast<T>(m_value);}

	virtual void fromString(std::string value){m_value=boost::lexical_cast<classType>(value); if(m_parent != nullptr) m_parent->attributeChanged(m_name);}
	virtual void fromInt(int value){m_value=boost::lexical_cast<classType>(value); if(m_parent != nullptr) m_parent->attributeChanged(m_name);}
	virtual void fromUInt(unsigned int value){m_value=boost::lexical_cast<classType>(value); if(m_parent != nullptr) m_parent->attributeChanged(m_name);}
	virtual void fromInt64(__int64 value){m_value=boost::lexical_cast<classType>(value); if(m_parent != nullptr) m_parent->attributeChanged(m_name);}
	virtual void fromUInt64(unsigned __int64 value){m_value=boost::lexical_cast<classType>(value); if(m_parent != nullptr) m_parent->attributeChanged(m_name);}
	virtual void fromFloat(float value){m_value=boost::lexical_cast<classType>(value); if(m_parent != nullptr) m_parent->attributeChanged(m_name);}
	virtual void fromDouble(double value){m_value=boost::lexical_cast<classType>(value); if(m_parent != nullptr) m_parent->attributeChanged(m_name);}
	virtual void fromBool(bool value){m_value=boost::lexical_cast<classType>(value); if(m_parent != nullptr) m_parent->attributeChanged(m_name);}

	virtual std::string toString() const{return boost::lexical_cast<std::string>(m_value);}
	virtual int toInt() const{return boost::lexical_cast<int>(m_value);}
	virtual unsigned int toUInt() const{return boost::lexical_cast<int>(m_value);}
	virtual __int64 toInt64() const{return boost::lexical_cast<__int64>(m_value);}
	virtual unsigned __int64 toUInt64() const{return boost::lexical_cast<__int64>(m_value);}
	virtual float toFloat() const{return boost::lexical_cast<float>(m_value);}
	virtual double toDouble() const{return boost::lexical_cast<double>(m_value);}
	virtual bool toBool() const{return boost::lexical_cast<bool>(m_value);}

	virtual void serialize(Serializer *serializer) { serializer->addKey(name()); serializer->addString(toString()); }
	virtual void unserialize(Unserializer *unserializer) { if(unserializer->key(name())) fromString(unserializer->getString()); }

protected:
	classType m_value;
	IChangeNotify *m_parent;
};

template<> BASE_EXPORT AttributeTemplate<bool>::AttributeTemplate(const std::string name):Attribute(Type::BOOL, name), m_parent(nullptr), m_value(false) {};
template<> BASE_EXPORT AttributeTemplate<bool>::AttributeTemplate(const std::string name, bool value):Attribute(Type::BOOL, name), m_parent(nullptr), m_value(value) {};
template<> BASE_EXPORT AttributeTemplate<bool>::AttributeTemplate(IChangeNotify *parent, const std::string name):Attribute(Type::BOOL, name), m_parent(parent), m_value(false){};
template<> BASE_EXPORT AttributeTemplate<bool>::AttributeTemplate(IChangeNotify *parent, const std::string name, bool value):Attribute(Type::BOOL, name), m_parent(parent), m_value(value){};
template<> void BASE_EXPORT AttributeTemplate<bool>::serialize(Serializer *serializer) { serializer->addKey(name()); serializer->addBool(m_value); }
template<> void BASE_EXPORT AttributeTemplate<bool>::unserialize(Unserializer *serializer) { if(serializer->key(name())) m_value=serializer->getBool(); }
typedef AttributeTemplate<bool> AttributeBool;

template<> BASE_EXPORT AttributeTemplate<int>::AttributeTemplate(const std::string name):Attribute(Type::INT, name), m_parent(nullptr), m_value(0) {};
template<> BASE_EXPORT AttributeTemplate<int>::AttributeTemplate(const std::string name, int value):Attribute(Type::INT, name), m_parent(nullptr), m_value(value) {};
template<> BASE_EXPORT AttributeTemplate<int>::AttributeTemplate(IChangeNotify *parent, const std::string name):Attribute(Type::INT, name), m_parent(parent), m_value(0){};
template<> BASE_EXPORT AttributeTemplate<int>::AttributeTemplate(IChangeNotify *parent, const std::string name, int value):Attribute(Type::INT, name), m_parent(parent), m_value(value){};
template<> void BASE_EXPORT AttributeTemplate<int>::serialize(Serializer *serializer) { serializer->addKey(name()); serializer->addInt(m_value); }
template<> void BASE_EXPORT AttributeTemplate<int>::unserialize(Unserializer *serializer) { if(serializer->key(name())) m_value=serializer->getInt(); }
typedef AttributeTemplate<int> AttributeInt;

template<> BASE_EXPORT AttributeTemplate<unsigned int>::AttributeTemplate(const std::string name):Attribute(Type::UINT, name), m_parent(nullptr), m_value(0) {};
template<> BASE_EXPORT AttributeTemplate<unsigned int>::AttributeTemplate(const std::string name, unsigned int value):Attribute(Type::UINT, name), m_parent(nullptr), m_value(value) {};
template<> BASE_EXPORT AttributeTemplate<unsigned int>::AttributeTemplate(IChangeNotify *parent, const std::string name):Attribute(Type::UINT, name), m_parent(parent), m_value(0){};
template<> BASE_EXPORT AttributeTemplate<unsigned int>::AttributeTemplate(IChangeNotify *parent, const std::string name, unsigned int value):Attribute(Type::UINT, name), m_parent(parent), m_value(value){};
template<> void BASE_EXPORT AttributeTemplate<unsigned int>::serialize(Serializer *serializer) { serializer->addKey(name()); serializer->addUInt(m_value); }
template<> void BASE_EXPORT AttributeTemplate<unsigned int>::unserialize(Unserializer *serializer) { if(serializer->key(name())) m_value=serializer->getUInt(); }
typedef AttributeTemplate<unsigned int> AttributeUInt;

template<> BASE_EXPORT AttributeTemplate<__int64>::AttributeTemplate(const std::string name):Attribute(Type::INT64, name), m_parent(nullptr), m_value(0) {};
template<> BASE_EXPORT AttributeTemplate<__int64>::AttributeTemplate(const std::string name, __int64 value):Attribute(Type::INT64, name), m_parent(nullptr), m_value(value) {};
template<> BASE_EXPORT AttributeTemplate<__int64>::AttributeTemplate(IChangeNotify *parent, const std::string name):Attribute(Type::INT64, name), m_parent(parent), m_value(0){};
template<> BASE_EXPORT AttributeTemplate<__int64>::AttributeTemplate(IChangeNotify *parent, const std::string name, __int64 value):Attribute(Type::INT64, name), m_parent(parent), m_value(value){};
template<> void BASE_EXPORT AttributeTemplate<__int64>::serialize(Serializer *serializer) { serializer->addKey(name()); serializer->addInt64(m_value); }
template<> void BASE_EXPORT AttributeTemplate<__int64>::unserialize(Unserializer *serializer) { if(serializer->key(name())) m_value=serializer->getInt(); }
typedef AttributeTemplate<__int64> AttributeInt64;

template<> BASE_EXPORT AttributeTemplate<unsigned __int64>::AttributeTemplate(const std::string name):Attribute(Type::UINT64, name), m_parent(nullptr), m_value(0) {};
template<> BASE_EXPORT AttributeTemplate<unsigned __int64>::AttributeTemplate(const std::string name, unsigned __int64 value):Attribute(Type::UINT64, name), m_parent(nullptr), m_value(value) {};
template<> BASE_EXPORT AttributeTemplate<unsigned __int64>::AttributeTemplate(IChangeNotify *parent, const std::string name):Attribute(Type::UINT64, name), m_parent(parent), m_value(0){};
template<> BASE_EXPORT AttributeTemplate<unsigned __int64>::AttributeTemplate(IChangeNotify *parent, const std::string name, unsigned __int64 value):Attribute(Type::UINT64, name), m_parent(parent), m_value(value){};
template<> void BASE_EXPORT AttributeTemplate<unsigned __int64>::serialize(Serializer *serializer) { serializer->addKey(name()); serializer->addUInt64(m_value); }
template<> void BASE_EXPORT AttributeTemplate<unsigned __int64>::unserialize(Unserializer *serializer) { if(serializer->key(name())) m_value=serializer->getUInt(); }
typedef AttributeTemplate<unsigned __int64> AttributeUInt64;

template<> BASE_EXPORT AttributeTemplate<float>::AttributeTemplate(const std::string name):Attribute(Type::FLOAT, name), m_parent(nullptr), m_value(0.0f) {};
template<> BASE_EXPORT AttributeTemplate<float>::AttributeTemplate(const std::string name, float value):Attribute(Type::FLOAT, name), m_parent(nullptr), m_value(value) {};
template<> BASE_EXPORT AttributeTemplate<float>::AttributeTemplate(IChangeNotify *parent, const std::string name):Attribute(Type::FLOAT, name), m_parent(parent), m_value(0.0f){};
template<> BASE_EXPORT AttributeTemplate<float>::AttributeTemplate(IChangeNotify *parent, const std::string name, float value):Attribute(Type::FLOAT, name), m_parent(parent), m_value(value){};
template<> void BASE_EXPORT AttributeTemplate<float>::serialize(Serializer *serializer) { serializer->addKey(name()); serializer->addFloat(m_value); }
template<> void BASE_EXPORT AttributeTemplate<float>::unserialize(Unserializer *serializer) { if(serializer->key(name())) m_value=serializer->getFloat(); }
typedef AttributeTemplate<float> AttributeFloat;

template<> BASE_EXPORT AttributeTemplate<double>::AttributeTemplate(const std::string name):Attribute(Type::DOUBLE, name), m_parent(nullptr), m_value(0.0) {};
template<> BASE_EXPORT AttributeTemplate<double>::AttributeTemplate(const std::string name, double value):Attribute(Type::DOUBLE, name), m_parent(nullptr), m_value(value) {};
template<> BASE_EXPORT AttributeTemplate<double>::AttributeTemplate(IChangeNotify *parent, const std::string name):Attribute(Type::DOUBLE, name), m_parent(parent), m_value(0.0){};
template<> BASE_EXPORT AttributeTemplate<double>::AttributeTemplate(IChangeNotify *parent, const std::string name, double value):Attribute(Type::DOUBLE, name), m_parent(parent), m_value(value){};
template<> void BASE_EXPORT AttributeTemplate<double>::serialize(Serializer *serializer) { serializer->addKey(name()); serializer->addDouble(m_value); }
template<> void BASE_EXPORT AttributeTemplate<double>::unserialize(Unserializer *serializer) { if(serializer->key(name())) m_value=serializer->getDouble(); }
typedef AttributeTemplate<double> AttributeDouble;

template<> BASE_EXPORT AttributeTemplate<std::string>::AttributeTemplate(const std::string name):Attribute(Type::STRING, name), m_parent(nullptr), m_value("") {};
template<> BASE_EXPORT AttributeTemplate<std::string>::AttributeTemplate(const std::string name, std::string value):Attribute(Type::STRING, name), m_parent(nullptr), m_value(value) {};
template<> BASE_EXPORT AttributeTemplate<std::string>::AttributeTemplate(IChangeNotify *parent, const std::string name):Attribute(Type::STRING, name), m_parent(parent), m_value(""){};
template<> BASE_EXPORT AttributeTemplate<std::string>::AttributeTemplate(IChangeNotify *parent, const std::string name, std::string value):Attribute(Type::STRING, name), m_parent(parent), m_value(value){};
template<> BASE_EXPORT void AttributeTemplate<std::string>::serialize(Serializer *serializer) { serializer->addKey(name()); serializer->addString(m_value); }
template<> void BASE_EXPORT AttributeTemplate<std::string>::unserialize(Unserializer *serializer) { if(serializer->key(name())) m_value=serializer->getString(); }
typedef AttributeTemplate<std::string> AttributeString;

template<typename classType>
class BASE_EXPORT AttributeEnum:public AttributeTemplate<classType>
{
public:
	AttributeEnum(const std::string name):AttributeTemplate(nullptr, name) {};
	AttributeEnum(const std::string name, const classType value):AttributeTemplate(nullptr, name, value) {};
	AttributeEnum(const std::string name, const classType  value, const std::vector<classType> values):AttributeTemplate(nullptr, name, value), m_values(values) {};
	AttributeEnum(IChangeNotify *parent, const std::string name):AttributeTemplate(parent, name){};
	AttributeEnum(IChangeNotify *parent, const std::string name, const classType value):AttributeTemplate(parent, name, value){};
	AttributeEnum(IChangeNotify *parent, const std::string name, const classType  value, const std::vector<classType> values):AttributeTemplate(parent, name, value), m_values(values){};
	AttributeEnum(const AttributeEnum<classType> &attribute):AttributeTemplate(nullptr, attribute.m_name){m_value=attribute.m_value; m_values=attribute.m_values;}
	AttributeEnum(IChangeNotify *parent, const AttributeEnum<classType> &attribute):AttributeTemplate(parent, attribute.m_name){m_value=attribute.m_value; m_values=attribute.m_values;}
	virtual ~AttributeEnum(){};

	std::vector<classType> enums() const{return m_values;}
	void setEnums(std::vector<classType> enumValues){m_values=enumValues;}

	virtual SharedAttributes values()
	{
		SharedAttributes values;

		for(size_t i=0; i<m_values.size(); ++i)
		{
			SharedAttribute attribute(new AttributeTemplate(nullptr, "enum", m_values[i]));
			
			values.push_back(attribute);
		}

		return values;
	}

protected:
	std::vector<classType> m_values;
};

template AttributeEnum<int>;
typedef AttributeEnum<int> AttributeIntEnum;

template AttributeEnum<float>;
typedef AttributeEnum<float> AttributeFloatEnum;

template AttributeEnum<std::string>;
typedef AttributeEnum<std::string> AttributeStringEnum;
typedef boost::shared_ptr<AttributeStringEnum> SharedAttributeStringEnum;

}//namespace Limitless

#endif //_Limitless_Attribute_h_