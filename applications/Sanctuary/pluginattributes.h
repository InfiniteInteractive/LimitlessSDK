#ifndef PLUGINATTRIBUTES_H
#define PLUGINATTRIBUTES_H

#include <QDockWidget>
#include "ui_pluginattributes.h"
#include "Media/IMediaFilter.h"

class PluginAttributes : public QDockWidget
{
	Q_OBJECT

public:
	PluginAttributes(QWidget *parent = 0);
	~PluginAttributes();

	void update();
	void updateFilterAttribtes(Limitless::SharedMediaFilter filter);

private slots:
	void on_filterClasses_currentIndexChanged(const QString &text);
	void on_filterAdd_clicked();
	void on_filters_cellClicked(int row, int column);
	void on_filterAttributes_itemChanged(QTableWidgetItem *item);

private:
	Ui::PluginAttributes ui;

	Limitless::SharedMediaFilter m_currentFilter;
};

#endif // PLUGINATTRIBUTES_H
