#include "Media/MediaSampleFlags.h"

#include <map>
#include <mutex>

class SampleFlags
{
public:
	SampleFlags():m_nextFlag(1) {}

	unsigned int registerFlag(std::string name)
	{
		std::unique_lock<std::mutex> lock(m_flagMutex);

		FlagMap::iterator iter=m_flags.find(name);

		if(iter!=m_flags.end())
			return iter->second;

		m_flags.insert(FlagMap::value_type(name, m_nextFlag));
		m_nextFlag++;
	}

	unsigned int getFlag(std::string name)
	{
		std::unique_lock<std::mutex> lock(m_flagMutex);

		FlagMap::iterator iter=m_flags.find(name);

		if(iter==m_flags.end())
			return 0;

		return iter->second;
	}

private:
	typedef std::map<std::string, unsigned int> FlagMap;

	FlagMap m_flags;
	unsigned int m_nextFlag;

	std::mutex m_flagMutex;
};

namespace Limitless
{

SampleFlags &getInstance()
{
	static SampleFlags flags;

	return flags;
}

unsigned int registerMediaSampleFlag(std::string name)
{
	return getInstance().registerFlag(name);
}

unsigned int getMediaSampleFlag(std::string name)
{
	return getInstance().getFlag(name);
}

}//namespace Limitless