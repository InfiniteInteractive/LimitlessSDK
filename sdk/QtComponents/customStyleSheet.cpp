#include "customStyleSheet.h"
#include "Utilities\loadFileToVar.h"

QString Limitless::getCustomStyleSheet()
{
	extern std::string darkorange_stylesheet;
	std::string styleSheet;

	styleSheet=loadResource(darkorange_stylesheet, "darkorange.stylesheet");
	return QString::fromStdString(darkorange_stylesheet);
}
