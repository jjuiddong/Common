//
// 2019-10-29, jjuiddong
// script interpreter main header
//
// 2021-05-05
//	- cDebugger + cInterpreter
//
#pragma once


namespace common
{
	namespace script
	{

		class cVirtualMachine;

		class cInterpreter
		{
		public:
			cInterpreter();
			virtual ~cInterpreter();

			bool Init(iFunctionCallback *callback, void *arg = nullptr);
			bool ReadIntermediateCode(const StrPath &icodeFileName);
			bool Process(const float deltaSeconds);
			bool Run();
			bool Stop();
			bool DebugRun();
			bool StepRun();
			bool Resume();
			bool ResumeVM(const StrId &vmName);
			bool OneStep();
			bool Break();
			bool BreakPoint(const bool enable, const uint id);
			bool PushEvent(const cEvent &evt);
			bool IsRun() const;
			bool IsStop() const;
			bool IsDebug();
			bool IsBreak();
			void Clear();


		protected:
			bool RunProcess(const float deltaSeconds);
			bool DebugProcess(const float deltaSeconds);
			int ProcessUntilNodeEnter(const float deltaSeconds);
			bool CheckBreakPoint();
			bool InitAndRunVM();


		public:
			enum class eState {Stop, Run, DebugRun};
			enum class eRunState {Stop, Run};
			enum class eDebugState {
				Stop  // no load code
				, Wait // load code, wait operation
				, Step // one step execute instruction, and move DebugWait state
				, Break // meet breakpoint, next frame change Wait state
				, Run // run until break
			};

			eState m_state;
			eRunState m_runState;
			eDebugState m_dbgState;
			StrPath m_fileName; // intermediate code filename
			cIntermediateCode m_code;
			queue<cEvent> m_events;
			vector<cVirtualMachine*> m_vms;
			iFunctionCallback *m_callback;
			void *m_callbackArgPtr;
			float m_dt;
			set<uint> m_breakPoints;
		};

	}
}
