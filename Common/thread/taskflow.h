//
// 2025-05-06, jjuiddong
// task flow
//	- execute hierarchy task
//
#pragma once


namespace common
{

	//----------------------------------------------------------------------------
	// TaskFlow Task
	class cTaskFlow;
	class cTfTask
	{
	public:
		friend class cTaskFlow;
		cTfTask(int id, const string& name = "");
		virtual ~cTfTask() { Clear(); }
		virtual bool Process() { return true; }
		virtual bool Complete() { return true; }
		virtual bool FinishChild(cTfTask *task) { return true; }
		virtual void Clear() {}
		bool IsFinish();
		bool IsAllChildSuccess();
		bool CloseTask();
		bool CloseChildTask(cTfTask* childTask);
		bool CloseAllChildTask();


	protected:
		bool PushChildTask(cTfTask* task);
		int FindTaskIndex(cTfTask* childTask);
		bool FinishChildTask(cTfTask* childTask);


	public:
		// task state, Process state? or complete state?
		enum class eState {Process, Complete};

		eState m_state;
		int m_id;
		string m_name;
		cTfTask* m_parent; // parent task
		cTaskFlow* m_tf; // reference
		int m_result; // task result, 0:fail, 1:success
		int m_syncVal; // synchronize value. default:0
		bool m_isShutdown; // shutdown?
		vector<std::pair<bool, cTfTask*>> m_children; // child task, first: finish?
	};

	inline cTfTask::cTfTask(int id, const string& name)
		: m_state(eState::Process), m_id(id), m_name(name), m_isShutdown(false)
		, m_result(0), m_tf(nullptr), m_parent(nullptr), m_syncVal(0)
	{
	}



	//----------------------------------------------------------------------------
	// TaskFlow
	class cTaskFlow
	{
	public:
		friend class cTfTask;
		cTaskFlow();
		virtual ~cTaskFlow();

		bool Init(const int threadCount = -1, const bool isSequence = true);
		bool PushTask(cTfTask* task);
		cTfTask* PopTask();
		bool CloseTask(const string& taskName);
		bool CloseTask(const int taskId);
		bool CloseAllTask();
		bool IsInit();
		void Join();
		void Terminate();
		void Clear(const int timeOut = 0);


	protected:
		bool ExecuteTask(cTfTask* task);
		bool ExecuteChildTask(cTfTask* parent);
		bool NextStep(cTfTask* task);
		static int ThreadFunction(cTaskFlow* tf);


	public:
		bool m_isSequence; // sequence process task?, default:true
		std::deque<cTfTask*> m_tasks; // execute queue (child task)
		std::deque<cTfTask*> m_waitQ; // executed root task (prevent insert task while running task)
		std::deque<cTfTask*> m_readyQ; // ready task queue (wait until finish execute-queue)
		vector<std::thread*> m_threads; // thread pool
		CriticalSection m_cs; // m_tasks synchronization
		CriticalSection m_readyCs; // m_readyQ synchronization
		CriticalSection m_finCs; // task finish synchronization
		CriticalSection m_rmCs; // remove task synchronization
		cSemaphore m_sema;
		bool m_isThreadLoop;
	};

}
