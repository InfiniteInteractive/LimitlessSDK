#ifndef _Conversion_h
#define _Conversion_h

#include <QtWidgets/QMainWindow>
#include "ui_conversion.h"

#ifndef Q_MOC_RUN 
#include "MediaPipeline/MediaPipeline.h"
#endif

class Conversion : public QMainWindow, Ui::ConversionClass
{
	Q_OBJECT

public:
	Conversion(QWidget *parent = 0, Qt::WindowFlags flags = 0);
	~Conversion();

private slots:
	void on_filterAdd_clicked();
	void on_filterRemove_clicked();
	void on_filterUp_clicked();
	void on_filterDown_clicked();

	void addFilter(QString filterName);

protected:
	virtual void Conversion::closeEvent(QCloseEvent *event);

	void updateFilters();

private:
	Limitless::SharedMediaPipeline m_mediaPipeline;

	Limitless::SharedMediaFilter m_inputFilter;
	Limitless::SharedMediaFilters m_filters;
	Limitless::SharedMediaFilter m_outputFilter;
};

#endif // _Conversion_h
