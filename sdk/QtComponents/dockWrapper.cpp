#include "dockWrapper.h"

#ifndef Q_MOC_RUN 
#endif

namespace Limitless
{

DockWrapper::DockWrapper(QWidget *widget, QWidget *parent):
    QDockWidget(parent)
{
    connect(widget, SIGNAL(accepted()), this, SLOT(childAccepted()));
    connect(widget, SIGNAL(rejected()), this, SLOT(childRejected()));
    connect(widget, SIGNAL(destroyed(QObject*)), this, SLOT(childDestroyed(QObject*)));

	setMinimumSize(widget->minimumSize());
	setMaximumSize(widget->maximumSize());
	setSizePolicy(widget->sizePolicy());

    setAttribute(Qt::WA_DeleteOnClose);
    setWidget(widget);
}

DockWrapper::~DockWrapper()
{

}

void DockWrapper::childAccepted()
{
    emit accepted();
}

void DockWrapper::childRejected()
{
    emit rejected();
}

void DockWrapper::childDestroyed(QObject *object)
{
    close();
}

}//namespace Limitless
