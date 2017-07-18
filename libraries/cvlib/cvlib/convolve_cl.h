#ifndef _cvlib_convolve_cl_h_
#define _cvlib_convolve_cl_h_

#include "cvlib/cvlibDefine.h"
#include "cvlib/convolve.h"

#include "CL/cl.hpp"

namespace cvlib{namespace cl
{

struct cvlib_EXPORT Kernel
{
    ::cl::Buffer kernel;
    int size;
};

struct cvlib_EXPORT SeparableKernel
{
    ::cl::Buffer kernel1;
    ::cl::Buffer kernel2;
    int size;
};

///
/// Performs convolution on serparable kernels
///
cvlib_EXPORT void separableConvolve(::cl::Context &context, ::cl::CommandQueue &commandQueue, ::cl::Image &src, ::cl::Image &dst, SeparableKernel &kernel, float scale=1.0,
    std::vector<::cl::Event> *events=nullptr, ::cl::Event *event=nullptr);
cvlib_EXPORT void separableConvolve(::cl::Context &context, ::cl::CommandQueue &commandQueue, ::cl::Image &src, int width, int height, ::cl::Image &dst, SeparableKernel &kernel, float scale=1.0,
    std::vector<::cl::Event> *events=nullptr, ::cl::Event *event=nullptr);
cvlib_EXPORT void separableConvolve(::cl::Context &context, ::cl::CommandQueue &commandQueue, ::cl::Image &src, int width, int height, ::cl::Image &dst, ::cl::Image &scratch,
    SeparableKernel &kernel, float scale=1.0f, std::vector<::cl::Event> *events=nullptr, ::cl::Event *event=nullptr);
cvlib_EXPORT void separableConvolve(::cl::Context &context, ::cl::CommandQueue &commandQueue, ::cl::Image &src, int width, int height,  ::cl::Image &dst, ::cl::Image &scratch,
    ::cl::Buffer &kernelXBuffer, int kernelXSize, ::cl::Buffer &kernelYBuffer, int kernelYSize, float scale=1.0f, std::vector<::cl::Event> *events=nullptr, ::cl::Event *event=nullptr);
cvlib_EXPORT void separableConvolve_local(::cl::Context &context, ::cl::CommandQueue &commandQueue, ::cl::Image &src, int width, int height, ::cl::Image &dst, ::cl::Image &scratch,
    ::cl::Buffer &kernelXBuffer, int kernelXSize, ::cl::Buffer &kernelYBuffer, int kernelYSize, float scale=1.0f, std::vector<::cl::Event> *events=nullptr, ::cl::Event *event=nullptr);

}}//namespace cvlib::cl

#endif //_cvlib_convolve_cl_h_