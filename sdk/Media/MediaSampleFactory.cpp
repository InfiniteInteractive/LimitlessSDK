#include "Media/MediaSampleFactory.h"
#include "Media/MediaSample.h"
#include "boost/foreach.hpp"
#include <functional>

using namespace Limitless;

//MediaSampleFactory::MediaSampleNodeHolder MediaSampleFactory::s_nodes;
MediaSampleFactory::MediaSampleNodeHolder *s_nodes=nullptr;

//MediaSampleFactory::MediaSampleNodes MediaSampleFactory::s_mediaSampleNodes;
//unsigned int MediaSampleFactory::s_sampleTypeIndex=0;

SharedMediaSample MediaSampleFactory::createType(std::string typeName, std::function<void(MediaSample *)> deleteFunction)
{
	BOOST_FOREACH(MediaSampleNode &mediaSampleNode, s_nodes->s_mediaSampleNodes)
	{
		if(mediaSampleNode.typeName() == typeName)
		{
			if(mediaSampleNode.factoryFunction() != nullptr)
			{
				SharedMediaSample object(mediaSampleNode.factoryFunction(), deleteFunction);

				if(object != SharedMediaSample())
					return object;
			}
			break;
		}
	}

	return SharedMediaSample();
}

SharedMediaSample MediaSampleFactory::createType(unsigned int type, std::function<void(MediaSample *)> deleteFunction)
{
	if((type < 0)||(type>=s_nodes->s_mediaSampleNodes.size()))
		return SharedMediaSample();

	MediaSample *sample=s_nodes->s_mediaSampleNodes[type].factoryFunction();

	if(sample == nullptr)
		return SharedMediaSample();

	SharedMediaSample object(sample, deleteFunction);

	return object;
}

//std::string MediaSampleFactory::registerType(std::string typeName, FactoryFunc factoryFunc)
//{
//	if(s_nodes == nullptr)
//		s_nodes=new MediaSampleFactory::MediaSampleNodeHolder();
//
//	s_nodes->s_mediaSampleNodes.push_back(MediaSampleNode(s_nodes->s_sampleTypeIndex, typeName, factoryFunc));
//	s_nodes->s_sampleTypeIndex++;
//	return typeName;
//}
SampleType MediaSampleFactory::registerType(std::string typeName, std::vector<unsigned int> ids, FactoryFunc factoryFunc)
{
	if(s_nodes == nullptr)
		s_nodes=new MediaSampleFactory::MediaSampleNodeHolder();

	SampleType sampleType;

	sampleType.id=s_nodes->s_sampleTypeIndex;
	sampleType.idInheritance=ids;
	sampleType.name=typeName;

	s_nodes->s_mediaSampleNodes.push_back(MediaSampleNode(s_nodes->s_sampleTypeIndex, typeName, factoryFunc));
	s_nodes->s_sampleTypeIndex++;
	return sampleType;
}

size_t MediaSampleFactory::getTypeId(std::string typeName)
{
	BOOST_FOREACH(MediaSampleNode &mediaSampleNode, s_nodes->s_mediaSampleNodes)
	{
		if(mediaSampleNode.typeName() == typeName)
			return mediaSampleNode.type();
	}
	return -1;
}

void MediaSampleFactory::deleteSample(MediaSample *sample)
{
	delete sample;
}