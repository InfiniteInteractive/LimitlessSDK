#include "cvlib/scharr_cl.h"
#include "cvlib/kernels_cl.h"

#include "cvlib/convolve.h"

#include "Eigen/Core"

//from emebeded opencl source
extern std::string convolve_cl;

namespace cvlib{namespace cl
{

SeparableKernel buildScharrSeparableKernel(::cl::Context &context, int kernelSize, bool normalize)
{
    SeparableKernel kernel;

    assert(kernelSize%2==1);
    int convolveSize=kernelSize/2;

    Eigen::Matrix<float, Eigen::Dynamic, Eigen::Dynamic> smoothingKernel;
    Eigen::Matrix<float, Eigen::Dynamic, Eigen::Dynamic> edgeKernel;
    Eigen::Vector3f smoothingVector;
    Eigen::Vector3f edgeVector;

    float smoothNorm=16.0;
    float edgeNorm=2.0;

    smoothingVector<<3, 10, 3;
    edgeVector<<-1, 0, 1;

    smoothingKernel=smoothingVector;
    edgeKernel=edgeVector;

    while(convolveSize>1)
    {
        smoothingKernel=convolveMatrix(smoothingKernel, smoothingVector);
        smoothNorm*=16.0;
        edgeKernel=convolveMatrix(edgeKernel, smoothingVector);
        edgeNorm*=16.0;
        convolveSize--;
    }

    if(normalize)
    {
        smoothingKernel/=smoothNorm;
        edgeKernel/=edgeNorm;
    }

    kernel.kernel1=::cl::Buffer(context, CL_MEM_READ_ONLY|CL_MEM_COPY_HOST_PTR, sizeof(cl_float)*smoothingKernel.size(), smoothingKernel.data());
    kernel.kernel2=::cl::Buffer(context, CL_MEM_READ_ONLY|CL_MEM_COPY_HOST_PTR, sizeof(cl_float)*edgeKernel.size(), edgeKernel.data());
    kernel.size=kernelSize;

    return kernel;
}

void scharr(::cl::Context &context, ::cl::CommandQueue &commandQueue, ::cl::Image2D &src, ::cl::Image2D &dst, int kernelSize, Direction direction, bool normalize, float scale,
    std::vector<::cl::Event> *events, ::cl::Event *event)
{
    int width, height;

    src.getImageInfo(CL_IMAGE_WIDTH, &width);
    src.getImageInfo(CL_IMAGE_HEIGHT, &height);
    
    scharr(context, commandQueue, src, width, height, dst, kernelSize, direction, normalize, scale, events, event);
}

void scharr(::cl::Context &context, ::cl::CommandQueue &commandQueue, ::cl::Image2D &src, size_t width, size_t height, ::cl::Image2D &dst, int kernelSize, Direction direction, bool normalize, float scale,
    std::vector<::cl::Event> *events, ::cl::Event *event)
{
    SeparableKernel convolveKernel=buildScharrSeparableKernel(context, kernelSize, normalize);
    cl_image_format format;

    src.getImageInfo(CL_IMAGE_FORMAT, &format);

    ::cl::Image2D scratch(context, CL_MEM_READ_WRITE, ::cl::ImageFormat(format.image_channel_order, format.image_channel_data_type), width, height);

    scharr(context, commandQueue, src, width, height, dst, scratch, convolveKernel, direction, normalize, scale, events, event);

    if(event!=nullptr)
        event->wait();
}

void scharr(::cl::Context &context, ::cl::CommandQueue &commandQueue, ::cl::Image2D &src, size_t width, size_t height, ::cl::Image2D &dst, ::cl::Image2D &scratch, SeparableKernel &kernelBuffer, 
    Direction direction, bool normalize, float scale, std::vector<::cl::Event> *events, ::cl::Event *event)
{
    if(direction==Direction::Y)
        separableConvolve(context, commandQueue, src, width, height, dst, scratch, kernelBuffer.kernel1, kernelBuffer.size, kernelBuffer.kernel2, kernelBuffer.size, scale, events, event);
    else
        separableConvolve(context, commandQueue, src, width, height, dst, scratch, kernelBuffer.kernel2, kernelBuffer.size, kernelBuffer.kernel1, kernelBuffer.size, scale, events, event);
}

}}//namespace libAKAZE::cl