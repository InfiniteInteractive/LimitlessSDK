#ifndef GLVIEW_H
#define GLVIEW_H

#include <QtWidgets/QWidget>
#include "ui_glview.h"
#include "glwidget.h"

class GlView : public QWidget
{
	Q_OBJECT

public:
	GlView(QWidget *parent = 0);
	~GlView();

	GLWidget *getGlWidget() { return ui.openglWidget; }

	void setDisplayMode(GLWidget::DisplayMode displayMode);
	void displaySample(Limitless::SharedMediaSample sample);

	void setFrameRate(double frameRate);

signals:
	void setFrameRateSignal(double frameRate);

public slots:
	void onFrameRate(double frameRate);

private:
	Ui::GlView ui;

//	GLWidget m_display;
//	GLWidget *m_display;
};

#endif // GLVIEW_H
