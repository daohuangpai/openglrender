#include "tasks.h"

namespace GLUI 
{

	void TaskList::PostTask(std::function<void()> task)
	{
		boost::unique_lock<boost::mutex> locker(m_lock);
		m_tasks.push(task);
	}

	void TaskList::Run()
	{
		boost::unique_lock<boost::mutex> locker(m_lock);
		//Ö´ÐÐÈÎÎñ;
		while (m_tasks.size()>0) {
			m_tasks.front()();
			m_tasks.pop();
		}
	}



}