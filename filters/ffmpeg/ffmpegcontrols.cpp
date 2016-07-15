#include "ffmpegcontrols.h"
#include "ffmpegencoder.h"

FfmpegControls::FfmpegControls(FfmpegEncoder *encoder, QWidget *parent):
QWidget(parent),
m_encoder(encoder)
{
	ui.setupUi(this);
}

FfmpegControls::~FfmpegControls()
{

}
