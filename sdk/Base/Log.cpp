#include "Log.h"
#include <boost/foreach.hpp>

using namespace Limitless;

std::vector<SharedLogCallback> Log::m_callbacks;
std::string Log::emptyString;

const std::string &Limitless::logLevelString(LogLevel level)
{
	static std::vector<std::string> logLevelNames={"Message", "Warning", "Error", "Debug"};

	return logLevelNames[(size_t)level];
}

void Log::attachCallback(SharedLogCallback callback)
{	m_callbacks.push_back(callback);}

void Log::attachCallbackTop(SharedLogCallback callback)
{
	m_callbacks.insert(m_callbacks.begin(), callback);
}

void Log::dettachCallback(SharedLogCallback callback)
{
	std::vector<SharedLogCallback>::iterator iter=std::find(m_callbacks.begin(), m_callbacks.end(), callback);

	if(iter != m_callbacks.end())
		m_callbacks.erase(iter);
}

void Log::write(LogLevel level, const std::string &system, const std::string &subSystem, const std::string &entry)
{
	for(SharedLogCallback &callback:m_callbacks)
	{
		if(callback->write(level, system, subSystem, entry))
			break;
	}
}

void Log::message(std::string system, std::string entry)
{
	write(LogLevel::Message, system, emptyString, entry);
}

void Log::warning(std::string system, std::string entry)
{
	write(LogLevel::Warning, system, emptyString, entry);
}

void Log::error(std::string system, std::string entry)
{
	write(LogLevel::Error, system, emptyString, entry);
}

void Log::debug(std::string system, std::string entry)
{
	write(LogLevel::Debug, system, emptyString, entry);
}

void Log::message(std::string system, std::string subSystem, std::string entry)
{
	write(LogLevel::Message, system, subSystem, entry);
}

void Log::warning(std::string system, std::string subSystem, std::string entry)
{
	write(LogLevel::Warning, system, subSystem, entry);
}

void Log::error(std::string system, std::string subSystem, std::string entry)
{
	write(LogLevel::Error, system, subSystem, entry);
}

void Log::debug(std::string system, std::string subSystem, std::string entry)
{
	write(LogLevel::Debug, system, subSystem, entry);
}

//void Log::write(QString entry)
//{
//	BOOST_FOREACH(SharedLogCallback &callback, m_callbacks)
//	{
//		callback->write(entry.toStdString());
//	}
//}