#include "Media/QImageSample.h"

QImageSample::operator QImage() const
{
	return m_image;
}

std::string QImageSample::format() const
{
	return "";
}

int QImageSample::width() const
{
	return m_image.width();
}

int QImageSample::pitch() const
{
	return m_image.bytesPerLine();
}

int QImageSample::height() const
{
	return m_image.height();
}

unsigned char *QImageSample::buffer()
{
	return NULL;
}

size_t QImageSample::size() const
{
	return 0;
}