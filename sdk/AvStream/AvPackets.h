#ifndef _AvPackets_h_
#define _AvPackets_h_

#include "AvStream/avstream_define.h"

#define AVSTREAM_INIT 0x01
#define AVSTREAM_RQCLOCK 0x02
#define AVSTREAM_CLOCK 0x03
#define AVSTREAM_START 0x04
#define AVSTREAM_PAUSE 0x05
#define AVSTREAM_STOP 0x05

#define AVSTREAM_RQFORMAT 0x10
#define AVSTREAM_FORMAT 0x11
#define AVSTREAM_FRAME 0x12
#define AVSTREAM_FRAME_ACK 0x13

namespace Limitless
{

#pragma pack(push)
#pragma pack(1)

struct AvStreamHeader
{
	AvStreamHeader(){}
	AvStreamHeader(int type, int size):type(type), size(size){}

	int type;
	int size;
};

struct AvStreamInit
{
	AvStreamInit():header(AVSTREAM_INIT, sizeof(unsigned int)){};
	AvStreamInit(int streamId):header(AVSTREAM_INIT, sizeof(unsigned int)), streamId(streamId){};

	AvStreamHeader header;
	unsigned int streamId;
};

struct AvStreamRqClock
{
	AvStreamRqClock():header(AVSTREAM_RQCLOCK, 0){};

	AvStreamHeader header;
};

struct AvStreamClock
{
	AvStreamClock():header(AVSTREAM_CLOCK, sizeof(__int64)){};

	AvStreamHeader header;
	__int64 clock;
};

struct AvStreamStart
{
	AvStreamStart():header(AVSTREAM_START, 0){};

	AvStreamHeader header;
};

struct AvStreamPause
{
	AvStreamPause():header(AVSTREAM_PAUSE, 0){};

	AvStreamHeader header;
};

struct AvStreamRqFormat
{
	AvStreamRqFormat():header(AVSTREAM_RQFORMAT, 0){};

	AvStreamHeader header;
	__int64 clock;
};

struct AvStreamFormat
{
	int width;
	int height;
	int channels;
	int channelBits;
	int frameTime;
};

struct AvStreamFormatPacket
{
	AvStreamFormatPacket():header(AVSTREAM_FORMAT, sizeof(AvStreamFormat)){};

	AvStreamHeader header;
	AvStreamFormat format;
};


struct AvStreamFrameHeader
{
//	int width;
//	int height;
//	int channels;
//	int bitsPerChannel;
	unsigned int sequenceNumber;
	unsigned int mediaIndex;
	unsigned __int64 timeStamp;
//	long bufferSize;
};

struct AvStreamFrame
{
	AvStreamFrame():header(AVSTREAM_FRAME, 0){};

	AvStreamHeader header;
	AvStreamFrameHeader frameHeader;
};

struct AvStreamFrameAck
{
	AvStreamFrameAck(unsigned int sequenceNumber):header(AVSTREAM_FRAME_ACK, sizeof(unsigned int)), sequenceNumber(sequenceNumber){};

	AvStreamHeader header;
	unsigned int sequenceNumber;
};

#pragma pack(pop)

}//namespace Limitless

#endif //_AvPackets_h_