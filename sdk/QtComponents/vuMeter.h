#ifndef _Limitless_VuMeter_h_
#define _Limitless_VuMeter_h_

#include "qtcomponents_define.h"
#include <QtWidgets\QWidget>

#include <limits>
#include <cmath>

namespace Limitless
{

template<typename _SampleType> float convertToDb(_SampleType sample)
{
	_SampleType min=std::numeric_limits<_SampleType>::min();
	_SampleType max=std::numeric_limits<_SampleType>::max();

	float normalizedSample=(float)abs(sample)/(max);
//	float normalizedSample=(sample-std::numeric_limits<_SampleType>::min())/(std::numeric_limits<_SampleType>::max()-std::numeric_limits<_SampleType>::min());

	return 20.0f*std::log10(normalizedSample);
}

template<> inline float convertToDb<float>(float sample)
{
	//float db calculated between -1.0 and 1.0
	return 20.0f*std::log10(abs(sample));
}

template<> inline float convertToDb<double>(double sample)
{
	//double db calculated between -1.0 and 1.0
	return 20.0f*std::log10(abs(sample));
}

class QTCOMPONENTS_EXPORT VuMeter: public QWidget
{
	Q_OBJECT

public:
	VuMeter(QWidget *parent=0);

	QSize minimumSizeHint() const;
	QSize sizeHint() const;

	float value() { return m_value; }

	public slots:
	void setValue(float value);

protected:
	void paintEvent(QPaintEvent *);

	QColor m_backgroundColor;
	QColor m_highColor;
	QColor m_color;
	QColor m_lowColor;
	QColor m_cellColor;

	float m_maxiumValue;
	float m_minimumValue;
	float m_value;

	unsigned int m_cells;
};

}//namespace Limitless
#endif //_Limitless_VuMeter_h_