
#include "stdafx.h"
#include "tpsemaphore.h"

using namespace common;


cTPSemaphore::cTPSemaphore()
	: m_isThreadLoop(false)
{
}

cTPSemaphore::~cTPSemaphore()
{
	Clear();
}


// if threadCount -1, threadCount = processor count
bool cTPSemaphore::Init(const int threadCount //=-1
)
{
	Clear();

	int generateThreadCount = threadCount;
	if (threadCount < 0)
	{
		SYSTEM_INFO si;
		GetSystemInfo(&si);
		generateThreadCount = (int)si.dwNumberOfProcessors;
	}

	m_isThreadLoop = true;
	for (int i = 0; i < generateThreadCount; ++i)
		m_threads.push_back(new std::thread(cTPSemaphore::ThreadFunction, this));

	return true;
}


bool cTPSemaphore::PushTask(cTask *task)
{
	m_cs.Lock();
	m_tasks.push_front(task);
	m_cs.Unlock();
	m_sema.Signal();
	return true;
}


cTask* cTPSemaphore::PopTask()
{
	cTask *task = NULL;

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


bool cTPSemaphore::RemoveTask(const StrId &taskName)
{
	m_cs.Lock();
	auto it = std::find_if(m_tasks.begin(), m_tasks.end()
		, [&](auto &t) { return t->m_name == taskName; });
	const bool isExist = (m_tasks.end() != it);
	if (isExist)
	{
		delete *it;
		m_tasks.erase(it);
		m_sema.Wait();
	}
	m_cs.Unlock();
	return true;
}


bool cTPSemaphore::RemoveTask(const int taskId)
{
	m_cs.Lock();
	auto it = std::find_if(m_tasks.begin(), m_tasks.end()
		, [&](auto &t) { return t->m_id == taskId; });
	const bool isExist = (m_tasks.end() != it);
	if (isExist)
	{
		delete *it;
		m_tasks.erase(it);
		m_sema.Wait();
	}
	m_cs.Unlock();
	return true;
}


// wait all task finish
// no optimize
void cTPSemaphore::Wait()
{
	while (!m_tasks.empty())
		Sleep(10);
}


// is initialize?
bool cTPSemaphore::IsInit()
{
	return !m_threads.empty();
}


// thread terminate
void cTPSemaphore::Terminate()
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


void cTPSemaphore::Clear()
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

	m_sema.Clear();

	AutoCSLock cs(m_cs);
	while (!m_tasks.empty())
	{
		delete m_tasks.back();
		m_tasks.pop_back();
	}
}


int cTPSemaphore::ThreadFunction(cTPSemaphore *tpSemaphore)
{
	while (tpSemaphore->m_isThreadLoop)
	{
		cTask *task = tpSemaphore->PopTask();
		if (!task) // end thread command?
			break;
		try 
		{
			task->Run(0.f);
		}
		catch (std::exception &e) 
		{
			dbg::Logc(2, "error cTPSemaphore Task exception, %s\n", e.what());
		}
		delete task;
	}
	return 1;
}
