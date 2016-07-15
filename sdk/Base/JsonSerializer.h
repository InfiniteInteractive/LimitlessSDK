#ifndef _JsonSerializer_h_
#define _JsonSerializer_h_

#include "base_define.h"
#include <string>
#include "Attribute.h"

namespace Limitless
{

struct JsonSerializerHidden;

class BASE_EXPORT JsonSerializer:public Serializer
{
public:
	JsonSerializer();
	~JsonSerializer();

	bool open(std::string fileName, bool pretty=true);

	virtual void startObject();
	virtual void endObject();

	virtual void endArray();
	virtual void startArray();

	virtual void addKey(const std::string key);
	virtual void addBool(const bool &value);
	virtual void addString(const std::string &value);
	virtual void addInt(const int &value);
	virtual void addUInt(const unsigned int &value);
	virtual void addInt64(const __int64 &value);
	virtual void addUInt64(const unsigned __int64 &value);
	virtual void addFloat(const float &value);
	virtual void addDouble(const double &value);

private:
	JsonSerializerHidden *m_hidden;
};


struct JsonUnserializerHidden;

class BASE_EXPORT JsonUnserializer:public Unserializer
{
public:
	JsonUnserializer();
	virtual ~JsonUnserializer();

	bool open(std::string fileName);
    bool parse(std::string json);

	virtual bool key(const std::string &key);
	virtual Type type();

	virtual std::string name();

	virtual bool openObject();
	virtual void closeObject();

	virtual bool openArray();
	virtual void closeArray();

    virtual void advance();

	virtual bool getBool();
	virtual std::string getString();
	virtual int getInt();
	virtual unsigned int getUInt();
	virtual __int64 getInt64();
	virtual unsigned __int64 getUInt64();
	virtual float getFloat();
	virtual double getDouble();

private:
	void advanceIterator();
	void advanceValueIterator();

	JsonUnserializerHidden *m_hidden;
};


}//namespace Limitless

#endif //_Attributes_h_