#ifndef _ImageSourceAttributeView_h_
#define _ImageSourceAttributeView_h_

#include <QWidget>
#include "ui_ImageSourceAttributeView.h"

class ImageSource;

class ImageSourceAttributeView : public QWidget
{
	Q_OBJECT

public:
	ImageSourceAttributeView(ImageSource *input, QWidget *parent=0);
	~ImageSourceAttributeView();

public slots:
	void on_fileBrowse_clicked();
	void on_ok_clicked();
	void on_cancel_clicked();
	void on_fileName_textChanged(const QString &text);

signals:
	void accepted();
	void rejected();

private:
	void checkFileExists(QString fileName);

	Ui::ImageSourceAttributeView ui;

    ImageSource *m_source;
};

#endif // FFMPEGCONTROLS_H
