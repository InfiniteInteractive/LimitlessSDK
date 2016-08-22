#include "AudioMixerOutputView.h"
#include "AudioMixerView.h"

#include "medialib/audioConversion.h"

AudioMixerOutputView::AudioMixerOutputView(OutputView *outputView, QWidget *parent):
m_outputView(outputView),
QWidget(parent)
{
	ui.setupUi(this);

	m_vuMeter=new Limitless::VuMeter(this);
	ui.vuMeter->layout()->addWidget(m_vuMeter);

	connect(this, SIGNAL(setMeterValue(float)), this, SLOT(onMeterValue(float)), Qt::QueuedConnection);
}

AudioMixerOutputView::~AudioMixerOutputView()
{

}

void AudioMixerOutputView::processSample(std::vector<float> &peak)
{
	unsigned int channel=m_outputView->outputInfo.channel;

	if(channel<peak.size())
	{
		emit(setMeterValue(peak[channel]));
	}
}

void AudioMixerOutputView::onMeterValue(float value)
{
	value=((medialib::convertToDb(value)+20.0f)/20.0f);

	m_vuMeter->setValue(value);
}