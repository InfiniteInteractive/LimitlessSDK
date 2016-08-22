#ifndef _AudioMixerView_h_
#define _AudioMixerView_h_

#include <QWidget>
#include "ui_AudioMixerView.h"

#include "Media/IAudioSample.h"
#include "QtComponents/vuMeter.h"

#include "AudioMixerInfo.h"
#include "AudioMixerInputView.h"
#include "AudioMixerMixView.h"
#include "AudioMixerOutputView.h"

#include <limits>
#include <cmath>

class AudioMixer;

struct OutputView
{
	OutputView(OutputInfo &info):outputInfo(info) {}

//    Limitless::SharedMediaPad pad;
	OutputInfo outputInfo;

    AudioMixerOutputView *outputView;
};
typedef std::shared_ptr<OutputView> SharedOutputView;
typedef std::vector<SharedOutputView> OutputViews;

struct MixView
{
	std::vector<ChannelMix> *channelMixes;
	Limitless::SharedMediaPad pad;
	AudioMixerMixView *mixView;
};
typedef std::shared_ptr<MixView> SharedMixView;
typedef std::vector<SharedMixView> MixViews;

struct InputView
{
	InputView(InputInfo &info):inputInfo(info) {}

    InputInfo inputInfo;

    AudioMixerInputView *inputView;
//    OutputViews outputViews;
	MixViews mixViews;
};
typedef std::shared_ptr<InputView> SharedInputView;
typedef std::vector<SharedInputView> InputViews;

class AudioMixerView : public QWidget
{
	Q_OBJECT

public:
	AudioMixerView(AudioMixer *audioMixer, QWidget *parent=0);
	~AudioMixerView();

	void processSamples(std::vector<Limitless::SharedIAudioSample> &inputSamples, Limitless::SharedIAudioSample outputSample);

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
	OutputViews m_outputViews;
};

#endif //_AudioMixerView_h_
