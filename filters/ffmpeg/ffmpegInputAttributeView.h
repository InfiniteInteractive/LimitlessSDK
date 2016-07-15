#ifndef _FfmpegInputAttributeView_h_
#define _FfmpegInputAttributeView_h_

#include <QWidget>
#include "ui_ffmpeginputattributeview.h"

class FfmpegInput;

class FfmpegInputAttributeView : public QWidget
{
	Q_OBJECT

public:
	FfmpegInputAttributeView(FfmpegInput *input, QWidget *parent=0);
	~FfmpegInputAttributeView();

public slots:
	void on_fileBrowse_clicked();
	void on_ok_clicked();
	void on_cancel_clicked();
	void on_fileName_textChanged(const QString &text);
	void on_loop_stateChanged(int state);

signals:
	void accepted();
	void rejected();

private:
	void checkFileExists(QString fileName);

	Ui::FfmpegInputAttributeView ui;

	FfmpegInput *m_input;
};

#endif // FFMPEGCONTROLS_H
