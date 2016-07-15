#include "AvStream/AvStreamPipe.h"
#include "AvStream/AvPackets.h"

#include <boost/format.hpp>

#include <windows.h>

using namespace Limitless;

std::string getErrorMessage(DWORD error)
{
	LPVOID lpMsgBuf;
	
	FormatMessageA( 
		FORMAT_MESSAGE_ALLOCATE_BUFFER | 
		FORMAT_MESSAGE_FROM_SYSTEM | 
		FORMAT_MESSAGE_IGNORE_INSERTS,
		NULL,
		error,
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // Default language
		(LPSTR) &lpMsgBuf,
		0,
		NULL 
		);

	std::string errorMessage((LPSTR)lpMsgBuf);
	LocalFree( lpMsgBuf );
	return errorMessage;
}

namespace Limitless
{

class AvStreamPipeHidden
{
public:
	AvStreamPipeHidden():m_messagePipe(INVALID_HANDLE_VALUE),m_dataPipe(INVALID_HANDLE_VALUE){}
	~AvStreamPipeHidden(){}

	HANDLE m_messagePipe;
	HANDLE m_writeMessageEvent;
	HANDLE m_dataPipe;
	HANDLE m_stopEvent;

	OVERLAPPED m_overlapedMessage;
	OVERLAPPED m_overlapedMessageRead;
	OVERLAPPED m_overlapedData;
	OVERLAPPED m_overlapedDataRead;
};

}//namespace Limitless

AvStreamPipe::AvStreamPipe(int inputBufferSize, int outputBufferSize):
hidden(new AvStreamPipeHidden()),
m_inputBufferSize(m_inputBufferSize),
m_outputBufferSize(m_outputBufferSize),
m_created(false),
m_run(false),
m_running(false),
m_writeMessages(20)
{
	hidden->m_overlapedMessage.hEvent=CreateEvent(NULL, true, false, NULL);
	hidden->m_overlapedMessage.Offset=0;
	hidden->m_overlapedMessage.OffsetHigh=0;

	hidden->m_writeMessageEvent=CreateEvent(NULL, true, false, NULL);

	hidden->m_overlapedMessageRead.hEvent=CreateEvent(NULL, true, false, NULL);
	hidden->m_overlapedMessageRead.Offset=0;
	hidden->m_overlapedMessageRead.OffsetHigh=0;

	hidden->m_overlapedData.hEvent=CreateEvent(NULL, true, false, NULL);
	hidden->m_overlapedData.Offset=0;
	hidden->m_overlapedData.OffsetHigh=0;
	
	hidden->m_overlapedDataRead.hEvent=CreateEvent(NULL, true, false, NULL);
	hidden->m_overlapedDataRead.Offset=0;
	hidden->m_overlapedDataRead.OffsetHigh=0;

	hidden->m_stopEvent=CreateEvent(NULL, true, false, NULL);
//	OutputDebugStringA((boost::format("0x%08x(0x%08x) - AvStreamPipe\n")%this%GetCurrentThreadId()).str().c_str());
}

AvStreamPipe::~AvStreamPipe()
{
//	OutputDebugStringA((boost::format("0x%08x(0x%08x) - ~AvStreamPipe\n")%this%GetCurrentThreadId()).str().c_str());
	disconnect();

	CloseHandle(hidden->m_overlapedMessage.hEvent);
	CloseHandle(hidden->m_writeMessageEvent);
	CloseHandle(hidden->m_overlapedMessageRead.hEvent);
	CloseHandle(hidden->m_overlapedData.hEvent);
	CloseHandle(hidden->m_overlapedDataRead.hEvent);
	CloseHandle(hidden->m_stopEvent);

	delete hidden;
}

bool AvStreamPipe::create(std::string pipeName)
{
	if(hidden->m_messagePipe != INVALID_HANDLE_VALUE)
		return false;

	m_pipeName=pipeName;
	std::string messagePipeName=(boost::format("\\\\.\\pipe\\%sMessage")%m_pipeName).str();

	hidden->m_messagePipe=CreateNamedPipeA(messagePipeName.c_str(), PIPE_ACCESS_DUPLEX|FILE_FLAG_OVERLAPPED, 
		PIPE_TYPE_BYTE|PIPE_READMODE_BYTE|PIPE_WAIT, PIPE_UNLIMITED_INSTANCES, m_outputBufferSize, m_inputBufferSize,
		NMPWAIT_USE_DEFAULT_WAIT, NULL);

	std::string dataPipeName=(boost::format("\\\\.\\pipe\\%sData")%m_pipeName).str();
	hidden->m_dataPipe=CreateNamedPipeA(dataPipeName.c_str(), PIPE_ACCESS_DUPLEX|FILE_FLAG_OVERLAPPED, 
		PIPE_TYPE_BYTE|PIPE_READMODE_BYTE|PIPE_WAIT, PIPE_UNLIMITED_INSTANCES, m_outputBufferSize, m_inputBufferSize,
		NMPWAIT_USE_DEFAULT_WAIT, NULL);

	if(hidden->m_messagePipe == INVALID_HANDLE_VALUE)
	{
//		OutputDebugStringA((boost::format("0x%08x(0x%08x) - create invalid handle for messagePipe\n")%this%GetCurrentThreadId()).str().c_str());
		return false;
	}
	if(hidden->m_dataPipe == INVALID_HANDLE_VALUE)
	{
//		OutputDebugStringA((boost::format("0x%08x(0x%08x) - create invalid handle for dataPipe\n")%this%GetCurrentThreadId()).str().c_str());
		disconnect();
		return false;
	}

	m_created=true;
	return true;
}

bool AvStreamPipe::connect(std::string pipeName)
{
	if(!pipeName.empty())
		m_pipeName=pipeName;
	else if(!m_created)
		return false;

	if(m_created)
	{
		if(hidden->m_messagePipe == INVALID_HANDLE_VALUE)
			return false;

		bool messageConnected=false;
		bool dataConnected=false;
		DWORD error;

		if(!ConnectNamedPipe(hidden->m_messagePipe, &hidden->m_overlapedMessage))
		{
			error=GetLastError();

			if(error == ERROR_PIPE_CONNECTED)
				messageConnected=true;
			else if((error != ERROR_PIPE_LISTENING) && (error != ERROR_IO_PENDING))
				return false;
//			OutputDebugStringA((boost::format("0x%08x(0x%08x) - ConnectNamedPipe messagePipe()\n")%this%GetCurrentThreadId()).str().c_str());
		}
		if(!ConnectNamedPipe(hidden->m_dataPipe, &hidden->m_overlapedData))
		{
			error=GetLastError();

			if(error == ERROR_PIPE_CONNECTED)
				dataConnected=true;
			else if((error != ERROR_PIPE_LISTENING) && (error != ERROR_IO_PENDING))
				return false;
			
//			OutputDebugStringA((boost::format("0x%08x(0x%08x) - ConnectNamedPipe dataPipe()\n")%this%GetCurrentThreadId()).str().c_str());
		}
		if(!messageConnected)
		{
			DWORD bytesWritten;

			while(true)
			{
				WaitForSingleObject(hidden->m_overlapedMessage.hEvent, INFINITE);
				if(!GetOverlappedResult(hidden->m_messagePipe, &hidden->m_overlapedMessage, &bytesWritten, false))
					return false;
				else
					break;
			}
		}
		if(!dataConnected)
		{
			DWORD bytesWritten;

			while(true)
			{
				WaitForSingleObject(hidden->m_overlapedData.hEvent, INFINITE);
				if(!GetOverlappedResult(hidden->m_dataPipe, &hidden->m_overlapedData, &bytesWritten, false))
					return false;
				else
					break;
			}
		}
	}
	else
	{
		if(hidden->m_messagePipe != INVALID_HANDLE_VALUE)
			return false;

		std::string messagePipeName=(boost::format("\\\\.\\pipe\\%sMessage")%m_pipeName).str();

		hidden->m_messagePipe=CreateFileA(messagePipeName.c_str(), GENERIC_READ|GENERIC_WRITE, 0, NULL, OPEN_EXISTING, FILE_FLAG_OVERLAPPED, NULL);

//		OutputDebugStringA((boost::format("0x%08x(0x%08x) - CreateFile messagePipe\n")%this%GetCurrentThreadId()).str().c_str());

		if(hidden->m_messagePipe != INVALID_HANDLE_VALUE)
		{
			std::string dataPipeName=(boost::format("\\\\.\\pipe\\%sData")%m_pipeName).str();

			hidden->m_dataPipe=CreateFileA(dataPipeName.c_str(), GENERIC_READ|GENERIC_WRITE, 0, NULL, OPEN_EXISTING, FILE_FLAG_OVERLAPPED, NULL);

//			OutputDebugStringA((boost::format("0x%08x(0x%08x) - CreateFile dataPipe\n")%this%GetCurrentThreadId()).str().c_str());
		}
	}
	if(hidden->m_messagePipe == INVALID_HANDLE_VALUE)
	{
//		OutputDebugStringA((boost::format("0x%08x(0x%08x) - connect invalid handle for messagePipe\n")%this%GetCurrentThreadId()).str().c_str());
		return false;
	}
	if(hidden->m_dataPipe == INVALID_HANDLE_VALUE)
	{
//		OutputDebugStringA((boost::format("0x%08x(0x%08x) - connect invalid handle for dataPipe\n")%this%GetCurrentThreadId()).str().c_str());
		disconnect();
		return false;
	}

	m_connected=true;
	m_run=true;
	ResetEvent(hidden->m_stopEvent);
	
	m_thread=boost::thread(boost::bind(&AvStreamPipe::processMessageThread, this));

	{
		boost::unique_lock<boost::mutex> lock(m_mutex);
	
		while(!m_running)
			m_event.wait(lock);
	}
	return true;
}

void AvStreamPipe::disconnect()
{
	boost::unique_lock<boost::mutex> lock(m_mutex);

//	OutputDebugStringA((boost::format("0x%08x(0x%08x) - disconnect()\n")%this%GetCurrentThreadId()).str().c_str());

	m_run=false;
	SetEvent(hidden->m_stopEvent);

//	while(m_running)
//		m_event.wait(lock);
	m_thread.join();

	if(hidden->m_messagePipe != INVALID_HANDLE_VALUE)
	{
		if(m_created)
		{
			DisconnectNamedPipe(hidden->m_messagePipe);
//			OutputDebugStringA((boost::format("0x%08x(0x%08x) - DisconnectNamedPipe messagePipe()\n")%this%GetCurrentThreadId()).str().c_str());
		}
		else
		{
			CloseHandle(hidden->m_messagePipe);
//			OutputDebugStringA((boost::format("0x%08x(0x%08x) - CloseHandle messagePipe()\n")%this%GetCurrentThreadId()).str().c_str());
		}
		hidden->m_messagePipe=INVALID_HANDLE_VALUE;
	}
	if(hidden->m_dataPipe != INVALID_HANDLE_VALUE)
	{
		if(m_created)
		{
			DisconnectNamedPipe(hidden->m_dataPipe);
//			OutputDebugStringA((boost::format("0x%08x(0x%08x) - DisconnectNamedPipe dataPipe()\n")%this%GetCurrentThreadId()).str().c_str());
		}
		else
		{
			CloseHandle(hidden->m_dataPipe);
//			OutputDebugStringA((boost::format("0x%08x(0x%08x) - CloseHandle dataPipe()\n")%this%GetCurrentThreadId()).str().c_str());
		}
		hidden->m_dataPipe=INVALID_HANDLE_VALUE;
	}
}

void AvStreamPipe::processMessageThread()
{
	AvStreamHeader streamHeader;
	DWORD bytesRead;
	std::vector<char> buffer(1024);
	HANDLE handles[3];

	m_running=true;
	m_event.notify_one();
	
	handles[0]=hidden->m_overlapedMessageRead.hEvent;
	handles[1]=hidden->m_writeMessageEvent;
	handles[2]=hidden->m_stopEvent;
//	handles[1]=hidden->m_stopEvent;
//	OutputDebugStringA((boost::format("0x%08x(0x%08x) - processMessageThread started\n")%this%GetCurrentThreadId()).str().c_str());

	while(m_run)
	{
		if(!ReadFile(hidden->m_messagePipe, &streamHeader, sizeof(AvStreamHeader), &bytesRead, &hidden->m_overlapedMessageRead))
		{
			DWORD error=GetLastError();

			if(error != ERROR_IO_PENDING)
			{
//				OutputDebugStringA((boost::format("0x%08x(0x%08x) - processMessageThread read header(%s)\n")%this%GetCurrentThreadId()%getErrorMessage(error)).str().c_str());
				onBrokenStream();
				break;
			}

			bool breakWhile=false;
			bool loop=true;

			while(loop)
			{
				switch(WaitForMultipleObjects(3, handles, false, INFINITE))
				{
				case WAIT_OBJECT_0:
					loop=false;
					break;
				case WAIT_OBJECT_0+1:
					internalWriteMessages();
					break;
				case WAIT_OBJECT_0+2:
					loop=false;
					breakWhile=true;
					break;
				}
			}
			
			if(breakWhile)
				continue;

//			WaitForSingleObject(hidden->m_overlapedMessageRead.hEvent, INFINITE);
			GetOverlappedResult(hidden->m_dataPipe, &hidden->m_overlapedMessageRead, &bytesRead, false);
			if(bytesRead != sizeof(AvStreamHeader))
				continue;
		}

		if(streamHeader.size > 0)
		{
			if(buffer.size() < streamHeader.size)
				buffer.resize(streamHeader.size);
			
			if(!ReadFile(hidden->m_messagePipe, &buffer[0], streamHeader.size, &bytesRead, &hidden->m_overlapedMessageRead))
			{
				DWORD error=GetLastError();
				
				if(error != ERROR_IO_PENDING)
				{

//					OutputDebugStringA((boost::format("0x%08x(0x%08x) - processMessageThread read packet(%s)\n")%this%GetCurrentThreadId()%getErrorMessage(error)).str().c_str());
					onBrokenStream();
					break;
				}

//				WaitForSingleObject(hidden->m_overlapedMessageRead.hEvent, INFINITE);
				if(WaitForMultipleObjects(2, handles, false, INFINITE) != WAIT_OBJECT_0)
					continue;
				GetOverlappedResult(hidden->m_dataPipe, &hidden->m_overlapedMessageRead, &bytesRead, false);
				if(bytesRead != streamHeader.size)
					continue;
			}
		}
		onMessage(streamHeader.type, &buffer[0], streamHeader.size);
	}
//	OutputDebugStringA((boost::format("0x%08x(0x%08x) - processMessageThread stopped\n")%this%GetCurrentThreadId()).str().c_str());
	m_running=false;
	m_event.notify_one();
}

void AvStreamPipe::internalWriteMessages()
{
	std::vector<char> *buffer;
	DWORD bytesWritten;

	ResetEvent(hidden->m_writeMessageEvent);
	while(m_writeMessages.pop(buffer))
	{
		if(!WriteFile(hidden->m_messagePipe, &(*buffer)[0], (DWORD)buffer->size(), &bytesWritten, &hidden->m_overlapedMessage))
		{
			DWORD error=GetLastError();

			if(error != ERROR_IO_PENDING)
			{
//				OutputDebugStringA((boost::format("0x%08x(0x%08x) - internalWriteMessages (%s)\n")%this%GetCurrentThreadId()%getErrorMessage(error)).str().c_str());
				delete buffer;
				return;
			}

			HANDLE handles[2];

			handles[0]=hidden->m_overlapedMessage.hEvent;
			handles[1]=hidden->m_stopEvent;

			//		WaitForSingleObject(hidden->m_overlapedMessage.hEvent, INFINITE);
			if(WaitForMultipleObjects(2, handles, false, INFINITE) != WAIT_OBJECT_0)
			{
				delete buffer;
				return;
			}
			GetOverlappedResult(hidden->m_messagePipe, &hidden->m_overlapedMessage, &bytesWritten, false);
			if(buffer->size() != bytesWritten)
			{
				delete buffer;
				return;
			}
		}
		delete buffer;
//		return true;
	}
//	return false;
}

bool AvStreamPipe::writeMessage(char *buffer, int size)
{
	std::vector<char> *writeBuffer=new std::vector<char>(size);

	memcpy(&(*writeBuffer)[0], buffer, size);
	m_writeMessages.push(writeBuffer);
	SetEvent(hidden->m_writeMessageEvent);
	return true;
}

//bool AvStreamPipe::writeMessage(char *buffer, int size)
//{
//	DWORD bytesWritten;
//
//	if(!WriteFile(hidden->m_messagePipe, buffer, size, &bytesWritten, &hidden->m_overlapedMessage))
//	{
//		DWORD error=GetLastError();
//
//		if(error != ERROR_IO_PENDING)
//			return false;
//
//		HANDLE handles[2];
//
//		handles[0]=hidden->m_overlapedMessage.hEvent;
//		handles[1]=hidden->m_stopEvent;
//
////		WaitForSingleObject(hidden->m_overlapedMessage.hEvent, INFINITE);
//		if(WaitForMultipleObjects(2, handles, false, INFINITE) != WAIT_OBJECT_0)
//			return false;
//		GetOverlappedResult(hidden->m_messagePipe, &hidden->m_overlapedMessage, &bytesWritten, false);
//		if(size != bytesWritten)
//			return false;
//	}
//	return true;
//}

bool AvStreamPipe::write(char *buffer, int size)
{
	DWORD bytesWritten;

	if(!WriteFile(hidden->m_dataPipe, buffer, size, &bytesWritten, &hidden->m_overlapedData))
	{
		DWORD error=GetLastError();

		if(error != ERROR_IO_PENDING)
		{
//			OutputDebugStringA((boost::format("0x%08x(0x%08x) - write (%s)\n")%this%GetCurrentThreadId()%getErrorMessage(error)).str().c_str());
			return false;
		}

		HANDLE handles[2];

		handles[0]=hidden->m_overlapedData.hEvent;
		handles[1]=hidden->m_stopEvent;

		if(WaitForMultipleObjects(2, handles, false, INFINITE) != WAIT_OBJECT_0)
			return false;
//		WaitForSingleObject(hidden->m_overlapedData.hEvent, INFINITE);
		GetOverlappedResult(hidden->m_dataPipe, &hidden->m_overlapedData, &bytesWritten, false);
		if(size != bytesWritten)
			return false;
	}
	return true;
}

bool AvStreamPipe::read(char *buffer, int size)
{
	DWORD bytesRead;

	if(!ReadFile(hidden->m_dataPipe, buffer, size, &bytesRead, &hidden->m_overlapedDataRead))
	{
		DWORD error=GetLastError();

		if(error != ERROR_IO_PENDING)
		{
//			OutputDebugStringA((boost::format("0x%08x(0x%08x) - read (%s)\n")%this%GetCurrentThreadId()%getErrorMessage(error)).str().c_str());
			return false;
		}

		HANDLE handles[2];

		handles[0]=hidden->m_overlapedDataRead.hEvent;
		handles[1]=hidden->m_stopEvent;

//		WaitForSingleObject(hidden->m_overlapedDataRead.hEvent, INFINITE);
		if(WaitForMultipleObjects(2, handles, false, INFINITE) != WAIT_OBJECT_0)
			return false;
		GetOverlappedResult(hidden->m_dataPipe, &hidden->m_overlapedDataRead, &bytesRead, false);
		if(size != bytesRead)
			return false;
	}
	return true;
}

int AvStreamPipe::getWriteBufferSize()
{
	DWORD bytesAvaible;

	PeekNamedPipe(hidden->m_dataPipe, NULL, NULL, NULL, &bytesAvaible, NULL);

	return bytesAvaible;
}

void AvStreamPipe::stopEvent()
{
	SetEvent(hidden->m_stopEvent);
}