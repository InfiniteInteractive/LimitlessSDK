#ifndef _cvlib_imageDLib_h_
#define _cvlib_imageDLib_h_

#include <cvlib/SimpleImage.h>
#include <dlib/image_processing/generic_image.h>

namespace dlib
{
long num_rows(const cvlib::SimpleImage& img) { return img.height; }
long num_columns(const cvlib::SimpleImage& img) { return img.width; }
void set_image_size(cvlib::SimpleImage& img, long rows, long cols) { img.width=cols; img.height=rows; }
void *image_data(cvlib::SimpleImage& img) { return (void *)img.data; }
const void *image_data(const cvlib::SimpleImage& img) { return img.data; }
long width_step(const cvlib::SimpleImage& img) { return img.stride; }
void swap(cvlib::SimpleImage& a, cvlib::SimpleImage& b) { cvlib::SimpleImage tmp=a;  a=b; b=tmp; }

template<>
struct image_traits<cvlib::SimpleImage>
{
	typedef uint8_t pixel_type;
};

}//namespace dlib

#endif //_cvlib_imageDLib_h_