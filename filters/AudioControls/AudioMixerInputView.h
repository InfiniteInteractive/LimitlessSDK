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

signals:

public slots:

private:
	Ui::AudioMixerInputView ui;

	InputView *m_inputView;
    std::vector<Limitless::VuMeter *> m_vuMeters;
};

#endif //_AudioMixerInputView_h_
