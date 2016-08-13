#ifndef _AudioMixerView_h_
#define _AudioMixerView_h_

#include <QWidget>
#include "ui_AudioMixerView.h"

#include "Media/IAudioSample.h"
#include "QtComponents/vuMeter.h"

#include "AudioMixerInfo.h"
#include "AudioMixerInputView.h"
#include "AudioMixerOutputView.h"

#include <limits>
#include <cmath>

class AudioMixer;

struct OutputView
{
    Limitless::SharedMediaPad pad;
    AudioMixerOutputView *outputView;
};
typedef std::shared_ptr<OutputView> SharedOutputView;
typedef std::vector<SharedOutputView> OutputViews;

struct InputView
{
	InputView(InputInfo &info) :inputInfo(info) {}

    InputInfo inputInfo;

    AudioMixerInputView *inputView;
    OutputViews outputViews;
};
typedef std::shared_ptr<InputView> SharedInputView;
typedef std::vector<SharedInputView> InputViews;

class AudioMixerView : public QWidget
{
	Q_OBJECT

public:
	AudioMixerView(AudioMixer *audioMixer, QWidget *parent=0);
	~AudioMixerView();

	void processSample(Limitless::SharedIAudioSample sample);

	void updateChannelMatrix();

signals:
	void updateChannelMatrixSignal();

public slots:
	void onUpdateChannelMatrix();
	void on_addInput_clicked(bool checked);
	void on_addOutput_clicked(bool checked);

private:
	void initMeter(int channels);

	Ui::AudioMixerView ui;

	AudioMixer *m_audioMixer;

	InputViews m_inputViews;
};

#endif //_AudioMixerView_h_
