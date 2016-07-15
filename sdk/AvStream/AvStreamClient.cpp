#include "AvStream/AvStreamClient.h"

#include <boost/format.hpp>
#include "windows.h"

using namespace Limitless;

AvStreamClient::AvStreamClient():
m_init(false),
m_processingData(false),
m_processThreadRunning(false),
m_streamId(0)
{
//	std::string output=(boost::format("0x%08x(0x%08x) %d - AvStreamClient()\n")%this%GetCurrentThreadId()%m_streamId).str();
//	OutputDebugStringA(output.c_str());
}

AvStreamClient::~AvStreamClient()
{
//	std::string output=(boost::format("0x%08x(0x%08x) %d - ~AvStreamClient()\n")%this%GetCurrentThreadId()%m_streamId).str();
//	OutputDebugStringA(output.c_str());
}

void AvStreamClient::disconnect()
{
	boost::unique_lock<boost::mutex> lock(m_dataThreadMutex);

	m_processingData=false;
	stopEvent();

//	std::string output=(boost::format("0x%08x(0x%08x) %d - disconnect set\n")%this%GetCurrentThreadId()%m_streamId).str();
//	OutputDebugStringA(output.c_str());

//	while(m_processThreadRunning)
//		m_dataThreadEvent.wait(lock);
	m_dataThread.join();
	
//	output=(boost::format("0x%08x(0x%08x) %d - disconnect base\n")%this%GetCurrentThreadId()%m_streamId).str();
//	OutputDebugStringA(output.c_str());

	AvStreamPipe::disconnect();

//	output=(boost::format("0x%08x(0x%08x) %d - disconnected\n")%this%GetCurrentThreadId()%m_streamId).str();
//	OutputDebugStringA(output.c_str());
}

void AvStreamClient::sendRqFormat()
{
	AvStreamRqFormat rqFormat;
	
	writeMessage((char *)&rqFormat, sizeof(AvStreamRqFormat));
}

void AvStreamClient::sendStart()
{
	AvStreamStart start;

	writeMessage((char *)&start, sizeof(AvStreamStart));
}

void AvStreamClient::sendPause()
{
	AvStreamPause pause;

	writeMessage((char *)&pause, sizeof(AvStreamPause));
}

void AvStreamClient::sendFrameAck(unsigned int sequenceNumber)
{
	AvStreamFrameAck frameAck(sequenceNumber);

	writeMessage((char *)&frameAck, sizeof(frameAck));
}

void AvStreamClient::processDataThread()
{
	AvStreamHeader streamHeader;

	m_processThreadRunning=true;
	m_dataThreadEvent.notify_one();

//	std::string output=(boost::format("0x%08x(0x%08x) %d - processDataThread started\n")%this%GetCurrentThreadId()%m_streamId).str();
//	OutputDebugStringA(output.c_str());

	while(m_processingData)
	{
		if(!read((char *)&streamHeader, sizeof(AvStreamHeader)))
		{
			std::string output=(boost::format("0x%08x(0x%08x) %d - processDataThread read failed thread stopping\n")%this%GetCurrentThreadId()%m_streamId).str();
			break;
		}

		switch(streamHeader.type)
		{
		case AVSTREAM_FRAME:
			onFrameRead(streamHeader.size);
			break;
		}
	}

//	output=(boost::format("0x%08x(0x%08x) %d - processDataThread stop\n")%this%GetCurrentThreadId()%m_streamId).str();
//	OutputDebugStringA(output.c_str());

	m_processThreadRunning=false;
	m_dataThreadEvent.notify_one();
}

void AvStreamClient::onMessage(int type, char *message, int size)
{
	switch(type)
	{
	case AVSTREAM_INIT:
		processInit(message, size);
		break;
	}
}

void AvStreamClient::onFrameRead(int size)
{
	std::vector<char> buffer(size);

	read((char *)&buffer[0], size);
}

void AvStreamClient::processInit(char *message, int size)
{
	if(!m_init)
	{
		m_init=true;
		unsigned int *initStreamId=(unsigned int *)message;

		m_streamId=*initStreamId;

//		OutputDebugStringA((boost::format("0x%08x(0x%08x) %d - processInit\n")%this%GetCurrentThreadId()%m_streamId).str().c_str());

		AvStreamRqFormat rqFormatPacket;
		
		writeMessage((char *)&rqFormatPacket, sizeof(AvStreamHeader));

		m_processingData=true;
		m_dataThread=boost::thread(boost::bind(&AvStreamClient::processDataThread, this));
		{
			boost::unique_lock<boost::mutex> lock(m_dataThreadMutex);

			while(!m_processThreadRunning)
				m_dataThreadEvent.wait(lock);
		}
	}
}