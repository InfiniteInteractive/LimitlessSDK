#include "cvlib/convolve_cl.h"
#include "cvlib/kernels_cl.h"
#include "cvlib/convolve.h"

#include "Eigen/Core"

//from emebeded opencl source
extern std::string convolve_cl;

namespace cvlib{namespace cl
{

void zeroBuffer(::cl::Context context, ::cl::CommandQueue commandQueue, ::cl::Buffer &buffer, size_t size, std::vector<::cl::Event> *events, ::cl::Event &event)
{
    cl_int status;
    ::cl::Kernel kernel=getKernel(context, "zeroMemory", "convolve.cl", convolve_cl);

    status=kernel.setArg(0, buffer);

    ::cl::NDRange globalThreads(size);

    status=commandQueue.enqueueNDRangeKernel(kernel, ::cl::NullRange, globalThreads, ::cl::NullRange, events, &event);
}

void zeroFloatBuffer(::cl::Context context, ::cl::CommandQueue commandQueue, ::cl::Buffer &buffer, size_t size, std::vector<::cl::Event> *events, ::cl::Event &event)
{
    cl_int status;
    ::cl::Kernel kernel=getKernel(context, "zeroFloatMemory", "convolve.cl", convolve_cl);

    status=kernel.setArg(0, buffer);

    ::cl::NDRange globalThreads(size);

    status=commandQueue.enqueueNDRangeKernel(kernel, ::cl::NullRange, globalThreads, ::cl::NullRange, events, &event);
}

void zeroIntBuffer(::cl::Context context, ::cl::CommandQueue commandQueue, ::cl::Buffer &buffer, size_t size, std::vector<::cl::Event> *events, ::cl::Event &event)
{
    cl_int status;
    ::cl::Kernel kernel=getKernel(context, "zeroIntMemory", "convolve.cl", convolve_cl);

    status=kernel.setArg(0, buffer);

    ::cl::NDRange globalThreads(size);

    status=commandQueue.enqueueNDRangeKernel(kernel, ::cl::NullRange, globalThreads, ::cl::NullRange, events, &event);
}

void zeroImage(::cl::Context context, ::cl::CommandQueue commandQueue, ::cl::Image2D &image, std::vector<::cl::Event> *events, ::cl::Event &event)
{
    cl_int status;
    size_t width, height;
    ::cl::Kernel kernel=getKernel(context, "zeroFloatImage", "convolve.cl", convolve_cl);

    image.getImageInfo(CL_IMAGE_WIDTH, &width);
    image.getImageInfo(CL_IMAGE_HEIGHT, &height);
    status=kernel.setArg(0, image);
    
    ::cl::NDRange globalThreads(width, height);
    
    status=commandQueue.enqueueNDRangeKernel(kernel, ::cl::NullRange, globalThreads, ::cl::NullRange, events, &event);
}

void separableConvolve(::cl::Context context, ::cl::CommandQueue commandQueue, ::cl::Image2D &src, ::cl::Buffer kernelXBuffer, int kernelXSize, ::cl::Buffer kernelYBuffer, int kernelYSize, ::cl::Image2D &dst)
{
    ::cl::Event event;

    size_t width, height;
    cl_image_format format;
        
    src.getImageInfo(CL_IMAGE_WIDTH, &width);
    src.getImageInfo(CL_IMAGE_HEIGHT, &height);
    src.getImageInfo(CL_IMAGE_FORMAT, &format);

    ::cl::Image2D scratch(context, CL_MEM_READ_WRITE, ::cl::ImageFormat(format.image_channel_order, format.image_channel_data_type), width, height);

    separableConvolve(context, commandQueue, src, width, height, kernelXBuffer, kernelXSize, kernelYBuffer, kernelYSize, 1.0, dst, scratch, nullptr, event);
    event.wait();
}

void separableConvolve(::cl::Context context, ::cl::CommandQueue commandQueue, ::cl::Image2D &src, size_t width, size_t height, ::cl::Buffer kernelXBuffer, int kernelXSize, 
    ::cl::Buffer kernelYBuffer, int kernelYSize, float scale, ::cl::Image2D &dst, ::cl::Image2D &scratch, std::vector<::cl::Event> *events, ::cl::Event &event)
{
    size_t filterSize;

    ::cl::Kernel kernelX=getKernel(context, "separableConvolveXImage2D", "convolve.cl", convolve_cl);
    ::cl::Kernel kernelY=getKernel(context, "separableConvolveYImage2D", "convolve.cl", convolve_cl);
    cl_int status;
    int index=0;
    ::cl::Event kernelYEvent;

    status=kernelY.setArg(index++, src);
    status=kernelY.setArg(index++, kernelYBuffer);
    status=kernelY.setArg(index++, kernelYSize);
    status=kernelY.setArg(index++, (float)1.0); //only scale once, so no scale here
    status=kernelY.setArg(index++, scratch);

    ::cl::NDRange globalThreads(width, height);

    status=commandQueue.enqueueNDRangeKernel(kernelY, ::cl::NullRange, globalThreads, ::cl::NullRange, events, &kernelYEvent);

    std::vector<::cl::Event> kernelYEvents={kernelYEvent};
    index=0;
    
    status=kernelX.setArg(index++, scratch);
    status=kernelX.setArg(index++, kernelXBuffer);
    status=kernelX.setArg(index++, kernelXSize);
    status=kernelX.setArg(index++, scale);
    status=kernelX.setArg(index++, dst);

    status=commandQueue.enqueueNDRangeKernel(kernelX, ::cl::NullRange, globalThreads, ::cl::NullRange, &kernelYEvents, &event);

//    commandQueue.flush();
}

void separableConvolve_local(::cl::Context context, ::cl::CommandQueue commandQueue, ::cl::Image2D &src, size_t width, size_t height, ::cl::Buffer kernelXBuffer, int kernelXSize,
    ::cl::Buffer kernelYBuffer, int kernelYSize, float scale, ::cl::Image2D &dst, ::cl::Image2D &scratch, std::vector<::cl::Event> *events, ::cl::Event &event)
{
    size_t filterSize;

    ::cl::Kernel kernelX=getKernel(context, "separableConvolveXImage2D_local", "convolve.cl", convolve_cl);
    ::cl::Kernel kernelY=getKernel(context, "separableConvolveYImage2D_local", "convolve.cl", convolve_cl);
    cl_int status;
    int index=0;
    ::cl::Event kernelYEvent;

    size_t localX=16;
    size_t localY=16;
    size_t globalX=(width/localX)*localX;
    size_t globalY=(height/localY)*localY;

    if(globalX<width)
        globalX+=localX;
    if(globalY<height)
        globalY+=localY;

    int cacheX=(kernelXSize/2)*2+localX;
    int cacheY=(kernelYSize/2)*2+localY;

    status=kernelY.setArg(index++, src);
    status=kernelY.setArg(index++, (int)width);
    status=kernelY.setArg(index++, (int)height);
    status=kernelY.setArg(index++, kernelYBuffer);
    status=kernelY.setArg(index++, kernelYSize);
    status=kernelY.setArg(index++, (float)1.0); //only scale once, so no scale here
    status=kernelY.setArg(index++, scratch);
    status=kernelY.setArg(index++, cacheX*cacheY*sizeof(float), nullptr); //setup local image cache

    ::cl::NDRange globalThreads(globalX, globalY);
    ::cl::NDRange localThreads(localX, localY);

    status=commandQueue.enqueueNDRangeKernel(kernelY, ::cl::NullRange, globalThreads, localThreads, events, &kernelYEvent);

    std::vector<::cl::Event> kernelYEvents={kernelYEvent};
    index=0;

    status=kernelX.setArg(index++, scratch);
    status=kernelX.setArg(index++, (int)width);
    status=kernelX.setArg(index++, (int)height);
    status=kernelX.setArg(index++, kernelXBuffer);
    status=kernelX.setArg(index++, kernelXSize);
    status=kernelX.setArg(index++, scale);
    status=kernelX.setArg(index++, dst);
    status=kernelX.setArg(index++, cacheX*cacheY*sizeof(float), nullptr); //setup local image cache

    status=commandQueue.enqueueNDRangeKernel(kernelX, ::cl::NullRange, globalThreads, localThreads, &kernelYEvents, &event);

    //    commandQueue.flush();
}

void separableConvolve(::cl::Context context, ::cl::CommandQueue commandQueue, ::cl::Image2D &src, size_t width, size_t height, ::cl::Buffer kernelXBuffer, int kernelXSize, 
    ::cl::Buffer kernelYBuffer, int kernelYSize, float scale, ::cl::Buffer &dst, size_t offset, ::cl::Image2D &scratch, std::vector<::cl::Event> *events, ::cl::Event &event)
{
    size_t filterSize;

    ::cl::Kernel kernelX=getKernel(context, "separableConvolveXImage2DBuffer", "convolve.cl", convolve_cl);
    ::cl::Kernel kernelY=getKernel(context, "separableConvolveYImage2D", "convolve.cl", convolve_cl);
    cl_int status;
    int index=0;
    ::cl::Event kernelYEvent;

    status=kernelY.setArg(index++, src);
    status=kernelY.setArg(index++, kernelYBuffer);
    status=kernelY.setArg(index++, kernelYSize);
    status=kernelY.setArg(index++, (float)1.0); //only scale once, so no scale here
    status=kernelY.setArg(index++, scratch);

    ::cl::NDRange globalThreads(width, height);

    status=commandQueue.enqueueNDRangeKernel(kernelY, ::cl::NullRange, globalThreads, ::cl::NullRange, events, &kernelYEvent);

    std::vector<::cl::Event> kernelYEvents={kernelYEvent};
    index=0;

    //(read_only image2d_t input, __constant float *kernelX, const int kernelSize, float scale, __global float *output, int offset, int width, int height)

    status=kernelX.setArg(index++, scratch);
    status=kernelX.setArg(index++, kernelXBuffer);
    status=kernelX.setArg(index++, kernelXSize);
    status=kernelX.setArg(index++, scale);
    status=kernelX.setArg(index++, dst);
    status=kernelX.setArg(index++, (int)offset);
    status=kernelX.setArg(index++, (int)width);
    status=kernelX.setArg(index++, (int)height);

    status=commandQueue.enqueueNDRangeKernel(kernelX, ::cl::NullRange, globalThreads, ::cl::NullRange, &kernelYEvents, &event);

//    commandQueue.flush();
}

void separableConvolve(::cl::Context context, ::cl::CommandQueue commandQueue, ::cl::Buffer &src, size_t srcOffset, size_t width, size_t height, ::cl::Buffer kernelXBuffer, int kernelXSize,
    ::cl::Buffer kernelYBuffer, int kernelYSize, float scale, ::cl::Buffer &dst, size_t dstOffset, ::cl::Buffer &scratch, std::vector<::cl::Event> *events, ::cl::Event &event)
{
    ::cl::Kernel kernelX=getKernel(context, "separableConvolveXBuffer", "convolve.cl", convolve_cl);
    ::cl::Kernel kernelY=getKernel(context, "separableConvolveYBuffer", "convolve.cl", convolve_cl);
    cl_int status;
    int index=0;
    ::cl::Event kernelYEvent;

    status=kernelY.setArg(index++, src);
    status=kernelY.setArg(index++, (int)srcOffset);
    status=kernelY.setArg(index++, (int)width);
    status=kernelY.setArg(index++, (int)height);
    status=kernelY.setArg(index++, kernelYBuffer);
    status=kernelY.setArg(index++, kernelYSize);
    status=kernelY.setArg(index++, (float)1.0); //only scale once, so no scale here
    status=kernelY.setArg(index++, scratch);
    status=kernelY.setArg(index++, (int)0);

    ::cl::NDRange globalThreads(width, height);

    status=commandQueue.enqueueNDRangeKernel(kernelY, ::cl::NullRange, globalThreads, ::cl::NullRange, events, &kernelYEvent);

    std::vector<::cl::Event> kernelYEvents={kernelYEvent};
    
    index=0;
    status=kernelX.setArg(index++, scratch);
    status=kernelX.setArg(index++, (int)0);
    status=kernelX.setArg(index++, (int)width);
    status=kernelX.setArg(index++, (int)height);
    status=kernelX.setArg(index++, kernelXBuffer);
    status=kernelX.setArg(index++, kernelXSize);
    status=kernelX.setArg(index++, scale);
    status=kernelX.setArg(index++, dst);
    status=kernelX.setArg(index++, (int)dstOffset);

    status=commandQueue.enqueueNDRangeKernel(kernelX, ::cl::NullRange, globalThreads, ::cl::NullRange, &kernelYEvents, &event);

//    commandQueue.flush();
}

void separableConvolve_localXY(::cl::Context context, ::cl::CommandQueue commandQueue, ::cl::Image2D &src, size_t width, size_t height, ::cl::Buffer kernelXBuffer, ::cl::Buffer kernelYBuffer, 
    int kernelSize, float scale, ::cl::Image2D &dst, std::vector<::cl::Event> *events, ::cl::Event &event)
{
    ::cl::Kernel kernel=getKernel(context, "separableConvolveImage2DXY", "convolve.cl", convolve_cl);
    cl_int status;
    int index=0;

    size_t localX=16;
    size_t localY=16;
    size_t globalX=(width/localX)*localX;
    size_t globalY=(height/localY)*localY;

    if(globalX<width)
        globalX+=localX;
    if(globalY<height)
        globalY+=localY;

    int cacheX=(kernelSize/2)*2+localX;
    int cacheY=(kernelSize/2)*2+localY;

    status=kernel.setArg(index++, src);
    status=kernel.setArg(index++, (int)width);
    status=kernel.setArg(index++, (int)height);
    status=kernel.setArg(index++, kernelXBuffer);
    status=kernel.setArg(index++, kernelYBuffer);
    status=kernel.setArg(index++, kernelSize);
    status=kernel.setArg(index++, scale);
    status=kernel.setArg(index++, dst);
    status=kernel.setArg(index++, cacheX*cacheY*sizeof(float), nullptr); //setup local image cache

    ::cl::NDRange globalThreads(globalX, globalY);
    ::cl::NDRange localThreads(localX, localY);

    status=commandQueue.enqueueNDRangeKernel(kernel, ::cl::NullRange, globalThreads, localThreads, events, &event);
}

::cl::Buffer buildGaussianKernel(::cl::Context context, ::cl::CommandQueue commandQueue, float sigma, int &filterSize)
{
    int size=(int)ceil((1.0+(sigma-0.8)/(0.3)));
    float sum=0.0f;
    float twoSigmaSquared=2*sigma*sigma;
    
    filterSize=2*size+1;
    float *filter=(float *)malloc(filterSize*filterSize*sizeof(float));

    for(int x=-size; x < size+1; x++)
    {
        for(int y=-size; y < size+1; y++)
        {
            float temp=exp(-((float)(x*x+y*y)/twoSigmaSquared));

            sum+=temp;
            filter[x+size+(y+size)*(filterSize)]=temp;
        }
    }
    
    //normalize filter
    for(int i=0; i < filterSize*filterSize; i++)
        filter[i]=filter[i]/sum;

//    ::cl::Buffer filterBuffer(context, CL_MEM_READ_ONLY|CL_MEM_USE_HOST_PTR, sizeof(cl_float)*filterSize*filterSize, filter);
    ::cl::Buffer filterBuffer(context, CL_MEM_READ_ONLY|CL_MEM_COPY_HOST_PTR, sizeof(cl_float)*filterSize*filterSize, filter);
    
//    commandQueue.enqueueWriteBuffer(filterBuffer, true, 0, )
    
    free(filter);

    return filterBuffer;
}

::cl::Buffer buildGaussianSeparableKernel(::cl::Context context, float sigma, int &kernelSize)
{
    int size=(int)ceil((1.0+(sigma-0.8)/(0.3)));
    float sum=0.0f;
    float twoSigmaSquared=2*sigma*sigma;

    kernelSize=2*size+1;
    float *kernel=(float *)malloc(kernelSize*sizeof(float));
    float temp;

    int x=-size;
    for(int i=0; i<kernelSize; ++i, ++x)
    {
        temp=std::exp(-(float)(x*x)/twoSigmaSquared);
        
        kernel[i]=temp;
        sum+=temp;
    }

    //normalize filter
    for(int i=0; i < kernelSize; i++)
        kernel[i]=kernel[i]/sum;

    ::cl::Buffer kernelBuffer(context, CL_MEM_READ_ONLY|CL_MEM_COPY_HOST_PTR, sizeof(cl_float)*kernelSize, kernel);
    free(kernel);

    return kernelBuffer;
}

void gaussianConvolution(::cl::Context context, ::cl::CommandQueue commandQueue, ::cl::Image2D &src, ::cl::Image2D &dst, size_t width, size_t height, float sigma, std::vector<::cl::Event> *events, ::cl::Event &event)
{
    int kernelSize;// =computeKernelSize(sigma);

    ::cl::Buffer kernelBuffer=buildGaussianKernel(context, commandQueue, sigma, kernelSize);

    ::cl::Kernel kernel=getKernel(context, "convolve", "convolve.cl", convolve_cl);
    cl_int status;
    int index=0;

    status=kernel.setArg(index++, src);
    status=kernel.setArg(index++, kernelBuffer);
    status=kernel.setArg(index++, kernelSize);
    status=kernel.setArg(index++, dst);

    ::cl::NDRange globalThreads(width, height);

    status=commandQueue.enqueueNDRangeKernel(kernel, ::cl::NullRange, globalThreads, ::cl::NullRange, events, &event);

//    commandQueue.flush();
//    event.wait();
}

void gaussianSeparable(::cl::Context context, ::cl::CommandQueue commandQueue, ::cl::Image2D &src, ::cl::Image2D &dst, size_t width, size_t height, float sigma, std::vector<::cl::Event> *events, ::cl::Event &event)
{
    int kernelSize;

    ::cl::Buffer kernelBuffer=buildGaussianSeparableKernel(context, sigma, kernelSize);

//    size_t width, height;
    cl_image_format format;

//    src.getImageInfo(CL_IMAGE_WIDTH, &width);
//    src.getImageInfo(CL_IMAGE_HEIGHT, &height);
    src.getImageInfo(CL_IMAGE_FORMAT, &format);

//    ::cl::Image2D scratch(context, CL_MEM_READ_WRITE, ::cl::ImageFormat(format.image_channel_order, format.image_channel_data_type), width, height);
//
//    separableConvolve(context, commandQueue, src, width, height, kernelBuffer, kernelSize, kernelBuffer, kernelSize, 1.0, dst, scratch, events, event);
    separableConvolve_localXY(context, commandQueue, src, width, height, kernelBuffer, kernelBuffer, kernelSize, 1.0, dst, events, event);
}

void gaussianSeparable(::cl::Context context, ::cl::CommandQueue commandQueue, ::cl::Image2D &src, ::cl::Image2D &dst, size_t width, size_t height, ::cl::Buffer kernelBuffer, int kernelSize, ::cl::Image2D scratch, 
    std::vector<::cl::Event> *events, ::cl::Event &event)
{
    separableConvolve(context, commandQueue, src, width, height, kernelBuffer, kernelSize, kernelBuffer, kernelSize, 1.0, dst, scratch, events, event);
}

void gaussianSeparable(::cl::Context context, ::cl::CommandQueue commandQueue, ::cl::Image2D &src, ::cl::Image2D &dst, size_t width, size_t height, ::cl::Buffer kernelBuffer, int kernelSize,
    std::vector<::cl::Event> *events, ::cl::Event &event)
{
    separableConvolve_localXY(context, commandQueue, src, width, height, kernelBuffer, kernelBuffer, kernelSize, 1.0, dst, events, event);
}

::cl::Buffer buildScharrFilter(::cl::Context context, int scale)
{
    ::cl::Buffer kernel;
    const int kernelSize=3+2*(scale-1);

//    std::vector<float> kernel
    return kernel;
}

ScharrSeparableKernel buildScharrSeparableKernel(::cl::Context context, int size, int &kernelSize, bool normalize)
{
    ScharrSeparableKernel kernel;

    kernelSize=(2*size)+1;
    
    if(false)
    {
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

        while(size>1)
        {
            smoothingKernel=convolveMatrix(smoothingKernel, smoothingVector);
            smoothNorm*=16.0;
            edgeKernel=convolveMatrix(edgeKernel, smoothingVector);
            edgeNorm*=16.0;
            size--;
        }

        if(normalize)
        {
            smoothingKernel/=smoothNorm;
            edgeKernel/=edgeNorm;
        }

        kernel.smooth=::cl::Buffer(context, CL_MEM_READ_ONLY|CL_MEM_COPY_HOST_PTR, sizeof(cl_float)*smoothingKernel.size(), smoothingKernel.data());
        kernel.edge=::cl::Buffer(context, CL_MEM_READ_ONLY|CL_MEM_COPY_HOST_PTR, sizeof(cl_float)*edgeKernel.size(), edgeKernel.data());
    }
    else
    {
        Eigen::RowVectorXf smoothingVector(kernelSize);
        Eigen::RowVectorXf edgeVector(kernelSize);

        smoothingVector.setZero();
        if(normalize)
        {
            float w=10.0/3.0;
            float norm=1.0/(2.0 * size * (w+2.0));

            smoothingVector(0)=norm;
            smoothingVector(kernelSize/2)=w * norm;
            smoothingVector(kernelSize-1)=norm;
        }
        else
        {
            smoothingVector(0)=3;
            smoothingVector(kernelSize/2)=10;
            smoothingVector(kernelSize-1)=3;
        }

        edgeVector.setZero();
        edgeVector(0)=-1.0;
        edgeVector(kernelSize-1)=1.0;

        kernel.smooth=::cl::Buffer(context, CL_MEM_READ_ONLY|CL_MEM_COPY_HOST_PTR, sizeof(cl_float)*smoothingVector.size(), smoothingVector.data());
        kernel.edge=::cl::Buffer(context, CL_MEM_READ_ONLY|CL_MEM_COPY_HOST_PTR, sizeof(cl_float)*edgeVector.size(), edgeVector.data());
    }
    return kernel;
}

//void scharrConvolution(::cl::Context context, ::cl::CommandQueue commandQueue, ::cl::Image2D &src, ::cl::Image2D &dst, size_t width, size_t height, float sigma, std::vector<::cl::Event> *events, ::cl::Event event)
//{
//    size_t filterSize;
//
//    builsdScharrKernel(sigma, ksize_x, ksize_y);
//    ::cl::Buffer filterBuffer=buildGaussianFilter(context, sigma, filterSize);
//
//    ::cl::Kernel kernel=getKernel(context, "convolve", "kernels/convolve.cl");
//    cl_int status;
//    int index=0;
//
//    status=kernel.setArg(index++, src);
//    status=kernel.setArg(index++, width);
//    status=kernel.setArg(index++, filterBuffer);
//    status=kernel.setArg(index++, filterSize);
//    status=kernel.setArg(index++, dst);
//
//    ::cl::NDRange globalThreads(width, height);
//
//    status=commandQueue.enqueueNDRangeKernel(kernel, ::cl::NullRange, globalThreads, ::cl::NullRange, events, &event);
//}

void scharrSeparable(::cl::Context context, ::cl::CommandQueue commandQueue, ::cl::Image2D &src, size_t width, size_t height, int size, float scale, bool yKernel, bool normalize, 
    ::cl::Image2D &dst, std::vector<::cl::Event> *events, ::cl::Event &event)
{
    int kernelSize;
    ScharrSeparableKernel kernelBuffer=buildScharrSeparableKernel(context, size, kernelSize, normalize);
    cl_image_format format;

    src.getImageInfo(CL_IMAGE_FORMAT, &format);

    ::cl::Image2D scratch(context, CL_MEM_READ_WRITE, ::cl::ImageFormat(format.image_channel_order, format.image_channel_data_type), width, height);

    scharrSeparable(context, commandQueue, src, width, height, size, scale, yKernel, normalize, dst,
        kernelBuffer, kernelSize, scratch, events, event);
}

void scharrSeparable(::cl::Context context, ::cl::CommandQueue commandQueue, ::cl::Image2D &src, size_t width, size_t height, int size, float scale, bool yKernel, bool normalize, ::cl::Image2D &dst, 
    ScharrSeparableKernel &kernelBuffer, int kernelSize, ::cl::Image2D scratch, std::vector<::cl::Event> *events, ::cl::Event &event)
{
    if(yKernel)
        separableConvolve(context, commandQueue, src, width, height, kernelBuffer.smooth, kernelSize, kernelBuffer.edge, kernelSize, scale, dst, scratch, events, event);
    else
        separableConvolve(context, commandQueue, src, width, height, kernelBuffer.edge, kernelSize, kernelBuffer.smooth, kernelSize, scale, dst, scratch, events, event);
}

void scharrSeparable(::cl::Context context, ::cl::CommandQueue commandQueue, ::cl::Image2D &src, size_t width, size_t height, int size, float scale, bool yKernel, bool normalize, ::cl::Buffer &dst, size_t offset, 
    std::vector<::cl::Event> *events, ::cl::Event &event)
{
    int kernelSize;
    ScharrSeparableKernel kernelBuffer=buildScharrSeparableKernel(context, size, kernelSize, normalize);
    cl_image_format format;

    src.getImageInfo(CL_IMAGE_FORMAT, &format);

    ::cl::Image2D scratch(context, CL_MEM_READ_WRITE, ::cl::ImageFormat(format.image_channel_order, format.image_channel_data_type), width, height);

    if(yKernel)
        separableConvolve(context, commandQueue, src, width, height, kernelBuffer.smooth, kernelSize, kernelBuffer.edge, kernelSize, scale, dst, offset, scratch, events, event);
    else
        separableConvolve(context, commandQueue, src, width, height, kernelBuffer.edge, kernelSize, kernelBuffer.smooth, kernelSize, scale, dst, offset, scratch, events, event);
}

void scharrSeparable(::cl::Context context, ::cl::CommandQueue commandQueue, ::cl::Buffer &src, size_t srcOffset, size_t width, size_t height, int size, float scale, bool yKernel, bool normalize, 
    ::cl::Buffer &dst, size_t dstOffset, std::vector<::cl::Event> *events, ::cl::Event &event)
{
    int kernelSize;
    ScharrSeparableKernel kernelBuffer=buildScharrSeparableKernel(context, size, kernelSize, normalize);
    cl_image_format format;

    ::cl::Buffer scratch(context, CL_MEM_READ_WRITE, width*height*sizeof(cl_float));

    if(yKernel)
        separableConvolve(context, commandQueue, src, srcOffset, width, height, kernelBuffer.smooth, kernelSize, kernelBuffer.edge, kernelSize, scale, dst, dstOffset, scratch, events, event);
    else
        separableConvolve(context, commandQueue, src, srcOffset, width, height, kernelBuffer.edge, kernelSize, kernelBuffer.smooth, kernelSize, scale, dst, dstOffset, scratch, events, event);
}

}}//namespace libAKAZE::cl