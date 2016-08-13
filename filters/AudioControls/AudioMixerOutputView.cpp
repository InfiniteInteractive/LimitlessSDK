#include "AudioMixerOutputView.h"
#include "AudioMixerView.h"

AudioMixerOutputView::AudioMixerOutputView(OutputView *outputView, QWidget *parent):
m_outputView(outputView),
QWidget(parent)
{
	ui.setupUi(this);
}

AudioMixerOutputView::~AudioMixerOutputView()
{

}