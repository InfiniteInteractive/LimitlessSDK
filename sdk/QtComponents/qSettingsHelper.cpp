#include "qSettingsHelper.h"
#include <algorithm>

namespace Limitless
{

void settingToAttribute(QSettings &settings, std::string settingsName, SharedAttribute attribute, const QVariant &defaultValue)
{
    switch(attribute->type())
    {
    case Limitless::BOOL:
        attribute->fromBool(settings.value(QString::fromStdString(settingsName), defaultValue).toBool());
        break;
    case Limitless::INT:
        attribute->fromInt(settings.value(QString::fromStdString(settingsName), defaultValue).toInt());
        break;
    case Limitless::UINT:
        attribute->fromUInt(settings.value(QString::fromStdString(settingsName), defaultValue).toUInt());
        break;
    case Limitless::INT64:
        attribute->fromInt64(settings.value(QString::fromStdString(settingsName), defaultValue).toLongLong());
        break;
    case Limitless::UINT64:
        attribute->fromUInt64(settings.value(QString::fromStdString(settingsName), defaultValue).toULongLong());
        break;
    case Limitless::FLOAT:
        attribute->fromFloat(settings.value(QString::fromStdString(settingsName), defaultValue).toFloat());
        break;
    case Limitless::DOUBLE:
        attribute->fromDouble(settings.value(QString::fromStdString(settingsName), defaultValue).toDouble());
        break;
    case Limitless::STRING:
    case Limitless::STRING_ENUM:
        attribute->fromString(settings.value(QString::fromStdString(settingsName)).toString().toStdString());
        break;
    default:
    case Limitless::UNKNOWN:
    case Limitless::ARRAY:
    case Limitless::OBJECT:
        break;
    }
}

void settingFromAttribute(QSettings &settings, std::string settingsName, SharedAttribute attribute)
{
    switch(attribute->type())
    {
    case Limitless::BOOL:
        settings.setValue(QString::fromStdString(settingsName), QVariant(attribute->toBool()));
        break;
    case Limitless::INT:
        settings.setValue(QString::fromStdString(settingsName), QVariant(attribute->toInt()));
        break;
    case Limitless::UINT:
        settings.setValue(QString::fromStdString(settingsName), QVariant(attribute->toUInt()));
        break;
    case Limitless::INT64:
        settings.setValue(QString::fromStdString(settingsName), QVariant(attribute->toInt64()));
        break;
    case Limitless::UINT64:
        settings.setValue(QString::fromStdString(settingsName), QVariant(attribute->toUInt64()));
        break;
    case Limitless::FLOAT:
        settings.setValue(QString::fromStdString(settingsName), QVariant(attribute->toFloat()));
        break;
    case Limitless::DOUBLE:
        settings.setValue(QString::fromStdString(settingsName), QVariant(attribute->toDouble()));
        break;
    case Limitless::STRING:
    case Limitless::STRING_ENUM:
        settings.setValue(QString::fromStdString(settingsName), QVariant(QString::fromStdString(attribute->toString())));
        break;
    default:
    case Limitless::UNKNOWN:
    case Limitless::ARRAY:
    case Limitless::OBJECT:
        break;
    }
}

}

