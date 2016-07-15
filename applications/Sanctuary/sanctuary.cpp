#include "sanctuary.h"
#include "pluginattributes.h"

Sanctuary::Sanctuary(QWidget *parent, Qt::WindowFlags flags)
	: QMainWindow(parent, flags)
{
	ui.setupUi(this);

//	setCentralWidget(new MediaView(this));
//
//	addDockWidget(Qt::BottomDockWidgetArea, new MediaLog(this));
	addDockWidget(Qt::RightDockWidgetArea, new PluginAttributes(this));
}

Sanctuary::~Sanctuary()
{

}
