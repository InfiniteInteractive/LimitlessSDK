#ifndef _AvStreamPipe_h_
#define _AvStreamPipe_h_

#include "AvStream/avstream_define.h"
#include <boost/thread.hpp>

#include <boost/lockfree/queue.hpp>

namespace Limitless
{

class AvStreamPipeHidden;

#pragma warning(push)
#pragma warning(disable:4251)

class AVSTREAM_EXPORT AvStreamPipe
{
public:
	AvStreamPipe(int inputBufferSize=4096, int outputBufferSize=4096);
	virtual ~AvStreamPipe();

	virtual unsigned int streamId(){return 0;}

	bool create(std::string pipeName);
	virtual bool connect(std::string pipeName="");
	virtual void disconnect();

	bool writeMessage(char *buffer, int size);
	bool write(char *buffer, int size);
	bool read(char *buffer, int size);

	bool connected();
	bool disconnected();

	void processMessageThread();

	int getWriteBufferSize();

protected:
	virtual void onMessage(int type, char *message, int size){/*default call back?*/}
	virtual void onBrokenStream(){/*default call back?*/}

	void stopEvent();

	void internalWriteMessages();

	
	std::string m_pipeName;
	bool m_created;
	bool m_connected;
	bool m_run;
	bool m_running;

private:
	int m_inputBufferSize;
	int m_outputBufferSize;

	boost::thread m_thread;
	boost::condition_variable m_event;
	boost::mutex m_mutex;

	boost::lockfree::queue<std::vector<char> *> m_writeMessages;
//	boost::lockfree::queue<void *> m_writeMessages;

	AvStreamPipeHidden *hidden;
};

#pragma warning(pop)

typedef boost::shared_ptr<AvStreamPipe> SharedAvStreamPipe;

}//namespace Limitless

#endif //_AvStreamPipe_h_