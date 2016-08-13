#include "Media/MediaPluginFactory.h"
//#include <QDir>
//#include <QStringList>
//#include <QString>

#include <boost/format.hpp>
#include <boost/filesystem.hpp>

#ifdef WIN32
#define WIN32_LEAN_AND_MEAN
#include "Windows.h"
#endif //WIN32

using namespace Limitless;

MediaPluginFactory::FilterDetailsMap MediaPluginFactory::s_objects;
SharedMediaFilters MediaPluginFactory::s_filterInstances;

void MediaPluginFactory::loadPlugins(std::string path)
{
//	QDir directory(QString::fromStdString(path));
//	
//	QStringList  directories=directory.entryList(QDir::Dirs|QDir::NoDotAndDotDot);
//
//	foreach(const QString &subDirectory, directories)
//	{
//		QString qPath=QString("%1%2%3").arg(QString::fromStdString(path)).arg(subDirectory).arg(QDir::separator());
//		//load subfolders
//		loadPlugins(qPath.toStdString());
//	}
//
//	QStringList filters;
//	
//	filters<<"*.dll";
//	directory.setNameFilters(filters);
//
//	QStringList files=directory.entryList();
//
//#ifdef WIN32
//	SetDllDirectoryA(path.c_str());
////	std::wstring wpath(path.begin(), path.end());
////
////	AddDllDirectory(L"./");
////	AddDllDirectory(wpath.c_str());
//#endif //WIN32
//	foreach(const QString &file, files)
//	{
//		QString fullPath=QString("%1%2").arg(QString::fromStdString(path)).arg(file);
//
//#ifdef WIN32
//		HINSTANCE instance;
//
//		try
//		{	instance=LoadLibrary(fullPath.toStdString().c_str()); } //Plugins should autoregister
////		{	instance=LoadLibraryEx(fullPath.toStdString().c_str(), NULL, LOAD_LIBRARY_SEARCH_DEFAULT_DIRS);} //Plugins should autoregister
//		catch(std::exception &except)
//		{	instance=NULL;}
//		catch(...)
//		{	instance=NULL;}
//
//		if(instance == NULL)
//		{
//			LPVOID lpMsgBuf;
//			
//			FormatMessage( 
//				FORMAT_MESSAGE_ALLOCATE_BUFFER | 
//				FORMAT_MESSAGE_FROM_SYSTEM | 
//				FORMAT_MESSAGE_IGNORE_INSERTS,
//				NULL,
//				GetLastError(),
//				MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // Default language
//				(LPTSTR) &lpMsgBuf,
//				0,
//				NULL 
//			);
//
//			OutputDebugString((LPCTSTR)lpMsgBuf);
//			LocalFree( lpMsgBuf );
//		}
//
//#endif //WIN32
//	}
//#ifdef WIN32
//	SetDllDirectoryA(NULL);
//#endif //WIN32
	boost::filesystem::path directory(path);

    if(!boost::filesystem::exists(directory)||!boost::filesystem::is_directory(directory))
		return;

	boost::filesystem::recursive_directory_iterator iter(directory);
	boost::filesystem::recursive_directory_iterator endIter;

	while(iter!=endIter)
	{
        std::string fileName=iter->path().string();
        std::string extension=iter->path().extension().string();

		if(boost::filesystem::is_regular_file(*iter) && iter->path().extension().string()==".dll")
		{
			boost::filesystem::path filePath=iter->path();
			HINSTANCE instance=NULL;

			try
			{	instance=LoadLibrary(filePath.string().c_str()); } //Plugins should autoregister
			catch(std::exception &except)
			{	instance=NULL;}
			catch(...)
			{	instance=NULL;}
	
			if(instance == NULL)
			{
				LPVOID lpMsgBuf;
				
				FormatMessage( 
					FORMAT_MESSAGE_ALLOCATE_BUFFER | 
					FORMAT_MESSAGE_FROM_SYSTEM | 
					FORMAT_MESSAGE_IGNORE_INSERTS,
					NULL,
					GetLastError(),
					MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // Default language
					(LPTSTR) &lpMsgBuf,
					0,
					NULL 
				);
	
				std::string message((char *)lpMsgBuf);
				std::string errorMessage="Failed to load "+iter->path().filename().string()+" error:"+message;

				Log::message("MediaPluginFactory::loadPlugins", errorMessage);
				OutputDebugString((LPCTSTR)lpMsgBuf);
				LocalFree( lpMsgBuf );
			}
		}

        ++iter;
	}
}

std::vector<std::string> MediaPluginFactory::getType(std::string type)
{
	std::vector<std::string> classes;

	for(FilterDetailsMap::iterator iter=s_objects.begin(); iter!=s_objects.end(); ++iter)
	{
/*		PluginObject *object=iter->second("getType", SharedMediaFilter());

		if(object->isType(type))
			classes.push_back(iter->first);
		delete object;
*/	}
	return classes;
}

bool MediaPluginFactory::isType(std::string className, std::string type)
{
	FilterDetailsMap::iterator iter=s_objects.find(className);

	if(iter != s_objects.end())
	{
/*		PluginObject *object=iter->second("isType", SharedMediaFilter());
		bool returnValue=object->isType(type);
		
		delete object;
		return returnValue;
*/	}
	return false;
}

void MediaPluginFactory::removeFilter(IMediaFilter *filter)
{
	for(SharedMediaFilters::iterator iter=s_filterInstances.begin(); iter!=s_filterInstances.end(); ++iter)
	{
		if(iter->get() == filter)
		{
			s_filterInstances.erase(iter);
			break;
		}
	}
}

FilterDefinitions MediaPluginFactory::registedFilters()
{
	FilterDefinitions filterDefinitions;
	FilterDetailsMap::iterator iter;

	for(iter=s_objects.begin(); iter!=s_objects.end(); ++iter)
		filterDefinitions.push_back(iter->second);
	return filterDefinitions;
}

FilterDefinitions MediaPluginFactory::registedFiltersByType(FilterType type)
{
	FilterDefinitions filterDefinitions;
	FilterDetailsMap::iterator iter;

	for(iter=s_objects.begin(); iter!=s_objects.end(); ++iter)
	{
		if(iter->second.type == type)
			filterDefinitions.push_back(iter->second);
	}
	return filterDefinitions;
}

FilterDefinitions MediaPluginFactory::registedFiltersByCategory(std::string category)
{
	FilterDefinitions filterDefinitions;
	FilterDetailsMap::iterator iter;

	for(iter=s_objects.begin(); iter!=s_objects.end(); ++iter)
	{
		if(iter->second.category == category)
			filterDefinitions.push_back(iter->second);
	}
	return filterDefinitions;
}

std::string MediaPluginFactory::uniqueFilterName(std::string filterName)
{
	//get unique filter name
	size_t index=1;
	std::string filterInstance=(boost::format("%s_%d")%filterName%index).str();

	if(!s_filterInstances.empty())
	{
		while(findByInstance(s_filterInstances, filterInstance) != SharedMediaFilter())
		{
			++index;
			filterInstance=(boost::format("%s_%d")%filterName%index).str();
		}
	}

	return filterInstance;
}