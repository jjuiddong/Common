//
// 2019-02-06, jjuiddong
// Wait Queue Semaphore
//
// book: IT EXPERT 윈도우 시스템 프로그램을 구현하는 기술
//	- Chapter 2.2.2
//		- WQSemaphore
// 
#pragma once


namespace common
{

	class cWQSemaphore
	{
	public:
		cWQSemaphore();
		virtual ~cWQSemaphore();

		bool PushTask(cTask *task);
		cTask* PopTask();
		void Wait();
		void Terminate();
		void Clear();

		static int ThreadFunction(cWQSemaphore *wqSemaphore);


	public:
		CriticalSection m_cs;
		cSemaphore m_sema;
		queue<cTask*> m_tasks;
		std::thread m_thread;
		bool m_isThreadLoop;
	};

}
