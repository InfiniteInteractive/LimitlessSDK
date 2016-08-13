#include "AudioMixerInputView.h"
#include "AudioMixerView.h"



AudioMixerInputView::AudioMixerInputView(InputView *inputView, QWidget *parent):
m_inputView(inputView),
QWidget(parent)
{
	ui.setupUi(this);

//    new VuMeter(ui.vuMeter)
//    ui.vuMeter
    update();
}

AudioMixerInputView::~AudioMixerInputView()
{

}

void AudioMixerInputView::update()
{
    ui.nameLabel->setText(QString::fromStdString(m_inputView->inputInfo.name));

    Limitless::SharedMediaPad intputPad=m_inputView->inputInfo.intputPad;
    QString linkLabel="Not Linked";

    if(intputPad->linked())
    {
        Limitless::SharedMediaPad &sourcePad=intputPad->linkedPad();

        if(sourcePad)
        {
            Limitless::SharedMediaFilter sourceFilter=sourcePad->parent();

            if(sourceFilter)
                linkLabel=QString("%1(%2)").arg(QString::fromStdString(sourceFilter->instance())).arg(QString::fromStdString(sourcePad->name()));
        }
    }
    ui.linkLabel->setText(linkLabel);
}