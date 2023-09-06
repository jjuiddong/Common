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

		// interpreter response interface		
		interface iInterpreterResponse 
		{
			// terminate virtual machine listener interface
			virtual void TerminateResponse(const int vmId) = 0;

			// start timeout response
			virtual void SetTimeOutResponse(const int vmId, const string &scopeName, const int timerId, const int time) {}
			// clear timeout response
			virtual void ClearTimeOutResponse(const int vmId, const int timerId, const int id) {}

			// start interval response
			virtual void SetIntervalResponse(const int vmId, const string& scopeName, const int timerId, const int time) {}
			// clear interval  response
			virtual void ClearIntervalResponse(const int vmId, const int timerId, const int id) {}

			// start sync timeout response
			virtual void SyncTimeOutResponse(const int vmId, const string& scopeName, const int timerId, const int time) {}
			// clear sync timeout response
			virtual void ClearSyncTimeOutResponse(const int vmId, const int timerId, const int id) {}

			// error occurred!
			virtual void ErrorVM(const int vmId, const string& msg) {}
		};


		class cVirtualMachine;

		class cInterpreter
		{
		public:
			cInterpreter();
			virtual ~cInterpreter();

			bool Init();
			int LoadIntermediateCode(const StrPath &icodeFileName
				, const int parentVmId = -1
				, const string& scopeName = "");
			int LoadIntermediateCode(const cIntermediateCode &icode
				, const int parentVmId = -1
				, const string& scopeName = "");
			bool AddModule(iModule *mod);
			bool RemoveModule(iModule *mod);
			bool Process(const float deltaSeconds);
			bool Run(const int vmId, const script::cSymbolTable& symbTable = {}
				, const string& startEvent = "Start Event");
			bool DebugRun(const int vmId, const script::cSymbolTable& symbTable = {}
				, const string& startEvent = "Start Event");
			bool StepRun(const int vmId, const script::cSymbolTable& symbTable = {}
				, const string& startEvent = "Start Event");
			bool Stop(const int vmId);
			bool Terminate(const int vmId, const script::cSymbolTable& symbTable = {}
				, const string &eventName = "_Exit");
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
			cVirtualMachine* GetRemoveVM(const int vmId);
			bool SetListener(iInterpreterResponse* listener);


		protected:
			bool RunProcess(const float deltaSeconds);
			bool DebugProcess(const float deltaSeconds);
			int ProcessUntilNodeEnter(const int vmId, const float deltaSeconds);
			bool CheckBreakPoint();
			bool RunVM(const int vmId, const script::cSymbolTable& symbTable = {}
				, const string &startEvent = "Start Event");
			int InitVM(const cIntermediateCode &icode, const int parentVmId = -1
				, const string& scopeName = "");


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
			vector<cVirtualMachine*> m_vms; // execute virtual machines
			vector<cVirtualMachine*> m_rmVms; // remove virtual machines
			iInterpreterResponse* m_listener;
			vector<iModule*> m_modules; // execute function module, reference
			float m_dt;
			set<std::pair<int, uint>> m_breakPoints; // first:vmId, second:node id
			int m_dbgVmId; // debug virtual machine id
			bool m_isCodeTrace; // vm code trace?
			bool m_isICodeStep; // one intermediate code step trace?
		};

	}
}
