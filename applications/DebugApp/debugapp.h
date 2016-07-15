#ifndef _DebugApp_h
#define _DebugApp_h

#include <QtWidgets/QMainWindow>
#include "ui_debugapp.h"

#ifndef Q_MOC_RUN 
#include "MediaPipeline/MediaPipeline.h"
#endif

class DebugApp : public QMainWindow, Ui::DebugAppClass
{
	Q_OBJECT

public:
	DebugApp(QWidget *parent = 0, Qt::WindowFlags flags = 0);
	~DebugApp();

	void DebugApp::init();
private slots:
	void on_run_clicked();

protected:
	virtual void DebugApp::closeEvent(QCloseEvent *event);

	void updateFilters();

private:
	Limitless::SharedMediaFilter m_gpuStitchFilter;
};

#endif // _DebugApp_h
