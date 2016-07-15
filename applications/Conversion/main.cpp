#include "conversion.h"
#include <QtWidgets/QApplication>
#include "Media/MediaPluginFactory.h"

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);

	QCoreApplication::setOrganizationName("Infinite Interactive");
	QCoreApplication::setOrganizationDomain("infiniteinteractive.com");
	QCoreApplication::setApplicationName("Conversion");

	Limitless::MediaPluginFactory::loadPlugins("./plugins/");

	Conversion w;

	w.show();
	return a.exec();
}
