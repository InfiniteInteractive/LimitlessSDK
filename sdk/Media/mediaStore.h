#ifndef _Limitless_Media_MediaStore_h_
#define _Limitless_Media_MediaStore_h_

#include "Media/media_define.h"
#include "media/IMedia.h"

#include <boost/shared_ptr.hpp>
#include <boost/function.hpp>

namespace Limitless
{

class MEDIA_EXPORT MediaSet:public IMedia
{
public:
	MediaSet(std::string name):m_name(name) {}
	MediaSet(std::string name, SharedMediaVector media);
	~MediaSet();

	virtual std::string name() { return m_name; }
	virtual std::string location() { return ""; }

	virtual std::shared_ptr<IMedia> duplicate();

//	virtual boost::shared_ptr<MediaDisplay> createDisplay(boost::shared_ptr<IMedia> media) { return boost::shared_ptr<MediaDisplay>(); }

	/*	virtual bool initialized(){return false;}
		virtual void init(float startX, float startY, float width, float height);
		virtual void display();
	*/
	virtual int width() { return 0; }
	virtual int height() { return 0; }

	/*	virtual float displayWidth(){return 0.0;}
		virtual float displayHeight(){return 0.0;}
	*/
	void push_back(SharedMedia media) { m_media.push_back(media); }

	SharedMedia get(size_t index) { return m_media[index]; }
	size_t size() { return m_media.size(); }

	template<class T> std::shared_ptr<T> get(size_t index)
	{
		size_t count=0;

		for(SharedMedia media:m_media)
		{
			std::shared_ptr<T> derivedClass=std::dynamic_pointer_cast<T>(media);

			if(derivedClass!=std::shared_ptr<T>())
			{
				if(count==index)
					return media;
				++count;
			}
		}
		return std::shared_ptr<T>();
	}

	template<class T> size_t size()
	{
		size_t count=0;

		for(SharedMedia media:m_media)
		{
			std::shared_ptr<T> derivedClass=std::dynamic_pointer_cast<T>(media);

			if(derivedClass!=std::shared_ptr<T>())
				++count;
		}
		return count;
	}

private:
	std::string m_name;
	SharedMediaVector m_media;
};
typedef std::shared_ptr<MediaSet> SharedMediaSet;
typedef std::vector<SharedMediaSet> SharedMediaSetVector;

class MEDIA_EXPORT MediaStore
{
public:
	MediaStore(void);
	~MediaStore(void);

	size_t mediaSets() { return m_mediaSets.size(); }
	SharedMediaSet mediaSet(size_t index) { return m_mediaSets[index]; }
	void pushMediaSet(SharedMediaSet mediaSet);

	void pushMedia(SharedMedia media, SharedMedia siblingMedia);

	void update();
	typedef std::function<void()> UpdateCallback;
	void addUpdateCallback(UpdateCallback callback);

	SharedMediaVector selectedMedia() { return m_selectedMedia; }
	void setSelectedMedia(SharedMediaVector selectedMedia);

	typedef std::function<void(SharedMediaVector media)> SelectionCallback;
	void addSelectionCallback(SelectionCallback callback);

private:
	SharedMediaSetVector m_mediaSets;

	std::vector<UpdateCallback> m_updateCallbacks;

	std::vector<SharedMediaSet> m_selectedSets;
	SharedMediaVector m_selectedMedia;
	std::vector<SelectionCallback> m_selectionCallbacks;

};
typedef std::shared_ptr<MediaStore> SharedMediaStore;

}//namespace Limitless

#endif //_Limitless_Media_MediaStore_h_