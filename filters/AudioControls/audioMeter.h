#ifndef AudioMeter_H
#define AudioMeter_H

#include "Media/MediaPluginFactory.h"
#include "Media/IMediaFilter.h"
#include "audioControls_global.h"

#include "AudioMeterView.h"

#include <queue>

class AudioControls_Export AudioMeter:public Limitless::MediaAutoRegister<AudioMeter, Limitless::IMediaFilter>
{
public:
	AudioMeter(std::string name, Limitless::SharedMediaFilter parent);
	~AudioMeter();

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

private:
	Limitless::SharedPluginView m_view;
	AudioMeterView *m_audioMeterView;

	size_t m_iAudioSampleId;
};

namespace Limitless{namespace traits
{
	template<> struct type<AudioMeter>
	{
		static FilterType get()
		{return Filter;}
	};
	template<> struct category<AudioMeter>
	{
		static std::string get()
		{return "filter";}
	};
}}//Limitless::traits

#endif // AudioMeter_H
