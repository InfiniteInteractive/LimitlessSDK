#ifndef AudioMixer_H
#define AudioMixer_H

#include "Media/MediaPluginFactory.h"
#include "Media/IMediaFilter.h"
#include "audioControls_global.h"

#include "AudioMixerView.h"

#include <queue>

struct OutputInfo
{
	Limitless::SharedMediaPad pad;
};

struct InputInfo
{
	std::string name;
	Limitless::SharedMediaPad pad;

	unsigned int sampleFrequency;

	std::vector<OutputInfo> outputs;
};

class AudioControls_Export AudioMixer:public Limitless::MediaAutoRegister<AudioMixer, Limitless::IMediaFilter>
{
public:
	AudioMixer(std::string name, Limitless::SharedMediaFilter parent);
	~AudioMixer();

	virtual bool initialize(const Limitless::Attributes &attributes);
	virtual bool shutdown(){return true;}

	virtual Limitless::SharedPluginView getView();

	virtual bool processSample(Limitless::SharedMediaPad sinkPad, Limitless::SharedMediaSample sample);

	void addSink();
	void removeSink(Limitless::SharedMediaPad mediaPad);
	void addSource();
	void removeSource(Limitless::SharedMediaPad mediaPad);

protected:
	//IMediaFilter
	virtual StateChange onReady();
	virtual StateChange onPaused();
	virtual StateChange onPlaying();

	bool onAcceptMediaFormat(Limitless::SharedMediaPad pad, Limitless::SharedMediaFormat format);
	void onLinkFormatChanged(Limitless::SharedMediaPad pad, Limitless::SharedMediaFormat format);

private:
	Limitless::SharedPluginView m_view;
	AudioMixerView *m_AudioMixerView;

	size_t m_iAudioSampleId;
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
