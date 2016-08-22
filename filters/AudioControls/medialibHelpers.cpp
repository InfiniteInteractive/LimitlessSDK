#include "medialibHelpers.h"

medialib::AudioFormat convertFormat(Limitless::AudioSampleFormat format)
{
	switch(format)
	{
	case Limitless::AudioSampleFormat::UInt8:
		return medialib::AudioFormat::UInt8;
		break;
	case Limitless::AudioSampleFormat::Int16:
		return medialib::AudioFormat::Int16;
		break;
	case Limitless::AudioSampleFormat::Int32:
		return medialib::AudioFormat::Int32;
		break;
	case Limitless::AudioSampleFormat::Float:
		return medialib::AudioFormat::Float;
		break;
	case Limitless::AudioSampleFormat::Double:
		return medialib::AudioFormat::Double;
		break;
	case Limitless::AudioSampleFormat::UInt8P:
		return medialib::AudioFormat::UInt8P;
		break;
	case Limitless::AudioSampleFormat::Int16P:
		return medialib::AudioFormat::Int16P;
		break;
	case Limitless::AudioSampleFormat::Int32P:
		return medialib::AudioFormat::Int32P;
		break;
	case Limitless::AudioSampleFormat::FloatP:
		return medialib::AudioFormat::FloatP;
		break;
	case Limitless::AudioSampleFormat::DoubleP:
		return medialib::AudioFormat::DoubleP;
		break;
	}
	return medialib::AudioFormat::Unknown;
}

Limitless::AudioSampleFormat convertFormat(medialib::AudioFormat format)
{
	switch(format)
	{
	case medialib::AudioFormat::UInt8:
		return Limitless::AudioSampleFormat::UInt8;
		break;
	case medialib::AudioFormat::Int16:
		return Limitless::AudioSampleFormat::Int16;
		break;
	case medialib::AudioFormat::Int32:
		return Limitless::AudioSampleFormat::Int32;
		break;
	case medialib::AudioFormat::Float:
		return Limitless::AudioSampleFormat::Float;
		break;
	case medialib::AudioFormat::Double:
		return Limitless::AudioSampleFormat::Double;
		break;
	case medialib::AudioFormat::UInt8P:
		return Limitless::AudioSampleFormat::UInt8P;
		break;
	case medialib::AudioFormat::Int16P:
		return Limitless::AudioSampleFormat::Int16P;
		break;
	case medialib::AudioFormat::Int32P:
		return Limitless::AudioSampleFormat::Int32P;
		break;
	case medialib::AudioFormat::FloatP:
		return Limitless::AudioSampleFormat::FloatP;
		break;
	case medialib::AudioFormat::DoubleP:
		return Limitless::AudioSampleFormat::DoubleP;
		break;
	}
	return Limitless::AudioSampleFormat::Unknown;
}