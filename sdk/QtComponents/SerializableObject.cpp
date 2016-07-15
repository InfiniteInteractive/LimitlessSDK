#include "SerializableObject.h"
#include <QMetaProperty>
#include <QByteArray>

using namespace Limitless;

QVariantMap SerializableObject::serialize()
{
	const QMetaObject *metaobject=metaObject();
	int count=metaobject->propertyCount();
	QVariantMap map;
	
	for(int i=0; i<count; ++i)
	{
		QMetaProperty metaproperty=metaobject->property(i);
		const char *name=metaproperty.name();

		if(strcmp(name, "objectName") == 0)
			continue;

		map[name]=property(name);
	}
	return map;
}

void SerializableObject::deserialization(const QVariantMap &map)
{
	QMapIterator<QString, QVariant> it(map);
    while(it.hasNext())
    {
		it.next();

		QByteArray key=it.key().toLocal8Bit();
		setProperty(key.data(), it.value());
    }
}