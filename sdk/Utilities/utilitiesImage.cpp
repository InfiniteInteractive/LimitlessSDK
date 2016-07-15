#include "Utilities/utilitiesImage.h"

#include <stdio.h>

using namespace Limitless;

extern "C"
{

UTILITIES_EXPORT void savePPM(const char* const strFile, ImageFormat format, unsigned char *data, int width, int height)
{
	FILE *file;
	char str[64];

	file=fopen(strFile, "wb");
	if(file != NULL)
	{
		fputs("P6\n", file);
		sprintf(str, "%d %d\n255\n", width, height);
		fputs(str, file);

		if(format == RGB)
			fwrite(data, sizeof(unsigned char), width*height*3, file);
		else
		{
			if(format == RGBA)
			{
				unsigned char *pos=data;

				for(size_t i=0; i<width*height; ++i)
				{
					fwrite(pos, sizeof(unsigned char), 3, file);
					pos+=4;
				}
			}
		}
		fclose(file);
	}
}

UTILITIES_EXPORT void savePGM(const char* const strFile, ImageFormat format, unsigned char *data, int width, int height)
{
	FILE *file;
	char str[64];

	file=fopen(strFile, "wb");
	if(file != NULL)
	{
		fputs("P5\n", file);
		sprintf(str, "%d %d\n255\n", width, height);
		fputs(str, file);
		fwrite(data, sizeof(unsigned char), width*height, file);
		fclose(file);
	}
}

}