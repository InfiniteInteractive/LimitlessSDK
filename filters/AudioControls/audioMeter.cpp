#include "audioMeter.h"
#include "AudioMeterView.h"

#include "QtComponents/QtPluginView.h"
#include "Media/MediaPad.h"
#include "Media/IAudioSample.h"

#include <boost/foreach.hpp>

using namespace Limitless;

AudioMeter::AudioMeter(std::string name, SharedMediaFilter parent):
MediaAutoRegister(name, parent),
m_audioMeterView(nullptr)
{
}

AudioMeter::~AudioMeter()
{

}

bool AudioMeter::initialize(const Attributes &attributes)
{
	m_iAudioSampleId=MediaSampleFactory::getTypeId("IAudioSample");

	addSinkPad("Sink", "[{\"mime\":\"audio/raw\"}]");
	addSourcePad("Source", "[{\"mime\":\"audio/raw\"}]");
	return true;
}

SharedPluginView AudioMeter::getView()
{
//	return SharedPluginView();

	if(m_view == SharedPluginView())
	{
		m_audioMeterView=new AudioMeterView();

		m_view.reset(new QtPluginView(m_audioMeterView));
	}
	return m_view;
}

bool AudioMeter::processSample(SharedMediaPad sinkPad, SharedMediaSample sample)
{
	if(sample->isType(m_iAudioSampleId))
	{
		if(m_audioMeterView != nullptr)
		{
			Limitless::SharedIAudioSample audioSample=boost::dynamic_pointer_cast<Limitless::IAudioSample>(sample);
			m_audioMeterView->processSample(audioSample);
		}
	}

	pushSample(sample);
	return true;
}

IMediaFilter::StateChange AudioMeter::onReady()
{
	return SUCCESS;
}

IMediaFilter::StateChange AudioMeter::onPaused()
{
	return SUCCESS;
}

IMediaFilter::StateChange AudioMeter::onPlaying()
{
	return SUCCESS;
}

bool AudioMeter::onAcceptMediaFormat(SharedMediaPad pad, SharedMediaFormat format)
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

void AudioMeter::onLinkFormatChanged(SharedMediaPad pad, SharedMediaFormat format)
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