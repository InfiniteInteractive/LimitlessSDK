#ifndef _Limitless_Base_Serializer_h_
#define _Limitless_Base_Serializer_h_

#include "Base/base_define.h"
#include "Base/types.h"

namespace Limitless
{

class BASE_EXPORT Serializer
{
public:
	Serializer() {}
	virtual ~Serializer() {}

	virtual void startObject()=0;
	virtual void endObject()=0;

	virtual void startArray()=0;
	virtual void endArray()=0;

	virtual void addKey(const std::string key)=0;
	virtual void addBool(const bool &value)=0;
	virtual void addString(const std::string &value)=0;
	virtual void addInt(const int &value)=0;
	virtual void addUInt(const unsigned int &value)=0;
	virtual void addInt64(const __int64 &value)=0;
	virtual void addUInt64(const unsigned __int64 &value)=0;
	virtual void addFloat(const float &value)=0;
	virtual void addDouble(const double &value)=0;
};

class BASE_EXPORT Unserializer
{
public:
	Unserializer() {}
	virtual ~Unserializer() {}

	virtual bool key(const std::string &key)=0;
	virtual Type type()=0;

	virtual std::string name()=0;

	virtual bool openObject()=0;
	virtual void closeObject()=0;

	virtual bool openArray()=0;
	virtual void closeArray()=0;

    virtual void advance()=0;

	virtual bool getBool()=0;
	virtual std::string getString()=0;
	virtual int getInt()=0;
	virtual unsigned int getUInt()=0;
	virtual __int64 getInt64()=0;
	virtual unsigned __int64 getUInt64()=0;
	virtual float getFloat()=0;
	virtual double getDouble()=0;
};

}//namespace Limitless

#endif //_Limitless_Base_Serializer_h_