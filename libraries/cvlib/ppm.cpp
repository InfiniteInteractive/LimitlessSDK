#include "cvlib/ppm.h"

namespace cvlib
{

void writePpmGreyScale(Image &image, FILE *file);
void writePpmGreyScale32f(Image &image, FILE *file);
void writePpmRGB(Image &image, FILE *file);
void writePpmRGBA(Image &image, FILE *file);


bool savePpm(Image image, std::string filename)
{
	FILE *file=fopen(filename.c_str(), "wb");

	if(file==NULL)
		return false;

	fprintf(file, "P6\n%d %d\n255\n", image.width(), image.height());

	switch(image.format())
	{
	case ImageFormat::Binary:
	case ImageFormat::GreyScale:
		writePpmGreyScale(image, file);
		break;
	case ImageFormat::GreyScale32f:
		writePpmGreyScale32f(image, file);
		break;
	case ImageFormat::Rgb:
		writePpmGreyScale(image, file);
		break;
	case ImageFormat::Rgba:
		writePpmGreyScale(image, file);
		break;
	}
	
	fclose(file);
	return true;
}

void writePpmGreyScale(Image &image, FILE *file)
{
	uint8_t *data=image.data();

	for(size_t y=0; y<image.height(); ++y)
	{
		for(size_t x=0; x<image.width(); ++x)
		{
			fwrite(data, 1, 1, file);
			fwrite(data, 1, 1, file);
			fwrite(data, 1, 1, file);
			++data;
		}
	}
}

void writePpmGreyScale32f(Image &image, FILE *file)
{
	float *data=(float *)image.data();

	float min=std::numeric_limits<float>::max();
	float max=std::numeric_limits<float>::min();

	for(size_t y=0; y<image.height(); ++y)
	{
		for(size_t x=0; x<image.width(); ++x)
		{
			float &value=*data;

			if(value>max)
				max=value;
			else if(value<min)
				min=value;
			++data;
		}
	}
	
	float range=max-min;
	float scale=255.0/range;

	data=(float *)image.data();
	for(size_t y=0; y<image.height(); ++y)
	{
		for(size_t x=0; x<image.width(); ++x)
		{
			uint8_t value=((*data)+min)*scale;
			fwrite(&value, 1, 1, file);
			fwrite(&value, 1, 1, file);
			fwrite(&value, 1, 1, file);
			++data;
		}
	}
}

void writePpmRGB(Image &image, FILE *file)
{
	uint8_t *data=image.data();

	for(size_t y=0; y<image.height(); ++y)
	{
		for(size_t x=0; x<image.width(); ++x)
		{
			fwrite(data, 1, 3, file);
			data+=3;
		}
	}
}

void writePpmRGBA(Image &image, FILE *file)
{
	uint8_t *data=image.data();

	for(size_t y=0; y<image.height(); ++y)
	{
		for(size_t x=0; x<image.width(); ++x)
		{
			fwrite(data, 1, 3, file);
			data+=4;
		}
	}
}

}//namespace cvlib