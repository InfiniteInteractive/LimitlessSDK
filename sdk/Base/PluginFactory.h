#ifndef _PluginFactory_h_
#define _PluginFactory_h_

#include "base_define.h"
#include "PluginObject.h"

#include "typeName.h"

#pragma warning(push)
#pragma warning(disable:4251)

namespace Limitless
{

template<typename CLASS, typename INTERFACE>
class AutoRegister:public INTERFACE
{
public:
	AutoRegister(std::string instance):INTERFACE(instance){&s_typeName;}

	static PluginObject *create(std::string instance){return new CLASS(instance);}
private:
	static std::string s_typeName;
};

class BASE_EXPORT PluginFactory
{
	typedef PluginObject *(*FactoryFunc)(std::string);
	typedef boost::unordered_map<std::string, FactoryFunc> FactoryFunctions;

private:
	PluginFactory();
public:
	~PluginFactory();

	static PluginFactory &instance();
	
	void loadPlugins(std::string directory);

	std::vector<std::string> getType(std::string type);
	template<typename CLASS> CLASS *createType(std::string type, std::string instance)
	{
		FactoryFunctions::iterator iter=m_objects.find(type);

		if(iter != m_objects.end())
		{
			CLASS *object=dynamic_cast<CLASS *>(iter->second(instance));

			if(object != NULL)
				return object;
		}
		return NULL;
	}

//Setup devices
	std::string registerType(std::string typeName, FactoryFunc factoryFunc)
	{
		m_objects[typeName]=factoryFunc;
		return typeName;
	}

private:
	FactoryFunctions m_objects;
};
#pragma warning(pop)

template<typename CLASS, typename INTERFACE> std::string AutoRegister<CLASS, INTERFACE>::s_typeName=\
PluginFactory::instance().registerType(TypeName<CLASS>::get(), &AutoRegister<CLASS, INTERFACE>::create);

}//namespace Limitless

#endif //_PluginFactory_h_