#ifndef _AvStreamClient_h_
#define _AvStreamClient_h_

#include "AvStream/avstream_define.h"

#include "AvStream/AvPackets.h"
#include "AvStream/AvStreamPipe.h"

#pragma warning(push)
#pragma warning(disable:4251)

namespace Limitless
{

class AVSTREAM_EXPORT AvStreamClient:public AvStreamPipe
{
public:
	AvStreamClient();
	~AvStreamClient();

	virtual unsigned int streamId(){return m_streamId;}
	virtual void disconnect();
//	void open(std::string pipeName);
//	void close();
//
//	void onInit();
//	void onPacketRecivied();
	void sendRqFormat();
	void sendStart();
	void sendPause();
	void sendFrameAck(unsigned int sequenceNumber);

	void processDataThread();
protected:
	virtual void onMessage(int type, char *message, int size);
	virtual void onFrameRead(int size);

private:
	void processInit(char *message, int size);

	unsigned int m_streamId;
	bool m_init;
	bool m_processingData;
	bool m_processThreadRunning;
	boost::thread m_dataThread;
	boost::condition_variable m_dataThreadEvent;
	boost::mutex m_dataThreadMutex;
};

}//namespace Limitless

#pragma warning(pop)

#endif //_AvStreamClient_h_