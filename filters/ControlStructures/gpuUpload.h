#ifndef _GpuUpload_h
#define _GpuUpload_h

#include "Media/MediaPluginFactory.h"
#include "Media/IMediaFilter.h"
#include "gpuUploadSample.h"

#include <queue>
#include <mutex>
#include <thread>
#include <condition_variable>

//#include <boost/thread.hpp>
//#include <boost/function.hpp>

struct GpuUploadTask
{
	GpuUploadTask(){}
	GpuUploadTask(Limitless::SharedMediaSample &sample){samples.push_back(sample);}
//	GpuUploadTask(Limitless::SharedMediaSample &sample, Limitless::SharedGpuImageSample &gpuSample, cl::Event event):event(event){samples.push_back(sample);gpuSamples.push_back(gpuSample);}
//	GpuUploadTask(std::vector<Limitless::SharedMediaSample> &samples, std::vector<Limitless::SharedGpuImageSample> &gpuSamples, cl::Event event):samples(samples), gpuSamples(gpuSamples), event(event){}
	
	std::vector<Limitless::SharedMediaSample> samples;
//	std::vector<Limitless::SharedGpuImageSample> gpuSamples;
//	cl::Event event;
};

class GpuUpload:public Limitless::MediaAutoRegister<GpuUpload, Limitless::IMediaFilter>
{
public:
	GpuUpload(std::string name, Limitless::SharedMediaFilter parent);
	~GpuUpload();

	virtual bool initialize(const Limitless::Attributes &attributes);
	virtual bool shutdown(){return true;}

	virtual bool processSample(Limitless::SharedMediaPad sinkPad, Limitless::SharedMediaSample sample);

protected:
	//IMediaFilter
	virtual StateChange onReady();
	virtual StateChange onPaused();
	virtual StateChange onPlaying();

	virtual bool onAcceptMediaFormat(Limitless::SharedMediaPad pad, Limitless::SharedMediaFormat format);
	virtual void onLinkFormatChanged(Limitless::SharedMediaPad pad, Limitless::SharedMediaFormat format);

	void processUploadSamples();

private:
	Limitless::SharedMediaFormat m_outputFormat;

//	std::queue<SharedGpuUploadSample> m_sampleQueue;
	std::queue<GpuUploadTask> m_sampleQueue;
	std::mutex m_samplesMutex;
	std::condition_variable m_sampleEvent;
	std::thread m_processThread;

	size_t m_gpuImageSampleSetId;
	size_t m_gpuImageSampleId;
	size_t m_gpuUploadSampleId;
	size_t m_imageSampleSetId;
	size_t m_imageSampleId;
};

namespace Limitless{namespace traits
{
	template<> struct type<GpuUpload>
	{
		static FilterType get()
		{return Filter;}
	};
	template<> struct category<GpuUpload>
	{
		static std::string get()
		{return "connector";}
	};
}}//Limitless::traits

#endif // _GpuUpload_h
