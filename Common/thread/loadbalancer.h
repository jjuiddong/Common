//
// 2021-09-28, jjuiddong
// load balancer
//	- load balance by task count
//
#pragma once


namespace common
{

	class cLoadBalancer
	{
	public:
		cLoadBalancer();
		virtual ~cLoadBalancer();

		bool Init(const int threadCount = -1);
		bool PushTask(cTask *task);
		bool RemoveTask(const StrId &taskName);
		bool RemoveTask(const int taskId);
		bool IsInit();
		void Terminate();
		void Clear();


	public:
		struct sMsg
		{
			enum eType { StartTask, RemoveTask };
			eType type;
			StrId name;
			cTask *task;
			int id;
		};

		struct sThreadContext
		{
			StrId name; // thread name
			queue<sMsg> msgs; // message
			std::vector<cTask*> tasks;
			std::atomic<uint> count; // task count, fast sync
			CriticalSection cs;
			std::thread *thr;
		};

		static int ThreadFunction(cLoadBalancer *tpSemaphore, sThreadContext *ctx);


	public:
		bool m_isThreadLoop;
		vector<sThreadContext*> m_ctxs;
	};

}
