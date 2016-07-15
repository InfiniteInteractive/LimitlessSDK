#ifndef _QImageSample_h_
#define _QImageSample_h_

#include "MediaQt/mediaqt_define.h"
#include "Media/IImageSample.h"
#include <QtGui/QImage>
#include <QtCore/QString>

namespace Limitless
{

class MEDIAQT_EXPORT QImageSample:public AutoRegisterMediaSample<QImageSample, IImageSample>
{
public:
	QImageSample(){};
	virtual ~QImageSample(){};

	operator QImage() const;

	bool load(QString fileName){return m_image.load(fileName);}

//IImageSample
	virtual std::string imageFormat() const;
	virtual unsigned int width() const;
	virtual unsigned int pitch() const;
	virtual unsigned int height() const;
	virtual unsigned int channels() const;
	virtual unsigned int channelBits() const;

//IMediaSample
	virtual unsigned char *buffer();
	virtual size_t size() const;

private:
	QImage m_image;
};
typedef boost::shared_ptr<QImageSample> SharedQImageSample;

}//namespace Limitless

#endif //_QImageSample_h_