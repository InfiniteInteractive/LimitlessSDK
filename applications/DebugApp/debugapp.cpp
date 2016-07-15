#include "debugapp.h"

#include "Base/log.h"
#include "Base/PluginFactory.h"
#include "QtComponents/QtPluginView.h"
#include "Media/MediaPluginFactory.h"
#include "Media/IMediaSource.h"
#include "Media/ImageSampleSet.h"
#include "Media/GpuImageSample.h"
#include "QtComponents\filterpopup.h"

#include <QtWidgets/QDockWidget>
#include <Media/MediaInterface.h>

using namespace Limitless;

DebugApp::DebugApp(QWidget *parent, Qt::WindowFlags flags)
	: QMainWindow(parent, flags)
{
	setupUi(this);

}

DebugApp::~DebugApp()
{
}

void DebugApp::init()
{
	WId handle=effectiveWinId();
	HDC hdc=GetDC((HWND)handle);

	Limitless::initOpenGl((Limitless::DisplayHandle)handle);
}

void DebugApp::closeEvent(QCloseEvent *event)
{
	Limitless::closeOpenGl();
	QMainWindow::closeEvent(event);
}

void DebugApp::on_run_clicked()
{
	
}