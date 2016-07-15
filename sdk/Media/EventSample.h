#ifndef _Limitless_EventSample_h_
#define _Limitless_EventSample_h_

#include "Media/media_define.h"
#include "Media/MediaSample.h"
#include "Media/MediaSampleFactory.h"

namespace Limitless
{

enum class Event
{
    Unknown,
    Start,
    Flush,
    Stop,
    Segement,
    EndOf
};

class MEDIA_EXPORT EventSample:public AutoRegisterMediaSample<EventSample, MediaSample>
{
public:
    EventSample():m_event(Event::Unknown) {}
    virtual ~EventSample() {}

    Event getEvent() { return m_event; }
    void setEvent(Event event) { m_event=event; }

//MediaSample
	virtual unsigned char *buffer(){return nullptr;}
	virtual size_t size() const{return 0;}

private:
    Event m_event;
};
typedef boost::shared_ptr<EventSample> SharedEventSample;

}//namespace Limitless

#endif //_Limitless_EventSample_h_