#include "AudioMixerView.h"
#include "audioMixer.h"

AudioMixerView::AudioMixerView(AudioMixer *audioMixer, QWidget *parent):
m_audioMixer(audioMixer),
QWidget(parent)
{
	ui.setupUi(this);

	connect(this, SIGNAL(updateInputOutput()), this, SLOT(onUpdateInputOutput()), Qt::QueuedConnection);
}

AudioMixerView::~AudioMixerView()
{

}

void AudioMixerView::updateInputOutput()
{
	emit(updateInputOutputSignal());
}

void AudioMixerView::onUpdateInputOutput()
{
//	Limitless::SharedMediaPads sinkPads=m_audioMixer->getSinkPads();
//
//	for(size_t i=0; i<sinkPads.size(); ++i)
//	{
//		Limitless::SharedMediaPad &pad=sinkPads[i];
//		std::string sourceName;
//
//		if(pad->linked())
//		{
//			Limitless::SharedMediaFilters sourceFilters=pad->findUpStream(Limitless::Source);
//
//			if(!sourceFilters.empty())
//			{
//				sourceFilters[0]->instance();
//			}
//		}
//	}
}

void AudioMixerView::on_addInput_clicked(bool checked)
{
	m_audioMixer->addInput();
}

void AudioMixerView::on_addOutput_clicked(bool checked)
{
	m_audioMixer->addOutput();
}

void AudioMixerView::processSample(Limitless::SharedIAudioSample sample)
{
	std::vector<float> audioAverages;
	Limitless::AudioSampleFormat audioFormat=sample->format();

	
}