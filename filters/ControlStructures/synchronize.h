#ifndef _Synchronize_h_
#define _Synchronize_h_

#include "Media/MediaPluginFactory.h"
#include "Media/IMediaFilter.h"
#include "Media/IImageSample.h"
#include "Media/IImageSampleSet.h"

//#include <boost/thread.hpp>
#include <deque>
#include <mutex>
#include <thread>
#include <condition_variable>

struct SinkInfo
{
    std::deque<Limitless::SharedMediaSample> samples;
};

class Synchronize:public Limitless::MediaAutoRegister<Synchronize, Limitless::IMediaFilter>
{
public:
	Synchronize(std::string name, Limitless::SharedMediaFilter parent);
	~Synchronize();

	virtual bool initialize(const Limitless::Attributes &attributes);
	virtual bool shutdown(){return true;}

	virtual Limitless::SharedPluginView getView();

	virtual bool processSample(Limitless::SharedMediaPad sinkPad, Limitless::SharedMediaSample sample);

	virtual bool capture(std::string directory);
protected:
//IMediaFilter
	virtual StateChange onReady();
	virtual StateChange onPaused();
	virtual StateChange onPlaying();

	virtual void onLinked(Limitless::SharedMediaPad pad, Limitless::SharedMediaPad filterPad);
	virtual void onDisconnected(Limitless::SharedMediaPad pad, Limitless::SharedMediaPad filterPad);
	virtual bool onAcceptMediaFormat(Limitless::SharedMediaPad pad, Limitless::SharedMediaFormat format);
	virtual void onLinkFormatChanged(Limitless::SharedMediaPad pad, Limitless::SharedMediaFormat format);

	void processSourceSample();

//AttributeContainer
	virtual void onAttributeChanged(std::string name, Limitless::SharedAttribute attribute);

	enum SyncSource
	{
		Stream,
		Source
	};

private:
    void copySamples(bool synced);
    void handleEvent(Limitless::SharedMediaSample sample);
    bool syncSamples();

	SyncSource m_syncSource;
	int m_bufferLength;

	int m_sinkCount;
	bool m_firstSample;
	Limitless::SharedMediaFormat m_outputFormat;
	Limitless::MediaTime m_lastSyncTime;

	uint64_t m_timeError;
	
	size_t m_imageSetSampleId;
    size_t m_eventSampleId;
    size_t m_imageSampleId;

	std::string m_captureDirectory;
	bool m_captureImages;
	int m_captureIndex;

	struct SampleInfo
	{
		SampleInfo(size_t index, Limitless::SharedMediaSample sample):index(index), sample(sample) {}

		size_t index;
		Limitless::SharedMediaSample sample;
	};

	std::deque<SampleInfo> m_samples;
    std::vector<SinkInfo> m_sinks;
//	std::vector<Limitless::SharedIImageSampleQueue> m_indexedSamples;
//	std::vector<int> m_filterBuffers;

	std::mutex m_samplesMutex;
	std::condition_variable m_sampleEvent;
	std::condition_variable m_sampleProcessedEvent;
	std::thread m_processThread;

	Limitless::SharedIImageSampleSet m_currentSampleSet;

//	size_t m_outputWidth;
//	size_t m_outputHeight;
//	size_t m_outputChannels;
};

namespace Limitless{namespace traits
{
	template<> struct type<Synchronize>
	{
		static FilterType get()
		{return Filter;}
	};
	template<> struct category<Synchronize>
	{
		static std::string get()
		{return "connector";}
	};
}}//Limitless::traits

#endif //_Synchronize_h_
