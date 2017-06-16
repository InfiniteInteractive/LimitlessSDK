#ifndef _cvlib_convolve_cl_h_
#define _cvlib_convolve_cl_h_

#include "cvlib/cvlibDefine.h"
#include "CL/cl.hpp"

namespace cvlib{namespace cl
{
///
/// Performs convolution on serparable kernels
///
cvlib_EXPORT void separableConvolve(::cl::Context context, ::cl::CommandQueue commandQueue, ::cl::Image2D &src, ::cl::Buffer kernelX, int kernelXSize, ::cl::Buffer kernelY, int kernelYSize, ::cl::Image2D &dst);

cvlib_EXPORT void separableConvolve(::cl::Context context, ::cl::CommandQueue commandQueue, ::cl::Image2D &src, size_t width, size_t height, ::cl::Buffer kernelXBuffer, int kernelXSize,
    ::cl::Buffer kernelYBuffer, int kernelYSize, float scale, ::cl::Image2D &dst, ::cl::Image2D &scratch, std::vector<::cl::Event> *events, ::cl::Event &event);
cvlib_EXPORT void separableConvolve(::cl::Context context, ::cl::CommandQueue commandQueue, ::cl::Image2D &src, size_t width, size_t height, ::cl::Buffer kernelXBuffer, int kernelXSize,
    ::cl::Buffer kernelYBuffer, int kernelYSize, float scale, ::cl::Buffer &dst, size_t offset, ::cl::Image2D &scratch, std::vector<::cl::Event> *events, ::cl::Event &event);
cvlib_EXPORT void separableConvolve(::cl::Context context, ::cl::CommandQueue commandQueue, ::cl::Buffer &src, size_t srcOffset, size_t width, size_t height, ::cl::Buffer kernelXBuffer, int kernelXSize,
    ::cl::Buffer kernelYBuffer, int kernelYSize, float scale, ::cl::Buffer &dst, size_t dstOffset, ::cl::Buffer &scratch, std::vector<::cl::Event> *events, ::cl::Event &event);

//Performs convolve in 2 seperate kernels using local memory for image storage
cvlib_EXPORT void separableConvolve_local(::cl::Context context, ::cl::CommandQueue commandQueue, ::cl::Image2D &src, size_t width, size_t height, ::cl::Buffer kernelXBuffer, int kernelXSize,
    ::cl::Buffer kernelYBuffer, int kernelYSize, float scale, ::cl::Image2D &dst, ::cl::Image2D &scratch, std::vector<::cl::Event> *events, ::cl::Event &event);

//Performs convolve in single kernel using local memory for image storage
cvlib_EXPORT void separableConvolve_localXY(::cl::Context context, ::cl::CommandQueue commandQueue, ::cl::Image2D &src, size_t width, size_t height, ::cl::Buffer kernelXBuffer, ::cl::Buffer kernelYBuffer,
    int kernelSize, float scale, ::cl::Image2D &dst, std::vector<::cl::Event> *events, ::cl::Event &event);
///
/// Constructs guassian kernel and copies it to a cl::Buffer object that is retuned.
///
cvlib_EXPORT::cl::Buffer buildGaussianKernel(::cl::Context context, ::cl::CommandQueue commandQueue, float sigma, int &filterSize);

///
/// Performs guassian convolution, will be cl::Buffer internally
///
cvlib_EXPORT void gaussianConvolution(::cl::Context context, ::cl::CommandQueue commandQueue, ::cl::Image2D &src, ::cl::Image2D &dst, size_t width, size_t height, float sigma, std::vector<::cl::Event> *events, ::cl::Event &event);

///
/// Construct guassian kernel as 2 kernels that work on the x/y direction.
///
cvlib_EXPORT::cl::Buffer buildGaussianSeparableKernel(::cl::Context context, float sigma, int &filterSize);

///
///
///
cvlib_EXPORT void gaussianSeparable(::cl::Context context, ::cl::CommandQueue commandQueue, ::cl::Image2D &src, ::cl::Image2D &dst, size_t width, size_t height, float sigma, std::vector<::cl::Event> *events, ::cl::Event &event);
cvlib_EXPORT void gaussianSeparable(::cl::Context context, ::cl::CommandQueue commandQueue, ::cl::Image2D &src, ::cl::Image2D &dst, size_t width, size_t height, ::cl::Buffer kernelBuffer, int kernelSize, ::cl::Image2D scratch,
    std::vector<::cl::Event> *events, ::cl::Event &event);
cvlib_EXPORT void gaussianSeparable(::cl::Context context, ::cl::CommandQueue commandQueue, ::cl::Image2D &src, ::cl::Image2D &dst, size_t width, size_t height, ::cl::Buffer kernelBuffer, int kernelSize,
    std::vector<::cl::Event> *events, ::cl::Event &event);

struct cvlib_EXPORT ScharrSeparableKernel
{
    ::cl::Buffer smooth;
    ::cl::Buffer edge;
};

///
///
///
cvlib_EXPORT ScharrSeparableKernel buildScharrSeparableKernel(::cl::Context context, int scale, int &kernelSize, bool normalize);

///
///
///
cvlib_EXPORT void scharrSeparable(::cl::Context context, ::cl::CommandQueue commandQueue, ::cl::Image2D &src, size_t width, size_t height, int size, float scale, bool yKernel, bool normalize, ::cl::Image2D &dst, std::vector<::cl::Event> *events, ::cl::Event &event);
cvlib_EXPORT void scharrSeparable(::cl::Context context, ::cl::CommandQueue commandQueue, ::cl::Image2D &src, size_t width, size_t height, int size, float scale, bool yKernel, bool normalize, ::cl::Image2D &dst,
    ScharrSeparableKernel &kernelBuffer, int kernelSize, ::cl::Image2D scratch, std::vector<::cl::Event> *events, ::cl::Event &event);

cvlib_EXPORT void scharrSeparable(::cl::Context context, ::cl::CommandQueue commandQueue, ::cl::Image2D &src, size_t width, size_t height, int size, float scale, bool yKernel, bool normalize, ::cl::Buffer &dst, size_t offset,
    std::vector<::cl::Event> *events, ::cl::Event &event);
cvlib_EXPORT void scharrSeparable(::cl::Context context, ::cl::CommandQueue commandQueue, ::cl::Buffer &src, size_t srcOffset, size_t width, size_t height, int size, float scale, bool yKernel, bool normalize,
    ::cl::Buffer &dst, size_t dstOffset, std::vector<::cl::Event> *events, ::cl::Event &event);

}}//namespace cvlib::cl

#endif //_cvlib_convolve_cl_h_