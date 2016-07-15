#include "AvStream/AvStreamServer.h"

#include <boost/format.hpp>
#include <boost/bind.hpp>
#include <boost/foreach.hpp>

using namespace Limitless;

AvStreamServer::AvStreamServer():
m_listening(false),
m_nextStreamId(1)
{
}

AvStreamServer::~AvStreamServer()
{
}

void AvStreamServer::open(std::string serverName)
{
	std::string pipeName=(boost::format("\\\\.\\pipe\\%s")%serverName).str();

	m_listening=true;
	m_thread=boost::thread(boost::bind(&AvStreamServer::listenThread, this));
}

void AvStreamServer::close()
{
	m_listening=true;
//	m_pipe.close();
}

void AvStreamServer::setFormat(SharedMediaFormat format)
{
	m_format.reset(new MediaFormat(*format));

	{
		boost::unique_lock<boost::mutex> lock(m_clientMutex);

		BOOST_FOREACH(SharedAvClientConnection &client, m_clients)
		{
			client->setFormat(format);
		}
	}
}

//void AvStreamServer::sendFrame(SharedImageSample sample)
void AvStreamServer::sendFrame(ImageSample *sample)
{
//	BOOST_FOREACH(SharedAvClientConnection &client, m_clients)
//	{
//		if(!client->sendFrame(sample))
//			closeClient(client);
//	}
	boost::unique_lock<boost::mutex> lock(m_clientMutex);

	for(SharedAvClientConnections::iterator iter=m_clients.begin(); iter!=m_clients.end(); )
	{
		if((*iter)->sendFrame(sample))
			++iter;
		else
		{
			(*iter)->disconnect();
			iter=m_clients.erase(iter);
		}
	}
}

void AvStreamServer::listenThread()
{
	while(m_listening)
	{
		SharedAvClientConnection client(new AvClientConnection(this, m_nextStreamId));
		
		m_nextStreamId++;
		if(!client->create("AvStream"))
			break;
		if(client->connect())
		{
			if(m_format != SharedMediaFormat())
				client->setFormat(m_format);
			{
				boost::unique_lock<boost::mutex> lock(m_clientMutex);
				m_clients.push_back(client);
			}
			client->sendInit();
		}
	}
}

//void AvStreamServer::closeClient(SharedAvClientConnection client)
//{
//	client->disconnect();
//
//	{
//		boost::unique_lock<boost::mutex> lock(m_clientMutex);
//
//		for(SharedAvClientConnections::iterator iter=m_clients.begin(); iter!=m_clients.end(); ++iter)
//		{
//			if(iter->get() == client.get())
//			{
//				m_clients.erase(iter);
//				break;
//			}
//		}
//	}
//	
//}

void AvStreamServer::closeClient(AvClientConnection *client)
{
	{
		boost::unique_lock<boost::mutex> lock(m_disconnectMutex);

		m_disconnectClient.push_back(client);
	}
	boost::thread(boost::bind(&AvStreamServer::disconnectThread, this));
}

void AvStreamServer::disconnectThread()
{
	boost::unique_lock<boost::mutex> disconnectLock(m_disconnectMutex);
	boost::unique_lock<boost::mutex> lock(m_clientMutex);

	for(int i=0; i<m_disconnectClient.size(); ++i)
	{
		AvClientConnection *client=m_disconnectClient[i];

		client->disconnect();
		for(SharedAvClientConnections::iterator iter=m_clients.begin(); iter!=m_clients.end(); ++iter)
		{
			if(iter->get() == client)
			{
				m_clients.erase(iter);
				break;
			}
		}
	}
	m_disconnectClient.clear();
}

void AvStreamServer::processClock(__int64 clock)
{
}

//void AvStreamServer::procesFrame()
//{
//}