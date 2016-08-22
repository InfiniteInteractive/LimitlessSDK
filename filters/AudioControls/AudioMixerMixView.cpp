#include "AudioMixerMixView.h"
#include "AudioMixerView.h"

AudioMixerMixView::AudioMixerMixView(MixView *view, QWidget *parent):
m_view(view),
QWidget(parent),
m_channels(0)
{
	ui.setupUi(this);

	connect(this, SIGNAL(updateChannels(unsigned int)), this, SLOT(onUpdateChannels(unsigned int)), Qt::QueuedConnection);
}

AudioMixerMixView::~AudioMixerMixView()
{

}

void AudioMixerMixView::initSliders(int channels)
{
	QGridLayout *layout=qobject_cast<QGridLayout *>(ui.channelLevels->layout());

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

	m_sliders.clear();
	for(size_t i=0; i<channels; ++i)
	{
		QSlider *slider=new QSlider(Qt::Vertical, this);

		slider->setMinimum(0);
		slider->setMaximum(100);
		slider->setProperty("channel", QVariant((int)i));

		connect(slider, SIGNAL(valueChanged(int)), this, SLOT(on_slider_valueChanged(int)));
		m_sliders.push_back(slider);
		layout->addWidget(slider, 0, i);
		slider->show();
	}

	QSpacerItem *spacer=new QSpacerItem(0, 10, QSizePolicy::Maximum, QSizePolicy::MinimumExpanding);

	layout->addItem(spacer, 0, channels);

	m_channels=channels;
}

void AudioMixerMixView::on_slider_valueChanged(int value)
{
//	QSlider *slider=qobject_cast<QSlider *>(this->sender());
	int channel=this->sender()->property("channel").toInt();

	std::vector<ChannelMix> &channelMixes=*m_view->channelMixes;

	if(channel>channelMixes.size())
		return;

	channelMixes[channel].level=(float)value/100.0f;

}

void AudioMixerMixView::processSample(Limitless::SharedIAudioSample sample)
{
	emit(updateChannels(sample->channels()));
}

void AudioMixerMixView::onUpdateChannels(unsigned int channels)
{
	if(m_channels!=channels)
	{
		initSliders(channels);

		std::vector<ChannelMix> &channelMixes=*m_view->channelMixes;

		for(size_t i=0; i<m_sliders.size() && i<channelMixes.size(); ++i)
		{
			int value=channelMixes[i].level*100.f;
			
			m_sliders[i]->setSliderPosition(value);
		}
	}
}