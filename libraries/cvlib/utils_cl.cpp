#include "cvlib/utils_cl.h"
#include "cvlib/kernels_cl.h"

//from emebeded opencl source
extern std::string utils_cl;

namespace cvlib{namespace cl
{

void zeroBuffer(::cl::Context &context, ::cl::CommandQueue &commandQueue, ::cl::Buffer &buffer, size_t size, std::vector<::cl::Event> *events, ::cl::Event &event)
{
    cl_int status;
    ::cl::Kernel kernel=getKernel(context, "zeroMemory", "utils.cl", utils_cl);

    status=kernel.setArg(0, buffer);

    ::cl::NDRange globalThreads(size);

    status=commandQueue.enqueueNDRangeKernel(kernel, ::cl::NullRange, globalThreads, ::cl::NullRange, events, &event);
}

void zeroFloatBuffer(::cl::Context &context, ::cl::CommandQueue &commandQueue, ::cl::Buffer &buffer, size_t size, std::vector<::cl::Event> *events, ::cl::Event &event)
{
    cl_int status;
    ::cl::Kernel kernel=getKernel(context, "zeroFloatMemory", "utils.cl", utils_cl);

    status=kernel.setArg(0, buffer);

    ::cl::NDRange globalThreads(size);

    status=commandQueue.enqueueNDRangeKernel(kernel, ::cl::NullRange, globalThreads, ::cl::NullRange, events, &event);
}

void zeroIntBuffer(::cl::Context &context, ::cl::CommandQueue &commandQueue, ::cl::Buffer &buffer, size_t size, std::vector<::cl::Event> *events, ::cl::Event &event)
{
    cl_int status;
    ::cl::Kernel kernel=getKernel(context, "zeroIntMemory", "utils.cl", utils_cl);

    status=kernel.setArg(0, buffer);

    ::cl::NDRange globalThreads(size);

    status=commandQueue.enqueueNDRangeKernel(kernel, ::cl::NullRange, globalThreads, ::cl::NullRange, events, &event);
}

void zeroImage(::cl::Context &context, ::cl::CommandQueue &commandQueue, ::cl::Image2D &image, std::vector<::cl::Event> *events, ::cl::Event &event)
{
    cl_int status;
    size_t width, height;
    ::cl::Kernel kernel=getKernel(context, "zeroFloatImage", "utils.cl", utils_cl);

    image.getImageInfo(CL_IMAGE_WIDTH, &width);
    image.getImageInfo(CL_IMAGE_HEIGHT, &height);
    status=kernel.setArg(0, image);

    ::cl::NDRange globalThreads(width, height);

    status=commandQueue.enqueueNDRangeKernel(kernel, ::cl::NullRange, globalThreads, ::cl::NullRange, events, &event);
}

}}//namespace cvlib::cl
