#ifndef _Log_h_
#define _Log_h_

#include <boost/shared_ptr.hpp>
//#include <QString>
#include <vector>
#include "base_define.h"

#pragma warning(push)
#pragma warning(disable:4251)

namespace Limitless
{

enum class LogLevel
{
	Message=0,
	Warning=1,
	Error=2,
	Debug=3
};

BASE_EXPORT const std::string &logLevelString(LogLevel);

class BASE_EXPORT ILogCallback
{
public:
	ILogCallback(){};
	virtual ~ILogCallback(){};

	virtual bool write(LogLevel level, const std::string &system, const std::string &subSystem, const std::string &entry)=0;
};
typedef boost::shared_ptr<ILogCallback> SharedLogCallback;

class BASE_EXPORT Log
{
public:
	Log(){};
	~Log(){};

	static void attachCallback(SharedLogCallback callback);
	static void attachCallbackTop(SharedLogCallback callback);
	static void dettachCallback(SharedLogCallback callback);

	static void write(LogLevel level, const std::string &string, const std::string &subSystem, const std::string &entry);

	static void message(std::string system, std::string entry);
	static void warning(std::string system, std::string entry);
	static void error(std::string system, std::string entry);
	static void debug(std::string system, std::string entry);

	static void message(std::string system, std::string subSystem, std::string entry);
	static void warning(std::string system, std::string subSystem, std::string entry);
	static void error(std::string system, std::string subSystem, std::string entry);
	static void debug(std::string system, std::string subSystem, std::string entry);
//	static void write(QString entry);

private:
	static std::vector<SharedLogCallback> m_callbacks;
	static std::string emptyString;
};

}//namespace Limitless

#pragma warning(pop)

#endif //_Log_h_