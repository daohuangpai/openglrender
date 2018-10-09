#pragma once
#include <queue>
#include <boost/thread.hpp>

namespace GLUI {


	class TaskList {
	public:
		void PostTask(std::function<void()> task);
		void Run();
	protected:
		std::queue<std::function<void()>>	m_tasks;
		boost::mutex m_lock;
	};


}