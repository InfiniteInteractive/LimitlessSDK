#ifndef _AudioMixerOutputView_h_
#define _AudioMixerOutputView_h_

#include <QWidget>
#include "ui_AudioMixerOutputView.h"

#include "Media/IAudioSample.h"
#include "QtComponents/vuMeter.h"

#include <limits>
#include <cmath>

struct OutputView;

class AudioMixerOutputView : public QWidget
{
	Q_OBJECT

public:
	AudioMixerOutputView(OutputView *outputView, QWidget *parent=0);
	~AudioMixerOutputView();

	void processSample(std::vector<float> &peak);

signals:
	void setMeterValue(float value);

public slots:
	void onMeterValue(float value);

private:
	Ui::AudioMixerOutputView ui;

	OutputView *m_outputView;
	Limitless::VuMeter *m_vuMeter;
};

#endif //_AudioMixerOutputView_h_
