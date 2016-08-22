#include "AudioMixerInputView.h"
#include "AudioMixerView.h"

#include "AudioMeterView.h"

#include "medialib/audioConversion.h"

AudioMixerInputView::AudioMixerInputView(InputView *inputView, QWidget *parent):
m_inputView(inputView),
m_channels(0),
QWidget(parent)
{
	ui.setupUi(this);

//    new VuMeter(ui.vuMeter)
//    ui.vuMeter
	connect(this, SIGNAL(setMeterValues(std::vector<float>)), this, SLOT(onMeterValues(std::vector<float>)), Qt::QueuedConnection);
	connect(this, SIGNAL(setVerticalMeters(bool)), this, SLOT(onVerticalMeter(bool)), Qt::QueuedConnection);

    update();
}

AudioMixerInputView::~AudioMixerInputView()
{

}

void AudioMixerInputView::update()
{
    ui.nameLabel->setText(QString::fromStdString(m_inputView->inputInfo.name));

    Limitless::SharedMediaPad intputPad=m_inputView->inputInfo.pad;
    QString linkLabel="Not Linked";

    if(intputPad->linked())
    {
        Limitless::SharedMediaPad &sourcePad=intputPad->linkedPad();

        if(sourcePad)
        {
            Limitless::SharedMediaFilter sourceFilter=sourcePad->parent();

            if(sourceFilter)
                linkLabel=QString("%1(%2)").arg(QString::fromStdString(sourceFilter->instance())).arg(QString::fromStdString(sourcePad->name()));
        }
    }
    ui.linkLabel->setText(linkLabel);
}

void AudioMixerInputView::initMeter(int channels)
{
	QGridLayout *layout=qobject_cast<QGridLayout *>(ui.audioMeters->layout());

	if(layout==nullptr)
		return;

	while(QLayoutItem *item=layout->takeAt(0))
	{
		QSpacerItem *spacer=dynamic_cast<QSpacerItem *>(item);

		if(item!=nullptr)
			delete spacer;
		else
		{
			item->widget()->hide();
			item->widget()->setParent(nullptr);
		}
	}

	m_vuMeters.clear();
	for(size_t i=0; i<channels; ++i)
	{
		Limitless::VuMeter *vuMeter=new Limitless::VuMeter(this);

		vuMeter->setVertical(m_verticalMeters);
		m_vuMeters.push_back(vuMeter);

		if(m_verticalMeters)
			layout->addWidget(vuMeter, 0, i);
		else
			layout->addWidget(vuMeter, i, 0);
		vuMeter->show();
	}

	if(m_verticalMeters)
	{
		QSpacerItem *spacer=new QSpacerItem(0, 10, QSizePolicy::Maximum, QSizePolicy::MinimumExpanding);

		layout->addItem(spacer, 0, channels);
	}
	else
	{
		QSpacerItem *spacer=new QSpacerItem(10, 0, QSizePolicy::MinimumExpanding, QSizePolicy::Maximum);

		layout->addItem(spacer, channels, 0);
	}

	//	ui.audioMeters->setLayout(layout);
	m_channels=channels;
}

void AudioMixerInputView::onMeterValues(std::vector<float> values)
{
	if(m_channels!=values.size())
		initMeter(values.size());

	for(size_t i=0; i<values.size(); ++i)
	{
		//convert dB to 0.0 - 1.0
		//		if(values[i] < 0.0f)
		//			values[i]=((values[i]+20.0f)/20.0f);
		//		else
		//			values[i]+=(values[i]/5.0f)*0.25f+0.75f;

		//		values[i]=((values[i]+1.0f)/2.0f);
		values[i]=((medialib::convertToDb(values[i])+20.0f)/20.0f);

		m_vuMeters[i]->setValue(values[i]);
	}
}

void AudioMixerInputView::processSample(Limitless::SharedIAudioSample sample)
{
	std::vector<float> audioAverages;
	Limitless::AudioSampleFormat audioFormat=sample->format();

	if(audioFormat==Limitless::AudioSampleFormat::UInt8)
		audioAverages=peakSamples<uint8_t>(sample->buffer(), sample->samples(), sample->channels());
	else if(audioFormat==Limitless::AudioSampleFormat::Int16)
		audioAverages=peakSamples<int16_t>(sample->buffer(), sample->samples(), sample->channels());
	else if(audioFormat==Limitless::AudioSampleFormat::Int32)
		audioAverages=peakSamples<int32_t>(sample->buffer(), sample->samples(), sample->channels());
	else if(audioFormat==Limitless::AudioSampleFormat::Float)
		audioAverages=peakSamples<float>(sample->buffer(), sample->samples(), sample->channels());
	else if(audioFormat==Limitless::AudioSampleFormat::Double)
		audioAverages=peakSamples<double>(sample->buffer(), sample->samples(), sample->channels());
	else if(audioFormat==Limitless::AudioSampleFormat::UInt8P)
		audioAverages=peakPlanarSamples<uint8_t>(sample->buffer(), sample->samples(), sample->channels());
	else if(audioFormat==Limitless::AudioSampleFormat::Int16P)
		audioAverages=peakPlanarSamples<int16_t>(sample->buffer(), sample->samples(), sample->channels());
	else if(audioFormat==Limitless::AudioSampleFormat::Int32P)
		audioAverages=peakPlanarSamples<int32_t>(sample->buffer(), sample->samples(), sample->channels());
	else if(audioFormat==Limitless::AudioSampleFormat::FloatP)
		audioAverages=peakPlanarSamples<float>(sample->buffer(), sample->samples(), sample->channels());
	else if(audioFormat==Limitless::AudioSampleFormat::DoubleP)
		audioAverages=peakPlanarSamples<double>(sample->buffer(), sample->samples(), sample->channels());

	if(!audioAverages.empty())
		emit(setMeterValues(audioAverages));
}

void AudioMixerInputView::setVertical(bool vertical)
{
	emit(setVerticalMeters(vertical));
}

void AudioMixerInputView::onVerticalMeter(bool value)
{
	m_verticalMeters=value;

	initMeter(m_channels);
	for(auto vuMeter:m_vuMeters)
	{
		vuMeter->setVertical(value);
	}
}