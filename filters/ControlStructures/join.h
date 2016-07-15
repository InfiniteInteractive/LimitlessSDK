#ifndef _JoinFilter_h_
#define _JoinFilter_h_

#include "Media/MediaPluginFactory.h"
#include "Media/IMediaFilter.h"

#include <boost/thread.hpp>

class JoinFilter:public Limitless::MediaAutoRegister<JoinFilter, Limitless::IMediaFilter>
{
public:
	JoinFilter(std::string name, Limitless::SharedMediaFilter parent);
	~JoinFilter();

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
	unsigned int m_currentSequence;
	Limitless::SharedMediaFormat m_outputFormat;
	
	Limitless::SharedMediaSamples m_samples;
	boost::mutex m_samplesMutex;
	boost::condition_variable m_sampleEvent;
	boost::thread m_processThread;
};

namespace Limitless{namespace traits
{
	template<> struct type<JoinFilter>
	{
		static FilterType get()
		{return Filter;}
	};
	template<> struct category<JoinFilter>
	{
		static std::string get()
		{return "connector";}
	};
}}//Limitless::traits

#endif //_JoinFilter_h_
