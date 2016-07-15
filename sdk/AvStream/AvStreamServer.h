#ifndef _AvStreamServer_h_
#define _AvStreamServer_h_

#include "AvStream/avstream_define.h"
#include "AvStream/AvPackets.h"
#include "AvStream/AvClientConnection.h"

#include "Media/MediaFormat.h"
#include "Media/MediaSample.h"
#include "Media/ImageSample.h"

#include <boost/thread.hpp>

#pragma warning(push)
#pragma warning(disable:4251)

namespace Limitless
{

class AVSTREAM_EXPORT AvStreamServer
{
public:
	AvStreamServer();
	~AvStreamServer();

	void open(std::string serverName);
	void close();

	void setFormat(SharedMediaFormat format);
//	void sendFrame(SharedImageSample imageSample);
	void sendFrame(ImageSample *imageSample);

	void listenThread();

//	void closeClient(SharedAvClientConnection client);
	void closeClient(AvClientConnection *client);
	
	void processClock(__int64 clock);

	void disconnectThread();
private:
	unsigned int m_nextStreamId;
	boost::thread m_thread;
	boost::condition_variable m_event;
	boost::mutex m_mutex;
	bool m_listening;

	boost::mutex m_clientMutex;

	SharedMediaFormat m_format;
	int width;
	int height;
	int channels;

	SharedAvClientConnections m_clients;

	boost::mutex m_disconnectMutex;
	boost::condition_variable m_discconectEvent;
	std::vector<AvClientConnection *> m_disconnectClient;
};

}//namespace Limitless

#pragma warning(pop)

#endif //_AvStreamServer_h_