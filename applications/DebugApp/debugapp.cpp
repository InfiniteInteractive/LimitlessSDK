#include "debugapp.h"

#include "Base/log.h"
#include "Base/PluginFactory.h"
#include "Base/QtPluginView.h"
#include "Media/MediaPluginFactory.h"
#include "Media/IMediaSource.h"
#include "Media/ImageSampleSet.h"
#include "Media/GpuImageSample.h"
#include "QtComponents\filterpopup.h"

#include <QtWidgets/QDockWidget>
#include <Media/MediaInterface.h>

using namespace Limitless;

DebugApp::DebugApp(QWidget *parent, Qt::WindowFlags flags)
	: QMainWindow(parent, flags)
{
	setupUi(this);

}

DebugApp::~DebugApp()
{
}

void DebugApp::init()
{
	WId handle=effectiveWinId();
	HDC hdc=GetDC((HWND)handle);

	Limitless::initOpenGl((Limitless::DisplayHandle)handle);
}

void DebugApp::closeEvent(QCloseEvent *event)
{
	Limitless::closeOpenGl();
	QMainWindow::closeEvent(event);
}

void DebugApp::on_run_clicked()
{
	if(!m_gpuStitchFilter)
	{
		m_gpuStitchFilter=MediaPluginFactory::create("GPUStitcher", "Sticher");

		m_gpuStitchFilter->ready();
		m_gpuStitchFilter->play();

		m_gpuStitchFilter->attribute("radius")->fromFloat(1000.0f);
		m_gpuStitchFilter->attribute("thetaStart")->fromFloat(-180.0f);
		m_gpuStitchFilter->attribute("thetaEnd")->fromFloat(180.0f);
		m_gpuStitchFilter->attribute("phiStart")->fromFloat(45.0f);
		m_gpuStitchFilter->attribute("phiEnd")->fromFloat(-45.0f);
	}

	QImage testImage;
	QImage testImage2;

	if(!testImage.load("1-0.ppm"))
		assert(false);
	if(!testImage2.load("2-0.ppm"))
		assert(false);

	Limitless::SharedMediaSample mediaSampleSet=Limitless::MediaSampleFactory::createType("ImageSampleSet");
	Limitless::SharedImageSampleSet imageSampleSet=boost::dynamic_pointer_cast<ImageSampleSet>(mediaSampleSet);

	Limitless::SharedMediaSample mediaSample=Limitless::MediaSampleFactory::createType("GpuImageSample");
	Limitless::SharedGpuImageSample gpuSample=boost::dynamic_pointer_cast<GpuImageSample>(mediaSample);

	if(!gpuSample)
		assert(false);

	Limitless::SharedMediaSample mediaSample2=Limitless::MediaSampleFactory::createType("GpuImageSample");
	Limitless::SharedGpuImageSample gpuSample2=boost::dynamic_pointer_cast<GpuImageSample>(mediaSample2);

	if(!gpuSample2)
		assert(false);

	imageSampleSet->addSample(gpuSample);
	imageSampleSet->addSample(gpuSample2);

	cl::Event event;
	
	gpuSample->write(testImage.bits(), testImage.width(), testImage.height(), event);
	event.wait();
	gpuSample2->write(testImage2.bits(), testImage2.width(), testImage2.height(), event);
	event.wait();

	SharedMediaPads pads=m_gpuStitchFilter->getSinkPads();

	if(pads.empty())
		assert(false);

//	for(size_t i=0; i<10; ++i)
		m_gpuStitchFilter->processSample(pads[0], imageSampleSet);

}