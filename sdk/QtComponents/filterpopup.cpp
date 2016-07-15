#include "filterpopup.h"

#include <QtGui/QMouseEvent>
#include <QtWidgets/qtreewidget.h>

#include <algorithm>
#include "ui_filterpopup.h"

using namespace Limitless;

FilterPopup::FilterPopup(QWidget *parent)
	: QDialog(parent)
{
	ui=new Ui::FilterPopup();

	ui->setupUi(this);
	setAttribute(Qt::WA_DeleteOnClose);
}

FilterPopup::~FilterPopup()
{
	delete ui;
}

void FilterPopup::setFilters(Limitless::FilterDefinitions &filters)
{
	ui->filterList->clear();

	std::vector<Limitless::FilterType> types(3);
	std::vector<std::string> categories[3];

	types[0]=Limitless::Source;
	types[1]=Limitless::Filter;
	types[2]=Limitless::Sink;

	for(size_t i=0; i<filters.size(); ++i)
	{
		Limitless::FilterDefinition &definition=filters[i];
		size_t typeIndex;
		
		if(definition.type==Limitless::Source)
			typeIndex=0;
		else if(definition.type==Limitless::Filter)
			typeIndex=1;
		else if(definition.type==Limitless::Sink)
			typeIndex=2;
		else
			continue;

		if(std::find(categories[typeIndex].begin(), categories[typeIndex].end(), definition.category)==categories[typeIndex].end())
			categories[typeIndex].push_back(definition.category);
	}

	for(size_t j=0; j<3; j++)
	{
		Limitless::FilterType type=types[j];
		size_t typeIndex;
		QTreeWidgetItem *typeItem=new QTreeWidgetItem(ui->filterList);

		if(type==Limitless::Source)
		{
			typeIndex=0;
			typeItem->setText(0, "Source");
			typeItem->setData(0, Qt::UserRole, 0);
		}
		else if(type==Limitless::Filter)
		{
			typeIndex=1;
			typeItem->setText(0, "Filter");
			typeItem->setData(0, Qt::UserRole, 0);
		}
		else if(type==Limitless::Sink)
		{
			typeIndex=2;
			typeItem->setText(0, "Sink");
			typeItem->setData(0, Qt::UserRole, 0);
		}

		for(std::string &category:categories[typeIndex])
		{
			QTreeWidgetItem *categoryItem=new QTreeWidgetItem(typeItem);

			categoryItem->setText(0, QString::fromStdString(category));
			categoryItem->setData(0, Qt::UserRole, 0);

			for(size_t i=0; i<filters.size(); ++i)
			{
				Limitless::FilterDefinition &definition=filters[i];

				if((definition.type == type) && (definition.category == category))
				{
					QTreeWidgetItem *filterItem=new QTreeWidgetItem(categoryItem);

					filterItem->setText(0, QString::fromStdString(definition.name));
					filterItem->setData(0, Qt::UserRole, 1);
				}
			}
		}
	}
}

void FilterPopup::mousePressEvent(QMouseEvent * event)
{
	QPointF position=event->windowPos();

	if((position.x() < 0)||(position.y() < 0))
		close();
	if((position.x() > width())||(position.y() > height()))
		close();
}

void FilterPopup::mouseReleaseEvent(QMouseEvent * event)
{

}

void FilterPopup::on_filterList_itemClicked(QTreeWidgetItem * item, int column)
{
	if(item->data(0, Qt::UserRole)==1)
	{
		emit filterSelected(item->text(0));
		close();
	}
}