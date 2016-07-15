#ifndef FILTERPOPUP_H
#define FILTERPOPUP_H

#include "qtcomponents_define.h"
#include <QDialog>

#include "Media/MediaPluginFactory.h"
#include <QtWidgets/qtreewidget.h>

namespace Ui
{
	class FilterPopup;
}

class QTCOMPONENTS_EXPORT FilterPopup:public QDialog
{
	Q_OBJECT

public:
	FilterPopup(QWidget *parent = 0);
	~FilterPopup();

	void setFilters(Limitless::FilterDefinitions &filters);

protected:
	virtual void mousePressEvent(QMouseEvent * event);
	virtual void mouseReleaseEvent(QMouseEvent * event);

public slots:
	void on_filterList_itemClicked(QTreeWidgetItem *item, int column);

signals:
	void filterSelected(QString filterName);

private:
	Ui::FilterPopup *ui;
};

#endif // FILTERPOPUP_H
