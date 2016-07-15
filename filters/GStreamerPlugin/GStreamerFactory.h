#ifndef _GStreamerFactory_h_
#define _GStreamerFactory_h_

#include <boost/utility/singleton.hpp>

class GStreamerFactory:public boost::singleton<GStreamerFactory>
{
private:
	GStreamerFactory(boost::restricted);
};
#endif //_GStreamerFactory_h_