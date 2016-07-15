Limitless SDK ![alt text](https://github.com/InfiniteInteractive/LimitlessSDK/blob/master/resources/logo_icon_48.png "Logo")
======

Media Processing Framework built with dependance on modern C++. Design is based on GStreamer's documentation but does not follow it completely. _The framework is pretty rough at the moment and you will likely need help getting something functional. Currently only handles live image sources, audio is in the works._

##The Framework is split across several Dlls.

* _sdk/Base_ - provides basics for the plugin system, attribute system, serializer/un, logger, etc...
* _sdk/Media_ - provides the media plugin system, basic media sample, plugin factory, clock, and OpenGl/OpenCL support
* _sdk/MediaPipeline_ - provides pipeline to be used in the frame, currently only a single pipeline avilable
* _sdk/QtComponents_ - area for Qt support items for the Framework
* _sdk/Utilities_ - area for utilities, currently thread helpers, math, etc...
* _sdk/AvStream_ - provides connections into Windows DirectShow API

##Framework also includes some applications and filter plugins

* _applications/Conversion_ - (not functional)
* _applications/DebugApp_ - app for testing
* _applications/Sanctuary_ - basic loading of filters (may not be functional at the moment)
* _filters/AudioControls_ - provides VU meter and Mixer (under development) (Qt based)
* _filters/AudioViewer_ - 
* _filters/AvStreamOutput_ - uses AvStream to output from pipeline into DirectShow
* _filters/ControlStructures_ - provides filters to branch, tee, join and synchronize parts of the pipeline
* _filters/GStreamerPlugin_ - start of a filter to allow use of gstreamer plugins, (not functional)
* _filters/ImageFilters_ - provides gpu base ColorConversion and Filters to move images to/from the gpu
* _filters/ImageViewer_ - filter to show images (Qt based)
* _filters/ffmpeg_ - filter using ffmpeg's libraries to read and write to file/streams

##Framework also includes some helper libraries and utility programs

* _libraries/cvlib_ - contais ImageConcept, SimpleImage, slidingWindow function, binary match, and save as ppm
* _libraries/medialib_ - contains spectrogram and audio synchronize using audio finger printing
* _tools/Embed_ - through cmake takes source files and creates c files with the source as a string, mainly used for embedding opencl files

##Application Development
Building an application requires creating a pipeline. 
```c
#include "MediaPipeline/MediaPipeline.h"

Limitless::SharedMediaPipeline pipeline;

pipeline.reset(new Limitless::MediaPipeline("Pipeline", Limitless::SharedMediaFilter()));
```

Then creating filters
```c
#include "Media/MediaPluginFactory.h"

Limitless::SharedMediaFilter input=Limitless::MediaPluginFactory::create("FfmpegInput", "Input");
Limitless::SharedMediaFilter inputDecoder=Limitless::MediaPluginFactory::create("FfmpegDecoder", "Decoder");
Limitless::SharedMediaFilter inputViewer=Limitless::MediaPluginFactory::create("ImageViewer", "Viewer");
```
and then adding them to the pipeline and linking
```c
pipeline->addMediaFilter(input);
pipeline->addMediaFilter(inputDecoder);
pipeline->addMediaFilter(inputViewer);

pipeline->link(input, inputDecoder);
pipeline->link(inputDecoder, inputViewer);
```
set any attributes required by the filter
```c
input->setAttribute("location", "test.mp4");
```
then start the pipeline playing
```c
pipeline->play();
```
There is a lot more functionality but for a simple video player this would be the extent. Later I will provide more detail in the wiki.

##Plugin Development
It uses an automatic registering system through a little template voodoo, base class your filter with Limitless::MediaAutoRegister<YourClass, BaseClass> and provide your class as the first template argument and the base class you want to inherient from as the second argument.

```c
#include "Media/MediaPluginFactory.h"
#include "Media/IMediaFilter.h"

class ImageViewer_EXPORT ImageViewer:public Limitless::MediaAutoRegister<ImageViewer, Limitless::IMediaFilter>
{}
```
as soon as the dll is loaded the class will auto register with the plugin factory. The other tidbits you need to define for your plugin is some traits.

```c
namespace Limitless{namespace traits
{
	template<> struct type<ImageViewer>
	{
		static FilterType get()
		{return Sink;}
	};
	template<> struct category<ImageViewer>
	{
		static std::string get()
		{return "sink";}
	};
}}//Limitless::traits
```
This allows applications to look up your filter by type and category.

Past the above you need to provide some function overrides to communicate with the pipeline. The main ones being
* _onAttributeChanged_ - notification that one of the plugins attributes was changed
* _onAcceptMediaFormat_ - used to accept or reject upstream formats
* _onLinkFormatChanged_ - notification of the format selected between this plugin and either upstream or down stream plugins
* _onReady, onPlay, onPause_ -  notification of state changes
* _processSample_ - sample provided by pipeline for plugin to process