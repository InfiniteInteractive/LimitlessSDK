#ifndef _Limitless_QtComponents_DockWrapper_h
#define _Limitless_QtComponents_DockWrapper_h

#include "QtComponents/qtcomponents_define.h"

#ifndef Q_MOC_RUN 
#endif
#include "QtComponents/QtPluginView.h"
#include <QtWidgets/QDockWidget>

namespace Limitless
{

class QTCOMPONENTS_EXPORT DockWrapper: public QDockWidget
{
    Q_OBJECT

public:
    DockWrapper(QWidget *widget, QWidget *parent=0);
    ~DockWrapper();

    virtual QSize sizeHint() const;
    virtual QSize minimumSizeHint() const;

 public slots:
    void childAccepted();
    void childRejected();
    void childDestroyed(QObject *object);

signals:
    void accepted();
    void rejected();

protected:

private:

};

}//namespace Limitless


#endif // _Limitless_QtComponents_DockWrapper_h
