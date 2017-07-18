#ifndef _cvlib_imageFunctions_cl_h_
#define _cvlib_imageFunctions_cl_h_

#include "cvlib/cvlibDefine.h"
#include "CL/cl.hpp"

#include <vector>

namespace cvlib{namespace cl
{

///
/// Zeros values of image
///
cvlib_EXPORT void zeroImage(::cl::Context &context, ::cl::CommandQueue &commandQueue, ::cl::Image &image, std::vector<::cl::Event> *events, ::cl::Event *event);

///
/// Sums values accross entire image
///
cvlib_EXPORT float sum(::cl::Context &context, ::cl::CommandQueue &commandQueue, ::cl::Image &src, std::vector<::cl::Event> *events=nullptr);
cvlib_EXPORT void sum(::cl::Context &context, ::cl::CommandQueue &commandQueue, ::cl::Image &src, int width, int height, ::cl::Buffer &sum, int offset, std::vector<::cl::Event> *events=nullptr, ::cl::Event *event=nullptr);

}}//namespace cvlib::cl

#endif //_cvlib_imageFunctions_cl_h_