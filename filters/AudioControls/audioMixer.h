#ifndef AudioMixer_H
#define AudioMixer_H

#include "Media/MediaPluginFactory.h"
#include "Media/IMediaFilter.h"
#include "Media/IAudioSample.h"
#include "audioControls_global.h"

#include "AudioMixerView.h"
#include "AudioMixerInfo.h"

#include "medialib/audioMix.h"
#include "Utilities/eventQueue.h"

#include <queue>
#include <mutex>
#include <thread>

struct PadSample
{
	PadSample():index(Limitless::IMediaFilter::InvalidPin) {}
	PadSample(size_t index, Limitless::SharedIAudioSample sample):index(index), sample(sample){}

	size_t index;
	Limitless::SharedIAudioSample sample;
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

	InputInfoVector &getInputs() { return m_inputs; }
    MixInfo &getInfo() { return m_mixInfo; }

	void addSink();
	void removeSink(Limitless::SharedMediaPad mediaPad);
	void addSource();
	void removeSource(Limitless::SharedMediaPad mediaPad);

	void addChannel();
	void removeChannel(size_t index);

	void processSourceSample();

protected:
	//IMediaFilter
	virtual StateChange onReady();
	virtual StateChange onPaused();
	virtual StateChange onPlaying();

	virtual bool onAcceptMediaFormat(Limitless::SharedMediaPad pad, Limitless::SharedMediaFormat format);
	virtual void onLinkFormatChanged(Limitless::SharedMediaPad pad, Limitless::SharedMediaFormat format);
    virtual void onLinked(Limitless::SharedMediaPad pad, Limitless::SharedMediaPad filterPad);
	virtual void onAttributeChanged(std::string name, Limitless::SharedAttribute attribute);

private:
	void updateInputOutputMix();
	void mixSamples();

	Limitless::SharedPluginView m_view;
	AudioMixerView *m_audioMixerView;
	
	std::mutex m_processingMutex;
	std::thread m_processThread;
	Limitless::EventQueue<PadSample> m_sampleQueue;
	std::vector<std::deque<Limitless::SharedIAudioSample>> m_padSamples;
	size_t m_linkedPads;

	size_t m_iaudioSampleId;
	size_t m_audioSampleId;

	medialib::MixMethod m_method;
	float m_drcThreshold;

	InputInfoVector m_inputs;
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
