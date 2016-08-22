#include "AudioMixerView.h"
#include "audioMixer.h"
#include "medialibHelpers.h"

#include "medialib/audioBufferWrapper.h"
#include "medialib/audioPeak.h"

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
//	m_audioMixer->addSource();
	m_audioMixer->addChannel();

	onUpdateChannelMatrix();
}

void AudioMixerView::processSamples(std::vector<Limitless::SharedIAudioSample> &inputSamples, Limitless::SharedIAudioSample outputSample)
{
	for(size_t i=0; i<m_inputViews.size() && i<inputSamples.size(); ++i)
	{
		SharedInputView &view=m_inputViews[i];

		view->inputView->processSample(inputSamples[i]);

		for(size_t j=0; j<view->mixViews.size(); ++j)
		{
			SharedMixView &mixView=view->mixViews[j];

			//make sure slider count is correct
			mixView->mixView->processSample(inputSamples[i]);
		}
	}

	medialib::AudioBufferWrapper wrapper(convertFormat(outputSample->format()), outputSample->channels(), outputSample->samples(), outputSample->sampleRate(),
		outputSample->buffer(), outputSample->size());

	std::vector<float> outputPeaks=medialib::peakAudioBuffer(wrapper);

	for(size_t i=0; i<m_outputViews.size(); ++i)
	{
		SharedOutputView &view=m_outputViews[i];

		view->outputView->processSample(outputPeaks);
	}
	
}

void AudioMixerView::onUpdateChannelMatrix()
{
	InputInfoVector &inputs=m_audioMixer->getInputs();
    MixInfo &mixInfo=m_audioMixer->getInfo();

//    Limitless::SharedMediaPads sourcePads=m_audioMixer->getSourcePads();
    size_t outputIndex=mixInfo.size();
    
    for(size_t i=0; i<inputs.size(); ++i)
    {
        InputInfo &inputInfo=inputs[i];
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

    while(m_inputViews.size()>inputs.size())
    {
		size_t i = inputs.size();
		SharedInputView view=m_inputViews[i];

        view->inputView->setParent(nullptr);
        delete view->inputView;

        for(size_t j=0; j<view->mixViews.size(); ++j)
        {
			view->mixViews[j]->mixView->setParent(nullptr);
            delete view->mixViews[j]->mixView;
        }

		m_inputViews.erase(m_inputViews.begin()+i);
    }

    for(size_t i=0; i<m_inputViews.size(); ++i)
    {
        SharedInputView &inputView=m_inputViews[i];

        for(size_t j=0; j<mixInfo.size(); ++j)
        {
			OutputInfo &outputInfo=mixInfo[j];
            SharedMixView view;

            if(inputView->mixViews.size()>j)
            {
                if(inputView->mixViews[j]->pad==outputInfo.pad)
                    view=inputView->mixViews[j];
            }

            QGridLayout *layout=ui.gridLayout;

            if(!view)
            {
                view.reset(new MixView());

				view->channelMixes=&outputInfo.inputs[i].channelMixes;
                view->pad=outputInfo.pad;
                view->mixView=new AudioMixerMixView(view.get());
                inputView->mixViews.insert(inputView->mixViews.begin()+j, view);

                layout->addWidget(view->mixView, j+2, i);
            }
        }

        while(inputView->mixViews.size() > mixInfo.size())
        {
            size_t j=mixInfo.size();
            SharedMixView view=inputView->mixViews[j];

            view->mixView->setParent(nullptr);
            delete view->mixView;

            inputView->mixViews.erase(inputView->mixViews.begin()+j);
        }
    }

	for(size_t i=0; i<mixInfo.size(); ++i)
	{
		OutputInfo &outputInfo=mixInfo[i];
		SharedOutputView view;

		if(m_outputViews.size()>i)
		{
			if(m_outputViews[i]->outputInfo.name==outputInfo.name)
				view=m_outputViews[i];
		}

		QGridLayout *layout=ui.gridLayout;

		if(!view)
		{
			view.reset(new OutputView(outputInfo));

			view->outputView=new AudioMixerOutputView(view.get());
			m_outputViews.insert(m_outputViews.begin()+i, view);

//			layout->addWidget(view->outputView, i+2, m_inputViews.size());
		}
		else
			view->outputView->update();
		layout->addWidget(view->outputView, i+2, m_inputViews.size());
	}

	while(m_outputViews.size()>mixInfo.size())
	{
		size_t i=mixInfo.size();
		SharedOutputView view=m_outputViews[i];

		view->outputView->setParent(nullptr);
		delete view->outputView;

		m_outputViews.erase(m_outputViews.begin()+i);
	}


    ui.gridLayout->addWidget(ui.addInput, 0, m_inputViews.size());
    ui.gridLayout->addWidget(ui.addOutput, outputIndex+2, m_inputViews.size());

    ui.gridLayout->addWidget(ui.inputViewSpacer, 1, m_inputViews.size()+1);
    ui.gridLayout->addWidget(ui.gridSpacer, outputIndex+3, m_inputViews.size()+1);
}