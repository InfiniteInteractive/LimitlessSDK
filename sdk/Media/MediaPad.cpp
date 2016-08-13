#include "MediaPad.h"
#include "IMediaFilter.h"
#include "MediaFormat.h"
#include "boost/foreach.hpp"

using namespace Limitless;

void MediaPad::addMediaFormat(SharedMediaFormat format)
{
	m_mediaFormats.push_back(format);
}

void MediaPad::addMediaFormat(std::string jsonFormat)
{
	SharedMediaFormat format(new MediaFormat());

	format->addMediaAttributes(jsonFormat);
	m_mediaFormats.push_back(format);
}

void MediaPad::addMediaFormat(Unserializer *unserializer)
{
    SharedMediaFormat format(new MediaFormat());

    format->addMediaAttributes(unserializer);
    m_mediaFormats.push_back(format);
}

//void MediaPad::addMediaFormat(QVariant *jsonVariant)
//{
//	SharedMediaFormat format(new MediaFormat());
//
//	format->addMediaAttributes(jsonVariant);
//	m_mediaFormats.push_back(format);
//}

void MediaPad::removeAllMediaFormats()
{
	m_mediaFormats.clear();
}

SharedMediaFormats MediaPad::queryMediaFormats()
{
	if(m_format != SharedMediaFormat())
	{
		SharedMediaFormats mediaFormats;

		mediaFormats.push_back(m_format);
		return mediaFormats;
	}
	return m_mediaFormats;
}

SharedMediaFormat MediaPad::format()
{
	return m_format;
}

bool MediaPad::setFormat(MediaFormat format)
{
	if(m_format == SharedMediaFormat())
		m_format.reset(new MediaFormat);

	*m_format=format;
//	m_state=LINK_CHANGED;

	//need to relink pad
	if(m_linkedPad != SharedMediaPad())
		return link(m_linkedPad, m_format);
	return true;
}

bool MediaPad::accept(SharedMediaFormat format)
{
	return parent()->onAcceptMediaFormat(shared_from_this(), format);
}

bool MediaPad::link(SharedMediaPad mediaPad)
{
    if(mediaPad->linked())
        return false;

    SharedMediaFormats mediaFormats=queryMediaFormats();

    BOOST_FOREACH(SharedMediaFormat &mediaFormat, mediaFormats)
    {
        DEBUG_MEDIA_FORMAT(mediaFormat);

        if(!mediaPad->accept(mediaFormat))
            continue;

        if(link(mediaPad, mediaFormat))
            return true;
    }
    return false;
}

bool MediaPad::link(SharedMediaPad mediaPad, SharedMediaFormat format)
{
	if(!accept(format))
		return false;
	if(!mediaPad->accept(format))
		return false;

	m_linkedPad=mediaPad;
	m_format=format;
	m_state=LINKED;

	if(mediaPad->m_format == SharedMediaFormat())
		mediaPad->m_format.reset(new MediaFormat());

	mediaPad->m_linkedPad=shared_from_this();
	*mediaPad->m_format=*format;
	mediaPad->m_state=LINKED;
	
	parent()->linked(shared_from_this(), mediaPad);
	mediaPad->parent()->linked(mediaPad, shared_from_this());
	mediaPad->parent()->onLinkFormatChanged(mediaPad, format);
	
	return true;
}

bool MediaPad::link(boost::shared_ptr<IMediaFilter> sinkFilter)
{
	SharedMediaFormats mediaFormats=queryMediaFormats();
	SharedMediaPads sinkPads=sinkFilter->getSinkPads();

	BOOST_FOREACH(SharedMediaFormat &mediaFormat, mediaFormats)
	{
		DEBUG_MEDIA_FORMAT(mediaFormat);
		BOOST_FOREACH(SharedMediaPad sinkPad, sinkPads)
		{
			if(sinkPad->linked())
				continue;

			if(!sinkPad->accept(mediaFormat))
				continue;
		
			if(link(sinkPad, mediaFormat))
				return true;
		}
	}
	return false;
}


bool MediaPad::relink()
{
	if(m_linkedPad != SharedMediaPad())
	{
		SharedMediaPad linkedPad=m_linkedPad;

		disconnect();
		return link(linkedPad, m_format);
	}
	return true;
}

void MediaPad::disconnect()
{
	SharedMediaPad pad=m_linkedPad;

	m_format.reset();
	m_linkedPad.reset();
	m_state=INIT;

	if(pad != SharedMediaPad())
	{
		parent()->disconnected(shared_from_this(), pad);
		pad->parent()->disconnected(pad, shared_from_this());
	}
}

SharedMediaFilters MediaPad::findUpStream(std::string typeName)
{
	SharedMediaFilters filters;

	if(linked())
	{
		SharedMediaFilter parentFilter=linkedPad()->parent();

		if(parentFilter == SharedMediaFilter())
			return filters;;

		if(parentFilter->typeName() == typeName)
			filters.push_back(parentFilter);
			
		SharedMediaFilters childFilters=parentFilter->findUpStream(typeName);

		if(!childFilters.empty())
			filters.insert(filters.end(), childFilters.begin(), childFilters.end());
	}

	return filters;
}

SharedMediaFilters MediaPad::findDownStream(std::string typeName)
{
	SharedMediaFilters filters;

	if(linked())
	{
		SharedMediaFilter parentFilter=linkedPad()->parent();

		if(parentFilter == SharedMediaFilter())
			return filters;

		if(parentFilter->typeName() == typeName)
			filters.push_back(parentFilter);
			
		SharedMediaFilters childFilters=parentFilter->findDownStream(typeName);

		if(!childFilters.empty())
			filters.insert(filters.end(), childFilters.begin(), childFilters.end());
	}
	return filters;
}

SharedMediaFilters MediaPad::findUpStream(FilterType filterType)
{
	SharedMediaFilters filters;

	if(linked())
	{
		SharedMediaFilter parentFilter=linkedPad()->parent();

		if(parentFilter == SharedMediaFilter())
			return filters;;

		if(parentFilter->type() == filterType)
			filters.push_back(parentFilter);

		SharedMediaFilters childFilters=parentFilter->findUpStream(filterType);

		if(!childFilters.empty())
			filters.insert(filters.end(), childFilters.begin(), childFilters.end());
	}

	return filters;
}

SharedMediaFilters MediaPad::findDownStream(FilterType filterType)
{
	SharedMediaFilters filters;

	if(linked())
	{
		SharedMediaFilter parentFilter=linkedPad()->parent();

		if(parentFilter == SharedMediaFilter())
			return filters;

		if(parentFilter->type() == filterType)
			filters.push_back(parentFilter);

		SharedMediaFilters childFilters=parentFilter->findDownStream(filterType);

		if(!childFilters.empty())
			filters.insert(filters.end(), childFilters.begin(), childFilters.end());
	}
	return filters;
}

bool MediaPad::onAccept(SharedMediaFormat format)
{
	if(m_format != SharedMediaFormat())
	{
		return m_format->match(format);
	}
	else
	{
		SharedMediaFormats mediaFormats=queryMediaFormats();

		BOOST_FOREACH(SharedMediaFormat &mediaFormat, mediaFormats)
		{
			if(mediaFormat->match(format))
				return true;
		}
	}
	return false;
}

bool MediaPad::processSample(SharedMediaSample mediaSample)
{
	mediaSample->touch(parent()->instance());
//	OutputDebugStringA((boost::format("****Sending Sample 0x%08x to %s\n")%mediaSample.get()%parent()->instance()).str().c_str());
	return parent()->processSample(shared_from_this(), mediaSample);
}

std::string MediaPad::typeString(Type type)
{
	std::string name="Unknown";

	switch(type)
	{
	case SOURCE:
		name="Source";
		break;
	case SINK:
		name="Sink";
		break;
	default:
		break;
	}

	return name;
}

std::string MediaPad::stateString(State state)
{
	std::string stateName="Unknown";

	switch(state)
	{
	case INIT:
		stateName="Initialize";
		break;
	case LINKED:
		stateName="Linked";
		break;
	case LINK_CHANGED:
		stateName="Link changed";
		break;
	case PROCESSING:
		stateName="Processing";
		break;
	default:
		break;
	}

	return stateName;
}