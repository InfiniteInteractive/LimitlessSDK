#include <Media/MediaSample.h>

using namespace Limitless;

unsigned int MediaSample::m_uniqueIdCount=0;

bool MediaSample::isType(unsigned int type) const
{
	if(type == m_type.id)
		return true;
	if(std::find(m_type.idInheritance.begin(), m_type.idInheritance.end(), type) != m_type.idInheritance.end())
		return true;
	return false;
}

void MediaSample::copyHeader(boost::shared_ptr<MediaSample> sample)
{
	m_timestamp=sample->timestamp();
	m_sourceTimestamp=sample->sourceTimestamp();
	m_sequenceNumber=sample->sequenceNumber();
	m_mediaIndex=sample->m_mediaIndex;
	m_flags=sample->m_flags;
	m_copied=true;
}

void MediaSample::copyHeader(boost::shared_ptr<MediaSample> sample, std::string filterName)
{
	m_lastFilter=filterName;
	copyHeader(sample);
}