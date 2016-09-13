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

void ListPopup::setSubItems(QString &item, std::vector<QString> &subItems)
{
	m_subItems.insert(SubitemMap::value_type(item, subItems));
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
	auto subItem=m_subItems.find(item->text());

	if(subItem!=m_subItems.end())
	{
		m_currentItemSelected=subItem->first;

		ListPopup *listPopup=new ListPopup();

		connect(listPopup, SIGNAL(itemSelected(QString)), this, SLOT(on_subItemClick(QString)));

		listPopup->setItems(subItem->second);

		QPoint buttonPos(ui->list->width(), 0);

		buttonPos=ui->list->mapToGlobal(buttonPos);

		listPopup->move(buttonPos);
		listPopup->show();
		listPopup->exec();
	}
	else
		emit itemSelected(item->text());
	close();
}

void ListPopup::on_subItemClick(QString item)
{
	QString itemName=QString("%1/%2").arg(m_currentItemSelected).arg(item);

	emit itemSelected(itemName);
}