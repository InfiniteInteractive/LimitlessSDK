#include "AudioMeterView.h"

AudioMeterView::AudioMeterView(QWidget *parent):
QWidget(parent),
m_channels(0)
{
	ui.setupUi(this);

	connect(this, SIGNAL(setMeterValues(std::vector<float>)), this, SLOT(onMeterValues(std::vector<float>)), Qt::QueuedConnection);
}

AudioMeterView::~AudioMeterView()
{

}

void AudioMeterView::initMeter(int channels)
{
	QGridLayout *layout=new QGridLayout;

	m_vuMeters.clear();
	for(size_t i=0; i<channels; ++i)
	{
		Limitless::VuMeter *vuMeter=new Limitless::VuMeter(this);

		m_vuMeters.push_back(vuMeter);
		layout->addWidget(vuMeter, 0, i);
		vuMeter->show();
	}

	QSpacerItem *spacer=new QSpacerItem(10, 10, QSizePolicy::Minimum, QSizePolicy::Expanding);

	layout->addItem(spacer, 0, channels);

	ui.audioMeters->setLayout(layout);
	m_channels=channels;
}

void AudioMeterView::onMeterValues(std::vector<float> values)
{
	if(m_channels != values.size())
		initMeter(values.size());

	for(size_t i=0; i<values.size(); ++i)
	{
		//convert dB to 0.0 - 1.0
//		if(values[i] < 0.0f)
			values[i]=((values[i]+20.0f)/20.0f);
//		else
//			values[i]+=(values[i]/5.0f)*0.25f+0.75f;

		m_vuMeters[i]->setValue(values[i]);
	}
}

void AudioMeterView::processSample(Limitless::SharedIAudioSample sample)
{
//	if(m_channels != sample->channels())
//		initMeter(sample->channels());

	std::vector<float> audioAverages;
	Limitless::AudioSampleFormat audioFormat=sample->format();

//	if(audioFormat == Limitless::AudioSampleFormat::UInt8)
//		audioAverages=averageSamples<uint8_t>(sample->buffer(), sample->samples(), sample->channels());
//	else if(audioFormat == Limitless::AudioSampleFormat::Int16)
//		audioAverages=averageSamples<int16_t>(sample->buffer(), sample->samples(), sample->channels());
//	else if(audioFormat == Limitless::AudioSampleFormat::Int32)
//		audioAverages=averageSamples<int32_t>(sample->buffer(), sample->samples(), sample->channels());
//	else if(audioFormat == Limitless::AudioSampleFormat::Float)
//		audioAverages=averageSamples<float>(sample->buffer(), sample->samples(), sample->channels());
//	else if(audioFormat == Limitless::AudioSampleFormat::Double)
//		audioAverages=averageSamples<double>(sample->buffer(), sample->samples(), sample->channels());
//	else if(audioFormat == Limitless::AudioSampleFormat::UInt8P)
//		audioAverages=averagePlanarSamples<uint8_t>(sample->buffer(), sample->samples(), sample->channels());
//	else if(audioFormat == Limitless::AudioSampleFormat::Int16P)
//		audioAverages=averagePlanarSamples<int16_t>(sample->buffer(), sample->samples(), sample->channels());
//	else if(audioFormat == Limitless::AudioSampleFormat::Int32P)
//		audioAverages=averagePlanarSamples<int32_t>(sample->buffer(), sample->samples(), sample->channels());
//	else if(audioFormat == Limitless::AudioSampleFormat::FloatP)
//		audioAverages=averagePlanarSamples<float>(sample->buffer(), sample->samples(), sample->channels());
//	else if(audioFormat == Limitless::AudioSampleFormat::DoubleP)
//		audioAverages=averagePlanarSamples<double>(sample->buffer(), sample->samples(), sample->channels());

	if(audioFormat == Limitless::AudioSampleFormat::UInt8)
		audioAverages=peakSamples<uint8_t>(sample->buffer(), sample->samples(), sample->channels());
	else if(audioFormat == Limitless::AudioSampleFormat::Int16)
		audioAverages=peakSamples<int16_t>(sample->buffer(), sample->samples(), sample->channels());
	else if(audioFormat == Limitless::AudioSampleFormat::Int32)
		audioAverages=peakSamples<int32_t>(sample->buffer(), sample->samples(), sample->channels());
	else if(audioFormat == Limitless::AudioSampleFormat::Float)
		audioAverages=peakSamples<float>(sample->buffer(), sample->samples(), sample->channels());
	else if(audioFormat == Limitless::AudioSampleFormat::Double)
		audioAverages=peakSamples<double>(sample->buffer(), sample->samples(), sample->channels());
	else if(audioFormat == Limitless::AudioSampleFormat::UInt8P)
		audioAverages=peakPlanarSamples<uint8_t>(sample->buffer(), sample->samples(), sample->channels());
	else if(audioFormat == Limitless::AudioSampleFormat::Int16P)
		audioAverages=peakPlanarSamples<int16_t>(sample->buffer(), sample->samples(), sample->channels());
	else if(audioFormat == Limitless::AudioSampleFormat::Int32P)
		audioAverages=peakPlanarSamples<int32_t>(sample->buffer(), sample->samples(), sample->channels());
	else if(audioFormat == Limitless::AudioSampleFormat::FloatP)
		audioAverages=peakPlanarSamples<float>(sample->buffer(), sample->samples(), sample->channels());
	else if(audioFormat == Limitless::AudioSampleFormat::DoubleP)
		audioAverages=peakPlanarSamples<double>(sample->buffer(), sample->samples(), sample->channels());

	if(!audioAverages.empty())
	{
		emit(setMeterValues(audioAverages));
//			m_vuMeters[i]->setValue(audioAverages[i]);
	}
}