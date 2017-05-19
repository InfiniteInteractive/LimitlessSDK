#include "MediaLogView.h"
#include "ui_MediaLogView.h"

namespace Limitless
{

bool MediaLogCallback::write(Limitless::LogLevel level, const std::string &system, const std::string &subSystem, const std::string &entry)
{
    if((level!=Limitless::LogLevel::Debug)&&(system!="Ffmpeg"))
    {
        std::string message=Limitless::logLevelString(level)+": "+system+"("+subSystem+") - "+entry;

        emit(writeEntry((int)level, QString::fromStdString(message)));
    }
    return false;
}

MediaLogView::MediaLogView(QWidget *parent):
QDockWidget(parent),
m_popOnError(false)
{
    ui=std::make_unique<Ui::MediaLogView>();

    ui->setupUi(this);
    MediaLogCallback *mediaLogCallback=new MediaLogCallback(this);
    m_callback.reset(mediaLogCallback);

    connect(mediaLogCallback, SIGNAL(writeEntry(int, QString)), this, SLOT(write(int, QString)), Qt::QueuedConnection);
    Limitless::Log::attachCallback(m_callback);
}

MediaLogView::~MediaLogView()
{
    Limitless::Log::dettachCallback(m_callback);
}

void MediaLogView::write(int level, QString entry)
{
    Limitless::LogLevel logLevel=(Limitless::LogLevel)level;

    ui->logList->addItem(entry);
    ui->logList->setCurrentRow(ui->logList->count()-1);

    if((m_popOnError)&&(logLevel==Limitless::LogLevel::Error))
        show();
}

}//namespace Limitless
