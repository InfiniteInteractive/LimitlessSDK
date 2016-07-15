#ifndef FFMPEGCONTROLS_H
#define FFMPEGCONTROLS_H

#include <QWidget>
#include "ui_ffmpegcontrols.h"

class FfmpegEncoder;

class FfmpegControls : public QWidget
{
	Q_OBJECT

public:
	FfmpegControls(FfmpegEncoder *encoder, QWidget *parent = 0);
	~FfmpegControls();

private:
	Ui::FfmpegControls ui;

	FfmpegEncoder *m_encoder;
};

#endif // FFMPEGCONTROLS_H
