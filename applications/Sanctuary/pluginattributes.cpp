#include "pluginattributes.h"
#include "boost/foreach.hpp"
#include "Media/MediaPluginFactory.h"

using namespace Limitless;

PluginAttributes::PluginAttributes(QWidget *parent):
QDockWidget(parent)
{
	ui.setupUi(this);

	FilterDefinitions filterDefinitions=MediaPluginFactory::registedFilters();

	BOOST_FOREACH(FilterDefinition &filterDefinition, filterDefinitions)
	{
		ui.filterClasses->addItem(QString::fromStdString(filterDefinition.name));
	}

	ui.filters->setColumnCount(2);
	ui.filters->setHorizontalHeaderItem(0, new QTableWidgetItem(QString("Instance")));
	ui.filters->setHorizontalHeaderItem(1, new QTableWidgetItem(QString("Type")));
	ui.filterAttributes->setColumnCount(2);
	ui.filterAttributes->setHorizontalHeaderItem(0, new QTableWidgetItem(QString("Attribute")));
	ui.filterAttributes->setHorizontalHeaderItem(1, new QTableWidgetItem(QString("Value")));

	update();
}

PluginAttributes::~PluginAttributes()
{

}

void PluginAttributes::update()
{
	SharedMediaFilters filterInstances=MediaPluginFactory::filterInstances();
	int row=0;

	m_currentFilter=SharedMediaFilter();

	ui.filters->clearContents();

	ui.filters->setRowCount(filterInstances.size());
	BOOST_FOREACH(SharedMediaFilter &mediaFilter, filterInstances)
	{
		ui.filters->setItem(row, 0, new QTableWidgetItem(QString::fromStdString(mediaFilter->instance())));
		ui.filters->setItem(row, 1, new QTableWidgetItem(QString::fromStdString(mediaFilter->typeName())));
		++row;
	}
}

void PluginAttributes::on_filterClasses_currentIndexChanged(const QString &text)
{
	ui.filterName->setText(QString("%1_instance").arg(text));
}

void PluginAttributes::on_filterAdd_clicked()
{
	SharedMediaFilter newFilter=MediaPluginFactory::create(ui.filterClasses->currentText().toStdString(), ui.filterName->text().toStdString(), SharedMediaFilter());

	update();
}

void PluginAttributes::on_filters_cellClicked(int row, int column)
{
	SharedMediaFilters filterInstances=MediaPluginFactory::filterInstances();

	if(filterInstances.size() >= row)
		updateFilterAttribtes(filterInstances[row]);
}

void PluginAttributes::updateFilterAttribtes(SharedMediaFilter filter)
{
	const Attributes &attributes=filter->attributes();
	
	m_currentFilter=filter;

	ui.filterAttributes->clearContents();
	ui.filterAttributes->setRowCount(attributes.size());
	ui.filterAttributes->blockSignals(true);

	int row=0;
	BOOST_FOREACH(const Attributes::value_type &pair, attributes)
	{
		SharedAttribute attribute=pair.second;

		ui.filterAttributes->setItem(row, 0, new QTableWidgetItem(QString::fromStdString(pair.first)));
		switch(attribute->type())
		{
		case Attribute::BOOL:
			ui.filterAttributes->setItem(row, 1, new QTableWidgetItem(attribute->toBool()?QString("true"):QString("false")));
			break;
		case Attribute::INT:
			ui.filterAttributes->setItem(row, 1, new QTableWidgetItem(QString::fromStdString(attribute->toString())));
			break;
		case Attribute::FLOAT:
			ui.filterAttributes->setItem(row, 1, new QTableWidgetItem(QString::fromStdString(attribute->toString())));
			break;
		case Attribute::STRING:
			ui.filterAttributes->setItem(row, 1, new QTableWidgetItem(QString::fromStdString(attribute->toString())));
			break;
		case Attribute::STRING_ENUM:
			ui.filterAttributes->setItem(row, 1, new QTableWidgetItem(QString::fromStdString(attribute->toString())));
			break;
		}
		++row;
	}
	ui.filterAttributes->blockSignals(false);
}

void PluginAttributes::on_filterAttributes_itemChanged(QTableWidgetItem *item)
{
	if(m_currentFilter != SharedMediaFilter())
	{
		Attributes &attributes=m_currentFilter->attributes();
		std::string attributeName=ui.filterAttributes->item(item->row(), 0)->text().toStdString();
		SharedAttribute attribute=attributes[attributeName];

		attribute->fromString(ui.filterAttributes->item(item->row(), 1)->text().toStdString());
	}
}