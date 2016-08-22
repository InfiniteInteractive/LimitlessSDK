#ifndef _AudioMixerMixView_h_
#define _AudioMixerMixView_h_

#include <QWidget>
#include "ui_AudioMixerMixView.h"

#include "Media/IAudioSample.h"
#include "QtComponents/vuMeter.h"

#include <limits>
#include <cmath>

struct MixView;

class AudioMixerMixView : public QWidget
{
	Q_OBJECT

public:
	AudioMixerMixView(MixView *outputView, QWidget *parent=0);
	~AudioMixerMixView();

	void initSliders(int channels);

	void processSample(Limitless::SharedIAudioSample sample);

signals:
	void updateChannels(unsigned int);

public slots:
	void on_slider_valueChanged(int value);
	void onUpdateChannels(unsigned int channels);

private:
	Ui::AudioMixerMixView ui;

	MixView *m_view;
	std::vector<QSlider *> m_sliders;
	int m_channels;
};

#endif //_AudioMixerMixView_h_
