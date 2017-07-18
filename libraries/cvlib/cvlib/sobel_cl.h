#ifndef _cvlib_sobel_cl_h_
#define _cvlib_sobel_cl_h_

#include "cvlib/cvlibDefine.h"
#include "cvlib/convolve_cl.h"

#include "CL/cl.hpp"

namespace cvlib{namespace cl
{
cvlib_EXPORT SeparableKernel buildSobelSeparableKernel(::cl::Context &context, int kernelSize, bool normalize);

///
/// Runs Sobel Kernel
///
cvlib_EXPORT void sobel(::cl::Context &context, ::cl::CommandQueue &commandQueue, ::cl::Image &src, ::cl::Image &dst, int kernelSize=3, Direction direction=Direction::X, bool normalize=true, float scale=1.0,
    std::vector<::cl::Event> *events=nullptr, ::cl::Event *event=nullptr);
cvlib_EXPORT void sobel(::cl::Context &context, ::cl::CommandQueue &commandQueue, ::cl::Image &src, size_t width, size_t height, ::cl::Image &dst, int kernelSize=3, Direction direction=Direction::X, bool normalize=true, float scale=1.0,
    std::vector<::cl::Event> *events=nullptr, ::cl::Event *event=nullptr);
cvlib_EXPORT void sobel(::cl::Context &context, ::cl::CommandQueue &commandQueue, ::cl::Image &src, size_t width, size_t height, ::cl::Image &dst, ::cl::Image &scratch, SeparableKernel &kernelBuffer, Direction direction=Direction::X,
    bool normalize=true, float scale=1.0, std::vector<::cl::Event> *events=nullptr, ::cl::Event *event=nullptr);

}}//namespace cvlib::cl

#endif //_cvlib_sobel_cl_h_