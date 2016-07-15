#include "Media/MediaFilterContainer.h"
#include "Base/Log.h"
#include "Media/MediaPluginFactory.h"

#include "boost/foreach.hpp"
#include "boost/format.hpp"

using namespace Limitless;

bool MediaFilterContainer::addMediaFilter(SharedMediaFilter mediaFilter)
{
	m_mediaFilters.push_back(mediaFilter);

	//all filters are sources and sinks till linked
	m_sourceFilters.push_back(mediaFilter);
	m_sinkFilters.push_back(mediaFilter);

	mediaFilter->setParent(shared_from_this());

	//bring new filter up to speed on current state
	IMediaFilter::FilterState currentState=getState();

	if(mediaFilter->setState(currentState) == IMediaFilter::FAILED) //failed to add in current state
	{
		Log::warning("MediaFilterContainer", (boost::format("Failed to add %s to %s in current state. State:%s:%s")%mediaFilter->instance()%instance()%stateName(currentState)%stateName(mediaFilter->getState())).str());
		return false;
	}

	return onAddMediaFilter(mediaFilter);
}

bool MediaFilterContainer::removeMediaFilter(SharedMediaFilter mediaFilter)
{
	onRemoveMediaFilter(mediaFilter);

	//about to remove so revert current state
	mediaFilter->setState(INIT);

	SharedMediaFilters::iterator iter=std::find(m_mediaFilters.begin(), m_mediaFilters.end(), mediaFilter);
	
	if(iter != m_mediaFilters.end())
		m_mediaFilters.erase(iter);

	iter=std::find(m_sourceFilters.begin(), m_sourceFilters.end(), mediaFilter);

	if(iter != m_sourceFilters.end())
		m_sourceFilters.erase(iter);

	iter=std::find(m_sinkFilters.begin(), m_sinkFilters.end(), mediaFilter);

	if(iter != m_sinkFilters.end())
		m_sinkFilters.erase(iter);

	return true;
}

SharedMediaFilters MediaFilterContainer::mediaFilters()
{
	SharedMediaFilters mediaFilters;

	BOOST_FOREACH(SharedMediaFilter mediaFilter, m_mediaFilters)
	{
		mediaFilters.push_back(mediaFilter);
	}
	return mediaFilters;
}

SharedMediaFilter MediaFilterContainer::mediaFilter(std::string instance)
{
	BOOST_FOREACH(SharedMediaFilter mediaFilter, m_mediaFilters)
	{
		if(mediaFilter == SharedMediaFilter())
			continue;

		if(mediaFilter->instance() == instance)
			return mediaFilter;
	}
	return SharedMediaFilter();
}

void MediaFilterContainer::clear()
{
	SharedMediaFilters localMediaFilters=mediaFilters();

	for(SharedMediaFilter &mediaFilter:localMediaFilters)
		removeMediaFilter(mediaFilter);
}
//void MediaFilterContainer::setSourceFilter(SharedMediaFilter mediaFilter)
//{
//	if(mediaFilter == SharedMediaFilter())
//	{
//		m_mediaSourcePad.reset();
//		m_sourceFilter.reset();
//		return;
//	}
//
//	SharedMediaFilters::iterator iter=std::find(m_mediaFilters.begin(), m_mediaFilters.end(), mediaFilter);
//	
//	if(iter == m_mediaFilters.end())
//		m_mediaFilters.push_back(mediaFilter);
//
//	m_sourceFilter=mediaFilter;
//	SharedMediaPads mediaPads=m_sourceFilter->getSourcePads();
//
//	BOOST_FOREACH(SharedMediaPad mediaPad, mediaPads)
//	{
//		SharedMediaSourcePad mediaSourcePad=boost::dynamic_pointer_cast<MediaSourcePad>(mediaPad);
//
//		if(mediaSourcePad != SharedMediaSourcePad())
//		{
//			if(mediaSourcePad->linked())
//			{
//				m_mediaSourcePad=mediaSourcePad;
//				break;
//			}
//		}
//	}
//}
//
//void MediaFilterContainer::setSinkFilter(SharedMediaFilter mediaFilter)
//{
//	SharedMediaFilters::iterator iter=std::find(m_mediaFilters.begin(), m_mediaFilters.end(), mediaFilter);
//	
//	if(iter == m_mediaFilters.end())
//		m_mediaFilters.push_back(mediaFilter);
//
//	m_sinkFilter=mediaFilter;
//}

bool MediaFilterContainer::link(SharedMediaFilter source, SharedMediaFilter sink)
{
	size_t sampleType=-1;
	std::vector<MediaSample *> sourceSamples;

	bool linked=source->link(sink);

	if(linked)
	{
		//remove source filter from sinkFilters as now it is only a source
		SharedMediaFilters::iterator iter=std::find(m_sinkFilters.begin(), m_sinkFilters.end(), source);

		if(iter != m_sinkFilters.end())
			m_sinkFilters.erase(iter);

		//remove sink filter from sourceFilters as now it is only a sink
		iter=std::find(m_sourceFilters.begin(), m_sourceFilters.end(), sink);

		if(iter != m_sourceFilters.end())
			m_sourceFilters.erase(iter);
	}

	return linked;

//	if(format != SharedMediaFormat())
//	{
//		//need to check current source and make sure it is not down stream
//		if(m_sourceFilter == SharedMediaFilter())
//			m_sourceFilter=source;
//		else
//		{
//			SharedMediaFilters upStreamFilters=m_sourceFilter->upStreamFilters();
//			
//			while(!upStreamFilters.empty())
//			{
//				m_sourceFilter=upStreamFilters
//			}
//		}
//	}
//	return format;
}

bool MediaFilterContainer::link(boost::shared_ptr<IMediaFilter> sinkFilter)
{
	bool linked=false;

	BOOST_FOREACH(SharedMediaFilter &filter, m_sinkFilters)
	{
		if(filter->link(sinkFilter))
		{
			linked=true;
			break;
		}
	}
	return linked;
}

SharedMediaFilters MediaFilterContainer::findUpStream(std::string typeName)
{
	SharedMediaFilters filters;

	for(SharedMediaFilter &sourceFilter:m_sourceFilters)
	{
		SharedMediaFilters childFilters=sourceFilter->findUpStream(typeName);

		if(!childFilters.empty())
			filters.insert(filters.end(), childFilters.begin(), childFilters.end());
	}

	return filters;
}

SharedMediaFilters MediaFilterContainer::findUpStream(FilterType filterType)
{
	SharedMediaFilters filters;

	for(SharedMediaFilter &sourceFilter:m_sourceFilters)
	{
		SharedMediaFilters childFilters=sourceFilter->findUpStream(filterType);

		if(!childFilters.empty())
			filters.insert(filters.end(), childFilters.begin(), childFilters.end());
	}

	return filters;
}

SharedMediaFilters MediaFilterContainer::findDownStream(std::string typeName)
{
	SharedMediaFilters filters;

	for(SharedMediaFilter &sourceFilter:m_sourceFilters)
	{
		SharedMediaFilters childFilters=sourceFilter->findDownStream(typeName);

		if(!childFilters.empty())
			filters.insert(filters.end(), childFilters.begin(), childFilters.end());
	}

	return filters;
}

SharedMediaFilters MediaFilterContainer::findDownStream(FilterType filterType)
{
	SharedMediaFilters filters;

	for(SharedMediaFilter &sourceFilter:m_sourceFilters)
	{
		SharedMediaFilters childFilters=sourceFilter->findDownStream(filterType);

		if(!childFilters.empty())
			filters.insert(filters.end(), childFilters.begin(), childFilters.end());
	}

	return filters;
}

IMediaFilter::StateChange MediaFilterContainer::onReady()
{
	StateChange stateChange;
//	m_sourceFilter->ready();
	BOOST_FOREACH(SharedMediaFilter mediaFilter, m_mediaFilters)
	{
		stateChange=mediaFilter->ready();
		if(stateChange != SUCCESS)
			return stateChange;
	}
	return SUCCESS;
//	m_sinkFilter->ready();
}

IMediaFilter::StateChange MediaFilterContainer::onPaused()
{
	StateChange stateChange;
//	m_sourceFilter->pause();
	BOOST_FOREACH(SharedMediaFilter mediaFilter, m_mediaFilters)
	{
		stateChange=mediaFilter->pause();
	}
	return SUCCESS;
//	m_sinkFilter->pause();
}

IMediaFilter::StateChange MediaFilterContainer::onPlaying()
{
	StateChange stateChange;
//	m_sourceFilter->play();
	BOOST_FOREACH(SharedMediaFilter mediaFilter, m_mediaFilters)
	{
		stateChange=mediaFilter->play();
	}
	return SUCCESS;
//	m_sinkFilter->play();
}

bool MediaFilterContainer::processSample(SharedMediaPad sinkPad, SharedMediaSample sample)
{
	//Container holds a source filter
	if(sinkPad == SharedMediaPad())
	{
		for(SharedMediaFilter &sourceFilter:m_sourceFilters)
		{
			if(sourceFilter->type() == Source)
				sourceFilter->processSample(sinkPad, sample);
		}
//		m_sourceFilter->processSample(sinkPad, sample);
	}
	else //push sample to source filter sink pad
	{
	}
	//process each filter then return
	return false;
}

SharedMediaPads MediaFilterContainer::getSourcePads()
{
	SharedMediaPads pads;

	BOOST_FOREACH(SharedMediaFilter &filter, m_sinkFilters)
	{
		SharedMediaPads filterPads=filter->getSourcePads();

		BOOST_FOREACH(SharedMediaPad &mediaPad, filterPads)
		{
			pads.push_back(mediaPad);
		}
	}
	return pads;
}

SharedMediaPads MediaFilterContainer::getSinkPads()
{
	SharedMediaPads pads;

	BOOST_FOREACH(SharedMediaFilter &filter, m_sourceFilters)
	{
		SharedMediaPads filterPads=filter->getSinkPads();

		BOOST_FOREACH(SharedMediaPad &mediaPad, filterPads)
		{
			pads.push_back(mediaPad);
		}
	}
	return pads;
}

void MediaFilterContainer::serialize(Serializer *serializer)
{
	serializer->addKey("type");
	serializer->addString("container");

	serializer->addKey("instance");
	serializer->addString(instance());

	serializer->addKey("plugins");
	serializer->startArray();

	for(SharedMediaFilter &mediaFilter:m_mediaFilters)
		mediaFilter->serialize(serializer);

	serializer->endArray();

	AttributeContainer::serialize(serializer);
}

void MediaFilterContainer::unserialize(Unserializer *unserializer)
{
	if(unserializer->key("plugins") && unserializer->type() == Type::ARRAY)
	{
		struct PluginLink
		{
			std::string instance;
			std::string pad;
			std::string linkedFilter;
			std::string linkedPad;
		};

		std::vector<PluginLink> pluginLinks;

		if(unserializer->openArray())
		{
			while(unserializer->openObject())
			{
				if(!unserializer->key("type"))
				{
					unserializer->closeObject();
					continue;
				}
				std::string type=unserializer->getString();

				if(!unserializer->key("instance"))
				{
					unserializer->closeObject();
					continue;
				}
				std::string instance=unserializer->getString();

				SharedMediaFilter mediaFilter=MediaPluginFactory::create(type, instance);

				mediaFilter->unserialize(unserializer);
				addMediaFilter(mediaFilter);

				if(unserializer->key("links") && unserializer->type() == Type::ARRAY)
				{
					if(unserializer->openArray())
					{
						while(unserializer->openObject())
						{
							PluginLink link;

							if(!unserializer->key("padName"))
							{
								unserializer->closeObject();
								continue;
							}
							link.pad=unserializer->getString();
							if(!unserializer->key("linkedFilter"))
							{
								unserializer->closeObject();
								continue;
							}
							link.linkedFilter=unserializer->getString();
							if(!unserializer->key("linkedPad"))
							{
								unserializer->closeObject();
								continue;
							}
							link.linkedPad=unserializer->getString();
							link.instance=instance;

							pluginLinks.push_back(link);
							unserializer->closeObject();
						}
						unserializer->closeArray();
					}
				}

				unserializer->closeObject();
			}
			unserializer->closeArray();
		}

		//linkely need to re-order from source to sink then link in that order
		if(!pluginLinks.empty())
		{
			for(PluginLink &pluginLink:pluginLinks)
			{
				SharedMediaFilter filter=mediaFilter(pluginLink.instance);

				if(filter != SharedMediaFilter())
				{
					SharedMediaFilter linkedFilter=mediaFilter(pluginLink.linkedFilter);

					if(linkedFilter != SharedMediaFilter())
						link(filter,linkedFilter);
				}
			}
		}
	}

	AttributeContainer::unserialize(unserializer);
}