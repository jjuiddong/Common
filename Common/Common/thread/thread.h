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
		cThread(const std::string &name="");
		virtual ~cThread();

		enum MSG_OPT
		{
			MSG_REMOVE,	// 메세지 제거
			MSG_KEEP,			// 메세지 보존
		};
		enum STATE
		{
			WAIT,			// 대기중
			RUN,			// 실행중
			END,			// 종료
		};

		STATE			GetState() const;
		const char* GetName() const;
		HANDLE		GetHandle() const;

		void			Start();				// 쓰레드 실행
		void			Terminate(const int milliSeconds=-1);		// 쓰레드 종료

		void			Send2ThreadMessage( threadmsg::MSG msg, WPARAM wParam, LPARAM lParam, LPARAM added=0);
		void			Send2ExternalMessage( int msg, WPARAM wParam, LPARAM lParam, LPARAM added=0 );
		bool			GetThreadMsg( OUT SExternalMsg *pMsg, MSG_OPT opt = MSG_REMOVE );
		bool			GetExternalMsg( OUT SExternalMsg *pMsg, MSG_OPT opt = MSG_REMOVE );
		
		bool			AddTask(cTask *pTask);
		bool			RemoveTask(cTask *pTask);
		int			GetTaskCount();
		cTask*		GetTask(const int taskId);
		void			Clear();
		void			Run();
		void			Exit(); // call exit thread

		// Overriding
		virtual void	MessageProc( threadmsg::MSG msg, WPARAM wParam, LPARAM lParam, LPARAM added );

	protected:
		void			DispatchMessage();

	public:
		STATE m_State;
		HANDLE m_hThread;
		char m_Name[ 32];

		/// 외부 쓰레드에서 접근하기 때문에 동기화 코드가 들어가 있다.
		CriticalSection m_TaskCS;
		TaskList m_Tasks;
		CriticalSection m_MsgCS;
		ExternalMsgList m_ThreadMsgs;				/// 쓰레드가 받은 메세지
		ExternalMsgList m_ExternalMsgs;				/// 쓰레드에서 외부로 보낸 메세지
	};

	inline cThread::STATE	cThread::GetState() const { return m_State; }
	inline const char* cThread::GetName() const { return m_Name; }
	inline HANDLE cThread::GetHandle() const { return m_hThread; }

}
