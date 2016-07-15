#include "taskThread.h"
#include "Base/Log.h"

#include <boost/format.hpp>
#include <unordered_map>

namespace Limitless
{

typedef std::unordered_map<std::string, unsigned int> TaskMap;

unsigned int requestTaskId(const std::string &name)
{
	static TaskMap taskMap;
	static unsigned int id=0;

	TaskMap::iterator iter=taskMap.find(name);

	if(iter!=taskMap.end())
		return iter->second;
	
	id++;
	taskMap.insert(TaskMap::value_type(name, id));
	return id;
}

TaskThread::TaskThread()
{
}

void TaskThread::startThread()
{
	Limitless::Log::debug("TaskThread", "request start");
	
	m_threadRunning=false;
	m_stopThread=false;
	m_thread=std::thread(std::bind(&TaskThread::processThread, this));

	{//wait for thread to start
		std::unique_lock<std::mutex> lock(m_mutex);
	
		while(!m_threadRunning)
			m_event.wait(lock);
	}

	Limitless::Log::debug("TaskThread", "request started");
}

void TaskThread::stopThread()
{
	Limitless::Log::debug("TaskThread", "request stop");

	{
		std::unique_lock<std::mutex> lock(m_mutex);
	
		m_stopThread=true;

	}
	m_event.notify_all();
	m_thread.join();

	Limitless::Log::debug("TaskThread", "stopped");
}

void TaskThread::processThread()
{
	std::unique_lock<std::mutex> lock(m_mutex);

	m_threadRunning=true;
	m_event.notify_all();

	while(!m_stopThread)
	{
		if(m_tasks.empty())
			m_event.wait(lock);

		if(m_tasks.empty())
			continue;

		SharedITask task=m_tasks.front();

		m_tasks.pop();
		task->mutex=&m_mutex;
		lock.unlock();

		processTask(task);
		task->complete();

		lock.lock();
	}
}

void TaskThread::addTask(SharedITask task)
{
	{
		std::unique_lock<std::mutex> lock(m_mutex);

		m_tasks.push(task);
	}
	m_event.notify_all();
}

}//namespace Limitless