#ifndef _AudioMixerInputView_h_
#define _AudioMixerInputView_h_

#include <QWidget>
#include "ui_AudioMixerInputView.h"

#include "Media/IAudioSample.h"
#include "QtComponents/vuMeter.h"

#include <limits>
#include <cmath>

struct InputView;

class AudioMixerInputView : public QWidget
{
	Q_OBJECT

public:
    AudioMixerInputView(InputView *inputView, QWidget *parent=0);
	~AudioMixerInputView();

    void update();
	void processSample(Limitless::SharedIAudioSample sample);

	void setVertical(bool vertical);

signals:
	void setMeterValues(std::vector<float> values);
	void setVerticalMeters(bool value);

public slots:
	void onMeterValues(std::vector<float> values);
	void onVerticalMeter(bool value);

private:
	void initMeter(int channels);

	Ui::AudioMixerInputView ui;

	std::vector<Limitless::VuMeter *> m_vuMeters;
	int m_channels;
	bool m_verticalMeters;

	InputView *m_inputView;
};

#endif //_AudioMixerInputView_h_
