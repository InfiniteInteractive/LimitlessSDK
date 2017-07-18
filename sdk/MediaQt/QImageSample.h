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
    virtual size_t buffers() { return 1; }
	virtual unsigned char *buffer(size_t index=0);
    virtual size_t bufferSize(size_t index=0) { if(index==0) return size(); return 0; }
	virtual size_t size() const;

	virtual bool save(std::string fileName){return false;}

private:
	QImage m_image;
};
typedef boost::shared_ptr<QImageSample> SharedQImageSample;

}//namespace Limitless

#endif //_QImageSample_h_