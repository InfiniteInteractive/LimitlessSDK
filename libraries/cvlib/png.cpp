#include "cvlib/png.h"
#include <png.h>
#include <vector>

namespace cvlib
{

cvlib_EXPORT bool loadPng(Image image, std::string filename)
{
    FILE *file=fopen(filename.c_str(), "rb");

    if(!file)
        return false;

    char header[8];

    fread(header, 1, 8, file);

    if(png_sig_cmp((png_const_bytep)header, 0, 8))
        return false;

    png_structp png_ptr;

    /* initialize stuff */
    png_ptr=png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);

    if(!png_ptr)
        return false;

    png_infop info_ptr;

    info_ptr=png_create_info_struct(png_ptr);
    if(!info_ptr)
        return false;

    if(setjmp(png_jmpbuf(png_ptr)))
        return false;

    int width, height;
    png_byte color_type;
    png_byte bit_depth;
    int number_of_passes;

    png_init_io(png_ptr, file);
    png_set_sig_bytes(png_ptr, 8);
    png_read_info(png_ptr, info_ptr);

    width=png_get_image_width(png_ptr, info_ptr);
    height=png_get_image_height(png_ptr, info_ptr);
    color_type=png_get_color_type(png_ptr, info_ptr);
    bit_depth=png_get_bit_depth(png_ptr, info_ptr);

    number_of_passes=png_set_interlace_handling(png_ptr);
    png_read_update_info(png_ptr, info_ptr);

    if(setjmp(png_jmpbuf(png_ptr)))
        return false;

    ImageFormat format;
    int channels;

    if(color_type==PNG_COLOR_TYPE_GRAY)
    {
        channels=1;
        format=ImageFormat::GreyScale;
    }
    else if(color_type==PNG_COLOR_TYPE_GRAY_ALPHA)
    {
        channels=2;
        format=ImageFormat::Ra;
    }
    else if(color_type==PNG_COLOR_TYPE_RGB)
    {
        channels=3;
        format=ImageFormat::Rgb;
    }
    else if(color_type==PNG_COLOR_TYPE_RGB_ALPHA)
    {
        channels=4;
        format=ImageFormat::Rgba;
    }

    image.resize(format, width, height);
    std::vector<png_bytep> row_pointers(height);
    png_byte *imageData=(png_byte *)image.data();

    size_t pos=0;
    size_t stride=width*channels;

    for(int y=0; y<height; y++)
    {
        row_pointers[y]=&imageData[pos];
        pos+=stride;
    }

    png_read_image(png_ptr, row_pointers.data());

    fclose(file);
}

bool savePng(Image image, std::string filename)
{
    return false;
}
}//namespace cvlib