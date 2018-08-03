
#include "stdafx.h"
#include "Thread.h"
#include "Task.h"
#include <boost/bind.hpp>
#include <chrono>
#include <MMSystem.h>


namespace common
{
	unsigned __stdcall ThreadProcess(void *pThreadPtr )
	{
		CoInitialize(NULL); // for COM interface

		cThread *pThread = (cThread*)pThreadPtr;
		pThread->Run();
		pThread->Exit();
		return 0;
	}

	template<class T>
	static bool IsSameId(T *p, int id)
	{
		if (!p) return false;
		return p->m_id == id;
	}
}
using namespace common;


cThread::cThread(const StrId &name
	, const int maxTask //= -1
	, iMemoryPool3Destructor *memPool //= NULL
) 
	: m_state(eState::WAIT)
	, m_name(name)
	, m_procTaskIndex(0)
	, m_maxTask(maxTask)
	, m_memPoolTask(memPool)
	, m_sortObj(NULL)
{
	m_tasks.reserve(32);
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
	if ((eState::WAIT == m_state) || (eState::END == m_state))
	{
		if (eState::END != m_state)
		{
			Terminate(INFINITE);
		}
		else
		{
			if (m_thread.joinable())
				m_thread.join();
		}

		m_state = eState::RUN;
		m_thread = std::thread(ThreadProcess, this);
	}
}


// 쓰레드를 일시 정지 시킨다.
bool cThread::Pause()
{
	if (eState::RUN == m_state)
	{
		m_taskCS.Lock();
		m_state = eState::PAUSE;
		return true;
	}
	return false;
}


// 쓰레드를 재 실행한다. Pause 상태일때만 가능하다.
bool cThread::Resume()
{
	if ((eState::PAUSE == m_state) || (eState::IDLE == m_state))
	{
		m_taskCS.Unlock();
		m_state = eState::RUN;
		return true;
	}
	return false;
}


//------------------------------------------------------------------------
// 쓰레드 종료
//------------------------------------------------------------------------
void cThread::Terminate(const int milliSeconds) //milliSeconds = -1
{
	m_state = eState::END;
	DWORD timeOutTime = (milliSeconds>=0)? milliSeconds : INFINITE;
	if (m_thread.joinable())
		m_thread.join();
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


// m_tasks, m_addTasks sorting
// 인자로 넘어온 compareFn 는 동적으로 생성된 (new) 객체여야 한다.
// 소팅이 끝나고 compareFn 객체는 제거된다. (delete)
// 멀티 쓰레딩으로 동작하기 때문에, 동기화를 위해 이렇게 처리했다.
void cThread::SortTasks(iSortingTasks *sortObj)
{	
	AutoCSLock cs(m_containerCS);

	SAFE_DELETE(m_sortObj);
	m_sortObj = sortObj;
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
// 쓰레드 실행
// Task를 실행시킨다.
//------------------------------------------------------------------------
int cThread::Run()
{
	using namespace std::chrono_literals;

	cTimer timer;
	timer.Create();

	while ((eState::RUN == m_state)
		|| (eState::PAUSE == m_state))
	{
		m_taskCS.Lock();

		//1. Add & Remove Task
		UpdateTask();

		if (m_tasks.empty()) // break no task
			break;

		//2. Task Process
		if ((eState::RUN == m_state) && !m_tasks.empty())
		{
			if ((int)m_tasks.size() <= m_procTaskIndex)
				m_procTaskIndex = 0;

			const double dt = (float)timer.GetDeltaSeconds();
			int taskProcCnt = 0;

			do
			{
				++taskProcCnt;
				cTask *task = m_tasks[m_procTaskIndex];
				if (cTask::eRunResult::END == task->Run(dt))
				{
					// finish task , remove taks
					common::rotatepopvector(m_tasks, m_procTaskIndex);
					ReleaseTask(task);
				}
				else
				{
					++m_procTaskIndex;
				}

			} while (((eState::RUN == m_state)
				//|| (eState::PAUSE == m_state)
				)
				&& (m_procTaskIndex < (int)m_tasks.size())
				&& (taskProcCnt < 5)				
				);
		}

		//3. Message Process
		DispatchMessage();

		m_taskCS.Unlock();
	}

	m_taskCS.Unlock();

	// 남았을지도 모를 메세지를 마지막으로 처리한다.
	DispatchMessage();

	return 1;
}


//------------------------------------------------------------------------
// call exit thread
//------------------------------------------------------------------------
void cThread::Exit()
{
	m_state = eState::END;
}


void cThread::UpdateTask()
{
	AutoCSLock cs1(m_containerCS);

	// Sorting
	if (m_sortObj)
	{
		m_sortObj->Sorting(m_tasks);
		m_sortObj->Sorting(m_addTasks);
		SAFE_DELETE(m_sortObj);
	}

	for (auto &p : m_addTasks)
	{
		auto it = find_if(m_tasks.begin(), m_tasks.end(), IsTask(p->m_id));
		if (m_tasks.end() == it) // not exist
		{
			if (m_maxTask < 0) // infinity
			{
				if (p->m_isTopPriority)
				{
					m_tasks.push_back(p);
					common::rotateright(m_tasks); // 오른쪽으로 회전시켜, 추가한것이 가장 앞으로 오게한다.
				}
				else
				{
					m_tasks.push_back(p);
				}
			}
			else
			{
				if ((int)m_tasks.size() >= m_maxTask)
				{
					dbg::Logp("Err Over Maximize Task Count\n");

					// remove most old task
					auto &task = m_tasks.back();
					ReleaseTask(task);
					m_tasks.pop_back();
				}

				if (p->m_isTopPriority)
				{
					m_tasks.push_back(p);
					common::rotateright(m_tasks); // 오른쪽으로 회전시켜, 추가한것이 가장 앞으로 오게한다.
				}
				else
				{
					m_tasks.push_back(p);
				}
			}
		}
		else
		{
			assert(0); // already exist
		}
	}
	m_addTasks.clear();

	for (auto &id : m_removeTasks)
	{
		auto it = find_if(m_tasks.begin(), m_tasks.end(), IsTask(id));
		if (m_tasks.end() != it)
		{
			cTask *p = *it;
			m_tasks.erase(it);
			ReleaseTask(p);
		}
	}
	m_removeTasks.clear();
}


void cThread::ReleaseTask(cTask *task)
{
	RET(!task);

	if (m_memPoolTask)
	{
		task->Clear();
		m_memPoolTask->Free(task);
	}
	else
	{
		delete task;
	}
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
void cThread::MessageProc( threadmsg::MSG msg, WPARAM wParam, LPARAM lParam, LPARAM added )
{
	switch (msg)
	{
	case threadmsg::TERMINATE_TASK:
		{
			// terminate task of id wParam
			auto it = std::find_if( m_tasks.begin(), m_tasks.end(), 
				bind( &IsSameId<common::cTask>, _1, (int)wParam) );
			if (m_tasks.end() != it)
			{
				ReleaseTask(*it);
				m_tasks.erase(it);
			}
		}
		break;
	}
}


bool cThread::IsRun()
{
	switch (m_state)
	{
	case eState::RUN:
	case eState::PAUSE:
	case eState::IDLE:
		return true;
	}
	return false;
}


void cThread::Join()
{
	if (IsRun() && m_thread.joinable())
		m_thread.join();
}


//------------------------------------------------------------------------
// 쓰레드 종료, 동적으로 생성된 변수 제거
//------------------------------------------------------------------------
void cThread::Clear()
{
	Terminate(INFINITE);

	{
		AutoCSLock cs(m_containerCS);
		for (auto &p : m_addTasks)
			ReleaseTask(p);
		SAFE_DELETE(m_sortObj);
		m_addTasks.clear();
	}

	auto it = m_tasks.begin();
	while (m_tasks.end() != it)
	{
		cTask *p = *it++;
		ReleaseTask(p);
	}
	m_tasks.clear();

	{
		AutoCSLock cs2(m_msgCS);
		m_threadMsgs.clear();
		m_externalMsgs.clear();
	}
}
