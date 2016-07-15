#include "debugapp.h"
#include <QtWidgets/QApplication>
#include "Media/MediaPluginFactory.h"

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);

	QCoreApplication::setOrganizationName("Infinite Interactive");
	QCoreApplication::setOrganizationDomain("infiniteinteractive.com");
	QCoreApplication::setApplicationName("DebugApp");

	Limitless::MediaPluginFactory::loadPlugins("./plugins/");

	DebugApp w;

	w.show();
	w.init();
	return a.exec();
}
