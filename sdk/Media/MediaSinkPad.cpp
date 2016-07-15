#include "Media/MediaSinkPad.h"
#include "Media/IMediaFilter.h"
#include <boost/foreach.hpp>

//SharedMediaFormat MediaSinkPad::accept(SharedMediaPad sourcePad, SharedMediaFormat format)
//{}
//
//SharedMediaFormat MediaSinkPad::onAccept(SharedMediaPad sourcePad, SharedMediaFormat format)
//{
//	SharedMediaFormats sourceMediaFormats=sourcePad->queryMediaFormats();
//	SharedMediaFormats sinkMediaFormats=queryMediaFormats();
//
//	if(format == SharedMediaFormat())
//	{
//		BOOST_FOREACH(SharedMediaFormat &sourceFormat, sourceMediaFormats)
//		{
//			BOOST_FOREACH(SharedMediaFormat &sinkFormat, sinkMediaFormats)
//			{
//				if(sourceFormat == sinkFormat)
//				{
//					return sourceFormat;
//				}
//			}
//		}
//	}
//	else
//	{
//		BOOST_FOREACH(SharedMediaFormat &sourceFormat, sourceMediaFormats)
//		{
//			if(sourceFormat != format)
//				continue;
//
//			BOOST_FOREACH(SharedMediaFormat &sinkFormat, sinkMediaFormats)
//			{
//				if(sinkFormat == format)
//				{
//					return sourceFormat;
//				}
//			}
//		}
//	}
//	return SharedMediaFormat();
//}
//
//bool MediaSinkPad::link(SharedMediaPad sourcePad);
//{
//	if(m_state == INIT)
//	{
//		SharedMediaFormat acceptedFormat=accept(sourcePad, format);
//
//		if(acceptedFormat != SharedMediaFormat())
//		{
//			m_linkedSourcePad=sourcePad;
//			m_state=LINKED;
//			return true;
//		}
//	}
//	return false;
//}
//
//SharedMediaFormat MediaSinkPad::connect(SharedMediaPad sourcePad, SharedMediaFormat format)
//{
//	if(m_state == INIT)
//		link(sourcePad);
//
//	if(m_state == LINKED)
//	{
//		SharedMediaFormat acceptedFormat=parent()->connectPad(share_from_this(), sourcePad);
//
//		if(acceptedFormat != SharedMediaFormat())
//		{
//			m_state=CONNECTED;
//		}
//		return acceptedFormat;
////		SharedMediaFormat acceptedFormat=accept(sourcePad, format);
////		
////		if(acceptedFormat != SharedMediaFormat())
////			m_connectedSourcePad=sourcePad;
////		return acceptedFormat;
//	}
//}

//bool MediaSinkPad::onAccept(SharedMediaFormat format)
//{
////	SharedMediaFormats sourceMediaFormats=sourcePad->queryMediaFormats();
//	SharedMediaFormats sinkMediaFormats=queryMediaFormats();
//
//	BOOST_FOREACH(SharedMediaFormat &sinkFormat, sinkMediaFormats)
//	{
//		if(sinkFormat->match(format))
//			return true;
//	}
//	return false;
//}

//bool MediaSinkPad::onLink(SharedMediaPad mediaPad, SharedMediaFormat format)
//{
//	SharedMediaFormats mediaFormats=queryMediaFormats();
//
//	BOOST_FOREACH(SharedMediaFormat &mediaFormat, mediaFormats)
//	{
//		if(mediaPad->accept(mediaFormat))
//			return true;
//	}
//	return false;
//}
//
//SharedMediaFormat MediaSinkPad::onConnect(SharedMediaPad mediaPad, SharedMediaFormat format)
//{
//	SharedMediaFormats sourceMediaFormats=mediaPad->queryMediaFormats();
//	SharedMediaFormats sinkMediaFormats=queryMediaFormats();
//
//	if(format == SharedMediaFormat())
//	{
//		BOOST_FOREACH(SharedMediaFormat &sourceFormat, sourceMediaFormats)
//		{
//			BOOST_FOREACH(SharedMediaFormat &sinkFormat, sinkMediaFormats)
//			{
//				if(sourceFormat == sinkFormat)
//				{
//					return sourceFormat;
//				}
//			}
//		}
//	}
//	else
//	{
//		BOOST_FOREACH(SharedMediaFormat &sourceFormat, sourceMediaFormats)
//		{
//			if(sourceFormat != format)
//				continue;
//
//			BOOST_FOREACH(SharedMediaFormat &sinkFormat, sinkMediaFormats)
//			{
//				if(sinkFormat == format)
//				{
//					return sinkFormat;
//				}
//			}
//		}
//	}
//	return SharedMediaFormat();
//}
//
//void MediaSinkPad::disconnect()
//{
//	m_connectedSourcePad=SharedMediaPad();
//}
//
//bool MediaSinkPad::connected()
//{
//	if(m_connectedSourcePad != SharedMediaPad())
//		return true;
//	return false;
//}

//bool MediaSinkPad::processSample(SharedMediaSample mediaSample)
//{
//	return parent()->processSample(shared_from_this(), mediaSample);
//}