#ifndef _MediaContainer_h_
#define _MediaContainer_h_

#include "Media/media_define.h"

#include <vector>
#include "Media/MediaSample.h"

namespace Limitless
{

class MEDIA_EXPORT MediaGroup
{
public:
	MediaGroup(){};
	virtual ~MediaGroup(){};

	size_t size() const{return m_mediaSamples.size();}

	SharedMediaSample operator[](size_t i){return m_mediaSamples[i];}
	SharedMediaSample getMediaSample(int i){return m_mediaSamples[i];}

private:
	SharedMediaSamples m_mediaSamples;
};
typedef boost::shared_ptr<MediaGroup> SharedMediaGroup;
typedef std::vector<SharedMediaGroup> SharedMediaGroups;

class MEDIA_EXPORT MediaContainer
{
public:
	MediaContainer(){};
	virtual ~MediaContainer(){};

	size_t size() const{return m_mediaGroups.size();}

	SharedMediaGroup operator[](size_t i){return m_mediaGroups[i];}
	SharedMediaGroup getMediaGroup(int i){return m_mediaGroups[i];}
	SharedMediaGroup getMediaGroup(){return m_mediaGroups.back();}

private:
	SharedMediaGroups m_mediaGroups;
};
typedef boost::shared_ptr<MediaContainer> SharedMediaContainer;
typedef std::vector<SharedMediaContainer> SharedMediaContainers;

}//namespace Limitless

#endif //_MediaContainer_h_