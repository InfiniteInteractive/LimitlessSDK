#ifndef _CombineFilter_h_
#define _CombineFilter_h_

#include "Media/MediaPluginFactory.h"
#include "Media/IMediaFilter.h"

//#include <boost/thread.hpp>
#include <deque>
#include <mutex>
#include <thread>
#include <condition_variable>

class CombineFilter:public Limitless::MediaAutoRegister<CombineFilter, Limitless::IMediaFilter>
{
public:
	CombineFilter(std::string name, Limitless::SharedMediaFilter parent);
	~CombineFilter();

	virtual bool initialize(const Limitless::Attributes &attributes);
	virtual bool shutdown(){return true;}

	virtual Limitless::SharedPluginView getView();

	virtual bool processSample(Limitless::SharedMediaPad sinkPad, Limitless::SharedMediaSample sample);

protected:
	//IMediaFilter
	virtual StateChange onReady();
	virtual StateChange onPaused();
	virtual StateChange onPlaying();

	bool onAcceptMediaFormat(Limitless::SharedMediaPad pad, Limitless::SharedMediaFormat format);
	void onLinkFormatChanged(Limitless::SharedMediaPad pad, Limitless::SharedMediaFormat format);

	void processSourceSample();

private:
	int m_sinkCount;
	bool m_firstSample;
	Limitless::SharedMediaFormat m_outputFormat;
	
	size_t m_imageSetSampleId;

	std::vector<Limitless::SharedMediaSampleQueue> m_samples;

	std::mutex m_samplesMutex;
	std::condition_variable m_sampleEvent;
	std::thread m_processThread;

	size_t m_outputWidth;
	size_t m_outputHeight;
	size_t m_outputChannels;
};

namespace Limitless{namespace traits
{
	template<> struct type<CombineFilter>
	{
		static FilterType get()
		{return Filter;}
	};
	template<> struct category<CombineFilter>
	{
		static std::string get()
		{return "connector";}
	};
}}//Limitless::traits

#endif //_CombineFilter_h_
