#include "Media/IMediaFilter.h"
#include "Media/MediaPad.h"
#include "Media/MediaSampleFactory.h"
#include "Media/MediaPluginFactory.h"
#include "Media/MediaSourcePad.h"
#include "Media/SystemMediaClock.h"
#include "Media/MediaSampleFlags.h"

#include "Base/JsonSerializer.h"

#include "boost/foreach.hpp"
#include <boost/tokenizer.hpp>

//#include "Base/json.h"
//#include <QtCore/QStringList>
#include "rapidjson/document.h"

namespace Limitless
{

IMediaFilter::IMediaFilter(std::string instance, SharedMediaFilter parent):
PluginObject(instance), 
m_parent(parent),
m_state(INIT),
m_sampleBin(0)
{
	m_baseClasses.push_back("IMediaFilter");
}

//IMediaFilter::~IMediaFilter()
//{
//	MediaPluginFactory::removeFilter(this);
//}

void IMediaFilter::addSourcePad(SharedMediaPad mediaPad)
{
	m_mediaSourcePads.push_back(mediaPad);
}

SharedMediaPad IMediaFilter::addSourcePad(std::string name, const char *jsonFormat)
{
	return addSourcePad(name, std::string(jsonFormat));
}

SharedMediaPad IMediaFilter::addSourcePad(std::string name, const std::string &jsonFormat)
{
    JsonUnserializer unserializer;

    unserializer.parse(jsonFormat);

    if(unserializer.type() == Type::ARRAY)
    {
        SharedMediaPad mediaPad(new MediaSourcePad(name, shared_from_this()));

        unserializer.openArray();

        while(unserializer.openObject())
        {
            mediaPad->addMediaFormat(&unserializer);
			unserializer.closeObject();
        }
        m_mediaSourcePads.push_back(mediaPad);

		unserializer.closeArray();

        return mediaPad;
    }
    else if(unserializer.type()==Type::OBJECT)
    {
		unserializer.openObject();

        if(unserializer.key("formats"))
        {
            if(unserializer.type() == Type::ARRAY)
            {
				unserializer.openArray();

                SharedMediaPad mediaPad(new MediaSourcePad(name, shared_from_this()));

                while(unserializer.openObject())
                {
                    mediaPad->addMediaFormat(&unserializer);
					unserializer.closeObject();
                }
                m_mediaSourcePads.push_back(mediaPad);

				unserializer.closeArray();
            }
        }
        else
        {
            SharedMediaPad mediaPad(new MediaSourcePad(name, shared_from_this()));

            mediaPad->addMediaFormat(&unserializer);
            m_mediaSourcePads.push_back(mediaPad);
        }

		unserializer.closeObject();
    }

//	rapidjson::Document document;
//
//	document.Parse(jsonFormat);
//
//	if(document.IsArray())
//	{ 
//		SharedMediaPad mediaPad(new MediaSourcePad(name, shared_from_this()));
//
//		for(rapidjson::Value::ConstMemberIterator iter=document.MemberBegin(); iter!=document.MemberEnd(); ++iter)
//			mediaPad->addMediaFormat(iter->value.GetString());
//		m_mediaSourcePads.push_back(mediaPad);
//
//		return mediaPad;
//	}
//	else if(document.IsObject())
//	{ 
//		if(document.HasMember("formats"))
//		{
//			rapidjson::Value &value=document["formats"];
//
//			if(value.IsArray())
//			{
//				SharedMediaPad mediaPad(new MediaSourcePad(name, shared_from_this()));
//
//				for(rapidjson::Value::ConstMemberIterator iter=value.MemberBegin(); iter!=value.MemberEnd(); ++iter)
//					mediaPad->addMediaFormat(iter->value.GetString());
//				m_mediaSourcePads.push_back(mediaPad);
//			}
//		}
//		else
//		{
//			SharedMediaPad mediaPad(new MediaSourcePad(name, shared_from_this()));
//
//			mediaPad->addMediaFormat(jsonFormat);
//			m_mediaSourcePads.push_back(mediaPad);
//		}
//	}

	return SharedMediaPad();
}


//SharedMediaPad IMediaFilter::addSourcePad(std::string name, const std::string &jsonFormat)
//{
//	QVariant jsonVariant=QtJson::parse(QString::fromStdString(jsonFormat));
//
//	if(jsonVariant.type() == QVariant::List)
//	{
//		SharedMediaPad mediaPad(new MediaSourcePad(name, shared_from_this()));
//		QVariantList formats=jsonVariant.toList();
//
//		for(int i=0; i<formats.size(); ++i)
//		{
//			mediaPad->addMediaFormat(&formats[i]);
//		}
//		m_mediaSourcePads.push_back(mediaPad);
//
//		return mediaPad;
//	}
//	else if(jsonVariant.type() == QVariant::Map)
//	{
//		QVariantMap variantMap=jsonVariant.toMap();
//		QStringList keys=variantMap.keys();
//
//		if(keys.size() > 0)
//		{
//			if(keys[0] == "formats")
//			{
//				SharedMediaPad mediaPad(new MediaSourcePad(name, shared_from_this()));
//				QVariant &formatsVariant=variantMap["formats"];
//
//				if(formatsVariant.type() == QVariant::List)
//				{
//					QVariantList formats=formatsVariant.toList();
//
//					for(int i=0; i<formats.size(); ++i)
//					{
//						mediaPad->addMediaFormat(&formats[i]);
//					}
//					m_mediaSourcePads.push_back(mediaPad);
//				}
//				return mediaPad;
//			}
//			else
//			{
//				SharedMediaPad mediaPad(new MediaSourcePad(name, shared_from_this()));
//
//				mediaPad->addMediaFormat(&jsonVariant);
//				m_mediaSourcePads.push_back(mediaPad);
//				return mediaPad;
//			}
//		}
//	}
//
//	return SharedMediaPad();
////	mediaPad->addMediaFormat(jsonFormat);
////	m_mediaSourcePads.push_back(mediaPad);
//}

SharedMediaPad IMediaFilter::addSourcePad(std::string name, SharedMediaFormat format)
{
	SharedMediaPad mediaPad(new MediaSourcePad(name, shared_from_this()));

	mediaPad->addMediaFormat(format);
	m_mediaSourcePads.push_back(mediaPad);
	return mediaPad;
}

void IMediaFilter::removeSourcePad(SharedMediaPad mediaPad)
{
	SharedMediaPads::iterator iter=std::find(m_mediaSourcePads.begin(), m_mediaSourcePads.end(), mediaPad);

	if(iter != m_mediaSourcePads.end())
		m_mediaSourcePads.erase(iter);
}

SharedMediaPads IMediaFilter::getSourcePads()
{
	return m_mediaSourcePads;
}

size_t IMediaFilter::sourcePads()
{
	return m_mediaSourcePads.size();
}

void IMediaFilter::addSinkPad(SharedMediaPad mediaPad)
{
	m_mediaSinkPads.push_back(mediaPad);
}

SharedMediaPad IMediaFilter::addSinkPad(std::string name, const char *jsonFormat)
{
	return addSinkPad(name, std::string(jsonFormat));
}

SharedMediaPad IMediaFilter::addSinkPad(std::string name, const std::string &jsonFormat)
{
	JsonUnserializer unserializer;

	unserializer.parse(jsonFormat);

	Type type=unserializer.type();

	if(unserializer.type() == Type::ARRAY)
	{
		unserializer.openArray();

		SharedMediaPad mediaPad(new MediaSinkPad(name, shared_from_this()));

		while(unserializer.openObject())
		{
			mediaPad->addMediaFormat(&unserializer);
			unserializer.closeObject();
		}

		unserializer.closeArray();
		m_mediaSinkPads.push_back(mediaPad);

		return mediaPad;
	}
	else if(unserializer.type()==Type::OBJECT)
	{
		unserializer.openObject();

		if(unserializer.key("formats"))
		{
			if(unserializer.type()==Type::ARRAY)
			{
				unserializer.openArray();
				SharedMediaPad mediaPad(new MediaSinkPad(name, shared_from_this()));
				
				while(unserializer.openObject())
				{
					mediaPad->addMediaFormat(&unserializer);
					unserializer.closeObject();
				}

				unserializer.closeArray();
				m_mediaSinkPads.push_back(mediaPad);
			}
		}
		else
		{
			SharedMediaPad mediaPad(new MediaSinkPad(name, shared_from_this()));

			mediaPad->addMediaFormat(&unserializer);
			m_mediaSinkPads.push_back(mediaPad);
		}

		unserializer.closeObject();
	}

//	rapidjson::Document document;
//
//    rapidjson::ParseResult result=document.Parse(jsonFormat);
//    
//    if(!result)
//        return SharedMediaPad();
//    
//	if(document.IsArray())
//	{
//		SharedMediaPad mediaPad(new MediaSinkPad(name, shared_from_this()));
//
//		for(rapidjson::Value::ConstValueIterator iter=document.Begin(); iter!=document.End(); ++iter)
//			mediaPad->addMediaFormat(iter->GetString());
//		m_mediaSourcePads.push_back(mediaPad);
//
//		return mediaPad;
//	}
//	else if(document.IsObject())
//	{
//		if(document.HasMember("formats"))
//		{
//			rapidjson::Value &value=document["formats"];
//
//			if(value.IsArray())
//			{
//				SharedMediaPad mediaPad(new MediaSinkPad(name, shared_from_this()));
//
//				for(rapidjson::Value::ConstMemberIterator iter=value.MemberBegin(); iter!=value.MemberEnd(); ++iter)
//					mediaPad->addMediaFormat(iter->value.GetString());
//				m_mediaSourcePads.push_back(mediaPad);
//			}
//		}
//		else
//		{
//			SharedMediaPad mediaPad(new MediaSinkPad(name, shared_from_this()));
//
//			mediaPad->addMediaFormat(jsonFormat);
//			m_mediaSourcePads.push_back(mediaPad);
//		}
//	}

	return SharedMediaPad();
}

//SharedMediaPad IMediaFilter::addSinkPad(std::string name, const std::string &jsonFormat)
//{
////	SharedMediaPad mediaPad(new MediaSinkPad(shared_from_this()));
////
////	mediaPad->addMediaFormat(jsonFormat);
////	m_mediaSinkPads.push_back(mediaPad);
//	QVariant jsonVariant=QtJson::parse(QString::fromStdString(jsonFormat));
//
//	if(jsonVariant.type() == QVariant::List)
//	{
//		SharedMediaPad mediaPad(new MediaSinkPad(name, shared_from_this()));
//		QVariantList formats=jsonVariant.toList();
//
//		for(int i=0; i<formats.size(); ++i)
//		{
//			mediaPad->addMediaFormat(&formats[i]);
//		}
//		m_mediaSinkPads.push_back(mediaPad);
//		return mediaPad;
//	}
//	else if(jsonVariant.type() == QVariant::Map)
//	{
//		QVariantMap variantMap=jsonVariant.toMap();
//		QStringList keys=variantMap.keys();
//
//		if(keys.size() > 0)
//		{
//			if(keys[0] == "formats")
//			{
//				SharedMediaPad mediaPad(new MediaSinkPad(name, shared_from_this()));
//				QVariant &formatsVariant=variantMap["formats"];
//
//				if(formatsVariant.type() == QVariant::List)
//				{
//					QVariantList formats=formatsVariant.toList();
//
//					for(int i=0; i<formats.size(); ++i)
//					{
//						mediaPad->addMediaFormat(&formats[i]);
//					}
//					m_mediaSinkPads.push_back(mediaPad);
//					return mediaPad;
//				}
//			}
//			else
//			{
//				SharedMediaPad mediaPad(new MediaSinkPad(name, shared_from_this()));
//
//				mediaPad->addMediaFormat(&jsonVariant);
//				m_mediaSinkPads.push_back(mediaPad);
//				return mediaPad;
//			}
//		}
//	}
//	return SharedMediaPad();
//}

SharedMediaPad IMediaFilter::addSinkPad(std::string name, SharedMediaFormat format)
{
	SharedMediaPad mediaPad(new MediaSinkPad(name, shared_from_this()));

	mediaPad->addMediaFormat(format);
	m_mediaSinkPads.push_back(mediaPad);
	return mediaPad;
}

void IMediaFilter::removeSinkPad(SharedMediaPad mediaPad)
{
	SharedMediaPads::iterator iter=std::find(m_mediaSinkPads.begin(), m_mediaSinkPads.end(), mediaPad);

	if(iter != m_mediaSinkPads.end())
		m_mediaSinkPads.erase(iter);
}

SharedMediaPads IMediaFilter::getSinkPads()
{
	return m_mediaSinkPads;
}

size_t IMediaFilter::sinkPads()
{
	return m_mediaSinkPads.size();
}

SharedMediaPad IMediaFilter::sourcePad(std::string name)
{
	BOOST_FOREACH(SharedMediaPad pad, m_mediaSourcePads)
	{
		if(pad->name() == name)
			return pad;
	}
	return SharedMediaPad();
}

SharedMediaPad IMediaFilter::sinkPad(std::string name)
{
	BOOST_FOREACH(SharedMediaPad pad, m_mediaSinkPads)
	{
		if(pad->name() == name)
			return pad;
	}
	return SharedMediaPad();
}

size_t IMediaFilter::sourcePadIndex(SharedMediaPad mediaPad)
{
	for(size_t i=0; i<m_mediaSourcePads.size(); ++i)
	{
		if(m_mediaSourcePads[i] == mediaPad)
			return i;
	}
	return InvalidPin;
}

size_t IMediaFilter::sinkPadIndex(SharedMediaPad mediaPad)
{
	for(size_t i=0; i<m_mediaSinkPads.size(); ++i)
	{
		if(m_mediaSinkPads[i] == mediaPad)
			return i;
	}
	return InvalidPin;
}

size_t IMediaFilter::sourcePadIndex(std::string name)
{
	for(size_t i=0; i<m_mediaSourcePads.size(); ++i)
	{
		MediaPad *pad=m_mediaSourcePads[i].get();

		if(pad->name() == name)
			return i;
	}
	return InvalidPin;
}

size_t IMediaFilter::sinkPadIndex(std::string name)
{
	for(size_t i=0; i<m_mediaSinkPads.size(); ++i)
	{
		MediaPad *pad=m_mediaSinkPads[i].get();

		if(pad->name() == name)
			return i;
	}
	return InvalidPin;
}

//bool IMediaFilter::accept(SharedMediaPad mediaPad, SharedMediaFormat mediaFormat)
//{
//	if(mediaPad->type() == MediaPad::SOURCE)
//	{
//		SharedMediaPads sourcePads=getSourcePads();
//
//		BOOST_FOREACH(SharedMediaPad &sourcePad, sourcePads)
//		{
//			if(sourcePad->accept(mediaFormat))
//				return true;
//		}
//	}
//	else
//	{
//		SharedMediaPads sinkPads=getSinkPads();
//
//		BOOST_FOREACH(SharedMediaPad &sinkPad, sinkPads)
//		{
//			if(sinkPad->accept(mediaFormat))
//				return true;
//		}
//	}
//	return true;
//}

//bool IMediaFilter::accept(SharedMediaFilter sink, SharedMediaFormat format)
//{
//	SharedMediaPads sourcePads=getSourcePads();
//
//	if(format != SharedMediaFormat())
//	{
//		BOOST_FOREACH(SharedMediaPad mediaPad, sourcePads)
//		{
//			SharedMediaSourcePad sourcePad=boost::dynamic_pointer_cast<MediaSourcePad>(mediaPad);
//
//			if(sourcePad->accept(format))
//			{
//				SharedMediaPads sinkPads=sink->getSinkPads();
//
//				BOOST_FOREACH(SharedMediaPad sinkPad, sinkPads)
//				{
//					if(sinkPad->accept(format))
//						return true;
//				}
//			}
//		}
//	}
//	else
//	{
//		BOOST_FOREACH(SharedMediaPad mediaPad, sourcePads)
//		{
//			SharedMediaSourcePad sourcePad=boost::dynamic_pointer_cast<MediaSourcePad>(mediaPad);
//			SharedMediaFormats sourceMediaFormats=sourcePad->queryMediaFormats();
//
//			BOOST_FOREACH(SharedMediaFormat sourceMediaFormat, sourceMediaFormats)
//			{
//				SharedMediaPads sinkPads=sink->getSinkPads();
//
//				BOOST_FOREACH(SharedMediaPad sinkPad, sinkPads)
//				{
//					if(sinkPad->accept(sourceMediaFormat))
//						return true;
//				}
//			}
//		}
//	}
//	return false;
//}

bool IMediaFilter::link(boost::shared_ptr<IMediaFilter> sinkFilter)
{
	SharedMediaPads sourcePads=getSourcePads();
	SharedMediaPads sinkPads=sinkFilter->getSinkPads();

	BOOST_FOREACH(SharedMediaPad &sourcePad, sourcePads)
	{
		if(!sourcePad->linked())
		{
			if(sourcePad->link(sinkFilter))
				return true;
//			SharedMediaFormats sourceMediaFormats=sourcePad->queryMediaFormats();
//
//			BOOST_FOREACH(SharedMediaFormat &sourceMediaFormat, sourceMediaFormats)
//			{
//				DEBUG_MEDIA_FORMAT(sourceMediaFormat);
//				BOOST_FOREACH(SharedMediaPad sinkPad, sinkPads)
//				{
//					if(!sinkPad->linked())
//					{
//						if(sinkPad->accept(sourceMediaFormat))
//						{
//							return sourcePad->link(sinkPad, sourceMediaFormat);
//						}
//					}
//				}
//			}
		}
	}
	return false;
}

//SharedMediaFormat IMediaFilter::connect(boost::shared_ptr<IMediaFilter> sink, SharedMediaFormat format)
//{
//	SharedMediaFormat acceptedFormat=accept(sink);
//	SharedMediaPads sourcePads=getSourcePads();
//
//	if(acceptedFormat != SharedMediaFormat())
//	{
//		BOOST_FOREACH(SharedMediaPad sourcePad, sourcePads)
//		{
//			if(!sourcePad->accept(acceptedFormat))
//				continue;
//
//			SharedMediaPads sinkPads=sink->getSinkPads();
//			
//			BOOST_FOREACH(SharedMediaPad sinkPad, sinkPads)
//			{
//				if(!sinkPad->accept(acceptedFormat))
//					continue;
//
//				acceptedFormat=sourcePad->connect(sinkPad, acceptedFormat);
//				return acceptedFormat;
//			}
//		}
//	}
//	return SharedMediaFormat();
//}

//bool IMediaFilter::connectLinkedPads()
//{
//	bool connected=false;
//
//	BOOST_FOREACH(SharedMediaPad &sourceMediaPad, m_mediaSourcePads)
//	{
//		if(!sourceMediaPad->connect())
//			return false;
//		else
//			connected=true;
//	}
//	return connected;
//}

bool IMediaFilter::reLinkPads()
{
	bool linked=true;

	BOOST_FOREACH(SharedMediaPad &sourceMediaPad, m_mediaSourcePads)
	{
		if(sourceMediaPad->linked())
			linked=true;
		else if(sourceMediaPad->linkChanged())
		{
			linked=sourceMediaPad->relink();
			if(!linked)
				return false;
		}
	}
	return linked;
}

bool IMediaFilter::onAcceptMediaFormat(SharedMediaPad pad, SharedMediaFormat format)
{
	if(pad->type() == MediaPad::SOURCE)
	{
		SharedMediaPads::iterator iter=std::find(m_mediaSourcePads.begin(), m_mediaSourcePads.end(), pad);

		if(iter != m_mediaSourcePads.end())
		{
			return pad->onAccept(format);
		}
	}
	else
	{
		SharedMediaPads::iterator iter=std::find(m_mediaSinkPads.begin(), m_mediaSinkPads.end(), pad);

		if(iter != m_mediaSinkPads.end())
		{
			return pad->onAccept(format);
		}
	}
	return false;
}

//bool IMediaFilter::linkSourcePad(SharedMediaPad sourcePad, SharedMediaPad sinkPad, SharedMediaFormat format)
//{
//	return sourcePad->onLink(sinkPad, format);
//}
//
//bool IMediaFilter::linkSinkPad(SharedMediaPad sourcePad, SharedMediaPad sinkPad, SharedMediaFormat format)
//{
//	return sourcePad->onLink(sinkPad, format);
//}
//
//SharedMediaFormat IMediaFilter::connectSourcePad(SharedMediaPad sourcePad, SharedMediaPad sinkPad, SharedMediaFormat format)
//{
//	return sourcePad->onConnect(sinkPad, format);
//}
//
//SharedMediaFormat IMediaFilter::connectSinkPad(SharedMediaPad sourcePad, SharedMediaPad sinkPad, SharedMediaFormat format)
//{
//	return sourcePad->onConnect(sinkPad, format);
//}

void IMediaFilter::disconnect(boost::shared_ptr<IMediaFilter> sink)
{
	SharedMediaPads sourcePads=getSourcePads();

	BOOST_FOREACH(SharedMediaPad sourcePad, sourcePads)
	{
		if(sourcePad->linked())
		{
			SharedMediaPad linkedPad=sourcePad->linkedPad();
			
			if(linkedPad != SharedMediaPad())
			{
				if(linkedPad->parent() == sink)
					sourcePad->disconnect();
			}
		}
	}
}

void IMediaFilter::disconnect(SharedMediaPad localPad)
{
	SharedMediaPads sourcePads=getSourcePads();
	SharedMediaPads::iterator iter=std::find(sourcePads.begin(), sourcePads.end(), localPad);

	if(iter != sourcePads.end())
	{
		SharedMediaPad sourcePad=(*iter);

		if(sourcePad->linked())
			sourcePad->disconnect();
	}
}

bool IMediaFilter::inStream(boost::shared_ptr<IMediaFilter> filter)
{
	if(upStream(filter))
		return true;
	if(downStream(filter))
		return true;
	return false;
}

bool IMediaFilter::upStream(boost::shared_ptr<IMediaFilter> filter)
{
	BOOST_FOREACH(SharedMediaPad mediaPad, m_mediaSinkPads)
	{
		if(mediaPad->linked())
		{
			if(mediaPad->parent() == filter)
				return true;
			if(mediaPad->parent()->upStream(filter))
				return true;
		}
	}
	return false;
}

bool IMediaFilter::downStream(SharedMediaFilter filter)
{
	BOOST_FOREACH(SharedMediaPad mediaPad, m_mediaSourcePads)
	{
		if(mediaPad->linked())
		{
			if(mediaPad->parent() == filter)
				return true;
			if(mediaPad->parent()->downStream(filter))
				return true;
		}
	}
	return false;
}

SharedMediaFilters IMediaFilter::findUpStream(std::string typeName)
{
	SharedMediaFilters filters;

	BOOST_FOREACH(SharedMediaPad mediaPad, m_mediaSinkPads)
	{
		SharedMediaFilters childFilters=mediaPad->findUpStream(typeName);

		if(!childFilters.empty())
			filters.insert(filters.end(), childFilters.begin(), childFilters.end());
	}

	return filters;
}

SharedMediaFilters IMediaFilter::findUpStream(FilterType filterType)
{
	SharedMediaFilters filters;

	BOOST_FOREACH(SharedMediaPad mediaPad, m_mediaSinkPads)
	{
		SharedMediaFilters childFilters=mediaPad->findUpStream(filterType);

		if(!childFilters.empty())
			filters.insert(filters.end(), childFilters.begin(), childFilters.end());
	}

	return filters;
}

SharedMediaFilter IMediaFilter::findFirstUpStream(std::string typeName, FilterSearch search)
{
	if(search == FilterSearch::Depth)
	{
		BOOST_FOREACH(SharedMediaPad mediaPad, m_mediaSinkPads)
		{
			if(mediaPad->linked())
			{
				SharedMediaFilter parentFilter=mediaPad->linkedPad()->parent();

				if(parentFilter == SharedMediaFilter())
					return SharedMediaFilter();

				if(parentFilter->typeName() == typeName)
					return mediaPad->parent();
			
				SharedMediaFilter filter=parentFilter->findFirstUpStream(typeName);

				if(filter != SharedMediaFilter())
					return filter;
			}
		}
	}
	else
		assert(false);
	return SharedMediaFilter();
}


SharedMediaFilters IMediaFilter::findDownStream(std::string typeName)
{
	SharedMediaFilters filters;

	BOOST_FOREACH(SharedMediaPad mediaPad, m_mediaSourcePads)
	{
		SharedMediaFilters childFilters=mediaPad->findDownStream(typeName);

		if(!childFilters.empty())
			filters.insert(filters.end(), childFilters.begin(), childFilters.end());
	}

	return filters;
}

SharedMediaFilters IMediaFilter::findDownStream(FilterType filterType)
{
	SharedMediaFilters filters;

	BOOST_FOREACH(SharedMediaPad mediaPad, m_mediaSinkPads)
	{
		SharedMediaFilters childFilters=mediaPad->findDownStream(filterType);

		if(!childFilters.empty())
			filters.insert(filters.end(), childFilters.begin(), childFilters.end());
	}

	return filters;
}

SharedMediaFilter IMediaFilter::findFirstDownStream(std::string typeName, FilterSearch search)
{
	if(search == FilterSearch::Depth)
	{
		BOOST_FOREACH(SharedMediaPad mediaPad, m_mediaSourcePads)
		{
			if(mediaPad->linked())
			{
				SharedMediaFilter parentFilter=mediaPad->linkedPad()->parent();

				if(parentFilter == SharedMediaFilter())
					return SharedMediaFilter();

				if(parentFilter->typeName() == typeName)
					return parentFilter;
			
				SharedMediaFilter filter=parentFilter->findFirstDownStream(typeName);

				if(filter != SharedMediaFilter())
					return filter;
			}
		}
	}
	else
		assert(false);
	return SharedMediaFilter();
}

SharedMediaFilters IMediaFilter::upStreamFilters()
{
	SharedMediaFilters upStreamFilters;

	BOOST_FOREACH(SharedMediaPad mediaPad, m_mediaSinkPads)
	{
		if(mediaPad->linked())
			upStreamFilters.push_back(mediaPad->parent());
	}
	return upStreamFilters;
}

SharedMediaFilters IMediaFilter::downStreamFilters()
{
	SharedMediaFilters downStreamFilters;

	BOOST_FOREACH(SharedMediaPad mediaPad, m_mediaSourcePads)
	{
		if(mediaPad->linked())
			downStreamFilters.push_back(mediaPad->parent());
	}
	return downStreamFilters;
}

size_t IMediaFilter::getSampleBin()
{
	if(m_sampleBin == 0)
		m_sampleBin=allocSampleBin();
	return m_sampleBin;
}

size_t IMediaFilter::allocSampleBin()
{
	return allocSampleBin(this);
}

size_t IMediaFilter::allocSampleBin(IMediaFilter *filter)
{
	if(m_parent != SharedMediaFilter())
		return m_parent->allocSampleBin();
	return 0;
}

void IMediaFilter::releaseSampleBin(size_t bin)
{
	releaseSampleBin(bin, this);
}

void IMediaFilter::releaseSampleBin(size_t bin, IMediaFilter *filter)
{
	if(m_parent != SharedMediaFilter())
		return m_parent->releaseSampleBin(bin, this);
}

SharedMediaSample IMediaFilter::newSample(unsigned int type)
{
	return newSample(type, getSampleBin());
}

SharedMediaSample IMediaFilter::newSample(unsigned int type, size_t sampleBin)
{
	if(m_parent != SharedMediaFilter())
		return m_parent->newSample(type, sampleBin);

	SharedMediaSample sample=MediaSampleFactory::createType(type);

	return sample;
}

void IMediaFilter::deleteSample(SharedMediaSample sample)
{
//	if(m_parent != SharedMediaFilter())
//		return m_parent->deleteSample(sample);
	sample.reset();
}

MediaTime IMediaFilter::getTime()
{
	if(m_mediaClock != SharedMediaClock())
		return m_mediaClock->time();
	else
	{
		if(m_parent != SharedMediaFilter())
			return m_parent->getTime();
		else
		{
			m_mediaClock.reset(new SystemMediaClock());
			return m_mediaClock->time();
		}
	}
}

MediaTime IMediaFilter::getStreamTime()
{
	return getTime();
}

IMediaFilter::FilterState IMediaFilter::getState() const
{
	return m_state;
}

IMediaFilter::StateChange IMediaFilter::setState(IMediaFilter::FilterState state)
{
	StateChange stateChange=SUCCESS;

	if(state == READY)
		stateChange=ready();
	else if(state == PAUSED)
		stateChange=pause();
	else if(state == PLAYING)
		stateChange=play();
	return stateChange;
}

IMediaFilter::StateChange IMediaFilter::ready()
{
	StateChange stateChange;

	if(m_state == READY)
		return SUCCESS;

	if(m_state == PLAYING)
	{
		stateChange=onPaused();
		if(stateChange != SUCCESS)
			return stateChange;
		m_state=PAUSED;
	}
	if((m_state == INIT) || (m_state == PAUSED))
	{
//		if(m_state == INIT)
//		{
//			if(!connectLinkedPads())
//				return FAILED;
//		}
		stateChange=onReady();
		if(stateChange != SUCCESS)
			return stateChange;
		if(!reLinkPads())
			return FAILED;
		m_state=READY;
		return stateChange;
//		m_state=READY;
//		return stateChange;
	}
	return FAILED;
}

IMediaFilter::StateChange IMediaFilter::pause()
{
	StateChange stateChange;

	if(m_state == PAUSED)
		return SUCCESS;

	if(m_state == INIT)
	{
		stateChange=onReady();
		if(stateChange != SUCCESS)
			return stateChange;
		m_state=READY;
	}
	if((m_state == READY) || (m_state == PLAYING))
	{
		stateChange=onPaused();
		if(stateChange != SUCCESS)
			return stateChange;
		m_state=PAUSED;
		return stateChange;
	}
	return FAILED;
}

IMediaFilter::StateChange IMediaFilter::play()
{
	StateChange stateChange;

	if(m_state == PLAYING)
		return SUCCESS;

	if(m_state == INIT)
	{
		stateChange=onReady();
		if(stateChange != SUCCESS)
			return stateChange;
		m_state=READY;
	}
	if(m_state == READY)
	{
		stateChange=onPaused();
		if(stateChange != SUCCESS)
			return stateChange;
		m_state=PAUSED;
	}
	if(m_state == PAUSED)
	{
		stateChange=onPlaying();
		if(stateChange != SUCCESS)
			return stateChange;
		m_state=PLAYING;
		return stateChange;
	}
	return FAILED;
}

void IMediaFilter::onLinkFormatChanged(SharedMediaPad mediaPad, SharedMediaFormat format)
{
}

void IMediaFilter::pushSample(SharedMediaPad mediaPad, SharedMediaSample sample)
{
	if(mediaPad->linked())
		mediaPad->linkedPad()->processSample(sample);
}

void IMediaFilter::pushSample(SharedMediaSample sample)
{
	SharedMediaPads sourcePads=getSourcePads();
	bool pushed=false;

	BOOST_FOREACH(SharedMediaPad &sourcePad, sourcePads)
	{
		if(sourcePad->linked())
		{
//			sourcePad->processSample(sample);
			sourcePad->linkedPad()->processSample(sample);
			pushed=true;
		}
	}
	if(!pushed) //sample was not pushed please delete
		deleteSample(sample);
}

void IMediaFilter::sendMessage(Attribute *attribute)
{
	sendMessage(this, attribute);
}

void IMediaFilter::sendMessage(IMediaFilter *sender, Attribute *attribute)
{
	sendMessageUpStream(sender, attribute);
	sendMessageDownStream(sender, attribute);
}

void IMediaFilter::sendMessageUpStream(Attribute *attribute)
{
	sendMessageUpStream(this, attribute);
}

void IMediaFilter::sendMessageUpStream(IMediaFilter *sender, Attribute *attribute)
{
	BOOST_FOREACH(SharedMediaPad mediaPad, m_mediaSinkPads)
	{
		if(mediaPad->linked())
		{
			SharedMediaFilter linkedFilter;

			if(mediaPad->linkedPad() != SharedMediaPad())
				linkedFilter=mediaPad->linkedPad()->parent();

			if(linkedFilter != SharedMediaFilter())
			{
				linkedFilter->onMessage(mediaPad.get(), sender, attribute);
				linkedFilter->sendMessageUpStream(sender, attribute);
			}
		}
	}
}

void IMediaFilter::sendMessageDownStream(Attribute *attribute)
{
	sendMessageDownStream(this, attribute);
}

void IMediaFilter::sendMessageDownStream(IMediaFilter *sender, Attribute *attribute)
{
	BOOST_FOREACH(SharedMediaPad mediaPad, m_mediaSourcePads)
	{
		if(mediaPad->linked())
		{
			SharedMediaFilter linkedFilter;

			if(mediaPad->linkedPad() != SharedMediaPad())
				linkedFilter=mediaPad->linkedPad()->parent();

			if(linkedFilter != SharedMediaFilter())
			{
				linkedFilter->onMessage(mediaPad.get(), sender, attribute);
				linkedFilter->sendMessageDownStream(sender, attribute);
			}
		}
	}
}

void IMediaFilter::serialize(Serializer *serializer)
{
	serializer->startObject();

	serializer->addKey("type");
	serializer->addString(typeName());
	serializer->addKey("instance");
	serializer->addString(instance());

	SharedMediaPads sourcePads=getSourcePads();

	if(!sourcePads.empty())
	{
		serializer->addKey("links");
		serializer->startArray();

		for(SharedMediaPad sourcePad:sourcePads)
		{
			if(sourcePad->linked())
			{
				serializer->startObject();

				SharedMediaPad linkedPad=sourcePad->linkedPad();

				serializer->addKey("padName");
				serializer->addString(sourcePad->name());
				serializer->addKey("linkedFilter");
				serializer->addString(linkedPad->parent()->instance());
				serializer->addKey("linkedPad");
				serializer->addString(linkedPad->name());

				serializer->endObject();
			}
		}

		serializer->endArray();
	}

	AttributeContainer::serialize(serializer);

	serializer->endObject();
}

void IMediaFilter::unserialize(Unserializer *unserializer)
{
	if(unserializer->key("attributes"))
		AttributeContainer::unserialize(unserializer);
}


SharedMediaFilter Limitless::findByInstance(SharedMediaFilters mediaFilters, std::string instance)
{
	for(SharedMediaFilters::iterator iter=mediaFilters.begin(); iter!=mediaFilters.end(); ++iter)
	{
		if((*iter)->instance() == instance)
			return *iter;
	}
	return SharedMediaFilter();
}

std::string Limitless::stateName(IMediaFilter::FilterState state)
{
	std::string stateString="Unknown";

	switch(state)
	{
	case IMediaFilter::INIT:
		stateString="Init";
		break;
	case IMediaFilter::READY:
		stateString="Ready";
		break;
	case IMediaFilter::PAUSED:
		stateString="Paused";
		break;
	case IMediaFilter::PLAYING:
		stateString="Playing";
		break;
	};
	return stateString;
}

Limitless::MimeDetail Limitless::parseMimeDetail(std::string mime)
{
	MimeDetail detail;

	boost::char_separator<char> separator("/");
	boost::tokenizer<boost::char_separator<char> > mimeTokens(mime, separator);

	std::string mimeType;
	std::string mimeFormat;

	int index=0;
	for(const auto &token:mimeTokens)
	{
		if(index == 0)
			detail.type=token;
		else if(index == 1)
			detail.codec=token;
		else if(index == 2)
			detail.format=token;
		index++;
	}

	return detail;
}

}//namespace Limitless