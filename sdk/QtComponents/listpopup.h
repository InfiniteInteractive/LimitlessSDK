#ifndef LISTPOPUP_H
#define LISTPOPUP_H

#include "qtcomponents_define.h"
#include <QDialog>

#include "Media/MediaPluginFactory.h"
#include <QtWidgets/QListWidget>

namespace Ui
{
	class ListPopup;
}

class QTCOMPONENTS_EXPORT ListPopup:public QDialog
{
	Q_OBJECT

public:
	ListPopup(QWidget *parent = 0);
	~ListPopup();

	void setItems(std::vector<QString> &items);

protected:
	virtual void mousePressEvent(QMouseEvent * event);
	virtual void mouseReleaseEvent(QMouseEvent * event);

public slots:
	void on_list_itemClicked(QListWidgetItem *item);

signals:
	void itemSelected(QString item);

private:
	Ui::ListPopup *ui;
};

#endif // LISTPOPUP_H
