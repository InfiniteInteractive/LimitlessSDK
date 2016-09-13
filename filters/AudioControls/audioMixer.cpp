#include "AudioMixer.h"
#include "AudioMixerView.h"
#include "medialibHelpers.h"

#include "QtComponents/QtPluginView.h"
#include "Media/MediaPad.h"
#include "Media/AudioSample.h"

#include "medialib/audioBufferWrapper.h"
#include "medialib/simpleAudioBuffer.h"
#include "medialib/audioChannels.h"

#include <boost/foreach.hpp>

using namespace Limitless;

AudioMixer::AudioMixer(std::string name, SharedMediaFilter parent):
MediaAutoRegister(name, parent),
m_audioMixerView(nullptr),
m_sinkIndex(0),
m_sourceIndex(0),
m_linkedPads(0)
{
}

AudioMixer::~AudioMixer()
{

}

bool AudioMixer::initialize(const Attributes &attributes)
{
	m_iaudioSampleId=MediaSampleFactory::getTypeId("IAudioSample");
	m_audioSampleId=MediaSampleFactory::getTypeId("AudioSample");

    
	addSource(); 
	addSink();
//	addSinkPad("Sink", "[{\"mime\":\"audio/raw\"}]");
//	addSourcePad("Source", "[{\"mime\":\"audio/raw\"}]");
	std::vector<std::string> methods;

	methods.push_back("Clipping");
	methods.push_back("Linear Attenuation");
	methods.push_back("Linear DRC");
	methods.push_back("Logrithmic DRC");

	m_method=medialib::MixMethod::LinearAttenuation;
	addAttribute("method", methods[1], methods);
	m_drcThreshold=0.6f;
	addAttribute("drcThreshold", m_drcThreshold);
    
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
//    std::string padName="Sink"+std::to_string(m_sinkIndex++);
//	Limitless::SharedMediaPad pad=addSinkPad(padName, "[{\"mime\":\"audio/raw\"}]");
//    InputInfo inputInfo;
//
//    inputInfo.name=padName;
//    inputInfo.intputPad=pad;
//    inputInfo.mute=false;
//    
//    Limitless::SharedMediaPads sourcePads=getSourcePads();
//
//    for(size_t i=0; i<sourcePads.size(); ++i)
//    {
//        Limitless::SharedMediaPad sourcePad=sourcePads[i];
//        
//        OutputInfo outputInfo;
//
//        outputInfo.pad=sourcePad;
//        outputInfo.mute=false;
//        outputInfo.level=0.0f;
//
//        inputInfo.outputs.push_back(outputInfo);
//    }
//
//    m_mixInfo.push_back(inputInfo);

	//abuse queue mutex
	std::unique_lock<std::mutex> lock=m_sampleQueue.acquireLock();

	std::string padName="Sink"+std::to_string(m_sinkIndex++);
	Limitless::SharedMediaPad pad=addSinkPad(padName, "[{\"mime\":\"audio/raw\"}]");

	InputInfo inputInfo;

	inputInfo.pad=pad;
	inputInfo.mute=false;

	m_inputs.push_back(inputInfo);
	updateInputOutputMix();
}

void AudioMixer::removeSink(Limitless::SharedMediaPad mediaPad)
{
}

void AudioMixer::addSource()
{
	{
		//abuse queue mutex
		std::unique_lock<std::mutex> lock=m_sampleQueue.acquireLock();

		std::string padName="Source"+std::to_string(m_sourceIndex++);
		Limitless::SharedMediaPad pad=addSourcePad("Source", "[{\"mime\":\"audio/raw\"}]");

		addChannel();
	}

	event("newSourcePad");

//    std::string padName="Source"+std::to_string(m_sourceIndex++);
//    Limitless::SharedMediaPad pad=addSourcePad("Source", "[{\"mime\":\"audio/raw\"}]");
//
//    for(size_t i=0; i<m_mixInfo.size(); ++i)
//    {
//        InputInfo &inputInfo=m_mixInfo[i];
//        OutputInfo outputInfo;
//
//        outputInfo.pad=pad;
//        outputInfo.mute=false;
//        outputInfo.level=0.0f;
//
//        inputInfo.outputs.push_back(outputInfo);
//		event("newSourcePad");
//    }
}

void AudioMixer::addChannel()
{
	std::unique_lock<std::mutex> lock(m_processingMutex);

	Limitless::SharedMediaPads sourcePads=getSourcePads();

	OutputInfo outputInfo;
	std::string padName="Channel"+std::to_string(m_mixInfo.size());

	outputInfo.name=padName;
	outputInfo.pad=sourcePads[0];
	outputInfo.channel=m_mixInfo.size();
	outputInfo.mute=false;

	m_mixInfo.push_back(outputInfo);
	updateInputOutputMix();
}

void AudioMixer::removeChannel(size_t index)
{}

void AudioMixer::updateInputOutputMix()
{
	for(size_t i=0; i<m_mixInfo.size(); ++i)
	{
		auto &inputs=m_mixInfo[i].inputs;

		for(size_t j=0; j<m_inputs.size(); ++j)
		{
			InputInfo &inputInfo=m_inputs[j];

			if(j<inputs.size())
			{
				if(inputs[j].inputInfo==&inputInfo)
					continue;

				auto iter=std::find_if(inputs.begin()+j, inputs.end(), [&inputInfo](const auto &input){return input.inputInfo==&inputInfo;});

				if(iter!=inputs.end())
				{
					inputs.erase(inputs.begin()+j, iter);
					continue;
				}
			}

			InputOutputMix ioMix;

			ioMix.inputInfo=&inputInfo;
			ioMix.mute=false;
//			ioMix.level=0.0f;

			inputs.insert(inputs.begin()+j, ioMix);
		}

		while(inputs.size()>m_inputs.size())
			inputs.pop_back();
	}
}

void AudioMixer::removeSource(Limitless::SharedMediaPad mediaPad)
{

}

bool AudioMixer::processSample(SharedMediaPad sinkPad, SharedMediaSample sample)
{
	if(!sample->isType(m_iaudioSampleId))
		return false;

	size_t padIndex=sinkPadIndex(sinkPad);

	if(padIndex!=IMediaFilter::InvalidPin)
	{
		Limitless::SharedIAudioSample audioSample=boost::dynamic_pointer_cast<Limitless::IAudioSample>(sample);
		PadSample padSample(padIndex, audioSample);

		m_sampleQueue.push_back(padSample);
	}

//	pushSample(sample);
	return true;
}

void AudioMixer::processSourceSample()
{
	while(true)
	{
		PadSample padSample=m_sampleQueue.pop_front();

		if(padSample.index==IMediaFilter::InvalidPin)
		{
			//EventQueu::breakWait must have been called
			break;
		}

		if(padSample.index>=m_padSamples.size())
			m_padSamples.resize(padSample.index+1);

		m_padSamples[padSample.index].push_back(padSample.sample);

		mixSamples();
	}
}

void AudioMixer::mixSamples()
{
	if(m_padSamples.size()<1)
		return;

	SharedIAudioSample sample=m_padSamples[0].front();
	
	m_padSamples[0].pop_front();

	medialib::AudioBufferWrapper wrapper(convertFormat(sample->format()), sample->channels(), sample->samples(), sample->sampleRate(),
						sample->buffer(), sample->size());

	Limitless::SharedAudioSample outSample=newSampleType<Limitless::AudioSample>(m_audioSampleId);
	
	outSample->resize(sample->format(), 2, sample->samples(), sample->sampleRate());

	medialib::AudioBufferWrapper outWrapper(convertFormat(outSample->format()), outSample->channels(), outSample->samples(), outSample->sampleRate(),
		outSample->buffer(), outSample->size());

	std::vector<float> channelLevels(sample->channels());

	for(size_t i=0; i<sample->channels(); i++)
		channelLevels[i]=0.0;

	channelLevels[0]=1.0;
	channelLevels[1]=1.0;

	medialib::copyAudioBufferChannel(wrapper, 0, outWrapper, 0);
	medialib::copyAudioBufferChannel(wrapper, 1, outWrapper, 1);

	outSample->copyHeader(sample);
	pushSample(m_mixInfo[0].pad, outSample);
}

//void AudioMixer::mixSamples()
//{
//	std::unique_lock<std::mutex> lock(m_processingMutex);
//
//	//@TODO: need to sync samples, taking shortcut for the moment
//	std::vector<size_t> activePads;
//	std::vector<SharedIAudioSample> inputSamples(m_padSamples.size());
//
//	for(size_t i=0; i<m_padSamples.size(); ++i)
//	{
//		if(m_padSamples[i].empty())
//			continue;
//
//		activePads.push_back(i);
//		inputSamples[i]=m_padSamples[i].front();
//		m_padSamples[i].pop_front();
//	}
//
//	if(activePads.size()<m_linkedPads)
//		return;
//
//	std::vector<medialib::SimpleAudioBuffer> channelAudioBuffers(m_linkedPads);
//	std::vector<medialib::SimpleAudioBuffer> mixAudioBuffers(m_mixInfo.size());
//	
////	std::vector<medialib::AudioBufferWrapper> audioBuffers;
//	for(size_t i=0; i<m_mixInfo.size(); ++i)
//	{
//		OutputInfo &outputInfo=m_mixInfo[i];
//		auto &ioMixes=outputInfo.inputs;
//		std::vector<medialib::AudioBuffer> audioBuffers;
//		Limitless::SharedIAudioSample firstSample;
//
//		for(size_t j=0; j<inputSamples.size(); ++j)
//		{
//			if(!inputSamples[j])
//				continue;
//			
//			auto &ioMix=ioMixes[j];
//			auto &channelMixes=ioMix.channelMixes;
//
//			Limitless::SharedIAudioSample &sample=inputSamples[j];
//
//			if(!firstSample)
//				firstSample=sample;
//
//			medialib::AudioBufferWrapper wrapper(convertFormat(sample->format()), sample->channels(), sample->samples(), sample->sampleRate(),
//				sample->buffer(), sample->size());
//
//			std::vector<float> channelLevels(sample->channels());
//			
//			if(channelMixes.size()<sample->channels())
//				channelMixes.resize(sample->channels());
//
//			for(size_t k=0; k<sample->channels(); ++k)
//				channelLevels[k]=channelMixes[k].level;
//
//			channelAudioBuffers[j].alloc(convertFormat(sample->format()), 1, sample->samples(), sample->sampleRate());
//			medialib::mixAudioBufferChannels(wrapper, channelLevels, channelAudioBuffers[j]);
//
//			audioBuffers.push_back(channelAudioBuffers[j]);
//		}		
//
//		if(!firstSample)
//			continue;
//
//		//mix multiple audio buffers to channel
//		mixAudioBuffers[i].alloc(convertFormat(firstSample->format()), 1, firstSample->samples(), firstSample->sampleRate());
//		medialib::mixAudioBuffer(audioBuffers, mixAudioBuffers[i]);
//	}
//
//	if(mixAudioBuffers.empty())
//		return;
//
//	medialib::SimpleAudioBuffer &firstAudioBuffer=mixAudioBuffers[0];
//	std::vector<medialib::AudioBuffer> audioBuffers(mixAudioBuffers.begin(), mixAudioBuffers.end());
//
//	Limitless::SharedAudioSample mixSample=newSampleType<Limitless::AudioSample>(m_audioSampleId);
//
//	mixSample->resize(convertFormat(firstAudioBuffer.format), mixAudioBuffers.size(), firstAudioBuffer.samples, firstAudioBuffer.sampleRate);
//
//	medialib::AudioBufferWrapper wrapper(convertFormat(mixSample->format()), mixSample->channels(), mixSample->samples(), mixSample->sampleRate(),
//		mixSample->buffer(), mixSample->size());
//
//	medialib::combineAudioBufferChannels(audioBuffers, wrapper);
//
//	if(m_audioMixerView!=nullptr)
//	{
//		m_audioMixerView->processSamples(inputSamples, mixSample);
//	}
//
//	mixSample->copyHeader(inputSamples[0]);
//	pushSample(m_mixInfo[0].pad, mixSample);
//}

IMediaFilter::StateChange AudioMixer::onReady()
{
	if(!m_processThread.joinable())
		m_processThread=std::thread(std::bind(&AudioMixer::processSourceSample, this));

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

		if(sourceFormat.exists("channels"))
			sourceFormat.attribute("channels")->fromInt(2);
		else
			sourceFormat.addAttribute("channels", 2);

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

	Limitless::SharedMediaPads sinkPads=getSinkPads();

	size_t linkedPads=0;
	for(auto &sinkPad:sinkPads)
	{
		if(sinkPad->linked())
			++linkedPads;
	}
	
	//abuse m_sampleQueue mutex to hold thread
	{
		std::unique_lock<std::mutex> lock=m_sampleQueue.acquireLock();

		m_linkedPads=linkedPads;
	}
    m_audioMixerView->updateChannelMatrix();
}

void AudioMixer::onAttributeChanged(std::string name, Limitless::SharedAttribute attribute)
{
	if(name=="methods")
	{
		std::string methodName=attribute->toString();

		if(methodName=="Clipping")
			m_method=medialib::MixMethod::Clipping;
		else if(methodName=="Linear Attenuation")
			m_method=medialib::MixMethod::LinearAttenuation;
		else if(methodName=="Linear DRC")
			m_method=medialib::MixMethod::LinearDRC;
		else if(methodName=="Logrithmic DRC")
			m_method=medialib::MixMethod::LogrithmicDRC;
	}
	else if(name=="drcThreshold")
	{
		m_drcThreshold=attribute->toFloat();
	}
}
