#include "MediaFormat.h"
//#include "Base/json.h"
//#include <QtCore/QStringList>
#include <boost/foreach.hpp>
#include "Base/JsonSerializer.h"

namespace Limitless
{

MediaAttribute::MediaAttribute(IChangeNotify *parent, std::string name, std::string value, Flags flags):
	AttributeString(parent, name, value),
	m_flags(flags)
{
	//	m_attribute=attribute;
}

//MediaAttribute::MediaAttribute(SharedAttributeString attribute, Flags flags):
//AttributeString(*attribute)
//{
////	m_attribute=attribute;
//	m_flags=flags;
//}

void MediaFormat::addMediaAttribute(SharedMediaAttribute mediaAttribute)
{
	//	m_mediaAttributes[mediaAttribute->name()]=mediaAttribute;
	*attribute(mediaAttribute->name())=*mediaAttribute;
}

void MediaFormat::addMediaAttributes(std::string jsonAttributes)
{
	//	QVariant jsonVariant=QtJson::parse(QString::fromStdString(jsonAttributes));
	//
	//	if(jsonVariant.type() != QVariant::Map)
	//		return;
	//
	//	addMediaAttributes(&jsonVariant);
	//	rapidjson::Document document;
	//
	//	document.Parse(jsonAttributes);
	JsonUnserializer unserializer;

	addMediaAttributes(&unserializer);
}

void MediaFormat::addMediaAttributes(Limitless::Unserializer *unserializer)
{
	while(unserializer->type()!=Type::UNKNOWN)
	{
		Attribute *attribute=nullptr;

		switch(unserializer->type())
		{
		case Type::UNKNOWN:
			break;
		case Type::BOOL:
			attribute=new AttributeBool(this, unserializer->name(), unserializer->getBool());
			break;
		case Type::OBJECT:
		case Type::ARRAY:
		{
			Strings values;

			unserializer->openArray();

			while(unserializer->type()!=Type::UNKNOWN)
			{
				if(unserializer->type()==Type::STRING)
					values.push_back(unserializer->getString());
				else
					unserializer->advance();
			}
			if(!values.empty())
				attribute=new AttributeStringEnum(this, unserializer->name(), values[0], values);

			unserializer->closeArray();
		}
		break;
		case Type::STRING:
			attribute=new AttributeString(this, unserializer->name(), unserializer->getString());
			break;
		case Type::DOUBLE:
			attribute=new AttributeDouble(this, unserializer->name(), unserializer->getDouble());
			break;
		}

		unserializer->advance();
		if(attribute!=nullptr)
		{
			SharedMediaAttribute mediaAttribute(new MediaAttribute(this, attribute->name(), attribute->toString()));

			addAttribute(mediaAttribute);
			delete attribute;
		}
	}

	//   for(rapidjson::Value::ConstMemberIterator iter=value.MemberBegin(); iter!=value.MemberEnd(); ++iter)
	//   {
	//       Attribute *attribute=nullptr;
	//
	//       switch(iter->value.GetType())
	//       {
	//       case rapidjson::kNullType:
	//           break;
	//       case rapidjson::kFalseType:
	//           attribute=new AttributeBool(this, iter->name.GetString(), false);
	//           break;
	//       case rapidjson::kTrueType:
	//           attribute=new AttributeBool(this, iter->name.GetString(), true);
	//           break;
	//       case rapidjson::kObjectType:
	//       case rapidjson::kArrayType:
	//       {
	//           Strings values;
	//
	//           for(rapidjson::Value::ConstMemberIterator valueIter=iter->value.MemberBegin(); valueIter!=iter->value.MemberEnd(); ++valueIter)
	//           {
	//               values.push_back(valueIter->value.GetString());
	//           }
	//           if(!values.empty())
	//               attribute=new AttributeStringEnum(this, iter->name.GetString(), values[0], values);
	//       }
	//       break;
	//       case rapidjson::kStringType:
	//           attribute=new AttributeString(this, iter->name.GetString(), iter->value.GetString());
	//           break;
	//       case rapidjson::kNumberType:
	//           attribute=new AttributeDouble(this, iter->name.GetString(), iter->value.GetDouble());
	//           break;
	//       }
	//
	//       if(attribute!=nullptr)
	//       {
	//           SharedMediaAttribute mediaAttribute(new MediaAttribute(this, attribute->name(), attribute->toString()));
	//
	//           addAttribute(mediaAttribute);
	//           delete attribute;
	//       }
	//   }
}

//void MediaFormat::addMediaAttributes(QVariant *jsonVariant)
//{
//	QVariantMap attributes=jsonVariant->toMap();
//	QStringList keys=attributes.keys();
//
//	foreach(const QString &key, keys)
//	{
//		QVariant &value=attributes[key];
//		Attribute *attribute=NULL;
//
//		switch(value.type())
//		{
//		case QVariant::Bool:
//			attribute=new AttributeBool(this, key.toStdString(), value.toBool());
//			break;
//		case QVariant::Int:
//			attribute=new AttributeInt(this, key.toStdString(), value.toInt());
//			break;
//		case QVariant::Double:
//			attribute=new AttributeFloat(this, key.toStdString(), value.toFloat());
//			break;
//		case QVariant::String:
//			attribute=new AttributeString(this, key.toStdString(), value.toString().toStdString());
//			break;
//		case QVariant::StringList:
//			{
//				QStringList stringList=value.toStringList();
//				Strings values;
//
//				foreach(const QString &item, stringList)
//					values.push_back(item.toStdString());
//
//				if(values.size() > 0)
//				{
//					attribute=new AttributeStringEnum(this, key.toStdString(), values[0], values);
//				}
//			}
//			break;
//		case QVariant::Map:
//			break;
//		}
//
//		if(attribute != NULL)
//		{
////			SharedAttribute sharedAttribute(attribute);
////			SharedMediaAttribute mediaAttribute(new MediaAttribute(sharedAttribute));
//			SharedMediaAttribute mediaAttribute(new MediaAttribute(this, attribute->name(), attribute->toString()));
//			
//			addAttribute(mediaAttribute);
//			delete attribute;
////			m_mediaAttributes[mediaAttribute->name()]=mediaAttribute;
//		}
//	}
//}

//bool MediaFormat::exists(std::string name) const
//{
//	return (m_mediaAttributes.find(name) != m_mediaAttributes.end());
//}
//
//SharedMediaAttribute MediaFormat::attribute(std::string name) const
//{
//	return m_mediaAttributes.at(name);
//}
//
//Strings MediaFormat::attributeKeys() const
//{	
//	Strings keys;
//
//	for(SharedMediaAttributes::const_iterator iter=m_mediaAttributes.begin(); iter != m_mediaAttributes.end(); ++iter)
//		keys.push_back(iter->first);
//	return keys;
//}

bool MediaFormat::match(SharedMediaFormat mediaFormat)
{
	Strings keys=mediaFormat->keys();
	bool intersectMatched=false;

	BOOST_FOREACH(std::string &key, keys)
	{
		//		if(!exists(key))
		//			return false;

		//		if(*(m_mediaAttributes.at(key).get()) == *(mediaFormat->attribute(key).get()))
		if(!exists(key))
			continue;

		if(*attribute(key)==*mediaFormat->attribute(key))
			intersectMatched=true;
	}
	return intersectMatched;
}

bool MediaFormat::operator==(const MediaFormat &mediaFormat) const
{
	Strings keys=mediaFormat.keys();

	BOOST_FOREACH(std::string &key, keys)
	{
		if(!exists(key))
			return false;

		//		if(*m_mediaAttributes.at(key) == *mediaFormat.attribute(key))
		if(*attribute(key)==*mediaFormat.attribute(key))
			return true;
	}
	return false;
}

}//namespace Limitless