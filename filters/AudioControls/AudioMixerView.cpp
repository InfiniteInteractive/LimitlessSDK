#include "AudioMixerView.h"
#include "audioMixer.h"

AudioMixerView::AudioMixerView(AudioMixer *audioMixer, QWidget *parent):
m_audioMixer(audioMixer),
QWidget(parent)
{
	ui.setupUi(this);

    onUpdateChannelMatrix();

	connect(this, SIGNAL(updateChannelMatrixSignal()), this, SLOT(onUpdateChannelMatrix()), Qt::QueuedConnection);
}

AudioMixerView::~AudioMixerView()
{

}

void AudioMixerView::updateChannelMatrix()
{
	emit(updateChannelMatrixSignal());
}

void AudioMixerView::on_addInput_clicked(bool checked)
{
	m_audioMixer->addSink();

	onUpdateChannelMatrix();
}

void AudioMixerView::on_addOutput_clicked(bool checked)
{
	m_audioMixer->addSource();

	onUpdateChannelMatrix();
}

void AudioMixerView::processSample(Limitless::SharedIAudioSample sample)
{
	std::vector<float> audioAverages;
	Limitless::AudioSampleFormat audioFormat=sample->format();
}

void AudioMixerView::onUpdateChannelMatrix()
{
    MixInfo mixInfo=m_audioMixer->getInfo();
    Limitless::SharedMediaPads sourcePads=m_audioMixer->getSourcePads();
    size_t outputIndex=sourcePads.size();
    
    for(size_t i=0; i<mixInfo.size(); ++i)
    {
        InputInfo &inputInfo=mixInfo[i];
		SharedInputView view;

        if(m_inputViews.size()>i)
        {
            if(m_inputViews[i]->inputInfo.name==inputInfo.name)
                view= m_inputViews[i];
        }

        QGridLayout *layout=ui.gridLayout;

        if(!view)
        {
            view.reset(new InputView(inputInfo));

            view->inputView=new AudioMixerInputView(view.get());
            m_inputViews.insert(m_inputViews.begin()+i, view);

            layout->addWidget(view->inputView, 1, i);
        }
        else
            view->inputView->update();
    }

    while(m_inputViews.size()>mixInfo.size())
    {
		size_t i = mixInfo.size();
		SharedInputView view=m_inputViews[i];

        view->inputView->setParent(nullptr);
        delete view->inputView;

        for(size_t j=0; j<view->outputViews.size(); ++j)
        {
			view->outputViews[j]->outputView->setParent(nullptr);
            delete view->outputViews[j]->outputView;
        }

		m_inputViews.erase(m_inputViews.begin()+i);
    }

    for(size_t i=0; i<m_inputViews.size(); ++i)
    {
        InputInfo &inputInfo=mixInfo[i];
        SharedInputView &inputView=m_inputViews[i];

        for(size_t j=0; j<inputInfo.outputs.size(); ++j)
        {
            OutputInfo &outputInfo=inputInfo.outputs[j];
            SharedOutputView view;

            if(inputView->outputViews.size()>j)
            {
                if(inputView->outputViews[j]->pad==outputInfo.pad)
                    view=inputView->outputViews[j];
            }

            QGridLayout *layout=ui.gridLayout;

            if(!view)
            {
                view.reset(new OutputView());

                view->pad=outputInfo.pad;
                view->outputView=new AudioMixerOutputView(view.get());
                inputView->outputViews.insert(inputView->outputViews.begin()+j, view);

                layout->addWidget(view->outputView, j+2, i);
            }
        }

        while(inputView->outputViews.size() > inputInfo.outputs.size())
        {
            size_t j=inputInfo.outputs.size();
            SharedOutputView view=inputView->outputViews[j];

            view->outputView->setParent(nullptr);
            delete view->outputView;

            inputView->outputViews.erase(inputView->outputViews.begin()+j);
        }
    }

    ui.gridLayout->addWidget(ui.addInput, 0, m_inputViews.size());
    ui.gridLayout->addWidget(ui.addOutput, outputIndex+2, m_inputViews.size());

    ui.gridLayout->addWidget(ui.inputViewSpacer, 1, m_inputViews.size()+1);
    ui.gridLayout->addWidget(ui.gridSpacer, outputIndex+3, m_inputViews.size()+1);
}