#include "DirectShowCapture.h"
#include <AtlBase.h>
#define __IDxtCompositor_INTERFACE_DEFINED__
#define __IDxtAlphaSetter_INTERFACE_DEFINED__
#define __IDxtJpeg_INTERFACE_DEFINED__
#define __IDxtKey_INTERFACE_DEFINED__
#include "QEdit.h"
#include <windows.h>
#include <dshow.h>
#include "dshowutil.h"
#include "Base/Log.h"
#include "boost/foreach.hpp"

DEFINE_GUID(CLSID_GrabberSample, 0x2fa4f053, 0x6d60, 0x4cb0, 0x95, 0x3, 0x8e, 0x89, 0x23, 0x4f, 0x3f, 0x73);
DEFINE_GUID(IID_IGrabberSample, 0x6b652fff, 0x11fe, 0x4fce, 0x92, 0xad, 0x02, 0x66, 0xb5, 0xd7, 0xc7, 0x8f);

using namespace Limitless;

//////////////////////////////////////////////////////////////////////
// SampleGrabberCB Declaration/Implementation
//////////////////////////////////////////////////////////////////////
class SampleGrabberCB:public ISampleGrabberCB
{
public:
	SampleGrabberCB(DirectShowCapture *parent, std::string pin, int width, int height);
	
private:
	HRESULT STDMETHODCALLTYPE QueryInterface( REFIID riid, void **ppvObject);
	ULONG STDMETHODCALLTYPE AddRef();
	ULONG STDMETHODCALLTYPE Release();

	//SampleCB worthless, MediaType always invalid
	HRESULT	STDMETHODCALLTYPE SampleCB(double SampleTime, IMediaSample *pSample){return S_OK;};
	HRESULT STDMETHODCALLTYPE BufferCB(double SampleTime, BYTE *pBuffer, long BufferLen);

	DirectShowCapture *m_parent;
	std::string m_pin;
	int m_width, m_height;
	
};

SampleGrabberCB::SampleGrabberCB(DirectShowCapture *parent, std::string pin, int width, int height):
m_parent(parent),
m_pin(pin),
m_width(width),
m_height(height)
{}

HRESULT SampleGrabberCB::QueryInterface(REFIID riid, void **ppvObject)
{
	if(riid==IID_ISampleGrabberCB)//__uuidof(ISampleGrabberCB)) 
    { 
        *ppvObject=(ISampleGrabberCB **)this; 
        return S_OK; 
    } 
    return E_NOINTERFACE; 
}
ULONG SampleGrabberCB::AddRef()
{	return 2;}
ULONG SampleGrabberCB::Release()
{	return 1;}

HRESULT SampleGrabberCB::BufferCB(double SampleTime, BYTE *pBuffer, long BufferLen)
{
	m_parent->grabberCallback(m_pin, SampleTime, pBuffer, BufferLen);
	return S_OK;
}

//////////////////////////////////////////////////////////////////////
// DirectShowCapture Implementation
//////////////////////////////////////////////////////////////////////

DirectShowCapture::DirectShowCapture(std::string name, SharedMediaFilter parent):
MediaAutoRegister(name, parent),
//m_eventThread(this),
m_graph(NULL),
m_seeking(NULL),
m_events(NULL),
m_control(NULL),
m_source(NULL),
m_grabber(NULL),
m_nullRenderer(NULL),
m_callback(NULL)
{
	HRESULT hr;
	
	hr = CoCreateInstance(CLSID_FilterGraph, NULL, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&m_graph));
	hr = m_graph->QueryInterface(IID_PPV_ARGS(&m_control));

	m_processThread=boost::thread(boost::bind(&DirectShowCapture::run, this));
}

DirectShowCapture::~DirectShowCapture()
{
	if(this->m_callback)
		delete this->m_callback;

	if(this->m_source)
		this->m_source->Release();

	if(this->m_grabber)
		this->m_grabber->Release();

	if(this->m_nullRenderer)
		this->m_nullRenderer->Release();

	if(this->m_seeking)
		this->m_seeking->Release();

	if(this->m_events)
		this->m_events->Release();

	if(this->m_control)
		this->m_control->Release();

	if(this->m_graph)
		this->m_graph->Release();
}

MediaDevices DirectShowCapture::devices()
{
	USES_CONVERSION;

	MediaDevices mediaDevices;

	HRESULT hr;
	IEnumMoniker *pEnum;
	ICreateDevEnum *pDevEnum;
    
	hr = CoCreateInstance(CLSID_SystemDeviceEnum, NULL, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&pDevEnum));
	if(SUCCEEDED(hr))
	{
		hr = pDevEnum->CreateClassEnumerator(CLSID_VideoInputDeviceCategory, &pEnum, 0);
		pDevEnum->Release();
	}
	
	if(pEnum == NULL) //no devices
		return mediaDevices;

	IMoniker *pMoniker = NULL;

    while (pEnum->Next(1, &pMoniker, NULL) == S_OK)
    {
		IPropertyBag *pPropBag;
		
		hr = pMoniker->BindToStorage(0, 0, IID_PPV_ARGS(&pPropBag));
		if(FAILED(hr))
		{
            pMoniker->Release();
            continue;  
        } 

        VARIANT varName;

		VariantInit(&varName);
        // Get description or friendly name.
        hr = pPropBag->Read(L"Description", &varName, 0);
        if(FAILED(hr))
        {
            hr = pPropBag->Read(L"FriendlyName", &varName, 0);
        }
        if(SUCCEEDED(hr))
        {
			VARIANT varPath;

			VariantInit(&varPath);
			hr = pPropBag->Read(L"DevicePath", &varPath, 0);

			if(SUCCEEDED(hr))
			{
				IBaseFilter *filter=NULL;
				IBindCtx * pbc; 
				
				hr = CreateBindCtx(NULL, &pbc);
				if((hr=pMoniker->BindToObject(pbc, 0, IID_IBaseFilter, (void **)&filter)) == S_OK)
				{
					IEnumPins *enumPins;
					std::vector<std::string> devicePins;

					if(filter->EnumPins(&enumPins) == S_OK)
					{
						IPin *pin;

						while(enumPins->Next(1, &pin, NULL) == S_OK)
						{
							PIN_INFO pinInfo;
							AM_MEDIA_TYPE *mediaType;

							pin->QueryPinInfo(&pinInfo);
							if(pinInfo.dir == PINDIR_OUTPUT)
							{
								IEnumMediaTypes *types;
								bool video=false;

								pin->EnumMediaTypes(&types);
								while(types->Next(1, &mediaType, NULL) == S_OK)
								{
									if(mediaType->majortype == MEDIATYPE_Video)
									{
										video=true;
										break;
									}
								}

								if(video)
								{
									std::wstring widePinName(pinInfo.achName);
									std::string pinName(widePinName.begin(), widePinName.end());
									devicePins.push_back(pinName);
								}
							}
							pin->Release();
						}
						enumPins->Release();
					}

					BOOST_FOREACH(std::string &pinName, devicePins)
					{
						std::string id=(boost::format("%s:%s")%OLE2A(varPath.bstrVal)%pinName).str();
						mediaDevices.push_back(MediaDevice(OLE2A(varName.bstrVal), id));
					}
				}
				VariantClear(&varPath);
				filter->Release();
				pbc->Release();
			}
            VariantClear(&varName); 
        }
		pPropBag->Release();
        pMoniker->Release();
	}
	pEnum->Release();
	
	return mediaDevices;
}

HRESULT DirectShowCapture::getDevice(const std::string &device, const std::string &uid, IBaseFilter **filter)
{
	USES_CONVERSION;

	HRESULT hr;
	IEnumMoniker *pEnum;
	ICreateDevEnum *pDevEnum;
    
	hr=CoCreateInstance(CLSID_SystemDeviceEnum, NULL, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&pDevEnum));
	if(SUCCEEDED(hr))
	{
		hr=pDevEnum->CreateClassEnumerator(CLSID_VideoInputDeviceCategory, &pEnum, 0);
		pDevEnum->Release();
	}
	
	IMoniker *pMoniker=NULL;

    while(pEnum->Next(1, &pMoniker, NULL) == S_OK)
    {
		IPropertyBag *pPropBag;
		
		hr=pMoniker->BindToStorage(0, 0, IID_PPV_ARGS(&pPropBag));
		if(FAILED(hr))
		{
            pMoniker->Release();
            continue;  
        } 

        VARIANT var;
        VariantInit(&var);

        // Get description or friendly name.
        hr=pPropBag->Read(L"Description", &var, 0);
        if(FAILED(hr))
        {
            hr=pPropBag->Read(L"FriendlyName", &var, 0);
        }
        if(SUCCEEDED(hr))
        {
            std::string filterName(OLE2A(var.bstrVal));
			VariantClear(&var); 
			if(filterName ==  device)
			{
				VARIANT varPath;

				VariantInit(&varPath);
				hr = pPropBag->Read(L"DevicePath", &varPath, 0);

				if(SUCCEEDED(hr))
				{
					std::string filterPath(OLE2A(varPath.bstrVal));

					if(filterPath == uid)
					{
//						IBaseFilter *filter;

						if((hr=pMoniker->BindToObject(0, 0, IID_IBaseFilter, (void **)filter)) == S_OK)
						{
//							pPropBag->Release();
//							pMoniker->Release();
//							pEnum->Release();
//							return filter;
							break;
						}
					}
				}
			}
        }
		pPropBag->Release();
        pMoniker->Release();
	}
	pEnum->Release();
	return hr;
}

bool DirectShowCapture::connect(MediaDevice mediaDevice)
{
	HRESULT hr;
	
/*	if(m_device.startsWith("file://"))
	{
		std::string file=m_device;
		file.remove(0,7);

		hr = m_graph->AddSourceFilter(file.toStdWString().c_str(), L"Source", &m_source);
	}
	else if(m_device.startsWith("rtsp://"))
	{
		//		std::wstring filter=L"RTSP Source filter";
		//		hr = getDevice(filter, &m_source);
		//		if(hr == S_OK)
		//		{
		//			IFileSourceFilter *fileFilter;
		//
		//			hr = m_source->QueryInterface(IID_IFileSourceFilter, (LPVOID *)&fileFilter);
		//			if(FAILED(hr))
		//			{}
		//			fileFilter->Load(device.toStdWString().c_str(), NULL);
		//			// Add Capture filter to our graph.
		//			hr=m_graph->AddFilter(m_source, L"Video Capture");
		//			if(FAILED(hr))
		//			{}
		//		}
	}
	else
	{
		hr=getDevice(m_device, m_uid, &m_source);
		if(SUCCEEDED(hr))
		{
			hr=m_graph->AddFilter(m_source, L"Video Capture");
			if(FAILED(hr))
			{}
		}
	}

	if(m_source == NULL)
		return false;

	return true;
*/
	return false;
}

bool DirectShowCapture::disconnect()
{
	if(m_nullRenderer != NULL)
	{
		m_graph->RemoveFilter(m_nullRenderer);
		m_nullRenderer->Release();
		m_nullRenderer=NULL;
	}
	if(m_grabber != NULL)
	{
		m_graph->RemoveFilter((IBaseFilter *)m_grabber);
		m_grabber->Release();
		m_grabber=NULL;
	}
	if(m_source != NULL)
	{
		m_graph->RemoveFilter(m_source);
		m_source->Release();
		m_source=NULL;
	}
	if(m_events != NULL)
	{
		m_events->Release();
		m_events=NULL;
	}
	if(m_seeking != NULL)
	{
		m_seeking->Release();
		m_seeking=NULL;
	}
	if(m_callback!=NULL)
	{
		delete m_callback;
		m_callback=NULL;
	}

	return true;

	return false;
}

bool DirectShowCapture::connected()
{
	return false;
}

void DirectShowCapture::onPinOpen(std::string pinName)
{
/*	HRESULT hr;
	IEnumPins *enumPins = NULL;
	IPin *pin = NULL;
	PIN_INFO pinInfo;
	IBaseFilter *grabberBase=NULL;
	AM_MEDIA_TYPE mediaType;

	hr = CoCreateInstance(CLSID_SampleGrabber, NULL, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&grabberBase));

	grabberBase->QueryInterface(IID_PPV_ARGS(&m_grabber));

	ZeroMemory(&mediaType, sizeof(mediaType));
	mediaType.majortype = MEDIATYPE_Video;
	mediaType.subtype = MEDIASUBTYPE_RGB24;
	m_grabber->SetMediaType(&mediaType);

	m_graph->AddFilter(grabberBase, L"FrameGrabber");

	hr = CoCreateInstance(CLSID_NullRenderer, NULL, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&m_nullRenderer));
	hr = m_graph->AddFilter(m_nullRenderer, L"Null Renderer");

	hr = m_source->EnumPins(&enumPins);
	while(S_OK == enumPins->Next(1, &pin, NULL))
	{
		hr=S_FALSE;
		if(pin->QueryPinInfo(&pinInfo) == S_OK)
		{
			std::string name=std::string::fromWCharArray(pinInfo.achName);

			if(name == pinName)
				hr = ConnectFilters(m_graph, pin, grabberBase);
		}
		pin->Release();
		if(SUCCEEDED(hr))
			break;
	}
	hr = ConnectFilters(m_graph, grabberBase, m_nullRenderer);

	VIDEOINFOHEADER *videoHeader=NULL;
	int width=640, height=480;

	m_grabber->GetConnectedMediaType(&mediaType);
	if(mediaType.formattype == FORMAT_VideoInfo)
	{
		if(mediaType.cbFormat >= sizeof(VIDEOINFOHEADER))
		{
			videoHeader =(VIDEOINFOHEADER *)mediaType.pbFormat;

			width=videoHeader->bmiHeader.biWidth;
			height=videoHeader->bmiHeader.biHeight;
		}
	}

	hr = m_grabber->SetOneShot(false);
    hr = m_grabber->SetBufferSamples(false);
	m_callback=new SampleGrabberCB(this, pinName, width, height);
	hr = m_grabber->SetCallback(m_callback, 1);

	hr = m_graph->QueryInterface(IID_PPV_ARGS(&m_events));
	if(SUCCEEDED(hr))
		m_events->CancelDefaultHandling(EC_COMPLETE);
	hr = m_graph->QueryInterface(IID_PPV_ARGS(&m_seeking));
*/
}

int DirectShowCapture::start()
{
	if(m_source == NULL)
		return -1;

	m_control->Run();
	m_eventThread.start();
	return 0;
}

int DirectShowCapture::stop()
{
	m_control->Stop();
	m_eventThread.stop();
	if(!m_eventThread.wait(1000))
		m_eventThread.terminate();
	return 0;
}

//void DirectShowCapture::grabberCallback(std::string pin, double sampleTime, SharedImageSample frame)
void DirectShowCapture::grabberCallback(std::string input, double SampleTime, unsigned char *buffer, size_t size);
{
	foreach(TargetEntry &target, m_targetMap)
	{
		foreach(PinEntry entry, target.pinMap)
		{
			if(entry.pin == pin)
			{
//				CaptureImages images;
//
//				images.push_back(CaptureImage(entry.id, frame.rgbSwapped().mirrored(), sampleTime));
//				target.device->pushImages(images);
				return;
			}
		}
	}
}

void DirectShowCapture::run()
{
	while(m_run)
	{m_parent->eventHandler();}
	return;
};

int DirectShowCapture::eventHandler()
{
	if(m_events == NULL)
		return -1;

	long evCode, param1, param2;
	HRESULT hr;

//	while(hr = m_events->GetEvent(&evCode, &param1, &param2, INFINITE), SUCCEEDED(hr))
	{
		hr = m_events->GetEvent(&evCode, &param1, &param2, 50);
		if(SUCCEEDED(hr))
		{
			Log::Write("Event recieved: 0x%08x\n", evCode);
			switch(evCode) 
			{
			case EC_COMPLETE:
				{
					LONGLONG currentPos=0;
					m_seeking->SetPositions(&currentPos, AM_SEEKING_AbsolutePositioning, NULL, AM_SEEKING_NoPositioning);
					m_control->Run();
				}
				break;
			} 
			hr = m_events->FreeEventParams(evCode, param1, param2);
		}
	}
	return 0;
}

int DirectShowCapture::buffer(QImage &color, QImage &ir)
{
	return 0;
}