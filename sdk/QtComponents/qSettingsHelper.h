#ifndef _Limitless_QtComponents_qSettinsHelper_h_
#define _Limitless_QtComponents_qSettinsHelper_h_

#include "qtcomponents_define.h"
#include <QtCore/QSettings>
#include "Base/AttributeContainer.h"

namespace Limitless
{

QTCOMPONENTS_EXPORT void settingToAttribute(QSettings &settings, std::string settingsName, SharedAttribute attribute, const QVariant &defaultValue=QVariant());
QTCOMPONENTS_EXPORT void settingFromAttribute(QSettings &settings, std::string settingsName, SharedAttribute attribute);

}

#endif // _Limitless_QtComponents_qSettinsHelper_h_
