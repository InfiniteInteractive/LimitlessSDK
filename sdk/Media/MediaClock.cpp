#include "Media/MediaClock.h"
#include "Utilities/utilitiesMath.h"

#include <limits>

using namespace Limitless;

MediaClock::MediaClock():
m_localOffset(0),
m_scaleNum(1),
m_scaleDen(1),
m_masterOffset(0), 
m_markerSize(0),
m_markerNext(0),
m_markerCalibrateCount(0),
m_markerWindowSize(32)
{
	m_timeMarkers=new MediaTime[m_markerWindowSize * 4];
}

MediaClock::~MediaClock()
{
	delete m_timeMarkers;
}

SharedMediaClock MediaClock::masterClock()
{
	return m_master;
}

void MediaClock::setMasterClock(SharedMediaClock masterClock)
{
	MediaTime currentTime=internalTime();
	MediaTime masterTime=masterClock->internalTime();

	m_master=masterClock;
	
	//reset calibration
	m_markerSize=0;
	setMasterCalibration(currentTime, resolution(), m_master->resolution(), masterTime);
}

MediaTime MediaClock::time()
{
	MediaTime currentTime=internalTime();
	
	if(m_master == SharedMediaClock())
		return currentTime;
	
	return convertToMasterTime(currentTime);
}

MediaTime MediaClock::convertToMasterTime(MediaTime internalTime)
{
	MediaTime newTime;
	
	if(internalTime >= m_localOffset)
	{
		newTime=internalTime-m_localOffset;
//		newTime*=(m_scaleNum/m_scaleDen);
		newTime=utilities_uint64_scale(newTime, m_scaleNum, m_scaleDen);
		newTime+=m_masterOffset;
	}
	else
	{
		newTime=m_localOffset - internalTime;
//		newTime*=(m_scaleNum/m_scaleDen);
		newTime=utilities_uint64_scale(newTime, m_scaleNum, m_scaleDen);

		if(m_masterOffset > newTime)
			newTime=m_masterOffset-newTime;
		else
			newTime = 0;
	}
	return newTime;
}

MediaTime MediaClock::convertToInternalTime(MediaTime masterTime)
{
	MediaTime newTime;

	if(masterTime >= m_masterOffset)
	{
		newTime=masterTime-m_masterOffset;
//		newTime*=(m_scaleDen/m_scaleNum);
		newTime=utilities_uint64_scale(newTime, m_scaleDen, m_scaleNum);
		newTime+=m_localOffset;
	}
	else
	{
		newTime=m_masterOffset - masterTime;
//		newTime*=(m_scaleDen/m_scaleNum);
		newTime=utilities_uint64_scale(newTime, m_scaleDen, m_scaleNum);

		if(m_localOffset > newTime)
			newTime=m_localOffset-newTime;
		else
			newTime = 0;
	}
	return newTime;
}

void MediaClock::addMarker()
{
	if(m_master != SharedMediaClock())
		addMarker(internalTime(), m_master->time());
}

void MediaClock::addMarker(MediaTime masterTime)
{
	addMarker(internalTime(), masterTime);
}

void MediaClock::addMarker(MediaTime internalTime, MediaTime masterTime)
{
	if(m_markerNext >= m_markerWindowSize)
		m_markerNext=0;

	int index=m_markerNext*4;

	m_timeMarkers[index]=internalTime;
	m_timeMarkers[index+1]=masterTime;
	m_markerNext++;

	if(m_markerSize < m_markerWindowSize)
		m_markerSize++;

//	if(m_markerSize >= m_markerWindowSize)
//		m_markerSize=0;
//	
//	int index=m_markerSize*4;
//	
//	m_timeMarkers[index]=internalTime;
//	m_timeMarkers[index+1]=masterTime;
//	m_markerSize++;
//
//	if(m_markerSize == m_markerWindowSize)
	if(m_markerCalibrateCount >= 10)
	{
		m_markerCalibrateCount=0;
		MediaTime localOffset, scaleNum, scaleDen, masterOffset;

		double correlation=leastSquaresFit(localOffset, scaleNum, scaleDen, masterOffset);

		if(correlation != 0.0)
			setMasterCalibration(localOffset, scaleNum, scaleDen, masterOffset);
	}
	m_markerCalibrateCount++;
}

void MediaClock::setMasterCalibration(const MediaTime &localOffset, const MediaTime &scaleNum, const MediaTime &scaleDen, const MediaTime &masterOffset)
{
	m_localOffset=localOffset;
	m_scaleNum=scaleNum;
	m_scaleDen=scaleDen;
	m_masterOffset=masterOffset;
}

double MediaClock::leastSquaresFit(MediaTime &localOffset, MediaTime &scaleNum, MediaTime &scaleDen, MediaTime &masterOffset)
{
	if(m_markerSize < 4) //need more for calculation
		return 0.0;

	MediaTime xmin, ymin;
	MediaTime xbar, ybar;
	MediaTime xbarShift, ybarShift;
	MediaTimeDiff ssxx, ssyy, ssxy;

	xbar=ybar=0;
	ssxx=ssyy=ssxy=0;

	xmin=ymin=std::numeric_limits<MediaTime>::max();
	for(int i=0, j=0; i<m_markerSize; i++, j+=4)
	{
		xmin=std::min(xmin, m_timeMarkers[j]);
		ymin=std::min(ymin, m_timeMarkers[j+1]);
	}
	for(int i=0, j=0; i<m_markerSize; i++, j+=4)
	{
		xbar+=m_timeMarkers[j+2]=m_timeMarkers[j]-xmin;
		ybar+=m_timeMarkers[j+3]=m_timeMarkers[j+1]-ymin;
	}
	xbar/=m_markerSize;
	ybar/=m_markerSize;

	xbarShift=xbar>>4;
	ybarShift=ybar>>4;
	for(int i=0, j=2; i<m_markerSize; i++, j+=4)
	{
		MediaTime xshift, yshift;

		xshift=m_timeMarkers[j]>>4;
		yshift=m_timeMarkers[j+1]>>4;

		ssxx+=xshift*xshift;
		ssyy+=yshift*yshift;
		ssxy+=xshift*yshift;
	}
	ssxx-=m_markerSize*xbarShift*xbarShift;
	ssyy-=m_markerSize*ybarShift*ybarShift;
	ssxy-=m_markerSize*xbarShift*ybarShift;

	if(ssxx == 0)
		return 0.0;

	localOffset=xmin;
	masterOffset=(ybar+ymin)-utilities_uint64_scale(xbar, ssxy, ssxx);
	scaleNum=ssxy;
	scaleDen=ssxx;

	return ((double)ssxy*(double)ssxy)/((double)ssxx*(double)ssyy);
}