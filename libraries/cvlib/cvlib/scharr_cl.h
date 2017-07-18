#ifndef _cvlib_scharr_cl_h_
#define _cvlib_scharr_cl_h

#include "cvlib/cvlibDefine.h"
#include "cvlib/convolve_cl.h"

#include "CL/cl.hpp"

namespace cvlib{namespace cl
{

///
/// Builds Scharr Kernel, for use with a separable convolution
///
cvlib_EXPORT SeparableKernel buildScharrSeparableKernel(::cl::Context &context, int kernelSize, bool normalize);

///
/// Runs Scharr Kernel
///
cvlib_EXPORT void scharr(::cl::Context &context, ::cl::CommandQueue &commandQueue, ::cl::Image2D &src, ::cl::Image2D &dst, int kernelSize=3, Direction direction=Direction::X, bool normalize=true, float scale=1.0,
    std::vector<::cl::Event> *events=nullptr, ::cl::Event *event=nullptr);
cvlib_EXPORT void scharr(::cl::Context &context, ::cl::CommandQueue &commandQueue, ::cl::Image2D &src, size_t width, size_t height, ::cl::Image2D &dst, int kernelSize=3, Direction direction=Direction::X, bool normalize=true, float scale=1.0,
    std::vector<::cl::Event> *events=nullptr, ::cl::Event *event=nullptr);
cvlib_EXPORT void scharr(::cl::Context &context, ::cl::CommandQueue &commandQueue, ::cl::Image2D &src, size_t width, size_t height, ::cl::Image2D &dst, ::cl::Image2D &scratch, SeparableKernel &kernelBuffer, Direction direction=Direction::X,
    bool normalize=true, float scale=1.0, std::vector<::cl::Event> *events=nullptr, ::cl::Event *event=nullptr);
}}//namespace cvlib::cl

#endif //_cvlib_scharr_cl_h