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

			bool Init();
			int LoadIntermediateCode(const StrPath &icodeFileName);
			int LoadIntermediateCode(const cIntermediateCode &icode);
			bool AddModule(iModule *mod);
			bool RemoveModule(iModule *mod);
			bool Process(const float deltaSeconds);
			bool Run(const int vmId);
			bool Stop(const int vmId);
			bool DebugRun(const int vmId);
			bool StepRun(const int vmId);
			bool Resume(const int vmId);
			bool OneStep(const int vmId);
			bool Break(const int vmId);
			bool BreakPoint(const bool enable, const int vmId, const uint id);
			bool PushEvent(const int vmId, const cEvent &evt);
			void SetDebugVM(const int vmId);
			void SetCodeTrace(const bool isTrace);
			void SetICodeStepDebug(const bool isICodeStep);
			bool IsRun() const;
			bool IsDebugRun() const;
			bool IsStop() const;
			bool IsDebug();
			bool IsBreak();
			void Clear();

			cVirtualMachine* GetVM(const int vmId);


		protected:
			bool RunProcess(const float deltaSeconds);
			bool DebugProcess(const float deltaSeconds);
			int ProcessUntilNodeEnter(const int vmId, const float deltaSeconds);
			bool CheckBreakPoint();
			bool RunVM(const int vmId);
			int InitVM(const cIntermediateCode &icode);


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
			deque<cEvent> m_events;
			vector<cVirtualMachine*> m_vms;
			vector<iModule*> m_modules; // execute function module, reference
			float m_dt;
			set<std::pair<int, uint>> m_breakPoints; // first:vmId, second:node id
			int m_dbgVmId; // debug virtual machine id
			bool m_isCodeTrace; // vm code trace?
			bool m_isICodeStep; // one intermediate code step trace?
		};

	}
}
