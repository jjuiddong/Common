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
// 2017-10-31
//		- Pause, Resume, Optimize Loop
//		- list -> vector
//2018-06-19
//		- Max Task
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

		cThread(const StrId &name="", const int maxTask=-1, iMemoryPool3Destructor *memPool=NULL);
		virtual ~cThread();

		void Start();
		bool Pause();
		bool Resume();
		void Terminate(const int milliSeconds=-1);
		void Send2ThreadMessage( threadmsg::MSG msg, WPARAM wParam, LPARAM lParam, LPARAM added=0);
		void Send2ExternalMessage( int msg, WPARAM wParam, LPARAM lParam, LPARAM added=0 );
		bool GetThreadMsg( OUT SExternalMsg *pMsg, eMessageOption::Enum opt = eMessageOption::REMOVE );
		bool GetExternalMsg( OUT SExternalMsg *pMsg, eMessageOption::Enum opt = eMessageOption::REMOVE );
		bool AddTask(cTask *pTask);
		bool RemoveTask(const int id);
		int GetTaskCount();
		cTask* GetTask(const int taskId);
		void Clear();
		bool IsRun();
		void Join();
		int Run();
		void Exit(); // call exit thread

		virtual void MessageProc( threadmsg::MSG msg, WPARAM wParam, LPARAM lParam, LPARAM added );


	protected:
		void DispatchMessage();
		void UpdateTask();
		void ReleaseTask(cTask *task);


	public:
		struct eState {enum Enum { WAIT, RUN, PAUSE, IDLE, END, };};
		eState::Enum m_state;
		StrId m_name;
		int m_maxTask; // default: -1 (infinity)
		CriticalSection m_msgCS;
		CriticalSection m_containerCS;
		int m_procTaskIndex;
		vector<cTask*> m_tasks;
		vector<cTask*> m_addTasks;
		vector<int> m_removeTasks;
		iMemoryPool3Destructor *m_memPoolTask; // task memory pool destructor
		list<SExternalMsg> m_threadMsgs;	// receive message
		list<SExternalMsg> m_externalMsgs;	// send to another process message

		std::thread m_thread;
		//cMutex m_mutex; // 쓰레드 Run, Pause 동기화 객체
		CriticalSection m_taskCS;
	};

}
