#ifndef _QImageSample_h_
#define _QImageSample_h_

#include "media_define.h"
#include "IImageSample.h"
#include <QImage>
#include <QString>

class MEDIA_EXPORT QImageSample:public AutoRegisterMediaSample<QImageSample, IImageSample>
{
public:
	QImageSample(){};
	virtual ~QImageSample(){};

	operator QImage() const;

	bool load(QString fileName){return m_image.load(fileName);}

//IImageSample
	virtual std::string format() const;
	virtual int width() const;
	virtual int pitch() const;
	virtual int height() const;

//MediaSample
	virtual unsigned char *buffer();
	virtual size_t size() const;

private:
	QImage m_image;
};
typedef boost::shared_ptr<QImageSample> SharedQImageSample;
#endif //_QImageSample_h_