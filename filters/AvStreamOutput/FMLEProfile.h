#ifndef _FMLEProfile_h_
#define _FMLEProfile_h_

#include "Base/common.h"
#include <boost/property_tree/xml_parser.hpp>

class FMLEProfile
{
public:
	FMLEProfile();
	~FMLEProfile(){}

	bool read(std::string filePath);
	void write(std::string filePath);

	Strings videoCaptueDevices();
	std::string videoCaptueDevice() const{return m_videoCaptureDevice;}
	void setVideoCaptueDevice(const std::string &caputeDevice){m_videoCaptureDevice=caputeDevice;}
	
	Strings videoEncoders();
	std::string videoEncoder() const{return m_videoEncoder;}
	void setVideoEncoder(const std::string &encoder){m_videoEncoder=encoder;}

	int videoEncoderBitrate() const{return m_videoEncoderBitrate;}
	void setVideoEncoderBitrate(int bitrate){m_videoEncoderBitrate=bitrate;}
	
	int videoEncoderWidth() const{return m_videoEncoderWidth;}
	void setVideoEncoderWidth(int width){m_videoEncoderWidth=width;}
	
	int videoEncoderHeight() const{return m_videoEncoderHeight;}
	void setVideoEncoderHeight(int height){m_videoEncoderHeight=height;}
	
	int videoEncoderKeyframeFrequency() const{return m_videoEncoderKeyframeFrequency;}
	void setVideoEncoderKeyframeFrequency(int keyframeFrequency){m_videoEncoderKeyframeFrequency=keyframeFrequency;}

	Strings audioCaptueDevices() const;
		std::string audioCaptueDevice() const{return m_audioCaptureDevice;}
	void setAudioCaptueDevice(const std::string &captureDevice){m_audioCaptureDevice=captureDevice;}

	std::vector<float> audioSampleRates();
	float audioSampleRate() const{return m_audioSampleRate;}
	void setAudioSampleRate(float sampleRate){m_audioSampleRate=sampleRate;}

	std::vector<int> audioChannelOptions();
	int audioChannels() const{return m_audioChannels;}
	void setAudioChannels(int channels){m_audioChannels=channels;}

	Strings audioEncoders();
	std::string audioEncoder() const{return m_audioEncoder;}
	void setAudioEncoder(const std::string &encoder){m_audioEncoder=encoder;}

	int audioEncoderBitrate() const{return m_audioEncoderBitrate;}
	void setAudioEncoderBitrate(int bitrate){m_audioEncoderBitrate=bitrate;}

	std::string outputUsername() const{return m_outputUsername;}
	void setOutputUsername(const std::string &username){m_outputUsername=username;}

	std::string outputPassword() const{return m_outputPassword;}
	void setOutputPassword(const std::string &password){m_outputPassword=password;}

	std::string outputUrl() const{return m_outputUrl;}
	void setOutputUrl(const std::string &url){m_outputUrl=url;}

	std::string outputBackupUrl() const{return m_outputBackupUrl;}
	void setOutputBackupUrl(const std::string url){m_outputBackupUrl=url;}

	std::string outputStream() const{return m_outputStream;}
	void setOutputStream(const std::string &stream){m_outputStream=stream;}

	bool outputSaveFile() const{return m_outputSaveFile;}
	void setOutputSaveFile(bool save){m_outputSaveFile=save;}

	std::string outputFilePath() const{return m_outputFilePath;}
	void setOutputFilePath(const std::string &filePath){m_outputFilePath=filePath;}

private:
	std::string m_videoCaptureDevice;
	std::string m_videoEncoder;
	int m_videoEncoderBitrate;
	int m_videoEncoderWidth;
	int m_videoEncoderHeight;
	int m_videoEncoderKeyframeFrequency;

	std::string m_audioCaptureDevice;
	float m_audioSampleRate;
	int m_audioChannels;
	std::string m_audioEncoder;
	int m_audioEncoderBitrate;

	std::string m_outputUsername;
	std::string m_outputPassword;
	std::string m_outputUrl;
	std::string m_outputBackupUrl;
	std::string m_outputStream;
	bool m_outputSaveFile;
	std::string m_outputFilePath;
};

#endif //_FMLEProfile_h_