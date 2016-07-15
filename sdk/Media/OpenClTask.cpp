#include "OpenClTask.h"
#include "Base/Log.h"

#include <boost/format.hpp>

namespace Limitless
{

void WriteImageTask::process()
{
	result=commandQueue.enqueueWriteImage(*image, blocking, origin, region, row_pitch, slice_pitch, ptr, events, event);
}


}//namespace Limitless
