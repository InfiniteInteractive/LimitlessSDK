#include "PluginFactory.h"
//#include <QDir>
//#include <QStringList>
//#include <QString>
#include <boost/filesystem.hpp>

#ifdef WIN32
#define WIN32_LEAN_AND_MEAN
#include "Windows.h"
#endif //WIN32

using namespace Limitless;

PluginFactory::PluginFactory()
{}

PluginFactory::~PluginFactory()
{}

PluginFactory &PluginFactory::instance()
{
	static PluginFactory s_instance;

	return s_instance;
}

void PluginFactory::loadPlugins(std::string path)
{
//	QDir directory(QString::fromStdString(path));
//	QStringList filters;
//	
//	filters<<"*.dll";
//	directory.setNameFilters(filters);
//
//	QStringList files=directory.entryList();
//
//	foreach(const QString &file, files)
//	{
//		QString fullPath=QString("%1%2").arg(QString::fromStdString(path)).arg(file);
//		
//		HINSTANCE instance=LoadLibrary(fullPath.toStdString().c_str());
//		//Plugins should autoregister
//	}
	boost::filesystem::path directory(path);

	if(!boost::filesystem::exists(directory)||!boost::filesystem::is_directory(directory))
		return;

	boost::filesystem::recursive_directory_iterator iter(directory);
	boost::filesystem::recursive_directory_iterator endIter;

	while(iter!=endIter)
	{
		if(boost::filesystem::is_regular_file(*iter)&&iter->path().extension()=="dll")
		{
			boost::filesystem::path filePath=iter->path();

			HINSTANCE instance=LoadLibrary(filePath.string().c_str());
		}
	}
}

std::vector<std::string> PluginFactory::getType(std::string type)
{
	std::vector<std::string> classes;

	for(FactoryFunctions::iterator iter=m_objects.begin(); iter!=m_objects.end(); ++iter)
	{
		PluginObject *object=iter->second("getType");

		if(object->isType(type))
			classes.push_back(iter->first);
		delete object;
	}
	return classes;
}