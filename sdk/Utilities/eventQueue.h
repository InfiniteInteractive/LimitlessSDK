#ifndef _Limitless_EventQueue_h_
#define _Limitless_EventQueue_h_

#include "utilities_define.h"
#include <deque>
#include <mutex>
#include <condition_variable>

namespace Limitless
{

template<typename _Type> class EventQueueThread;


template<typename _Type, size_t _bufferSize=0>
class EventQueue
{
public:
	EventQueue(){}
	EventQueue(const EventQueue &){}

	void push_front(_Type value)
	{
		{
			std::unique_lock<std::mutex> lock(m_queuMutex);

			if(_bufferSize > 0) //check for maxium buffer size
			{
				while(m_queue.size() > _bufferSize)
				{
					m_conditionVariable.wait(lock);

					if(m_breakWait)
						return;
				}
			}

			m_queue.push_front(value);
		}
		m_conditionVariable.notify_all();
	}

	_Type pop_front()
	{
		std::unique_lock<std::mutex> lock(m_queuMutex);

		m_breakWait=false;
		while(m_queue.empty())
		{
			m_conditionVariable.wait(lock);

			if(m_breakWait)
				return _Type();
		}

		_Type value=m_queue.front();

		m_queue.pop_front();
		return value;
	}

	template< class Rep, class Period >
	_Type pop_front_timeout(const std::chrono::duration<Rep, Period> &timeOut, _Type default)
	{
		std::unique_lock<std::mutex> lock(m_queuMutex);

		if(m_queue.empty())
			m_conditionVariable.wait_for(lock, timeOut);

		if(m_queue.empty())
			return default;

		_Type value=m_queue.front();

		m_queue.pop_front();
		return value;
	}

	void push_back(_Type value)
	{
		{
			std::unique_lock<std::mutex> lock(m_queuMutex);

			if(_bufferSize > 0) //check for maxium buffer size
			{
				while(m_queue.size() > _bufferSize)
				{
					m_conditionVariable.wait(lock);

					if(m_breakWait)
						return;
				}
			}

			m_queue.push_back(value);
		}
		m_conditionVariable.notify_all();
	}

	_Type pop_back()
	{
		std::unique_lock<std::mutex> lock(m_queuMutex);

		m_breakWait=false;
		while(m_queue.empty())
		{
			m_conditionVariable.wait(lock);

			if(m_breakWait)
				return _Type();
		}

		_Type value=m_queue.back();

		m_queue.pop_back();
		return value;
	}

	bool empty()
	{
		std::unique_lock<std::mutex> lock(m_queuMutex);

		return m_queue.empty();
	}

	size_t size()
	{
		std::unique_lock<std::mutex> lock(m_queuMutex);

		return m_queue.size();
	}

	void waitWhileEmpty()
	{
		std::unique_lock<std::mutex> lock(m_queuMutex);

		m_breakWait=false;
		while(m_queue.empty())
		{
			m_conditionVariable.wait();

			if(m_breakWait)
				return;
		}
	}

	std::unique_lock<std::mutex> acquireLock()
	{
		return std::unique_lock<std::mutex>(m_queuMutex);
	}

	void push_back(_Type value, std::unique_lock<std::mutex> &lock)
	{
		assert(lock.mutex() == &m_queuMutex);

		if(_bufferSize > 0) //check for maxium buffer size
		{
			while(m_queue.size() > _bufferSize)
			{
				m_conditionVariable.wait(lock);

				if(m_breakWait)
					return;
			}
		}

		m_queue.push_back(value);
		m_conditionVariable.notify_all();
	}

	void breakWait()
	{
		{
			std::unique_lock<std::mutex> lock(m_queuMutex);

			m_breakWait=true;
		}
		m_conditionVariable.notify_all();
	}

private:
	friend class EventQueueThread<_Type>;

	std::deque<_Type> m_queue;
	std::mutex m_queuMutex;
	std::condition_variable m_conditionVariable;

	bool m_breakWait;
};

}//namespace Limitless

#endif //_Limitless_EventQueue_h_