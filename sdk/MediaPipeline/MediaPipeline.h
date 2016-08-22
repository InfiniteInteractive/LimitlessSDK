#ifndef _MediaPipeline_h_
#define _MediaPipeline_h_

#include "mediapipeline_define.h"

#include "Media/MediaSample.h"
#include "Media/IMediaFilter.h"
#include "Media/IMediaSource.h"
#include "Media/MediaFilterContainer.h"
//#include "IPipelineCallback.h"
#include <string>
#include <vector>

#include <boost/thread.hpp>
#include <boost/atomic.hpp>
//#include <boost/lockfree/stack.hpp>

#include <mutex>

//class MediaPipelineNode
//{
//public:
//	MediaPipelineNode(SharedMediaFilter mediaFilter, size_t sampleType):
//	  m_mediaFilter(mediaFilter), m_sampleType(sampleType){};
//
//	SharedMediaFilter getMediaFilter(){return m_mediaFilter;}
//	size_t getSampleType(){return m_sampleType;}
//
//private:
//	SharedMediaFilter m_mediaFilter;
//	size_t m_sampleType;
////	SharedMediaSamples m_mediaSamples;
//};

//typedef boost::shared_ptr<MediaPipelineNode> SharedMediaPipelineNode;
//typedef std::vector<SharedMediaPipelineNode> SharedMediaPipelineNodes;
#pragma warning(push)
#pragma warning(disable:4251)

namespace Limitless
{

struct SourcePadDetail
{
	SourcePadDetail(SharedMediaFilter filter, SharedMediaPad mediaPad):filter(filter), mediaPad(mediaPad), threadRunning(false), threadStop(false){}

	SharedMediaFilter filter;
	SharedMediaPad mediaPad;

	bool threadRunning;
	boost::atomic<bool> threadStop;
	boost::thread thread;
};
typedef boost::shared_ptr<SourcePadDetail> SharedSourcePadDetail;
typedef std::vector<SharedSourcePadDetail> SharedSourcePadDetails;

struct SourceFilterDetail
{
	SourceFilterDetail(SharedMediaFilter filter):filter(filter){}//, threadRunning(false), threadStop(false){}

	SharedMediaFilter filter;

	SharedSourcePadDetails padDetails;

//	bool threadRunning;
//	boost::atomic<bool> threadStop;
//	boost::thread thread;
};
typedef boost::shared_ptr<SourceFilterDetail> SharedSourceFilterDetail;

struct SampleBin
{
	SampleBin():inUse(false), samplesInUse(0), maxBuffer(10){}
	~SampleBin(){}

	bool inUse;
	size_t seqId;

	IMediaFilter *filter;
	MediaSamples freeSamples;
	MediaSamples allSamples;
	
	size_t samplesInUse;
	size_t maxBuffer;

	std::mutex mutex;
	std::condition_variable event;
};
typedef std::vector<SampleBin *> SampleBins;

class MEDIAPIPELINE_EXPORT MediaPipeline:public MediaFilterContainer
{
public:
	MediaPipeline(std::string instance, SharedMediaFilter parent);
	virtual ~MediaPipeline();

	virtual bool initialize(const Attributes &attributes){return false;}
	virtual bool shutdown(){return false;}

	virtual StateChange onPaused();
	virtual StateChange onPlaying();
	
	void processThread(SharedSourcePadDetail sourceDetail);

	virtual SharedMediaSample newSample(unsigned int type, size_t bin);
//	virtual void deleteSample(SharedMediaSample sample);
	virtual void deleteSample(MediaSample *sample);

	virtual MediaTime getStreamTime();

protected:

	virtual bool onAddMediaFilter(SharedMediaFilter mediaFilter);
	virtual bool onRemoveMediaFilter(SharedMediaFilter mediaFilter);
//pad control
	virtual void onLinked(SharedMediaPad pad, SharedMediaPad filterPad);
	virtual void onDisconnected(SharedMediaPad pad, SharedMediaPad filterPad);

//sample handling
	virtual size_t allocSampleBin(IMediaFilter *filter);
	virtual void releaseSampleBin(size_t bin, IMediaFilter *filter);

private:
	void startSourceFilters();
	void stopDisconnectedPads();
	void stopAllSourceThreads();

//	boost::thread m_thread;
	bool m_running;

//	typedef std::vector<SharedMediaSample> MediaSampleStack;
//	typedef std::map<unsigned __int64, MediaSampleStack> FreeSampleStacks;
	
//	typedef std::map<unsigned __int64, MediaSampleStack> FreeSampleStacks;
	
	size_t m_framesInFlight;

	typedef std::map<IMediaFilter *, SharedSourceFilterDetail> SourceFilterDetailMap;

	SourceFilterDetailMap m_sourceFilterDetails;

	boost::mutex m_eventMutex;
	boost::condition_variable m_event;

    size_t m_eventSampleId;
    size_t m_eventSampleBin;
//	FreeSampleStacks m_freeSamples;
	std::mutex m_sampleBinMutex;
	SampleBins m_sampleBins;
	size_t m_sampleBinSeq;
	size_t m_sampleBinIdexMask;
//	boost::mutex m_freeStackMutex;

	MediaTime m_streamStartTime;
	MediaTime m_streamPauseTime;
};
typedef boost::shared_ptr<MediaPipeline> SharedMediaPipeline;

/*
#include "PipelineObject.h"
#include "IPipelineSource.h"
#include "IPipelineFilter.h"
#include "IPipelineCallback.h"
#include <string>
#include <vector>
#include <boost/unordered_map.hpp>
#include <boost/thread.hpp>
#include "MediaPipeline_define.h"

template <typename T> struct TypeName
{	
	static std::string get()
	{
		const char *fullName=typeid(T).name();
		const char *name=strstr(fullName, "class");
		return (name)? name+6 : fullName;
	}
};

class MEDIAPIPELINE_EXPORT MediaPipeline:public IPipelineCallback
{
	typedef PipelineObject *(*FactoryFunc)(std::string);
	typedef boost::unordered_map<std::string, FactoryFunc> FactoryFunctions;

private:
	MediaPipeline();
public:
	~MediaPipeline();

	static MediaPipeline &instance();

//	std::vector<std::string> imageSources();
	std::vector<std::string> getType(std::string type);
//	StreamObject *createType(std::string type, std::string instance);
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

	SharedPipelineSource addSource(const std::string &name, const std::string &instance, const Attributes &attributes=Attributes());
	bool addFilter(const std::string &name, const std::string &instance, const Attributes &attributes=Attributes());
	bool addConsumer(const std::string &name, const std::string &instance, const Attributes &attributes=Attributes());

//Setup devices
	std::string registerType(std::string typeName, FactoryFunc factoryFunc)
	{
		m_objects[typeName]=factoryFunc;
		return typeName;
	}

	bool start();
	void stop();
	bool isRunning();

	void process();
	virtual bool sampleCallback(PipelineSample *pushSample, PipelineSample **newSample);
private:
//	static MediaPipeline m_instance;
	boost::thread m_thread;
	bool m_running;

	FactoryFunctions m_objects;
	SharedPipelineSource m_source;

	typedef std::vector<IPipelineFilter *> PipelineFilters;
	PipelineFilters m_filters;

	IPipelineFilter *m_consumer;

	std::list<PipelineSample *> m_sourceSamples;
};


template<typename CLASS, typename INTERFACE> std::string AutoRegister<CLASS, INTERFACE>::s_typeName=\
MediaPipeline::instance().registerType(TypeName<CLASS>::get(), &AutoRegister<CLASS, INTERFACE>::create);

//template <typename BASE, typename DERIVED> PipelineObject *createInstance()
//{    return new DERIVED;}
//
//#define REGISTER_PIPLELINEOBJECT(NAME, BASE, DERIVED) static PipelineObject *_factory_##DERIVED=MediaPipeline::instance().registerType(#NAME, createInstance<#BASE, #DERIVED>)
//template <int TYPE, typename IMPL> const uint16_t PipelineObjectInst<TYPE, IMPL>::typeName = MediaPipeline::instance().registerType(
//     PipelineObjectInst<TYPE, IMPL>::typeName, &PipelineObjectInst<TYPE, IMPL>::create);
*/

}//namespace Limitless

#pragma warning(pop)

#endif //_MediaPipeline_h_

