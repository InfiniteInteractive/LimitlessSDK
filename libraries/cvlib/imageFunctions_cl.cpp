#include "cvlib/imageFunctions_cl.h"
#include "cvlib/kernels_cl.h"

#include <string>
#include <cassert>

//from emebeded opencl source
extern std::string imageFunctions_cl;

namespace cvlib{namespace cl
{

void zeroImage(::cl::Context &context, ::cl::CommandQueue &commandQueue, ::cl::Image &image, std::vector<::cl::Event> *events, ::cl::Event *event)
{
    cl_int status;
    size_t width, height;
    ::cl::Kernel kernel=getKernel(context, "zeroFloatImage", "imageFunctions.cl", imageFunctions_cl);

    assert(kernel());

    image.getImageInfo(CL_IMAGE_WIDTH, &width);
    image.getImageInfo(CL_IMAGE_HEIGHT, &height);
    
    status=kernel.setArg(0, image);
    assert(status==CL_SUCCESS);

    ::cl::NDRange globalThreads(width, height);

    status=commandQueue.enqueueNDRangeKernel(kernel, ::cl::NullRange, globalThreads, ::cl::NullRange, events, event);
    assert(status==CL_SUCCESS);
}

float sum(::cl::Context &context, ::cl::CommandQueue &commandQueue, ::cl::Image &src, std::vector<::cl::Event> *events)
{
    ::cl::Buffer sumBuffer;
    size_t width, height;
    std::vector<::cl::Event> event(1);
    ::cl::Event readEvent;
    float sumValue;

    sumBuffer=::cl::Buffer(context, CL_MEM_READ_ONLY, sizeof(cl_float), nullptr);

    src.getImageInfo(CL_IMAGE_WIDTH, &width);
    src.getImageInfo(CL_IMAGE_HEIGHT, &height);

    sum(context, commandQueue, src, width, height, sumBuffer, 0, events, &event[0]);

    commandQueue.enqueueReadBuffer(sumBuffer, CL_FALSE, 0, sizeof(float), &sumValue, &event, &readEvent);
    readEvent.wait();

    return sumValue;
}

void sum(::cl::Context &context, ::cl::CommandQueue &commandQueue, ::cl::Image &src, int width, int height, ::cl::Buffer &sum, int offset, std::vector<::cl::Event> *events, ::cl::Event *event)
{
    cl_int status;
    ::cl::Kernel kernel=getKernel(context, "sum", "imageFunctions.cl", imageFunctions_cl);

    assert(kernel());

    status=kernel.setArg(0, src);
    assert(status==CL_SUCCESS);
    status=kernel.setArg(1, width);
    assert(status==CL_SUCCESS);
    status=kernel.setArg(2, height);
    assert(status==CL_SUCCESS);
    status=kernel.setArg(3, sum);
    assert(status==CL_SUCCESS);
    status=kernel.setArg(4, offset);
    assert(status==CL_SUCCESS);

    ::cl::NDRange globalThreads(height);

    status=commandQueue.enqueueNDRangeKernel(kernel, ::cl::NullRange, globalThreads, ::cl::NullRange, events, event);
    assert(status==CL_SUCCESS);
}

}}//namespace libAKAZE::cl