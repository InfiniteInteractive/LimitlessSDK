#ifndef _Limitless_TaskThread_h_
#define _Limitless_TaskThread_h_

#include "Utilities/utilities_define.h"
#include "Base/typeName.h"

#include <boost/thread.hpp>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <atomic>
#include <queue>
#include <set>

namespace Limitless
{

unsigned int UTILITIES_EXPORT requestTaskId(const std::string &name);

struct ITask
{
	ITask():mutex(nullptr){}

	virtual void process()=0;
	virtual void complete()=0;

	virtual std::string getName()=0;
	virtual unsigned int getTaskId()=0;

	std::mutex *mutex;
};

typedef std::shared_ptr<ITask> SharedITask;

template<typename _TaskClass>
struct Task:public ITask
{
	Task() { &name; &taskId; }
	
	virtual void complete(){};

	virtual std::string getName(){return name;}
	virtual unsigned int getTaskId(){return taskId;}

	static const std::string name;
	static const unsigned int taskId;
};

template<typename _TaskClass> const std::string Task<_TaskClass>::name=TypeName<_TaskClass>::get();
template<typename _TaskClass> const unsigned int Task<_TaskClass>::taskId=requestTaskId(TypeName<_TaskClass>::get());

template<typename _TaskClass>
struct WaitTask:Task<_TaskClass>
{
	WaitTask():completed(false){}

	virtual void complete()
	{
		{
			std::unique_lock<std::mutex> lock(*mutex);

			completed=true;
		}
		event.notify_all();
	};

	void wait()
	{
		std::unique_lock<std::mutex> lock(*mutex); 
		
		while(!completed)
			event.wait(lock);
	}

	std::condition_variable event;
	bool completed;
};

template<typename _TaskClass>
struct CallbackTask:Task<_TaskClass>
{
	CallbackTask():WaitTask(){}

	virtual void complete()
	{
		callback();
	};

	std::function<void()> callback;
};

typedef std::queue<SharedITask> SharedITaskQueue;

class UTILITIES_EXPORT TaskThread
{
public:
	TaskThread();

protected:
	void startThread();
	void stopThread();

	void addTask(SharedITask task);

	void processThread();
	virtual void processTask(SharedITask task){task->process();}
	
private:
	std::thread m_thread;
	std::condition_variable m_event;
	std::mutex m_mutex;

	SharedITaskQueue m_tasks;

	bool m_threadRunning;
	bool m_stopThread;
};

}//namespace Limitless

#endif //_Limitless_TaskThread_h_