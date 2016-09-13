#ifndef _Limitless_EventQueueThread_h_
#define _Limitless_EventQueueThread_h_

#include "utilities_define.h"
#include "eventQueue.h"

#include <thread>

namespace Limitless
{

template<typename _Type>
class EventQueueThread
{
public:
	typedef std::function<void(_Type)> CallbackFunction;

	EventQueueThread(){}

	void start(CallbackFunction callback)
	{
		m_callback=callback;
		m_threadRunning=false;
		m_stopThread=false;
		m_thread=std::thread(std::bind(&EventQueueThread::thread, this));

		{//wait for thread to start
			std::unique_lock<std::mutex> lock(m_eventQueue.m_queuMutex);

			while(!m_threadRunning)
				m_eventQueue.m_conditionVariable.wait(lock);
		}
	}

	void stop()
	{
		{
			std::unique_lock<std::mutex> lock(m_eventQueue.m_queuMutex);

			m_stopThread=true;
		}
		m_eventQueue.m_conditionVariable.notify_all();
		m_thread.join();
	}

	void thread()
	{
		std::unique_lock<std::mutex> lock(m_eventQueue.m_queuMutex);

		m_threadRunning=true;
		m_eventQueue.m_conditionVariable.notify_all();

		while(!m_stopThread)
		{
			if(m_eventQueue.m_queue.empty())
				m_eventQueue.m_conditionVariable.wait(lock);

			if(m_eventQueue.m_queue.empty())
				continue;

			_Type value=m_eventQueue.m_queue.front();

			m_eventQueue.m_queue.pop_front();
			lock.unlock();

			//make call outside of mutex
			m_callback(value);

			lock.lock();
		}
	}

	void push_front(_Type value)
	{
		m_eventQueue.push_front(value);
	}

	void push_back(_Type value)
	{
		m_eventQueue.push_back(value);
	}

private:
	std::thread m_thread;
	bool m_threadRunning;
	bool m_stopThread;

	CallbackFunction m_callback;
	EventQueue<_Type> m_eventQueue;
};

}//namespace Limitless

#endif //_Limitless_EventQueueThread_h_