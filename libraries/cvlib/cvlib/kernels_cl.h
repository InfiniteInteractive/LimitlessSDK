#ifndef _cvlib_kernels_cl_h_
#define _cvlib_kernels_cl_h_

#include "cvlibDefine.h"

#include <CL/cl.hpp>
#include <memory>

namespace cvlib{namespace cl
{

struct cvlib_EXPORT KernelInfo
{
    ::cl::Kernel kernel;
    
    size_t workGroupSize;
    size_t preferredWorkGroupMultiple;
    cl_ulong localMemoryUsed;

    cl_uint deviceComputeUnits;
    cl_ulong deviceLocalMemory;
};
typedef std::shared_ptr<KernelInfo> SharedKernelInfo;

::cl::Kernel getKernel(::cl::Context &context, std::string kernelName, std::string programName, std::string &source);
SharedKernelInfo getKernelInfo(::cl::Context &context, std::string kernelName, std::string programName, std::string &source);

}}//namespace cvlib::cl

#endif //_cvlib_kernels_cl_h_