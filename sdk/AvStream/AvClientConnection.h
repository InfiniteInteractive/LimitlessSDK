#ifndef _AvClientConnection_h_
#define _AvClientConnection_h_

#include "AvStream/avstream_define.h"
#include "AvStream/AvPackets.h"
#include "AvStream/AvStreamPipe.h"

#include "Media/MediaFormat.h"
#include "Media/MediaSample.h"
#include "Media/ImageSample.h"

#include <boost/thread.hpp>

namespace Limitless
{

class AvStreamServer;

class AVSTREAM_EXPORT AvClientConnection:public AvStreamPipe
{
public:
	AvClientConnection(AvStreamServer *streamServer, unsigned int streamId);
	virtual ~AvClientConnection();

	virtual unsigned int streamId(){return m_streamId;}

//	void processThread();
	void setFormat(SharedMediaFormat format);
	void sendInit();
//	bool sendFrame(SharedImageSample imageSample);
	bool sendFrame(ImageSample *imageSample);

protected:
	virtual void onMessage(int type, char *message, int size);
//	virtual void onData(int type, char *message, int size);
	virtual void onBrokenStream();

private:
//	void processInit(char *message, int size);
	void processRqFormat(char *message, int size);
	void processClock(char *message, int size);
	void processStart(char *message, int size);
	void processPause(char *message, int size);
	void processFrameAck(char *message, int size);

	AvStreamServer *m_streamServer;
	AvStreamFormat m_streamFormat;

	bool m_started;
	unsigned int m_streamId;
	unsigned int m_outstandingImages;
	unsigned int m_maxOutstandingImages;
};
typedef boost::shared_ptr<AvClientConnection> SharedAvClientConnection;
typedef std::vector<SharedAvClientConnection> SharedAvClientConnections;

}//namespace Limitless

#endif //_AvClientConnection_h_