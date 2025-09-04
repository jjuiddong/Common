
#include "stdafx.h"
#include "taskflow.h"

using namespace common;


//-----------------------------------------------------------------------------
// cTfTask
// push child task, no process
bool cTfTask::PushChildTask(cTfTask* task)
{
	auto it = std::find_if(m_children.begin(), m_children.end()
		, [&](auto& a) { return a.second == task; });
	if (m_children.end() != it)
		return false; // error, already exist

	task->m_parent = this;
	task->m_tf = m_tf;
	m_children.push_back({ false, task });
	return true;
}


// find child task index
// return: index, -1:fail
int cTfTask::FindTaskIndex(cTfTask* childTask)
{
	auto it = std::find_if(m_children.begin(), m_children.end()
		, [&](auto& a) { return a.second == childTask; });
	if (m_children.end() == it)
		return -1; // error, not exist
	return it - m_children.begin();
}


// call from child task to notify finish
// return: all child task finish?
bool cTfTask::FinishChildTask(cTfTask* childTask)
{
	auto it = std::find_if(m_children.begin(), m_children.end()
		, [&](auto& a) { return a.second == childTask; });
	if (m_children.end() == it)
		return false; // error, not exist

	it->first = true; // finish

	FinishChild(childTask);
	return true;
}


// stop child task
// never access this method after Complete()
bool cTfTask::CloseChildTask(cTfTask* childTask)
{
	AutoCSLock cs(m_tf->m_rmCs);

	queue<cTfTask*> q;
	for (auto& child : m_children)
	{
		if ((!child.first) && (child.second == childTask))
			q.push(child.second);
	}
	while (!q.empty())
	{
		cTfTask* task = q.front(); q.pop();
		task->m_isShutdown = true;
		for (auto& child : task->m_children)
		{
			if (!child.first)
				q.push(child.second);
		}
	}

	return true;
}


// stop all child task
// never access this method after Complete()
bool cTfTask::CloseAllChildTask()
{
	AutoCSLock cs(m_tf->m_rmCs);

	queue<cTfTask*> q;
	for (auto& child : m_children)
	{
		if (!child.first)
			q.push(child.second);
	}
	while (!q.empty())
	{
		cTfTask* task = q.front(); q.pop();
		task->m_isShutdown = true;
		for (auto& child : task->m_children)
		{
			if (!child.first)
				q.push(child.second);
		}
	}
	return true;
}


// is finish all child task?
bool cTfTask::IsFinish()
{
	bool isFinish = true;
	for (auto& child : m_children)
	{
		if (!child.first)
		{
			isFinish = false;
			break;
		}
	}
	return isFinish;
}


// finish task
bool cTfTask::CloseTask()
{
	m_isShutdown = true;
	CloseAllChildTask();
	return true;
}


//-----------------------------------------------------------------------------
// cTaskFlow
cTaskFlow::cTaskFlow()
	: m_isThreadLoop(false)
	, m_isSequence(true)
{
}

cTaskFlow::~cTaskFlow()
{
	Clear();
}


// if threadCount -1, threadCount = processor count
// isSequence: sequence process task?
bool cTaskFlow::Init(const int threadCount //=-1
	, const bool isSequence //=true
)
{
	Clear();

	m_isSequence = isSequence;

	int generateThreadCount = threadCount;
	if (threadCount < 0)
	{
		SYSTEM_INFO si;
		GetSystemInfo(&si);
		generateThreadCount = (int)si.dwNumberOfProcessors;
	}

	m_isThreadLoop = true;
	for (int i = 0; i < generateThreadCount; ++i)
		m_threads.push_back(new std::thread(cTaskFlow::ThreadFunction, this));

	return true;
}


// push task
bool cTaskFlow::PushTask(cTfTask* task)
{
	AutoCSLock cs(m_readyCs);
	if (m_isSequence)
	{
		if (m_waitQ.empty())
		{
			ExecuteTask(task);
			m_waitQ.push_back(task);
		}
		else
		{
			m_readyQ.push_back(task);
		}
	}
	else
	{
		ExecuteTask(task);
	}
	return true;
}


// execute task directly
bool cTaskFlow::ExecuteTask(cTfTask* task)
{
	m_cs.Lock();
	if (task)
		task->m_tf = this;
	m_tasks.push_front(task);
	m_cs.Unlock();
	m_sema.Signal();
	return true;
}


// execute child task
bool cTaskFlow::ExecuteChildTask(cTfTask* parent)
{
	RETV(parent->m_children.empty(), false);
	parent->m_syncVal = 0; // clear
	for (auto& child : parent->m_children)
		if (!child.first) // spawn not finished task
			ExecuteTask(child.second);
	return true;
}


// next step current task
// task: current task
bool cTaskFlow::NextStep(cTfTask* task)
{
	const bool isRoot = !task->m_parent;
	if (isRoot)
	{
		// remove root task, child task
		m_rmCs.Lock();
		for (auto& child : task->m_children)
			delete child.second;
		task->m_children.clear();
		delete task;

		// check next task
		if (m_isSequence)
		{
			AutoCSLock cs(m_readyCs);
			m_waitQ.clear();
			if (!m_readyQ.empty())
			{
				cTfTask *next = m_readyQ.front(); m_readyQ.pop_front();
				ExecuteTask(next);
				m_waitQ.push_back(next);
			}
		}
		m_rmCs.Unlock();
	}
	else
	{
		// notify finished task to parent
		cTfTask* parent = task->m_parent;
		parent->FinishChildTask(task);
		//delete task;

		bool isRoot = false; // is remove root task?
		cTfTask* c = parent;
		while (c)
		{
			// all child finished?
			if (!c->IsFinish())
				break; // nothing to do

			// only one execute synchronization
			{
				AutoCSLock cs(m_finCs);
				if (0 != c->m_syncVal)
					break; // nothing to do
				c->m_syncVal = 1;
			}

			// all child task finished, execute next step
			if (cTfTask::eState::Process == c->m_state)
			{
				// change state, execute again
				c->m_state = cTfTask::eState::Complete;
				ExecuteTask(c);
				break;
			}
			else // complete
			{
				if (c->m_parent)
				{
					// notify finished task to parent
					c->m_parent->FinishChildTask(c);
					cTfTask* tmp = c->m_parent;

					m_rmCs.Lock();
					for (auto& child : c->m_children)
						delete child.second;
					c->m_children.clear();
					m_rmCs.Unlock();

					//delete c;
					c = tmp; // c->m_parent
				}
				else
				{
					// remove root task, child task
					//for (auto& c : c->m_children)
					//	delete c.second;
					//delete c;
					isRoot = true;
					break; // finish
				}
			}
		}

		if (isRoot)
		{
			m_rmCs.Lock();
			for (auto& child : c->m_children)
				delete child.second;
			c->m_children.clear();
			delete c;

			// check next task
			if (m_isSequence)
			{
				AutoCSLock cs(m_readyCs);
				m_waitQ.clear();
				if (!m_readyQ.empty())
				{
					cTfTask* next = m_readyQ.front(); m_readyQ.pop_front();
					ExecuteTask(next);
					m_waitQ.push_back(next);
				}
			}
			m_rmCs.Unlock();
		}
	}

	return true;
}


// pop task
cTfTask* cTaskFlow::PopTask()
{
	cTfTask* task = nullptr;

	m_sema.Wait();
	m_cs.Lock();
	if (m_tasks.empty())
	{
		m_cs.Unlock();
		return PopTask();
	}
	else
	{
		task = m_tasks.back();
		m_tasks.pop_back();
	}
	m_cs.Unlock();
	return task;
}


// close task (only remove root or ready task)
bool cTaskFlow::CloseTask(const string& taskName)
{
	m_rmCs.Lock();
	m_readyCs.Lock();

	while (1)
	{
		auto it = std::find_if(m_readyQ.begin(), m_readyQ.end()
			, [&](auto& t) { return taskName == t->m_name; });
		if (m_readyQ.end() == it)
			break;
		delete* it;
		m_readyQ.erase(it);
	}

	m_readyCs.Unlock();
	m_rmCs.Unlock();
	return true;
}


// close task (only remove root or ready task)
bool cTaskFlow::CloseTask(const int taskId)
{
	m_rmCs.Lock();	
	m_readyCs.Lock();
	while (1)
	{
		auto it = std::find_if(m_readyQ.begin(), m_readyQ.end()
			, [&](auto& t) { return taskId == t->m_id; });
		if (m_readyQ.end() == it)
			break;
		delete* it;
		m_readyQ.erase(it);
	}
	m_readyCs.Unlock();
	m_rmCs.Unlock();

	m_cs.Lock();
	while (1)
	{
		auto it = std::find_if(m_tasks.begin(), m_tasks.end()
			, [&](auto& t) { return (taskId == t->m_id) && !t->m_parent; }); // root task
		if (m_tasks.end() == it)
			break;
		delete* it;
		m_tasks.erase(it);
	}
	m_cs.Unlock();

	return true;
}


// close all task
bool cTaskFlow::CloseAllTask()
{
	m_rmCs.Lock();
	m_readyCs.Lock();
	for (auto &task : m_readyQ)
		delete task;
	m_readyQ.clear();
	m_readyCs.Unlock();
	m_rmCs.Unlock();

	m_cs.Lock();
	while (1)
	{
		auto it = std::find_if(m_tasks.begin(), m_tasks.end()
			, [&](auto& t) { return !t->m_parent; }); // root task
		if (m_tasks.end() == it)
			break;
		delete* it;
		m_tasks.erase(it);
	}
	m_cs.Unlock();
	return true;
}


// is initialize?
bool cTaskFlow::IsInit()
{
	return !m_threads.empty();
}


// wait until all thread finish
void cTaskFlow::Join()
{
	for (auto th : m_threads)
		if (th->joinable())
			PushTask(nullptr); // finish thread

	for (auto th : m_threads)
	{
		if (th->joinable())
			th->join();
		delete th;
	}
	m_threads.clear();
}


// thread terminate (memory leak)
void cTaskFlow::Terminate()
{
	m_isThreadLoop = false;

	for (auto th : m_threads)
		if (th->joinable())
			PushTask(nullptr); // finish thread

	for (auto th : m_threads)
	{
		if (th->joinable())
			th->join();
		delete th;
	}
	m_threads.clear();
}


// clear taskflow
// timeOut: millisecond
void cTaskFlow::Clear(
	const int timeOut //=0
)
{
	if (timeOut > 0)
	{
		// thread terminate, maybe contain infinity loop
		// check timeout, force terminate if time over
		bool isFinish = false;
		std::thread thread([&]() {
			for (auto th : m_threads)
				if (th->joinable())
					PushTask(nullptr); // finish thread
			for (auto th : m_threads)
				if (th->joinable())
					th->join();
			m_isThreadLoop = false;
			isFinish = true;
		});

		int t = 0;
		while (!isFinish && (t < timeOut))
		{
			std::this_thread::sleep_for(std::chrono::milliseconds(100));
			t += 10;
		}

		if (isFinish)
		{
			for (auto th : m_threads)
				delete th;
			m_threads.clear();
		}
		else
		{
			// force terminiate thread, maybe infinity loop
			const BOOL res01 = ::TerminateThread(thread.native_handle(), 0x1);
			thread.detach();

			for (auto th : m_threads)
			{
				if (th->joinable())
				{
					const BOOL res1 = ::TerminateThread(th->native_handle(), 0x1);
					th->detach();
				}
				delete th;
			}
			m_threads.clear();
		}
		if (thread.joinable())
			thread.join();
	}
	else
	{
		// wait all task finish
		for (auto th : m_threads)
			if (th->joinable())
				PushTask(nullptr); // finish thread

		m_isThreadLoop = false;

		for (auto th : m_threads)
		{
			if (th->joinable())
				th->join();
			delete th;
		}
		m_threads.clear();
	}

	m_sema.Clear();

	while (!m_tasks.empty())
	{
		delete m_tasks.back();
		m_tasks.pop_back();
	}
	while (!m_readyQ.empty())
	{
		delete m_readyQ.back();
		m_readyQ.pop_back();
	}
	m_waitQ.clear();
}


// taskflow thread
int cTaskFlow::ThreadFunction(cTaskFlow* tf)
{
	while (tf->m_isThreadLoop)
	{
		cTfTask* task = tf->PopTask();
		if (!task) // end thread command?
			break;

		if (cTfTask::eState::Process == task->m_state)
		{
			task->Process();

			if (task->IsFinish())
			{
				task->Complete();

				if (task->IsFinish())
					tf->NextStep(task); // finish task, next step
				else
					tf->ExecuteChildTask(task);
			}
			else
			{
				tf->ExecuteChildTask(task);
			}
		}
		else // Complete State
		{
			task->Complete();

			if (task->IsFinish())
				tf->NextStep(task); // finish task, next step
			else
				tf->ExecuteChildTask(task);
		}
	}
	return 1;
}
