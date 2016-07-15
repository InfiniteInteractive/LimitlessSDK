#ifndef _GStreamerPlugin_h_
#define _GStreamerPlugin_h_

#include "Media/MediaPluginFactory.h"
#include "Media/IMediaFilter.h"

class GStreamerPlugin:public MediaAutoRegister<GStreamerPlugin, IMediaFilter>
{
public:
	GStreamerPlugin(std::string instance, SharedMediaFilter parent);
	virtual ~GStreamerPlugin();

	virtual bool initialize(const Attributes &attributes);
	virtual bool shutdown();

	SharedMediaSample getSample(int &index);
	virtual bool processSample(SharedMediaPad sinkPad, SharedMediaSample sample);

private:
};

#endif //_GStreamerPlugin_h_