#ifndef _AudioMixerView_h_
#define _AudioMixerView_h_

#include <QWidget>
#include "ui_AudioMixerView.h"

#include "Media/IAudioSample.h"
#include "QtComponents/vuMeter.h"

#include <limits>
#include <cmath>

class AudioMixer;

class AudioMixerView : public QWidget
{
	Q_OBJECT

public:
	AudioMixerView(AudioMixer *audioMixer, QWidget *parent=0);
	~AudioMixerView();

	void processSample(Limitless::SharedIAudioSample sample);

	void updateInputOutput();

signals:
	void updateInputOutputSignal();

public slots:
	void onUpdateInputOutput();
	void on_addInput_clicked(bool checked);
	void on_addOutput_clicked(bool checked);

private:
	void initMeter(int channels);

	Ui::AudioMixerView ui;

	AudioMixer *m_audioMixer;
};

#endif //_AudioMixerView_h_
