#ifndef AudioMeter_H
#define AudioMeter_H

#include "Media/MediaPluginFactory.h"
#include "Media/IMediaFilter.h"
//#include "AudioMeter_global.h"

#include <queue>

class GlView;
class GLWidget;

#ifdef AudioMeter_EXPORTS
# define AudioMeter_EXPORT __declspec(dllexport)
#else
# define AudioMeter_EXPORT __declspec(dllimport)
#endif

class AudioMeter_EXPORT AudioMeter:public Limitless::MediaAutoRegister<AudioMeter, Limitless::IMediaFilter>
{
public:
	AudioMeter(std::string name, Limitless::SharedMediaFilter parent);
	~AudioMeter();

	void attachViewer(AudioMeter *AudioMeter);
	void removeViewer(AudioMeter *AudioMeter);

	virtual bool initialize(const Limitless::Attributes &attributes);
	virtual bool shutdown(){return true;}

	virtual Limitless::SharedPluginView getView();

	void setSample(Limitless::SharedMediaSample sample);
	virtual bool processSample(Limitless::SharedMediaPad sinkPad, Limitless::SharedMediaSample sample);

	void showControls(bool show);

protected:
	GLWidget *getGlWidget();

	//IMediaFilter
	virtual StateChange onReady();
	virtual StateChange onPaused();
	virtual StateChange onPlaying();

	bool onAcceptMediaFormat(Limitless::SharedMediaPad pad, Limitless::SharedMediaFormat format);
	void onLinkFormatChanged(Limitless::SharedMediaPad pad, Limitless::SharedMediaFormat format);

private:
	Limitless::SharedPluginView m_view;

	size_t m_iAudioSampleId;
};

namespace Limitless{namespace traits
{
	template<> struct type<AudioMeter>
	{
		static FilterType get()
		{return Sink;}
	};
	template<> struct category<AudioMeter>
	{
		static std::string get()
		{return "sink";}
	};
}}//Limitless::traits

#endif // AudioMeter_H
