#include "cvlib/kernels_cl.h"

#include <memory>
#include <unordered_map>
#include <cstdio>
#include <fstream>
#include <iomanip>
#include <iostream>


namespace cvlib{namespace cl
{

typedef std::unordered_map<std::string, SharedKernelInfo> KernelMap;
typedef std::unordered_map<cl_context, KernelMap> ContextKernelMap;

typedef std::unordered_map<std::string, ::cl::Program> ProgramMap;
typedef std::unordered_map<cl_context, ProgramMap> ContextProgramMap;

::cl::Program getProgram(::cl::Context &context, std::string name, std::string &source)
{
    static ContextProgramMap contextProgramMap;

    ProgramMap &programs=contextProgramMap[context()];
    ProgramMap::iterator iter=programs.find(name);

    if(iter!=programs.end())
        return ::cl::Program(iter->second);

    ::cl::Program::Sources programSource(1, std::make_pair(source.data(), source.size()));
    ::cl::Program program=::cl::Program(context, programSource);

    cl_int error;
    std::vector<::cl::Device> devices;
    
    context.getInfo(CL_CONTEXT_DEVICES, &devices);
//    devices[0]=openCLContext->device;
    error=program.build(devices, "");

    if(error!=CL_SUCCESS)
    {
        if(error==CL_BUILD_PROGRAM_FAILURE)
        {
            std::string str=program.getBuildInfo<CL_PROGRAM_BUILD_LOG>(devices[0]);

            std::cout<<std::endl<<" BUILD LOG: "<<name<<std::endl;
            std::cout<<" ************************************************"<<std::endl;
            std::cout<<str;
            std::cout<<" ************************************************"<<std::endl;

            return false;
        }
    }

    programs.insert({name, program});
    return program;
}

::cl::Kernel getKernel(::cl::Context &context, std::string kernelName, std::string programName, std::string &source)
{
    SharedKernelInfo kernelInfo=getKernelInfo(context, kernelName, programName, source);

    if(kernelInfo)
        return kernelInfo->kernel;

    return ::cl::Kernel();
}

SharedKernelInfo getKernelInfo(::cl::Context &context, std::string kernelName, std::string programName, std::string &source)
{
    static ContextKernelMap contextKernelMap;
    
    KernelMap &kernels=contextKernelMap[context()];
    KernelMap::iterator iter=kernels.find(kernelName);

    if(iter!=kernels.end())
        return iter->second;

    ::cl::Program program=getProgram(context, programName, source);
    SharedKernelInfo kernelInfo=std::make_shared<KernelInfo>();
    std::vector<::cl::Device> devices;

    context.getInfo(CL_CONTEXT_DEVICES, &devices);

    cl_int error;
    kernelInfo->kernel=::cl::Kernel(program, kernelName.c_str(), &error);

    kernelInfo->kernel.getWorkGroupInfo(devices[0], CL_KERNEL_WORK_GROUP_SIZE, &kernelInfo->workGroupSize);
    kernelInfo->kernel.getWorkGroupInfo(devices[0], CL_KERNEL_PREFERRED_WORK_GROUP_SIZE_MULTIPLE, &kernelInfo->preferredWorkGroupMultiple);
    kernelInfo->kernel.getWorkGroupInfo(devices[0], CL_KERNEL_LOCAL_MEM_SIZE, &kernelInfo->localMemoryUsed);

    //compying info so it is available when setting up kernel call
//    kernelInfo->deviceComputeUnits=openCLContext->deviceInfo.computeUnits;
//    kernelInfo->deviceLocalMemory=openCLContext->deviceInfo.localMemory;

    kernels.insert({kernelName, kernelInfo});

    return kernelInfo;
}

}}//namespace cvlib::cl