#include "listpopup.h"

#include <QtGui/QMouseEvent>
#include <QtWidgets/qtreewidget.h>

#include <algorithm>
#include "ui_listpopup.h"

using namespace Limitless;

ListPopup::ListPopup(QWidget *parent)
	: QDialog(parent)
{
	ui=new Ui::ListPopup();
	ui->setupUi(this);
	
	setWindowFlags(Qt::FramelessWindowHint|Qt::Popup);
	setAttribute(Qt::WA_DeleteOnClose);
}

ListPopup::~ListPopup()
{
	delete ui;
}

void ListPopup::setItems(std::vector<QString> &items)
{
	ui->list->clear();

	if(items.empty())
		return;

	for(QString &item:items)
	{
		QListWidgetItem *filterItem=new QListWidgetItem(ui->list);

		filterItem->setText(item);
	}

	int columnSize=ui->list->sizeHintForColumn(0);
	int rowSize=ui->list->sizeHintForRow(0);

	rowSize=rowSize*items.size();

	if(columnSize < 100)
		columnSize=100;
	if(rowSize < 100)
		rowSize=100;

	ui->list->setMinimumWidth(columnSize);
	ui->list->setMaximumWidth(columnSize);
	ui->list->setMinimumHeight(rowSize);
	ui->list->setMaximumHeight(rowSize);

	setMinimumWidth(columnSize);
	setMaximumWidth(columnSize);
	setMinimumHeight(rowSize);
	setMaximumHeight(rowSize);
}

void ListPopup::mousePressEvent(QMouseEvent * event)
{
	QPointF position=event->windowPos();

	if((position.x() < 0)||(position.y() < 0))
		close();
	if((position.x() > width())||(position.y() > height()))
		close();
}

void ListPopup::mouseReleaseEvent(QMouseEvent * event)
{

}

void ListPopup::on_list_itemClicked(QListWidgetItem * item)
{
	emit itemSelected(item->text());
	close();
}