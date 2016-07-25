#include "AudioMixer.h"
#include "AudioMixerView.h"

#include "QtComponents/QtPluginView.h"
#include "Media/MediaPad.h"
#include "Media/IAudioSample.h"

#include <boost/foreach.hpp>

using namespace Limitless;

AudioMixer::AudioMixer(std::string name, SharedMediaFilter parent):
MediaAutoRegister(name, parent),
m_AudioMixerView(nullptr)
{
}

AudioMixer::~AudioMixer()
{

}

bool AudioMixer::initialize(const Attributes &attributes)
{
	m_iAudioSampleId=MediaSampleFactory::getTypeId("IAudioSample");

	addSinkPad("Sink", "[{\"mime\":\"audio/raw\"}]");
	addSourcePad("Source", "[{\"mime\":\"audio/raw\"}]");
	return true;
}

SharedPluginView AudioMixer::getView()
{
//	return SharedPluginView();

	if(m_view == SharedPluginView())
	{
		m_AudioMixerView=new AudioMixerView(this);

		m_view.reset(new QtPluginView(m_AudioMixerView));
	}
	return m_view;
}

void AudioMixer::addSink()
{
	addSinkPad("Sink", "[{\"mime\":\"audio/raw\"}]");
}

void AudioMixer::removeSink(Limitless::SharedMediaPad mediaPad)
{
}

void AudioMixer::addSource()
{
	addSourcePad("Source", "[{\"mime\":\"audio/raw\"}]");
}

void AudioMixer::removeSource(Limitless::SharedMediaPad mediaPad)
{

}

bool AudioMixer::processSample(SharedMediaPad sinkPad, SharedMediaSample sample)
{
	if(sample->isType(m_iAudioSampleId))
	{
//		if(m_AudioMixerView != nullptr)
//		{
//			Limitless::SharedIAudioSample audioSample=boost::dynamic_pointer_cast<Limitless::IAudioSample>(sample);
//			m_AudioMixerView->processSample(audioSample);
//		}
	}

//	pushSample(sample);
	return true;
}

IMediaFilter::StateChange AudioMixer::onReady()
{
	return SUCCESS;
}

IMediaFilter::StateChange AudioMixer::onPaused()
{
	return SUCCESS;
}

IMediaFilter::StateChange AudioMixer::onPlaying()
{
	return SUCCESS;
}

bool AudioMixer::onAcceptMediaFormat(SharedMediaPad pad, SharedMediaFormat format)
{
	if(pad->type() == MediaPad::SINK)
	{
		bool accept=false;

		if(format->exists("mime"))
		{
			std::string mimeType=format->attribute("mime")->toString();

			if(mimeType == "audio/raw")
				accept=true;
		}
		return accept;
	}
	else
		return true;
	return false;
}

void AudioMixer::onLinkFormatChanged(SharedMediaPad pad, SharedMediaFormat format)
{
	if(pad->type() == MediaPad::SINK)
	{
		if(!format->exists("mime"))
			return;
		if((format->attribute("mime")->toString() != "audio/raw"))
			return;

		MediaFormat sourceFormat(*format);
		SharedMediaPads sourcePads=getSourcePads();

		BOOST_FOREACH(SharedMediaPad &sourcePad, sourcePads)
		{
			sourcePad->setFormat(sourceFormat);
		}
	}
}