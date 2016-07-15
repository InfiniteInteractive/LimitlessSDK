#ifndef _Limitless_TeeFilter_h
#define _Limitless_TeeFilter_h

#include "Media/MediaPluginFactory.h"
#include "Media/IMediaFilter.h"

#include <queue>
#include <boost/thread.hpp>
#include <boost/function.hpp>

class TeeFilter:public Limitless::MediaAutoRegister<TeeFilter, Limitless::IMediaFilter>
{
public:
	TeeFilter(std::string name, Limitless::SharedMediaFilter parent);
	~TeeFilter();

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
		PadThread(Limitless::SharedMediaPad pad):pad(pad){};
		~PadThread(){}
		
		void startThread(boost::function<void (Limitless::SharedMediaPad pad)> threadFunction){thread.reset(new boost::thread(threadFunction, pad));}

		bool operator==(const Limitless::SharedMediaPad &thatPad) const{return (pad == thatPad);}
		Limitless::SharedMediaPad pad; 
		boost::shared_ptr<boost::thread> thread;
		std::queue<Limitless::SharedMediaSample> sampleQueue;
	};
	typedef std::shared_ptr<PadThread> SharedPadThread;
	typedef std::vector<SharedPadThread> SharedPadThreads;

	Limitless::SharedMediaFormat m_outputFormat;

	boost::mutex m_sampleQueueMutex;
	boost::condition_variable m_sampleEvent;
	boost::condition_variable m_sampleProcessEvent;
	SharedPadThreads m_threads;

	unsigned int m_lastSequence;
	int m_sourceCount;
};

namespace Limitless{namespace traits
{
	template<> struct type<TeeFilter>
	{
		static FilterType get()
		{return Filter;}
	};
	template<> struct category<TeeFilter>
	{
		static std::string get()
		{return "connector";}
	};
}}//Limitless::traits

#endif // _Limitless_TeeFilter_h
