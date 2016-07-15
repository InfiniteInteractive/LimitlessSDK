#ifndef _Limitless_QtComponents_DialogWrapper_h
#define _Limitless_QtComponents_DialogWrapper_h

#include "QtComponents/qtcomponents_define.h"

#ifndef Q_MOC_RUN 
#endif

#include "QtComponents/QtPluginView.h"
#include <QtWidgets/QDialog>
#include <QtWidgets/QLayout>

namespace Limitless
{

class QTCOMPONENTS_EXPORT DialogWrapper: public QDialog
{
    Q_OBJECT

public:
    DialogWrapper(Limitless::SharedQtPluginView pluginView, QWidget *parent=0);
    ~DialogWrapper();

    public slots:
    void childDestroyed(QObject *object);

signals:

protected:

private:
    Limitless::SharedQtPluginView m_pluginView;

};

}//namespace Limitless

#endif // _Limitless_QtComponents_DialogWrapper_h
