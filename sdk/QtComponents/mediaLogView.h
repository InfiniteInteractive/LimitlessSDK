#ifndef _MediaLogView_h_
#define _MediaLogView_h_

#include "qtcomponents_define.h"

#include <QDockWidget>
#include "Base/log.h"

#include <memory>

namespace Ui
{
class MediaLogView;
}

namespace Limitless
{

class MediaLogView;
class MediaLogCallback:public QObject, public ILogCallback
{
	Q_OBJECT
public:
	MediaLogCallback(MediaLogView *parent):m_parent(parent){};
	~MediaLogCallback(){};

	virtual bool write(LogLevel level, const std::string &system, const std::string &subSystem, const std::string &entry);

signals:
	void writeEntry(int level, QString entry);

private:
    MediaLogView *m_parent;
};

class QTCOMPONENTS_EXPORT MediaLogView: public QDockWidget
{
	Q_OBJECT

public:
    MediaLogView(QWidget *parent = 0);
	~MediaLogView();

    void setPopOnError(bool pop) { m_popOnError=pop; }

public slots:
	void write(int, QString entry);

private:
	Limitless::SharedLogCallback m_callback;
    
    std::unique_ptr<Ui::MediaLogView> ui;
    bool m_popOnError;
};

}//namespace Limitless

#endif // _MediaLogView_h_
