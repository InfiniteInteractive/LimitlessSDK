#ifndef _cvlib_gaussian_cl_h_
#define _cvlib_gaussian_cl_h_

#include "cvlib/cvlibDefine.h"
#include "cvlib/convolve_cl.h"

#include "CL/cl.hpp"

namespace cvlib{namespace cl
{

///
/// Construct guassian kernel as 2 kernels that work on the x/y direction.
///
cvlib_EXPORT SeparableKernel buildGaussianSeparableKernel(::cl::Context &context, float sigma, int &filterSize);

///
/// Runs Guassian Kernel
///
cvlib_EXPORT void gaussian(::cl::Context &context, ::cl::CommandQueue &commandQueue, ::cl::Image2D &src, ::cl::Image2D &dst, float sigma=0.4, Direction direction=Direction::X, float scale=1.0,
    std::vector<::cl::Event> *events=nullptr, ::cl::Event *event=nullptr);
cvlib_EXPORT void gaussian(::cl::Context &context, ::cl::CommandQueue &commandQueue, ::cl::Image2D &src, int width, int height, ::cl::Image2D &dst, float sigma=0.4, Direction direction=Direction::X, float scale=1.0,
    std::vector<::cl::Event> *events=nullptr, ::cl::Event *event=nullptr);
cvlib_EXPORT void gaussian(::cl::Context &context, ::cl::CommandQueue &commandQueue, ::cl::Image2D &src, int width, int height, ::cl::Image2D &dst, ::cl::Image2D &scratch, SeparableKernel &KernelSeperable, Direction direction=Direction::X,
    float scale=1.0, std::vector<::cl::Event> *events=nullptr, ::cl::Event *event=nullptr);

}}//namespace cvlib::cl

#endif //_cvlib_gaussian_cl_h_