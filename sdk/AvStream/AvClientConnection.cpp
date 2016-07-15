#include "AvStream/AvClientConnection.h"
#include "AvStream/AvStreamServer.h"

using namespace Limitless;

AvClientConnection::AvClientConnection(AvStreamServer *streamServer, unsigned int streamId):
m_streamServer(streamServer),
m_started(false),
m_streamId(streamId),
m_outstandingImages(0),
m_maxOutstandingImages(10)
{
//	m_thread=boost::thread(boost::bind(&AvClientConnection::processThread, this));	
}

AvClientConnection::~AvClientConnection()
{
	m_running=false;
	disconnect(); //should force read to fail?
}

void AvClientConnection::onMessage(int type, char *message, int size)
{
	switch(type)
	{
//	case AVSTREAM_INIT:
//		processInit(message, size);
//		break;
	case AVSTREAM_RQFORMAT:
		processRqFormat(message, size);
		break;
	case AVSTREAM_CLOCK:
		processClock(message, size);
		break;
	case AVSTREAM_START:
		processStart(message, size);
		break;
	case AVSTREAM_PAUSE:
		processPause(message, size);
		break;
	case AVSTREAM_FRAME_ACK:
		processFrameAck(message, size);
		break;
	}
}

void AvClientConnection::onBrokenStream()
{
	m_streamServer->closeClient(this);
}
//void AvClientConnection::processThread()
//{
//	AvStreamHeader streamHeader;
//	while(m_running)
//	{
//		read(&streamHeader, sizeof(AvStreamHeader));
//
//		switch(streamHeader.type)
//		{
//		case AVSTREAM_INIT:
//			procesInit(streamHeader);
//			break;
//		case AVSTREAM_CLOCK:
//			processClock();
//			break;
//		}
//	}
//}

//void AvClientConnection::processInit(char *message, int size)
//{
//	AvStreamInit avStreamInit;
//	
//	writeMessage((char *)&avStreamInit, sizeof(AvStreamHeader));
//}

void AvClientConnection::processRqFormat(char *message, int size)
{
	AvStreamFormatPacket formatPacket;

	formatPacket.format=m_streamFormat;
	writeMessage((char *)&formatPacket, sizeof(AvStreamHeader)+formatPacket.header.size);
}

void AvClientConnection::processClock(char *message, int size)
{
	AvStreamClock *streamClock=(AvStreamClock *)message;

	m_streamServer->processClock(streamClock->clock);
}

void AvClientConnection::processStart(char *message, int size)
{
	m_started=true;
}

void AvClientConnection::processPause(char *message, int size)
{
//	m_started=false;
}

void AvClientConnection::processFrameAck(char *message, int size)
{
	m_outstandingImages--;
}

void AvClientConnection::setFormat(SharedMediaFormat format)
{
	if(format->exists("width"))
		m_streamFormat.width=format->attribute("width")->toInt();
	if(format->exists("height"))
		m_streamFormat.height=format->attribute("height")->toInt();
	m_streamFormat.channels=3;
	m_streamFormat.channelBits=8;
	m_streamFormat.frameTime=666667; //15fps
}

void AvClientConnection::sendInit()
{
	AvStreamInit initPacket(m_streamId);

	writeMessage((char *)&initPacket, sizeof(AvStreamHeader)+initPacket.header.size);
}

//bool AvClientConnection::sendFrame(SharedImageSample imageSample)
bool AvClientConnection::sendFrame(ImageSample *imageSample)
{
	if(m_started)
	{
		if(m_outstandingImages > m_maxOutstandingImages)
			return true;

		AvStreamFrame framePacket;
		
		framePacket.frameHeader.timeStamp=imageSample->timestamp();
		framePacket.frameHeader.sequenceNumber=imageSample->sequenceNumber();
		framePacket.frameHeader.mediaIndex=imageSample->mediaIndex();
		framePacket.header.size=(int)(sizeof(AvStreamFrameHeader)+imageSample->size());

		if(write((char *)&framePacket, sizeof(AvStreamFrame)))
		{
			if(write((char *)imageSample->buffer(), (int)imageSample->size()))
			{
				m_outstandingImages++;
				return true;	
			}
		}
		return false;
	}
	return true;
}