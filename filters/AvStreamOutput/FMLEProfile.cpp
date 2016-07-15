#include "FMLEProfile.h"
#include <boost/format.hpp>
#include <boost/lexical_cast.hpp>

#include <windows.h>
#include <dshow.h>
#include <uuids.h>
#include <strmif.h>

FMLEProfile::FMLEProfile()
{
	m_videoCaptureDevice="";
	m_videoEncoder="h.264";
	m_videoEncoderBitrate=1000;
	m_videoEncoderWidth=1280;
	m_videoEncoderHeight=720;
	m_videoEncoderKeyframeFrequency=10;

	m_audioCaptureDevice="None";
	m_audioSampleRate=46000;
	m_audioChannels=2;
	m_audioEncoder="MP3";
	m_audioEncoderBitrate=48;

	m_outputUsername="";
	m_outputPassword="";
	m_outputUrl="";
	m_outputBackupUrl="";
	m_outputStream="livestream";
	m_outputSaveFile=false;
	m_outputFilePath="";
}

bool FMLEProfile::read(std::string filePath)
{
	boost::property_tree::ptree propertyTree;

	try
	{	boost::property_tree::read_xml(filePath, propertyTree);}
	catch(...)
	{	return false;}
	
	boost::property_tree::ptree &root=propertyTree.get_child("flashmedialiveencoder_proile");
	boost::property_tree::ptree &capture=root.get_child("capture");
	boost::property_tree::ptree &videoCapture=capture.get_child("video");

	m_videoCaptureDevice=videoCapture.get<std::string>("device");

	boost::property_tree::ptree &audioCapture=capture.get_child("audio");

	m_audioCaptureDevice=audioCapture.get<std::string>("devices");
	m_audioSampleRate=audioCapture.get<int>("sample_rate");
	m_audioChannels=audioCapture.get<int>("channels");

	boost::property_tree::ptree &encode=root.get_child("encode");
	boost::property_tree::ptree &videoEncoder=encode.get_child("video");

	m_videoEncoder=videoEncoder.get<std::string>("format");
	std::string videoBitrate=videoEncoder.get<std::string>("datarate");
	sscanf(videoBitrate.c_str(), "%d;", &m_videoEncoderBitrate);
	std::string outputSize=videoEncoder.get<std::string>("outputsize");
	sscanf(outputSize.c_str(), "%dx%d;", &m_videoEncoderWidth, &m_videoEncoderHeight);
	m_videoEncoderKeyframeFrequency=videoEncoder.get<int>("advanced.keyframe_frequency");

	boost::property_tree::ptree &audioEncoder=encode.get_child("audio");

	m_audioEncoder=audioEncoder.get<std::string>("format");
	m_audioEncoderBitrate=audioEncoder.get<int>("datarate");

	boost::property_tree::ptree &output=root.get_child("output");
	boost::property_tree::ptree &rtmp=output.get_child("rtmp");

	m_outputUrl=rtmp.get<std::string>("url");
	m_outputBackupUrl=rtmp.get<std::string>("backup_url");
	m_outputStream=rtmp.get<std::string>("stream");

	try
	{	boost::property_tree::ptree &outputFile=output.get_child("file");}
	catch (...)
	{
		m_outputSaveFile=false;
		return true;
	}
	
	m_outputFilePath=output.get<std::string>("path");
	return true;
}

void FMLEProfile::write(std::string filePath)
{
	boost::property_tree::ptree propertyTree;
	
//	boost::property_tree::ptree root=propertyTree.put("flashmedialiveencoder_proile", "");
//	boost::property_tree::ptree capture=root.put("capture", "");
//	boost::property_tree::ptree videoCapture=capture.put("capture", "");
	boost::property_tree::ptree root;
	boost::property_tree::ptree capture;
	boost::property_tree::ptree videoCapture;

	videoCapture.put("device", m_videoCaptureDevice);
	videoCapture.put("crossbar_input", 0);
	videoCapture.put("frame_rate", 15);
	videoCapture.put("size.width", 320);
	videoCapture.put("size.height", 240);
	capture.put_child("video", videoCapture);

	boost::property_tree::ptree audioCapture;

	audioCapture.put("devices", m_audioCaptureDevice);
	audioCapture.put("crossbar_input", 0);
	audioCapture.put("sample_rate", m_audioSampleRate);
	audioCapture.put("channels", m_audioChannels);
	audioCapture.put("intput_volume", 75);
	capture.put_child("audio", audioCapture);

	root.put_child("capture", capture);
	root.put("process.video.preserve_aspect", "");

	boost::property_tree::ptree encode;//=root.put("encode", "");
	boost::property_tree::ptree videoEncoder;//=encode.put("video", "");

	videoEncoder.put("format", m_videoEncoder);
	videoEncoder.put("datarate", (boost::format("%d;")%m_videoEncoderBitrate).str());
	videoEncoder.put("outputsize", (boost::format("%dx%d;")%m_videoEncoderWidth%m_videoEncoderHeight).str());
	videoEncoder.put("advanced.keyframe_frequency", (boost::format("%d")%m_videoEncoderKeyframeFrequency).str());
	videoEncoder.put("advanced.quality", "Good Quality - Good Framerate");
	videoEncoder.put("advanced.noise_reduction", "None");
	videoEncoder.put("advanced.datarate_window", "Medium");
	videoEncoder.put("advanced.cpu_usage", "Dedicated");
	videoEncoder.put("autoadjust.enabled", "false");
	videoEncoder.put("autoadjust.maxbuffersize", 1);
	videoEncoder.put("autoadjust.dropframes.enabled", "false");
	videoEncoder.put("autoadjust.degradequality.enable", "false");
	videoEncoder.put("autoadjust.degradequality.minvideobitrate", "");
	videoEncoder.put("autoadjust.degradequality.preservepfq", "false");
	encode.put_child("video", videoEncoder);

	boost::property_tree::ptree audioEncoder;//=encode.put("audio", "");

	audioEncoder.put("format", m_audioEncoder);
	audioEncoder.put("datarate", m_audioEncoderBitrate);
	encode.put_child("audio", audioEncoder);

	root.put_child("encode", encode);
	root.put("restartinterval.days", "");
	root.put("restartinterval.hours", "");
	root.put("restartinterval.minutes", "");

	root.put("reconnectinterval.attempts", "");
	root.put("reconnectinterval.interval", "");

	boost::property_tree::ptree output;//=root.put("output", "");
	boost::property_tree::ptree rtmp;//=output.put("rtmp", "");

	rtmp.put("url", "");
	rtmp.put("backup_url", "");
	rtmp.put("stream", "");
	output.put_child("rtmp", rtmp);

	if(m_outputSaveFile)
	{
		boost::property_tree::ptree outputFile;//=output.put("file", "");

		outputFile.put("limitbysize.enable", "false");
		outputFile.put("limitbysize.size", 10);

		outputFile.put("limitbyduration.enable", "false");
		outputFile.put("limitbyduration.hours", 1);
		outputFile.put("limitbyduration.minutes", 0);
		outputFile.put("path", m_outputFilePath);
		output.put_child("file", outputFile);
	}

//	boost::property_tree::ptree metaData=output.put("metadata", "");

	//loop
//	boost::property_tree::ptree entry=metaData.put("entry", "");
//	entry.put("key", );
//	entry.put("value", );

	
	root.put_child("output", output);
	propertyTree.put_child("flashmedialiveencoder_proile", root);

	boost::property_tree::xml_writer_settings<char> settings(' ', 4, "UTF-16");
	boost::property_tree::write_xml(filePath, propertyTree, std::locale(), settings);
}

Strings FMLEProfile::videoCaptueDevices()
{
	Strings captureDevices;

	captureDevices.push_back("AvStreamVideo");
	return captureDevices;
}

Strings FMLEProfile::videoEncoders()
{
	Strings encoders;

	encoders.push_back("vp6");
	encoders.push_back("h.264");

	return encoders;
}

Strings FMLEProfile::audioCaptueDevices() const
{
	Strings captureDevices;
	HRESULT result;
	IMoniker *moniker;
	ICreateDevEnum *devEnum;
	IEnumMoniker *enumMoniker;

	captureDevices.push_back("None");
	if(FAILED(CoInitializeEx(NULL, COINIT_APARTMENTTHREADED)))
		return captureDevices;
	result=CoCreateInstance (CLSID_SystemDeviceEnum, NULL, CLSCTX_INPROC, IID_ICreateDevEnum, (void **) &devEnum);
	if(FAILED(result))
	{
		CoUninitialize();
		return captureDevices;
	}
	result=devEnum->CreateClassEnumerator (CLSID_AudioInputDeviceCategory, &enumMoniker, 0);
	if (FAILED(result) || (enumMoniker == NULL))
	{
		CoUninitialize();
		return captureDevices;
	}

	IPropertyBag *propertyBag;
	WCHAR friendlyName[120];

	while(enumMoniker->Next (1, &moniker, NULL)!=S_FALSE)
	{
		result = moniker->BindToStorage(0, 0, IID_IPropertyBag, (void **)&propertyBag);
		if(SUCCEEDED(result))
		{
			VARIANT var;

			var.vt=VT_BSTR;
			result=propertyBag->Read(L"FriendlyName", &var, NULL);
			if(result == NOERROR)
			{
				result=StringCchCopyW(friendlyName, sizeof(friendlyName) / sizeof(friendlyName[0]), (STRSAFE_LPCWSTR)var.bstrVal);
				SysFreeString(var.bstrVal);
			}
			propertyBag->Release();

			std::wstring wideName(friendlyName);
			captureDevices.push_back(std::string(wideName.begin(), wideName.end()));
		}
		moniker->Release();
	}
	devEnum->Release();
	enumMoniker->Release();
	CoUninitialize();
	
	return captureDevices;
}

std::vector<float> FMLEProfile::audioSampleRates()
{
	std::vector<float> sampleRates;

	sampleRates.push_back(48.0f);

	return sampleRates;
}

std::vector<int> FMLEProfile::audioChannelOptions()
{
	std::vector<int> options;

	options.push_back(1);
	options.push_back(2);

	return options;
}

Strings FMLEProfile::audioEncoders()
{
	Strings encoders;

	encoders.push_back("mp3");
	encoders.push_back("aac");

	return encoders;
}


