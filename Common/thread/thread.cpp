#include "stdafx.h"
#include "Thread.h"
#include "Task.h"
#include <boost/bind.hpp>

using namespace common;

namespace common
{
	unsigned __stdcall ThreadProcess(void *pThreadPtr )
	{
		cThread *pThread = (cThread*)pThreadPtr;
		pThread->Run();
		pThread->Exit();
		return 0;
	}

	template<class T>
	static bool IsSameId(T *p, int id)
	{
		if (!p) return false;
		return p->m_Id == id;
	}
}


cThread::cThread(const std::string &name) :
	m_state(eState::WAIT)
,	m_hThread(NULL)
,	m_name(name)
{
}

cThread::~cThread()
{
	Clear();
}


//------------------------------------------------------------------------
//  쓰레드 실행
//------------------------------------------------------------------------
void cThread::Start()
{
	if (eState::RUN != m_state)
	{
		m_state = eState::RUN;
		m_hThread = (HANDLE)_beginthreadex(NULL, 0, ThreadProcess, this, 0, NULL);
	}
}


//------------------------------------------------------------------------
// 쓰레드 종료
//------------------------------------------------------------------------
void cThread::Terminate(const int milliSeconds) //milliSeconds = -1
{
	m_state = eState::END;
	DWORD timeOutTime = (milliSeconds>=0)? milliSeconds : INFINITE;
	WaitForSingleObject(m_hThread, timeOutTime);
	CloseHandle(m_hThread);
	m_hThread = NULL;
}


//------------------------------------------------------------------------
// 쓰레드에게 메세지를 보낸다.
// rcvTaskId : 받을 태스크 아이디 ('0' 이라면 쓰레드가 받는다.)
//			   -1 : 외부로 가는 메세지를 뜻함
//------------------------------------------------------------------------
void cThread::Send2ThreadMessage( threadmsg::MSG msg, WPARAM wParam, LPARAM lParam, LPARAM added)
{
	AutoCSLock cs(m_msgCS);
	m_threadMsgs.push_back( SExternalMsg(-1, (int)msg, wParam, lParam, added) );
}


//------------------------------------------------------------------------
// 
//------------------------------------------------------------------------
void cThread::Send2ExternalMessage( int msg, WPARAM wParam, LPARAM lParam, LPARAM added )
{
	AutoCSLock cs(m_msgCS);
	m_externalMsgs.push_back( SExternalMsg(-1, msg, wParam, lParam, added) );
}


//------------------------------------------------------------------------
// 쓰레드가 받은 메세지를 리턴한다.
// 메세지가 없다면 false를 리턴한다.
//------------------------------------------------------------------------
bool cThread::GetThreadMsg( OUT SExternalMsg *out
	, eMessageOption::Enum opt  // = eMessageOption::REMOVE
)
{
	if (!out)
		return false;

	bool reval;
	{
		AutoCSLock cs(m_msgCS);
		if (m_threadMsgs.empty())
		{
			reval = false;
		}
		else
		{
			*out = m_threadMsgs.front();
			if (eMessageOption::REMOVE == opt)
				m_threadMsgs.pop_front();
			reval = true;
		}
	}
	return reval;
}


//------------------------------------------------------------------------
// 쓰레드에서 외부로 향하는 메세지를 리턴한다.
// 메세지가 없다면 false를 리턴한다.
//------------------------------------------------------------------------
bool cThread::GetExternalMsg( OUT SExternalMsg *out
	, eMessageOption::Enum opt // = eMessageOption::REMOVE
) 
{
	if (!out)
		return false;

	bool reval;
	{
		AutoCSLock cs(m_msgCS);
		if (m_externalMsgs.empty())
		{
			reval = false;
		}
		else
		{
			*out = m_externalMsgs.front();
			if (eMessageOption::REMOVE == opt)
				m_externalMsgs.pop_front();
			reval = true;
		}
	}
	return reval;
}


//------------------------------------------------------------------------
// 
//------------------------------------------------------------------------
bool cThread::AddTask(cTask *task)
{
	RETV(!task, false);

	AutoCSLock cs(m_containerCS);
	task->m_pThread = this;
	m_addTasks.push_back( task );
	return true;
}


//------------------------------------------------------------------------
// 
//------------------------------------------------------------------------
bool cThread::RemoveTask(const int id)
{		
	AutoCSLock cs(m_containerCS);
	m_removeTasks.push_back(id);
	return true;
}


int cThread::GetTaskCount()
{
	AutoCSLock cs(m_taskCS);
	return m_tasks.size();
}


/**
@brief  taskId에 해당하는 task를 리턴.
*/
cTask*	cThread::GetTask(const int taskId)
{
	//AutoCSLock cs(m_taskCS); 동기화 하지 않는다.
	auto it = find_if(m_tasks.begin(), m_tasks.end(), IsTask(taskId));
	if (m_tasks.end() == it)
		return NULL; // 없다면 실패
	return *it;
}


//------------------------------------------------------------------------
// 동적으로 생성된 클래스 제거
//------------------------------------------------------------------------
void cThread::Clear()
{
	{
		AutoCSLock cs(m_taskCS);
		auto it = m_tasks.begin();
		while (m_tasks.end() != it)
		{
			cTask *p = *it++;
			delete p;
		}
		m_tasks.clear();
	}

	{
		AutoCSLock cs(m_containerCS);
		for (auto &p : m_addTasks)
			delete p;
		m_addTasks.clear();
	}

	{
		AutoCSLock cs2(m_msgCS);
		m_threadMsgs.clear();
		m_externalMsgs.clear();
	}

	CloseHandle(m_hThread);
}


//------------------------------------------------------------------------
// 쓰레드 실행
// Task를 실행시킨다.
//------------------------------------------------------------------------
void cThread::Run()
{
	while (eState::RUN == m_state)
	{
		//1. Add & Remove Task
		UpdateTask();

		if (m_tasks.empty()) // break no task
			break;

		//2. Task Process
		{
			AutoCSLock cs(m_taskCS);
			auto it = m_tasks.begin();
			while (m_tasks.end() != it)
			{
				cTask *task = *it;
				if (cTask::eRunResult::END == task->Run())
				{
					// finish task , remove taks
					it = m_tasks.erase(it);
					delete task;
				}
				else
				{
					++it;
				}
			}
		}

		//3. Message Process
		DispatchMessage();

		Sleep(1);
	}

	// 남았을지도 모를 메세지를 마지막으로 처리한다.
	DispatchMessage();
}


//------------------------------------------------------------------------
// call exit thread
//------------------------------------------------------------------------
void	cThread::Exit()
{
	m_state = eState::END;
}


void cThread::UpdateTask()
{
	AutoCSLock cs(m_containerCS);
	for (auto &p : m_addTasks)
	{
		auto it = find_if(m_tasks.begin(), m_tasks.end(), IsTask(p->m_Id));
		if (m_tasks.end() == it) // not exist
			m_tasks.push_back(p);
		else
			assert(0); // already exist
	}
	m_addTasks.clear();

	for (auto &id : m_removeTasks)
	{
		auto it = find_if(m_tasks.begin(), m_tasks.end(), IsTask(id));
		if (m_tasks.end() != it)
		{
			cTask *p = *it;
			m_tasks.remove_if(IsTask(id));
			delete p;
		}
	}
	m_removeTasks.clear();
}


//------------------------------------------------------------------------
// 저장된 메세지들을 태스크로 보낸다.
//------------------------------------------------------------------------
void cThread::DispatchMessage()
{
	AutoCSLock cs(m_msgCS);
	auto it = m_threadMsgs.begin();
	while (m_threadMsgs.end() != it)
	{
		if (threadmsg::TASK_MSG == it->msg) // task message
		{
			{
				AutoCSLock cs(m_taskCS);
				auto t = find_if(m_tasks.begin(), m_tasks.end(), 
					boost::bind( &IsSameId<cTask>, _1, it->wParam) );
				if (m_tasks.end() != t)
				{
					(*t)->MessageProc((threadmsg::MSG)it->msg, it->wParam, it->lParam, it->added);
				}
				else
				{
					dbg::ErrLog("cThread::DispatchMessage Not Find Target Task\n");
				}
			}
		}
		else // Thread에게 온 메세지
		{
			MessageProc((threadmsg::MSG)it->msg, it->wParam, it->lParam, it->added);
		}
		++it;
	}
	m_threadMsgs.clear();
}


//------------------------------------------------------------------------
// Message Process
//------------------------------------------------------------------------
void	cThread::MessageProc( threadmsg::MSG msg, WPARAM wParam, LPARAM lParam, LPARAM added )
{
	switch (msg)
	{
	case threadmsg::TERMINATE_TASK:
		{
			// terminate task of id wParam
			{
				AutoCSLock cs(m_taskCS);
				auto it = std::find_if( m_tasks.begin(), m_tasks.end(), 
					bind( &IsSameId<common::cTask>, _1, (int)wParam) );
				if (m_tasks.end() != it)
				{
					delete *it;
					m_tasks.erase(it);
				}
			}
		}
		break;
	}
}


bool cThread::IsRun()
{
	return m_state == eState::RUN;
}
