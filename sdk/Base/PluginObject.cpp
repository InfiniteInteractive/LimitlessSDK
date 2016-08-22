#include "PluginObject.h"

using namespace Limitless;

void PluginEventNotify::addEventListener(PluginEventListener *listener)
{
	if(std::find(m_listners.begin(), m_listners.end(), listener)==m_listners.end())
		m_listners.push_back(listener);
}

void PluginEventNotify::removeEventListener(PluginEventListener *listener)
{
	auto iter=std::find(m_listners.begin(), m_listners.end(), listener);

	if(iter!=m_listners.end())
		m_listners.erase(iter);
}

void PluginEventNotify::event(std::string name)
{
	if(!m_listners.empty())
	{
		for(auto &listener:m_listners)
			listener->onPluginEvent((PluginObject *)this, name); //not pretty but its there
	}
}

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