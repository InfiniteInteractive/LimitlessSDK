#ifndef _AudioMeterView_h_
#define _AudioMeterView_h_

#include <QWidget>
#include "ui_AudioMeterView.h"

#include "Media/IAudioSample.h"
#include "QtComponents/vuMeter.h"

#include <limits>
#include <cmath>

Q_DECLARE_METATYPE(std::vector<float>)

template<typename _SampleType>
std::vector<float> averageSamples(uint8_t *buffer, int samples, int channels)
{
	std::vector<float> averages(channels);
	_SampleType *data=(_SampleType *)buffer;
	int channelSamples=samples/channels;

	for(size_t sample=0; sample<channelSamples; ++sample)
	{
		for(size_t channel=0; channel<channels; ++channel)
		{
			averages[channel]+=(*data);
			data++;
		}
	}

	for(size_t channel=0; channel<channels; ++channel)
	{
		averages[channel]/=channelSamples;
		averages[channel]=Limitless::convertToDb((_SampleType)averages[channel]);
	}

	return averages;
}

template<typename _SampleType>
std::vector<float> averagePlanarSamples(uint8_t *buffer, int samples, int channels)
{
	std::vector<float> averages(channels);
	_SampleType *data=(_SampleType *)buffer;
	int channelSamples=samples/channels;

	for(size_t channel=0; channel<channels; ++channel)
	{
		for(size_t sample=0; sample<channelSamples; ++sample)
		{

			averages[channel]+=(*data);
			data++;
		}
	}

	for(size_t channel=0; channel<channels; ++channel)
	{
		averages[channel]/=channelSamples;
		averages[channel]=Limitless::convertToDb((_SampleType)averages[channel]);
	}

	return averages;
}

template<typename _SampleType>
std::vector<float> peakSamples(uint8_t *buffer, int samples, int channels)
{
	std::vector<float> peak(channels);
	_SampleType *data=(_SampleType *)buffer;
	int channelSamples=samples/channels;

	for(size_t channel=0; channel<channels; ++channel)
		peak[channel]=std::numeric_limits<_SampleType>::min();

	for(size_t sample=0; sample<channelSamples; ++sample)
	{
		for(size_t channel=0; channel<channels; ++channel)
		{
			if((*data) > peak[channel])
				peak[channel]=(*data);
			data++;
		}
	}

	for(size_t channel=0; channel<channels; ++channel)
	{
		peak[channel]=Limitless::convertToDb((_SampleType)peak[channel]);
	}

	return peak;
}

template<typename _SampleType>
std::vector<float> peakPlanarSamples(uint8_t *buffer, int samples, int channels)
{
	std::vector<float> peak(channels);
	_SampleType *data=(_SampleType *)buffer;
	int channelSamples=samples/channels;

	for(size_t channel=0; channel<channels; ++channel)
	{
		peak[channel]=std::numeric_limits<_SampleType>::min();
		for(size_t sample=0; sample<channelSamples; ++sample)
		{
			if((*data) > peak[channel])
				peak[channel]=(*data);
			data++;
		}
	}

	for(size_t channel=0; channel<channels; ++channel)
	{
		peak[channel]=Limitless::convertToDb((_SampleType)peak[channel]);
	}

	return peak;
}

class AudioMeterView : public QWidget
{
	Q_OBJECT

public:
	AudioMeterView(QWidget *parent=0);
	~AudioMeterView();

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

	Ui::AudioMeterView ui;

	std::vector<Limitless::VuMeter *> m_vuMeters;
	int m_channels;
    bool m_verticalMeters;
};

#endif //_AudioMeterView_h_
