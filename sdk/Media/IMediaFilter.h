#ifndef _Limitless_IMediaFilter_h_
#define _Limitless_IMediaFilter_h_

#include "Media/media_define.h"

#include "Base/PluginObject.h"
#include "Media/FilterTypes.h"
#include "Media/MediaSample.h"
#include "Media/MediaFormat.h"
#include "Media/MediaPad.h"
#include "Media/MediaSourcePad.h"
#include "Media/MediaSinkPad.h"
#include "Media/MediaClock.h"

#include "boost/enable_shared_from_this.hpp"
//class IMediaFilter;
//typedef boost::shared_ptr<IMediaFilter> SharedMediaFilter;
namespace Limitless
{
#pragma warning(push)
#pragma warning(disable:4251)

enum FilterSearch
{
	Depth,
	Breadth
};

class MediaFilterContainer;

class MEDIA_EXPORT IMediaFilter:public boost::enable_shared_from_this<IMediaFilter>, public PluginObject
{
public:
	IMediaFilter(std::string instance, boost::shared_ptr<IMediaFilter> parent);
	virtual ~IMediaFilter(){};

	virtual FilterType type()=0;
	virtual std::string category()=0;

//Filter pad/connections
	virtual SharedMediaPads getSourcePads();
	virtual SharedMediaPads getSinkPads();
	virtual size_t sourcePads();
	virtual size_t sinkPads();
	virtual SharedMediaPad sourcePad(std::string name);
	virtual SharedMediaPad sinkPad(std::string name);
	virtual size_t sourcePadIndex(SharedMediaPad mediaPad);
	virtual size_t sinkPadIndex(SharedMediaPad mediaPad);
	virtual size_t sourcePadIndex(std::string name);
	virtual size_t sinkPadIndex(std::string name);

	virtual bool link(boost::shared_ptr<IMediaFilter> sinkFilter);
    virtual bool link(SharedMediaPad sinkPad);
	virtual void disconnect(boost::shared_ptr<IMediaFilter> sink);
	virtual void disconnect(SharedMediaPad localPad);

	virtual bool inStream(boost::shared_ptr<IMediaFilter> filter);
	virtual bool upStream(boost::shared_ptr<IMediaFilter> filter);
	virtual bool downStream(boost::shared_ptr<IMediaFilter> filter);

	virtual SharedMediaFilters findUpStream(std::string typeName);
	virtual SharedMediaFilters findUpStream(FilterType filterType);
	virtual SharedMediaFilter findFirstUpStream(std::string typeName, FilterSearch search=FilterSearch::Depth);
	virtual SharedMediaFilters findDownStream(std::string typeName);
	virtual SharedMediaFilters findDownStream(FilterType filterType);
	virtual SharedMediaFilter findFirstDownStream(std::string typeName, FilterSearch search=FilterSearch::Depth);

	virtual std::vector<boost::shared_ptr<IMediaFilter> > upStreamFilters();
	virtual std::vector<boost::shared_ptr<IMediaFilter> > downStreamFilters();

//Sample Handling
	size_t getSampleBin();
	virtual size_t allocSampleBin();
	virtual void releaseSampleBin(size_t bin);
	virtual SharedMediaSample newSample(unsigned int type);
	virtual SharedMediaSample newSample(unsigned int type, size_t sampleBin);
	template<typename _SampleType>boost::shared_ptr<_SampleType> newSampleType(unsigned int type){return boost::dynamic_pointer_cast<_SampleType>(newSample(type));}
	template<typename _SampleType>boost::shared_ptr<_SampleType> newSampleType(unsigned int type, size_t sampleBin){return boost::dynamic_pointer_cast<_SampleType>(newSample(type, sampleBin));}
	virtual void deleteSample(SharedMediaSample sample);

//Timing
	virtual MediaTime getTime();
	virtual MediaTime getStreamTime();

	virtual bool processSample(SharedMediaPad sinkPad, SharedMediaSample sample)=0;

//Filter state
	enum FilterState
	{
		INIT,
		READY,
		PAUSED,
		PLAYING
	};
	enum StateChange
	{
		FAILED,
		ASYNC,
		SUCCESS
	};
	IMediaFilter::FilterState getState() const;
	StateChange setState(IMediaFilter::FilterState state);
	StateChange ready();
	StateChange pause();
	StateChange play();

	void sendMessage(Attribute *attribute);
	void sendMessageUpStream(Attribute *attribute);
	void sendMessageDownStream(Attribute *attribute);

	static const size_t InvalidPin=(size_t)-1;

	virtual bool capture(std::string directory) { return false; }

	virtual void serialize(Serializer *serializer);
	virtual void unserialize(Unserializer *unserializer);
protected:
//Pad creation
	void addSourcePad(SharedMediaPad mediaPad);
	SharedMediaPad addSourcePad(std::string name, const char *jsonFormat);
	SharedMediaPad addSourcePad(std::string name, const std::string &jsonFormat);
	SharedMediaPad addSourcePad(std::string name, SharedMediaFormat format);
	void removeSourcePad(SharedMediaPad mediaPad);

	void addSinkPad(SharedMediaPad mediaPad);
	SharedMediaPad addSinkPad(std::string name, const char *jsonFormat);
	SharedMediaPad addSinkPad(std::string name, const std::string &jsonFormat);
	SharedMediaPad addSinkPad(std::string name, SharedMediaFormat format);
	void removeSinkPad(SharedMediaPad mediaPad);

//Pad Control
	bool reLinkPads();
	virtual bool onAcceptMediaFormat(SharedMediaPad Pad, SharedMediaFormat format);
	friend MediaPad;

	virtual void linked(SharedMediaPad pad, SharedMediaPad filterPad){onLinked(pad, filterPad); if(m_parent) m_parent->linked(pad, filterPad);}
	virtual void onLinked(SharedMediaPad pad, SharedMediaPad filterPad){}
	virtual void disconnected(SharedMediaPad pad, SharedMediaPad filterPad){onDisconnected(pad, filterPad); if(m_parent) m_parent->disconnected(pad, filterPad);}
	virtual void onDisconnected(SharedMediaPad pad, SharedMediaPad filterPad){}

//Filter transition functions
	virtual StateChange onReady(){return SUCCESS;}
	virtual StateChange onPaused(){return SUCCESS;}
	virtual StateChange onPlaying(){return SUCCESS;}

	virtual void onLinkFormatChanged(SharedMediaPad mediaPad, SharedMediaFormat format);

	void pushSample(SharedMediaPad mediaPad, SharedMediaSample sample);
	void pushSample(SharedMediaSample sample);

//Message System
	virtual void sendMessage(IMediaFilter *sender, Attribute *attribute);
	virtual void sendMessageUpStream(IMediaFilter *sender, Attribute *attribute);
	virtual void sendMessageDownStream(IMediaFilter *sender, Attribute *attribute);

	virtual void onMessage(MediaPad *mediaPad, IMediaFilter *sender, Attribute *attribute) {}

//Sample handling
	virtual size_t allocSampleBin(IMediaFilter *filter);
	virtual void releaseSampleBin(size_t bin, IMediaFilter *filter);

private:
	void setParent(boost::shared_ptr<IMediaFilter> parent){m_parent=parent;}
	friend MediaFilterContainer;

	FilterState m_state;

//	SharedMediaFilter m_parent;
	boost::shared_ptr<IMediaFilter> m_parent;
	SharedMediaPads m_mediaSourcePads;
	SharedMediaPads m_mediaSinkPads;

	SharedMediaClock m_mediaClock;

	size_t m_sampleBin;
};

#pragma warning(pop)

typedef boost::shared_ptr<IMediaFilter> SharedMediaFilter;
typedef std::vector<SharedMediaFilter> SharedMediaFilters;


std::string stateName(IMediaFilter::FilterState state);

MEDIA_EXPORT SharedMediaFilter findByInstance(SharedMediaFilters mediaFilters, std::string instance);

struct MEDIA_EXPORT MimeDetail
{
	std::string type;
	std::string codec;
	std::string format;
};

MEDIA_EXPORT MimeDetail parseMimeDetail(std::string mime);

namespace traits
{
	template <typename T> struct type
	{
		static FilterType get()
		{
			return Unknown;
		}
	};

	template <typename T> struct category
	{
		static std::string get()
		{
			return "unknown";
		}
	};
}//namespace traits

}//namespace Limitless


#endif //_Limitless_IMediaFilter_h_