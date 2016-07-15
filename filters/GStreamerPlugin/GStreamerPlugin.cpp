#include "GStreamerPlugin.h"
#include "MediaQt/QImageSample.h"

GStreamerPlugin::GStreamerPlugin(std::string instance, SharedMediaFilter parent):
MediaAutoRegister(instance, parent)
{
	addAttribute("plugin", "");
}

GStreamerPlugin::~GStreamerPlugin()
{
}

bool GStreamerPlugin::initialize(const Attributes &copyAttributes)
{
	merge(attributes(), copyAttributes);
	return true;
}

bool GStreamerPlugin::shutdown()
{
	return true;
}

bool GStreamerPlugin::processSample(SharedMediaPad sinkPad, SharedMediaSample sample)
{
	return false;
}

