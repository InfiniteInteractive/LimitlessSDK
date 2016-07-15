#include "VuMeter.h"

#include <QtGui/QPainter>

namespace Limitless
{

VuMeter::VuMeter(QWidget *parent): QWidget(parent)
{
	m_backgroundColor=QColor(0, 0, 0);
	m_cellColor=QColor(80, 80, 80);
	
	m_highColor=Qt::red;
	m_color=Qt::yellow;
	m_lowColor=Qt::green;

	m_maxiumValue=1.0f;
	m_minimumValue=0.0f;
	m_value=0.0f;

	m_cells=40;
}

QSize VuMeter::minimumSizeHint() const
{
	QSize size;

	size.setHeight(m_cells*4+1);
	size.setWidth(6);
	
	return size;
}

QSize VuMeter::sizeHint() const
{
	QSize size;

	size.setHeight(m_cells*4+3);
	size.setWidth(8);

	return size;
}

void VuMeter::setValue(float value)
{
	m_value=value;
	update();
}

void VuMeter::paintEvent(QPaintEvent *event)
{
	QPainter painter(this);
	QRect rect=painter.window();

	painter.setRenderHint(QPainter::Antialiasing);

	int cellSpacers=m_cells+1;
	int height=rect.height()-cellSpacers; //need minum 1 pixel space between blocks
	int cellHeight=height/m_cells;

	int extra=(rect.height()-(cellHeight*m_cells)-cellSpacers)/2;

	painter.setBrush(m_backgroundColor);
	painter.drawRect(rect);

	int cellIndex=ceil(m_value*m_cells);
	int colorIndex=ceil(0.5*m_cells);
	int highColorIndex=ceil(0.75*m_cells);

	if(cellIndex < 0)
		cellIndex=0;
	else if(cellIndex > m_cells)
		cellIndex=m_cells;

	QRect cellRect(rect.x()+1, rect.height()-cellHeight-extra-1, rect.width()-2, cellHeight);
	QColor currentColor=m_lowColor;
	int changeIndex=colorIndex;

	painter.setBrush(m_lowColor);
	for(size_t i=0; i<cellIndex; ++i)
	{
		if(i >= changeIndex)
		{
			if(changeIndex == colorIndex)
			{
				painter.setBrush(m_color);
				changeIndex=highColorIndex;
			}
			else if(changeIndex == highColorIndex)
			{
				painter.setBrush(m_highColor);
				changeIndex=m_cells;
			}
		}

		painter.drawRect(cellRect);
		cellRect.setBottom(cellRect.y()-1);
		cellRect.setTop(cellRect.y()-cellHeight-1);
	}

	painter.setBrush(m_cellColor);
	for(size_t i=cellIndex; i<m_cells; ++i)
	{
		painter.drawRect(cellRect);
		cellRect.setBottom(cellRect.y()-1);
		cellRect.setTop(cellRect.y()-cellHeight-1);
	}
}

}//namespace Limitless