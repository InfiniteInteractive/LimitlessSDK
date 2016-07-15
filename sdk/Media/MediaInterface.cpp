#include "MediaInterface.h"
#include "GPUContext.h"

namespace Limitless
{

void initOpenGl(DisplayHandle displayHandle)
{
	GPUContext::initOpenGL(displayHandle);
}

void initOpenCl()
{
	GPUContext::initOpenCL();
}

void closeOpenGl()
{
	GPUContext::close();
}

}//namespace Limitless