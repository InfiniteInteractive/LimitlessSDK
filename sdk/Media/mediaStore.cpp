#include "MediaStore.h"

#include <boost/foreach.hpp>
#include <boost/format.hpp>

namespace Limitless
{

MediaSet::MediaSet(std::string name, SharedMediaVector media):
	m_name(name)
{
	for(int i=0; i<media.size(); ++i)
		m_media.push_back(media[i]);
}

//MediaSet::MediaSet(std::vector<QImage> images)
//{
//	for(int i=0; i<images.size(); ++i)
//	{
//		SharedCameraMedia cameraMedia(new CameraMedia(images.at(i)));
//
//		m_cameras.push_back(cameraMedia);
//	}
//}

MediaSet::~MediaSet()
{}

std::shared_ptr<IMedia> MediaSet::duplicate()
{
	SharedMediaSet mediaSet(new MediaSet(*this));

	return mediaSet;
}

MediaStore::MediaStore(void)
{}

MediaStore::~MediaStore(void)
{}

void MediaStore::pushMediaSet(SharedMediaSet mediaSet)
{
	m_mediaSets.push_back(mediaSet);
	update();
}

void MediaStore::pushMedia(SharedMedia media, SharedMedia siblingMedia)
{
	for(SharedMediaSet mediaSet:m_mediaSets)
	{
		for(size_t i=0; i<mediaSet->size(); ++i)
		{
			if(siblingMedia==mediaSet->get(i))
			{
				mediaSet->push_back(media);
				update();
				return;
			}
		}
	}

	std::string setName=(boost::format("Set%d")%mediaSets()).str();
	SharedMediaSet mediaSet(new MediaSet(setName));

	mediaSet->push_back(media);
	pushMediaSet(mediaSet);
}

void MediaStore::update()
{
	for(UpdateCallback callback:m_updateCallbacks)
		callback();
}

void MediaStore::addUpdateCallback(UpdateCallback callback)
{
	m_updateCallbacks.push_back(callback);
}


void MediaStore::setSelectedMedia(SharedMediaVector selectedMedia)
{
	m_selectedMedia.clear();
	m_selectedMedia=selectedMedia;

	for(SelectionCallback callback:m_selectionCallbacks)
		callback(m_selectedMedia);
}

void MediaStore::addSelectionCallback(SelectionCallback callback)
{
	m_selectionCallbacks.push_back(callback);
}

} //namespace Limitless