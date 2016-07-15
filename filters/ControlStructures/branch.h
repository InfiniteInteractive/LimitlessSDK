#ifndef _BranchFilter_h
#define _BranchFilter_h

#include "Media/MediaPluginFactory.h"
#include "Media/IMediaFilter.h"

#include <queue>
#include <boost/thread.hpp>
#include <boost/function.hpp>

class BranchFilter:public Limitless::MediaAutoRegister<BranchFilter, Limitless::IMediaFilter>
{
public:
	BranchFilter(std::string name, Limitless::SharedMediaFilter parent);
	~BranchFilter();

	virtual bool initialize(const Limitless::Attributes &attributes);
	virtual bool shutdown(){return true;}

	virtual Limitless::SharedPluginView getView();

	virtual bool processSample(Limitless::SharedMediaPad sinkPad, Limitless::SharedMediaSample sample);

protected:
	//IMediaFilter
	virtual StateChange onReady();
	virtual StateChange onPaused();
	virtual StateChange onPlaying();

	virtual bool onAcceptMediaFormat(Limitless::SharedMediaPad pad, Limitless::SharedMediaFormat format);
	virtual void onLinkFormatChanged(Limitless::SharedMediaPad pad, Limitless::SharedMediaFormat format);

	virtual void onLinked(Limitless::SharedMediaPad pad, Limitless::SharedMediaPad filterPad);
private:
	void processSourceSample(Limitless::SharedMediaPad sourcePad);

	class PadThread
	{
	public:
		PadThread(Limitless::SharedMediaPad pad, boost::function<void (Limitless::SharedMediaPad pad)> threadFunction):pad(pad), thread(new boost::thread(threadFunction, pad)){};
		~PadThread(){}
		
		bool operator==(const Limitless::SharedMediaPad &thatPad) const{return (pad == thatPad);}
		Limitless::SharedMediaPad pad; 
		boost::shared_ptr<boost::thread> thread;
	};
	typedef std::vector<PadThread> PadThreads;

	Limitless::SharedMediaFormat m_outputFormat;

	std::queue<Limitless::SharedMediaSample> m_sampleQueue;
	boost::mutex m_sampleQueueMutex;
	boost::condition_variable m_sampleEvent;
	boost::condition_variable m_sampleProcessEvent;
	PadThreads m_threads;

	unsigned int m_lastSequence;
	int m_sourceCount;
};

namespace Limitless{namespace traits
{
	template<> struct type<BranchFilter>
	{
		static FilterType get()
		{return Filter;}
	};
	template<> struct category<BranchFilter>
	{
		static std::string get()
		{return "connector";}
	};
}}//Limitless::traits

#endif // IMAGEVIEWER_H
