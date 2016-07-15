#ifndef SANCTUARY_H
#define SANCTUARY_H

#include <QtWidgets/QMainWindow>
#include "ui_sanctuary.h"

class Sanctuary : public QMainWindow
{
	Q_OBJECT

public:
	Sanctuary(QWidget *parent = 0, Qt::WindowFlags flags = 0);
	~Sanctuary();

private:
	Ui::SanctuaryClass ui;
};

#endif // SANCTUARY_H
