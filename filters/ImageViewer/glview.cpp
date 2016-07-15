#include "glview.h"

#include "Media/GPUContext.h"
#include <QtGui/QOpenGlContext>
#include <QtPlatformHeaders/QWGLNativeContext>

GlView::GlView(QWidget *parent):
QWidget(parent)
{
	ui.setupUi(this);

	connect(this, SIGNAL(setFrameRateSignal(double)), this, SLOT(onFrameRate(double)), Qt::QueuedConnection);

	QPalette colorPalette(palette());

	colorPalette.setColor(QPalette::Background, Qt::black);

	setPalette(colorPalette);
	setAutoFillBackground(false);

	ui.openglWidget->setPalette(colorPalette);
	ui.openglWidget->setAutoFillBackground(false);

//setup opengl context
//	QOpenGLContext *nativeOpenglContext=new QOpenGLContext();
//	QWGLNativeContext nativeContext((HGLRC)Limitless::GPUContext::openGLContext(), (HWND)Limitless::GPUContext::nativeWindow());
//
//	nativeOpenglContext->setNativeHandle(QVariant::fromValue(nativeContext));
//	nativeOpenglContext->create();
//
//	QOpenGLContext *openglContext=new QOpenGLContext();
//
//	openglContext->setShareContext(nativeOpenglContext);
//	openglContext->setFormat(nativeOpenglContext->format());
//	openglContext->create();
//
//	QGLContext *glContext=QGLContext::fromOpenGLContext(openglContext);
//
//	m_display=new GLWidget(glContext, ui.glHolder);
//	m_display=new GLWidget(ui.glHolder);
//
//	m_display->setPalette(colorPalette);
//	m_display->setAutoFillBackground(false);
}

GlView::~GlView()
{

}

void GlView::setDisplayMode(GLWidget::DisplayMode displayMode)
{
	ui.openglWidget->setDisplayMode(displayMode);
//	m_display->setDisplayMode(displayMode);
}

void GlView::displaySample(Limitless::SharedMediaSample sample)
{
	ui.openglWidget->displaySample(sample);
//	m_display->displaySample(sample);
}

void GlView::setFrameRate(double frameRate)
{
	emit(setFrameRateSignal(frameRate));
}

void GlView::onFrameRate(double frameRate)
{
//	ui.frameRate->setText(QString("%1").arg(frameRate));
}