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

private:
	Ui::AudioMixerOutputView ui;

	OutputView *m_outputView;
};

#endif //_AudioMixerOutputView_h_
