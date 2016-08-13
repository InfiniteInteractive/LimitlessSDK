#include "AudioMixer.h"
#include "AudioMixerView.h"

#include "QtComponents/QtPluginView.h"
#include "Media/MediaPad.h"
#include "Media/IAudioSample.h"

#include <boost/foreach.hpp>

using namespace Limitless;

AudioMixer::AudioMixer(std::string name, SharedMediaFilter parent):
MediaAutoRegister(name, parent),
m_audioMixerView(nullptr),
m_sinkIndex(0),
m_sourceIndex(0)
{
}

AudioMixer::~AudioMixer()
{

}

bool AudioMixer::initialize(const Attributes &attributes)
{
	m_audioSampleId=MediaSampleFactory::getTypeId("IAudioSample");

    addSink();
    addSource();
//	addSinkPad("Sink", "[{\"mime\":\"audio/raw\"}]");
//	addSourcePad("Source", "[{\"mime\":\"audio/raw\"}]");

    
	return true;
}

SharedPluginView AudioMixer::getView()
{
//	return SharedPluginView();

	if(m_view == SharedPluginView())
	{
		m_audioMixerView=new AudioMixerView(this);

		m_view.reset(new QtPluginView(m_audioMixerView));
	}
	return m_view;
}

void AudioMixer::addSink()
{
    std::string padName="Sink"+std::to_string(m_sinkIndex++);
	Limitless::SharedMediaPad pad=addSinkPad(padName, "[{\"mime\":\"audio/raw\"}]");
    InputInfo inputInfo;

    inputInfo.name=padName;
    inputInfo.intputPad=pad;
    inputInfo.mute=false;
    
    Limitless::SharedMediaPads sourcePads=getSourcePads();

    for(size_t i=0; i<sourcePads.size(); ++i)
    {
        Limitless::SharedMediaPad sourcePad=sourcePads[i];
        
        OutputInfo outputInfo;

        outputInfo.pad=sourcePad;
        outputInfo.mute=false;
        outputInfo.level=0.0f;

        inputInfo.outputs.push_back(outputInfo);
    }

    m_mixInfo.push_back(inputInfo);
}

void AudioMixer::removeSink(Limitless::SharedMediaPad mediaPad)
{
}

void AudioMixer::addSource()
{
    std::string padName="Source"+std::to_string(m_sourceIndex++);
    Limitless::SharedMediaPad pad=addSourcePad("Source", "[{\"mime\":\"audio/raw\"}]");

    for(size_t i=0; i<m_mixInfo.size(); ++i)
    {
        InputInfo &inputInfo=m_mixInfo[i];
        OutputInfo outputInfo;

        outputInfo.pad=pad;
        outputInfo.mute=false;
        outputInfo.level=0.0f;

        inputInfo.outputs.push_back(outputInfo);
    }
}

void AudioMixer::removeSource(Limitless::SharedMediaPad mediaPad)
{

}

bool AudioMixer::processSample(SharedMediaPad sinkPad, SharedMediaSample sample)
{
	if(sample->isType(m_audioSampleId))
	{
//		if(m_audioMixerView != nullptr)
//		{
//			Limitless::SharedIAudioSample audioSample=boost::dynamic_pointer_cast<Limitless::IAudioSample>(sample);
//			m_audioMixerView->processSample(audioSample);
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

void AudioMixer::onLinked(Limitless::SharedMediaPad pad, Limitless::SharedMediaPad filterPad)
{
    if(m_audioMixerView == nullptr)
        return;

    m_audioMixerView->updateChannelMatrix();
}