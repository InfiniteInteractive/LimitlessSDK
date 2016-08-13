#ifndef AudioMixer_H
#define AudioMixer_H

#include "Media/MediaPluginFactory.h"
#include "Media/IMediaFilter.h"
#include "audioControls_global.h"

#include "AudioMixerView.h"
#include "AudioMixerInfo.h"

#include <queue>


class AudioControls_Export AudioMixer:public Limitless::MediaAutoRegister<AudioMixer, Limitless::IMediaFilter>
{
public:
	AudioMixer(std::string name, Limitless::SharedMediaFilter parent);
	~AudioMixer();

	virtual bool initialize(const Limitless::Attributes &attributes);
	virtual bool shutdown(){return true;}

	virtual Limitless::SharedPluginView getView();

	virtual bool processSample(Limitless::SharedMediaPad sinkPad, Limitless::SharedMediaSample sample);

    MixInfo getInfo() { return m_mixInfo; }

	void addSink();
	void removeSink(Limitless::SharedMediaPad mediaPad);
	void addSource();
	void removeSource(Limitless::SharedMediaPad mediaPad);

protected:
	//IMediaFilter
	virtual StateChange onReady();
	virtual StateChange onPaused();
	virtual StateChange onPlaying();

	virtual bool onAcceptMediaFormat(Limitless::SharedMediaPad pad, Limitless::SharedMediaFormat format);
	virtual void onLinkFormatChanged(Limitless::SharedMediaPad pad, Limitless::SharedMediaFormat format);
    virtual void onLinked(Limitless::SharedMediaPad pad, Limitless::SharedMediaPad filterPad);

private:
	Limitless::SharedPluginView m_view;
	AudioMixerView *m_audioMixerView;

	size_t m_audioSampleId;

    MixInfo m_mixInfo;
    unsigned int m_sinkIndex;
    unsigned int m_sourceIndex;
};

namespace Limitless{namespace traits
{
	template<> struct type<AudioMixer>
	{
		static FilterType get()
		{return Filter;}
	};
	template<> struct category<AudioMixer>
	{
		static std::string get()
		{return "filter";}
	};
}}//Limitless::traits

#endif // AudioMixer_H
