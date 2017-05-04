//------------------------------------------------------------------------
// Name:    Thread.h
// Author:  jjuiddong
// Date:    2012-12-02
// 
// 쓰레드 라이브러리
// CTask 를 실행한다.
//
// 2016-06-06
//		- refactoring
// 2017-05-03
//		- refactoring
//
//------------------------------------------------------------------------
#pragma once

#include "threaddef.h"


namespace common
{

	class cTask;
	class cThread
	{
	public:
		struct eMessageOption { 
			enum Enum { REMOVE, KEEP, };
		};
		struct eState {
			enum Enum { WAIT, RUN, END, };
		};

		cThread(const std::string &name="");
		virtual ~cThread();

		void Start();
		void Terminate(const int milliSeconds=-1);
		void Send2ThreadMessage( threadmsg::MSG msg, WPARAM wParam, LPARAM lParam, LPARAM added=0);
		void	Send2ExternalMessage( int msg, WPARAM wParam, LPARAM lParam, LPARAM added=0 );
		bool	GetThreadMsg( OUT SExternalMsg *pMsg, eMessageOption::Enum opt = eMessageOption::REMOVE );
		bool	GetExternalMsg( OUT SExternalMsg *pMsg, eMessageOption::Enum opt = eMessageOption::REMOVE );
		bool	AddTask(cTask *pTask);
		bool	RemoveTask(const int id);
		int GetTaskCount();
		cTask* GetTask(const int taskId);
		void Clear();
		bool IsRun();
		void Run();
		void Exit(); // call exit thread

		virtual void MessageProc( threadmsg::MSG msg, WPARAM wParam, LPARAM lParam, LPARAM added );


	protected:
		void DispatchMessage();
		void UpdateTask();


	public:
		eState::Enum m_state;
		string m_name;
		HANDLE m_hThread;
		CriticalSection m_taskCS;
		CriticalSection m_msgCS;
		CriticalSection m_containerCS;
		list<cTask*> m_tasks;
		vector<cTask*> m_addTasks;
		vector<int> m_removeTasks;
		list<SExternalMsg> m_threadMsgs;	// receive message
		list<SExternalMsg> m_externalMsgs;	// send to another process message
	};

}
