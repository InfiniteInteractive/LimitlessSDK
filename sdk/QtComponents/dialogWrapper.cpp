#include "dialogWrapper.h"
#include <QtWidgets/QVBoxLayout>
#include <QtCore/QEvent>

#ifndef Q_MOC_RUN 
#endif

namespace Limitless
{

DialogWrapper::DialogWrapper(Limitless::SharedQtPluginView pluginView, QWidget *parent):
    QDialog(parent),
    m_pluginView(pluginView)
{
    QWidget *pluginWidget=pluginView->widget;

    pluginWidget->setAttribute(Qt::WA_DeleteOnClose);

    assert(connect(pluginWidget, SIGNAL(accepted()), this, SLOT(accept())));
    assert(connect(pluginWidget, SIGNAL(rejected()), this, SLOT(reject())));
    assert(connect(pluginWidget, SIGNAL(destroyed(QObject*)), this, SLOT(childDestroyed(QObject*))));

    setAttribute(Qt::WA_DeleteOnClose);

    QVBoxLayout *layout=new QVBoxLayout();

    layout->addWidget(pluginWidget);
    setLayout(layout);
}

DialogWrapper::~DialogWrapper()
{

}

void DialogWrapper::childDestroyed(QObject *object)
{
    close();
}

}//namespace Limitless
