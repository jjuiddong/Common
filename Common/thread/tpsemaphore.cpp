
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

	int gerateThreadCount = threadCount;
	if (threadCount < 0)
	{
		SYSTEM_INFO si;
		GetSystemInfo(&si);
		gerateThreadCount = (int)si.dwNumberOfProcessors;
	}

	m_isThreadLoop = true;
	for (int i = 0; i < gerateThreadCount; ++i)
		m_threads.push_back(new std::thread(cTPSemaphore::ThreadFunction, this));

	return true;
}


bool cTPSemaphore::PushTask(cTask *task)
{
	m_cs.Lock();
	m_tasks.push(task);
	m_cs.Unlock();
	m_sema.Signal();
	return true;
}


cTask* cTPSemaphore::PopTask()
{
	m_sema.Wait();
	m_cs.Lock();
	cTask *task = m_tasks.front();
	m_tasks.pop();
	m_cs.Unlock();
	return task;
}


// wait all task finish
// no optimize
void cTPSemaphore::Wait()
{
	while (!m_tasks.empty())
		Sleep(10);
}


// thread terminate
void cTPSemaphore::Terminate()
{
	m_isThreadLoop = true;

	for (auto th : m_threads)
		if (th->joinable())
			PushTask(NULL); // finish thread

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
			PushTask(NULL); // finish thread

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
		delete m_tasks.front();
		m_tasks.pop();
	}
}


int cTPSemaphore::ThreadFunction(cTPSemaphore *tpSemaphore)
{
	while (tpSemaphore->m_isThreadLoop)
	{
		cTask *task = tpSemaphore->PopTask();
		if (!task) // end thread command
			break;

		task->Run(0.f);
		delete task;
	}
	return 1;
}
