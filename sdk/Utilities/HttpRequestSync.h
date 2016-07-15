#ifndef _Limitless_HttpRequestSync_h_
#define _Limitless_HttpRequestSync_h_

#include <QObject>
#include <QEventLoop>
#include <QNetworkReply>
#include "utilities_define.h"

namespace Limitless
{

class UTILITIES_EXPORT HttpRequestSync : public QObject
{
	Q_OBJECT

public:
	HttpRequestSync(){};

	void wait(QNetworkReply *reply)
	{
		connect(reply, SIGNAL(finished()), this, SLOT(finished()));
		this->m_eventLoop.exec();
		disconnect(reply, SIGNAL(finished()), this, SLOT(finished()));
	}
           	
public slots:
	void finished()
	{  	this->m_eventLoop.exit();}

private:
	QEventLoop m_eventLoop;
};

}//namespace Limitless

#endif //_Limitless_HttpRequestSync_h_
