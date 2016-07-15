#include "conversion.h"

#include "Base/log.h"
#include "Base/PluginFactory.h"
#include "Base/QtPluginView.h"
#include "Media/MediaPluginFactory.h"
#include "Media/IMediaSource.h"
#include "QtComponents\filterpopup.h"

#include <QtWidgets/QDockWidget>

using namespace Limitless;

Conversion::Conversion(QWidget *parent, Qt::WindowFlags flags)
	: QMainWindow(parent, flags)
{
	setupUi(this);

	m_mediaPipeline.reset(new MediaPipeline("Pipeline", SharedMediaFilter()));
}

Conversion::~Conversion()
{
}

void Conversion::closeEvent(QCloseEvent *event)
{
	QMainWindow::closeEvent(event);
}

void Conversion::on_filterAdd_clicked()
{
	bool connected;
	FilterPopup *filterPopup=new FilterPopup();

	connected=connect(filterPopup, SIGNAL(filterSelected(QString)), this, SLOT(addFilter(QString)));

	FilterDefinitions filters=MediaPluginFactory::registedFilters();

	filterPopup->setFilters(filters);

	QPoint buttonPos(filterAdd->width(), 0);

	buttonPos=filterAdd->mapToGlobal(buttonPos);

	filterPopup->setWindowFlags(Qt::FramelessWindowHint|Qt::Popup);
	filterPopup->move(buttonPos);
	filterPopup->show();
	filterPopup->exec();
}

void Conversion::on_filterRemove_clicked()
{

}

void Conversion::on_filterUp_clicked()
{

}

void Conversion::on_filterDown_clicked()
{

}

void Conversion::addFilter(QString filterName)
{
	SharedMediaFilter mediaFilter;
	std::string filterInstance=MediaPluginFactory::uniqueFilterName(filterName.toStdString());

	mediaFilter=MediaPluginFactory::create(filterName.toStdString(), filterInstance, m_mediaPipeline);

	if(mediaFilter != SharedMediaFilter())
	{
		if(mediaFilter->type()==Limitless::Source)
		{
			if(m_inputFilter!=SharedMediaFilter())
				m_mediaPipeline->removeMediaFilter(m_inputFilter);
			m_inputFilter=mediaFilter;
		}
		else if(mediaFilter->type()==Limitless::Sink)
		{
			if(m_outputFilter!=SharedMediaFilter())
				m_mediaPipeline->removeMediaFilter(m_outputFilter);
			m_outputFilter=mediaFilter;
		}
		else
			m_filters.push_back(mediaFilter);
		m_mediaPipeline->addMediaFilter(mediaFilter);

		updateFilters();
	}
}

void Conversion::updateFilters()
{

}