#ifndef _cvlib_utils_cl_h_
#define _cvlib_utils_cl_h_

#include "cvlibDefine.h"
#include "CL/cl.hpp"


namespace cvlib{namespace cl
{

cvlib_EXPORT void zeroBuffer(::cl::Context &context, ::cl::CommandQueue &commandQueue, ::cl::Buffer &buffer, size_t size, std::vector<::cl::Event> *events, ::cl::Event &event);
cvlib_EXPORT void zeroFloatBuffer(::cl::Context &context, ::cl::CommandQueue &commandQueue, ::cl::Buffer &buffer, size_t size, std::vector<::cl::Event> *events, ::cl::Event &event);
cvlib_EXPORT void zeroIntBuffer(::cl::Context &context, ::cl::CommandQueue &commandQueue, ::cl::Buffer &buffer, size_t size, std::vector<::cl::Event> *events, ::cl::Event &event);
cvlib_EXPORT void zeroImage(::cl::Context &context, ::cl::CommandQueue &commandQueue, ::cl::Image2D &image, std::vector<::cl::Event> *events, ::cl::Event &event);

}}//namespace cvlib::cl

#endif //_cvlib_utils_cl_h_