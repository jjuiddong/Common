//
// 2019-02-06, jjuiddong
// Thread Pool Semaphore
//
// book: IT EXPERT 윈도우 시스템 프로그램을 구현하는 기술
//	- Chapter 2.2.2
//		- TPSemaphore
//
#pragma once


namespace common
{

	class cTPSemaphore
	{
	public:
		cTPSemaphore();
		virtual ~cTPSemaphore();

		bool Init(const int threadCount = -1);
		bool PushTask(cTask *task);
		cTask* PopTask();
		void Wait();
		void Terminate();
		void Clear();

		static int ThreadFunction(cTPSemaphore *tpSemaphore);


	public:
		CriticalSection m_cs;
		cSemaphore m_sema;
		queue<cTask*> m_tasks;
		vector<std::thread*> m_threads;
		bool m_isThreadLoop;
	};

}
