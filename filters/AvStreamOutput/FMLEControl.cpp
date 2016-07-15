#include "FMLEControl.h"

#include <boost/format.hpp>
#include <QtCore/QDir>

using namespace Limitless;

const std::string FMLEExecutable("FMLECmd.exe");
const std::string FMLETitle("FlashMediaLiveEncoder");

FMLEControl::FMLEControl(std::string name, SharedMediaFilter parent):
MediaAutoRegister(name, parent)
{
	m_fmleExecutable=findInstalation();
	m_fmleProfilePath=QDir::current().absolutePath().toStdString()+"/test.xml";
}

FMLEControl::~FMLEControl()
{}

bool FMLEControl::initialize(const Attributes &attributes)
{
	addSinkPad("Sink", "[{\"mime\":\"any\"}]");

	Strings videoCaptureDevices=m_fmleProfile.videoCaptueDevices();
	addAttribute("videoCaptureDevice", videoCaptureDevices[0], videoCaptureDevices);

	Strings videoEncoders=m_fmleProfile.videoEncoders();
	addAttribute("videoEncoder", videoEncoders[0], videoEncoders);
	addAttribute("videoEncoderBitrate", 1000);
	addAttribute("videoEncoderWidth", 1280);
	addAttribute("videoEncoderHeight", 720);
	addAttribute("videoEncoderKeyframeFrequency", 5);

	Strings audioCaptureDevices=m_fmleProfile.audioCaptueDevices();
	addAttribute("audioCaptureDevice", audioCaptureDevices[0], audioCaptureDevices);

	std::vector<float> audioSampleRates=m_fmleProfile.audioSampleRates();
	addAttribute("audioSampleRate", audioSampleRates[0], audioSampleRates);
	
	std::vector<int> audioChannels=m_fmleProfile.audioChannelOptions();
	addAttribute("audioChannels", audioChannels[0], audioChannels);

	Strings audioEncoders=m_fmleProfile.audioEncoders();
	addAttribute("audioEncoder", audioEncoders[0], audioEncoders);
	addAttribute("audioEncoderBitrate", 64);

	addAttribute("outputStream", "");
	addAttribute("outputUrl", "");
	addAttribute("outputUrlUser", "");
	addAttribute("outputUrlPassword", "");
	addAttribute("outputBackupUrl", "");
	addAttribute("outputBackupUrlUser", "");
	addAttribute("outputBackupUrlPassword", "");
	addAttribute("outputBackupUrlUsePrimaryAuth", false);

	addAttribute("outputSaveFile", false);
	addAttribute("outputFilePath", "");

	m_fmleProfile.read(m_fmleProfilePath);
	updateProfileAttributes();

	return true;
}

bool FMLEControl::shutdown()
{
	return true;
}

SharedPluginView FMLEControl::getView()
{
	return SharedPluginView();
}

//IMediaFilter
bool FMLEControl::processSample(SharedMediaPad sinkPad, SharedMediaSample sample)
{
	deleteSample(sample);
	return true;
}

bool FMLEControl::onAcceptMediaFormat(SharedMediaPad pad, SharedMediaFormat format)
{
	if(pad->type() == MediaPad::SINK)
		return true; //we accept anything
	else
		return true;
	return false;
}

void FMLEControl::onLinkFormatChanged(SharedMediaPad pad, SharedMediaFormat format)
{
	return IMediaFilter::onLinkFormatChanged(pad, format);
}

IMediaFilter::StateChange FMLEControl::onReady()
{
	return SUCCESS;
}

IMediaFilter::StateChange FMLEControl::onPaused()
{
	stop();
	return SUCCESS;
}

IMediaFilter::StateChange FMLEControl::onPlaying()
{
	start();
	return SUCCESS;
}

std::string FMLEControl::findInstalation()
{
	std::string fmleLocation=(boost::format("%s/adobe")%getProgramFolder()).str();

	QDir adobeProgramFolder(QString::fromStdString(fmleLocation));
	QStringList directories;
	QStringList filters;

	filters<<"Flash Media Live Encoder*";
	adobeProgramFolder.setNameFilters(filters);
	directories=adobeProgramFolder.entryList(QDir::Dirs);

	if(directories.empty())
		return "";
	return (boost::format("%s/%s/%s")%fmleLocation%directories[0].toStdString()%FMLEExecutable).str(); 
}

std::string FMLEControl::getProgramFolder()
{
	std::string programFolder=getenv("ProgramFiles(x86)");

	if(programFolder.empty())
		programFolder=getenv("ProgramFiles");

	return programFolder;
}

std::string FMLEControl::buildOptions()
{
	std::string options;
	std::string profilePath;
	std::string primaryAuth;
	std::string backupAuth;
	
	std::string primaryUser=attribute("outputUrlUser")->toString();
	std::string primaryPassword=attribute("outputUrlPassword")->toString();
	std::string backupUser=attribute("outputBackupUrlUser")->toString();
	std::string backupPassword=attribute("outputBackupUrlPassword")->toString();
	bool backupUsePrimaryAuth=attribute("outputBackupUrlUsePrimaryAuth")->toBool();

	profilePath=(boost::format(" /p %s")%m_fmleProfilePath).str();
	primaryAuth=(boost::format(" /ap %s:%s")%primaryUser%primaryPassword).str();
	if(backupUsePrimaryAuth)
		backupAuth=(boost::format(" /ab %s:%s")%backupUser%backupPassword).str();
	else if(backupUsePrimaryAuth)
		backupAuth=(boost::format(" /ab %s:%s")%primaryUser%primaryPassword).str();

	options.append(profilePath);
	options.append(primaryAuth);
	options.append(backupAuth);

	return options;
}

void FMLEControl::start()
{
	std::string fmlePath=findInstalation();
	std::string fmleOptions=buildOptions();

	updateProfile();

	m_fmleProfile.write(m_fmleProfilePath);

	PROCESS_INFORMATION processInfo;
	STARTUPINFOA startupInfo;
	
	memset(&startupInfo, 0, sizeof(STARTUPINFO));
	startupInfo.cb=sizeof(STARTUPINFOA);
	startupInfo.lpTitle=(LPSTR)FMLETitle.c_str();
	startupInfo.dwFlags=STARTF_USESHOWWINDOW;
	startupInfo.wShowWindow=SW_MINIMIZE;

	bool created;

	created=CreateProcessA(fmlePath.c_str(), (LPSTR)fmleOptions.c_str(), 0, 0, false, 0, 0, 0, &startupInfo, &processInfo);

	if(created)
	{
		m_processHandle=processInfo.hProcess;
		m_statusThread=boost::thread(boost::bind(&FMLEControl::processThread, this));
	}
}

void FMLEControl::stop()
{
	if(m_run)
	{
		m_run=false;
		m_statusThread.join();
	}
}

void FMLEControl::processThread()
{
	HANDLE consoleHandle;
	
	if((consoleHandle=attachConsole(m_processHandle)) == NULL)
		return;

	CONSOLE_SCREEN_BUFFER_INFO screenBufferInfo;
	
	if(!GetConsoleScreenBufferInfo(consoleHandle, &screenBufferInfo))
		return;
	
	SMALL_RECT window;
	CharInfoVector buffer;
	COORD windowSize;
	COORD destCoord;

	window=screenBufferInfo.srWindow;
	windowSize.X=window.Right-window.Left;
	windowSize.Y=window.Bottom-window.Top;
	destCoord.X=0;
	destCoord.Y=0;
	
	buffer.resize(windowSize.X*windowSize.Y);

	while(m_run)
	{
		if(ReadConsoleOutput(consoleHandle, &buffer[0], windowSize, destCoord, &window))
		{
			std::string string=converToString(buffer, windowSize);
		}
	}
}

HANDLE FMLEControl::attachConsole(HANDLE process)
{
	int retries=10;
	int attempt=0;
	DWORD processId=GetProcessId(process);

	for(int attempts=0; attempt<retries; ++attempt)
	{
		if(AttachConsole(processId))
			break;
		Sleep(100);
//		boost::thread::sleep(100);
	}

	if(attempt >= retries)
		return NULL;
	return GetStdHandle(STD_OUTPUT_HANDLE);
}

std::string FMLEControl::converToString(const CharInfoVector &buffer, const COORD &size)
{
	std::string string;
	SHORT index=0;

	for(SHORT y=0; y<size.Y; ++y)
	{
		for(SHORT x=0; x<size.X; ++x)
		{
			string.append(1, buffer[index].Char.AsciiChar);
			index++;
		}
		string.append("\n");
	}
	return string;
}

void FMLEControl::updateProfileAttributes()
{
	setAttribute("videoCaptureDevice", m_fmleProfile.videoCaptueDevice());
	setAttribute("videoEncoder", m_fmleProfile.videoEncoder());
	setAttribute("videoEncoderBitrate", m_fmleProfile.videoEncoderBitrate());
	setAttribute("videoEncoderWidth", m_fmleProfile.videoEncoderWidth());
	setAttribute("videoEncoderHeight", m_fmleProfile.videoEncoderHeight());
	setAttribute("videoEncoderKeyframeFrequency", m_fmleProfile.videoEncoderKeyframeFrequency());

	setAttribute("audioCaptureDevice", m_fmleProfile.audioCaptueDevice());
	setAttribute("audioSampleRate", m_fmleProfile.audioSampleRate());
	setAttribute("audioChannels", m_fmleProfile.audioChannels());
	setAttribute("audioEncoder", m_fmleProfile.audioEncoder());
	setAttribute("audioEncoderBitrate", m_fmleProfile.audioEncoderBitrate());

	setAttribute("outputUrl", m_fmleProfile.outputUrl());
	setAttribute("outputBackupUrl", m_fmleProfile.outputBackupUrl());
	setAttribute("outputStream", m_fmleProfile.outputStream());
	setAttribute("outputSaveFile", m_fmleProfile.outputSaveFile());
	setAttribute("outputFilePath", m_fmleProfile.outputFilePath());
}

void FMLEControl::updateProfile()
{
	m_fmleProfile.setVideoCaptueDevice(attribute("videoCaptureDevice")->toString());
	m_fmleProfile.setVideoEncoder(attribute("videoEncoder")->toString());
	m_fmleProfile.setVideoEncoderBitrate(attribute("videoEncoderBitrate")->toInt());
	m_fmleProfile.setVideoEncoderWidth(attribute("videoEncoderWidth")->toInt());
	m_fmleProfile.setVideoEncoderHeight(attribute("videoEncoderHeight")->toInt());
	m_fmleProfile.setVideoEncoderKeyframeFrequency(attribute("videoEncoderKeyframeFrequency")->toInt());

	m_fmleProfile.setAudioCaptueDevice(attribute("audioCaptureDevice")->toString());
	m_fmleProfile.setAudioSampleRate(attribute("audioSampleRate")->toFloat());
	m_fmleProfile.setAudioChannels(attribute("audioChannels")->toInt());
	m_fmleProfile.setAudioEncoder(attribute("audioEncoder")->toString());
	m_fmleProfile.setAudioEncoderBitrate(attribute("audioEncoderBitrate")->toInt());

	m_fmleProfile.setOutputStream(attribute("outputStream")->toString());
	m_fmleProfile.setOutputUrl(attribute("outputUrl")->toString());
	m_fmleProfile.setOutputBackupUrl(attribute("outputBackupUrl")->toString());
	m_fmleProfile.setOutputSaveFile(attribute("outputSaveFile")->toBool());
	m_fmleProfile.setOutputFilePath(attribute("outputFilePath")->toString());
}