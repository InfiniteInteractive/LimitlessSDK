#ifndef _Limitless_Media_IMedia_h_
#define _Limitless_Media_IMedia_h_

#include "Media/media_define.h"

#include <memory>
#include <functional>
#include <vector>


namespace Limitless
{
//class MediaDisplay;

class IMedia;
std::shared_ptr<IMedia> requestLoad(std::string name, std::string location);

class MEDIA_EXPORT IMedia
{
public:
//	typedef boost::shared_ptr<IMedia> (*RequestLoadFunction)(std::string name, std::string location);
	typedef std::function<std::shared_ptr<IMedia> (std::string name, std::string location)> RequestLoadFunction;

	IMedia(){}
	virtual ~IMedia(){}

	virtual std::string name()=0;
	virtual std::string location()=0;

	virtual void load(std::string location, RequestLoadFunction requestLoad=&requestLoad){};
	virtual void save() { saveAs(location()); }
	virtual void saveAs(std::string location){};
	//
	virtual std::shared_ptr<IMedia> duplicate()=0;

//	enum DisplayType
//	{
//		Display_2D=0,
//		Display_3D
//	};
//	virtual DisplayType displayType(){return Display_2D;}
//	virtual std::shared_ptr<MediaDisplay> createDisplay(std::shared_ptr<IMedia>)=0;

	virtual int width()=0;
	virtual int height()=0;

	static int TypeId;
};

typedef std::shared_ptr<IMedia> SharedMedia;
typedef std::vector<SharedMedia> SharedMediaVector;

//class MediaDisplay
//{
//public:
//	MediaDisplay(){}
//	virtual ~MediaDisplay(){}
//
//	virtual SharedMedia media()=0;
//
//	virtual bool initialized()=0;
//	virtual void init(float startX, float startY, float width, float height)=0;
//	virtual void display()=0;
//
//	virtual int width()=0;
//	virtual int height()=0;
//
//	virtual float displayWidth()=0;
//	virtual float displayHeight()=0;
//};
//typedef boost::shared_ptr<MediaDisplay> SharedMediaDisplay;
//typedef std::vector<SharedMediaDisplay> SharedMediaDisplayVector;
//
//class MediaDisplay3D:public MediaDisplay
//{
//public:
//	MediaDisplay3D(){}
//	virtual ~MediaDisplay3D(){}
//
//	virtual SharedMedia media()=0;
//
//	virtual bool initialized()=0;
//	virtual void init(float startX, float startY, float width, float height)=0;
//	virtual void display()=0;
//
//	virtual int width()=0;
//	virtual int height()=0;
//	virtual int depth()=0;
//
//	virtual float displayWidth()=0;
//	virtual float displayHeight()=0;
//};
//typedef boost::shared_ptr<MediaDisplay3D> SharedMediaDisplay3D;
//typedef std::vector<SharedMediaDisplay3D> SharedMediaDisplay3DVector;

}//namespace Limitless
//template<class T> SharedMediaDisplay createMediaDisplay(boost::shared_ptr<T> media){return SharedMediaDisplay();}
#endif //_Limitless_Media_IMedia_h_