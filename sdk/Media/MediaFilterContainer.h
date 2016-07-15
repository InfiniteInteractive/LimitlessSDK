#ifndef _Limitless_MediaFilterContainer_h_
#define _Limitless_MediaFilterContainer_h_

#include "Media/media_define.h"

#include <vector>
#include "Media/IMediaFilter.h"
#pragma warning(push)
#pragma warning(disable:4251)

namespace Limitless
{

class MEDIA_EXPORT MediaFilterContainer:public IMediaFilter
{
public:
	MediaFilterContainer(std::string instance, SharedMediaFilter parent):IMediaFilter(instance, parent){};
	virtual ~MediaFilterContainer(){};

	virtual FilterType type() { return Container; }
	virtual std::string category() { return "Container"; }

	bool addMediaFilter(SharedMediaFilter mediaFilter);
	bool removeMediaFilter(SharedMediaFilter meditFilter);
	SharedMediaFilters mediaFilters();
	SharedMediaFilter mediaFilter(std::string instance);
	void clear();

//	void setSourceFilter(SharedMediaFilter mediaFilter);
//	SharedMediaFilter sourceFilter(){return m_sourceFilter;}
//	void setSinkFilter(SharedMediaFilter mediaFilter);
	SharedMediaFilters sourceFilters() { return m_sourceFilters; }

	SharedMediaSourcePad mediaSourcePad(){return m_mediaSourcePad;}

	bool link(SharedMediaFilter source, SharedMediaFilter sink);
	virtual bool link(boost::shared_ptr<IMediaFilter> sinkFilter);

	virtual SharedMediaFilters findUpStream(std::string typeName);
	virtual SharedMediaFilters findDownStream(std::string typeName);

	virtual SharedMediaFilters findUpStream(FilterType filterType);
	virtual SharedMediaFilters findDownStream(FilterType filterType);

	virtual StateChange onReady();
	virtual StateChange onPaused();
	virtual StateChange onPlaying();

	virtual SharedMediaPads getSourcePads();
	virtual SharedMediaPads getSinkPads();

	virtual bool processSample(SharedMediaPad sinkPad, SharedMediaSample sample);

	virtual void serialize(Serializer *serializer);
	virtual void unserialize(Unserializer *unserializer);

protected:
	virtual bool onAddMediaFilter(SharedMediaFilter mediaFilter) { return true; }
	virtual bool onRemoveMediaFilter(SharedMediaFilter mediaFilter) { return true; }

private:
	SharedMediaFilters m_sourceFilters;
	SharedMediaFilters m_sinkFilters;

	SharedMediaFilters m_mediaFilters;
//	SharedMediaFilter m_sourceFilter;
//	SharedMediaFilter m_sinkFilter;

	SharedMediaSourcePad m_mediaSourcePad;
	SharedMediaSourcePad m_mediaSinkPad;
};

}//namespace Limitless

#pragma warning(pop)

#endif //_Limitless_MediaFilterContainer_h_