//
// 2019-10-29, jjuiddong
// virtual machine
//	- excute script intermediate code
//
// 2021-05-18
//	- add array, map type
//
#pragma once


namespace common
{
	namespace script
	{

		class cInterpreter;
		class cVirtualMachine
		{
		public:
			struct sRegister
			{
				uint idx; // instruction index (next execute instruction)
				uint exeIdx; // executed instruction index
				bool cmp; // compare output flag
				float tim; // delay timer (milliseconds unit)
				vector<variant_t> reg; // register[10]
				
				sRegister() : idx(0), exeIdx(0), cmp(false), tim(0.f) {
					reg.resize(10);
				}
			};

			struct sTimer
			{
				int id; // unique timer id
				StrId name;
				float interval; // seconds unit
				float t; // decrease time, seconds unit
				bool isLoop;
				int syncId; // sync timeout, syncId, default:-1
			};

			// synchronize execute flow
			struct sSync
			{
				int id; // unique id
				int type; // 0:Sync, 1:SyncOrder
				bool enable; // enable sync?
				int timerId; // timer id
				vector<std::pair<int, bool>> syncs; // first: pin index, second:sync?
			};

			cVirtualMachine(const string &name);
			virtual ~cVirtualMachine();

			bool Init(cInterpreter* interpreter, const cIntermediateCode &code
				, const int parentVmId = -1, const string &scopeName ="");
			bool AddModule(iModule *mod);
			bool RemoveModule(iModule *mod);
			bool Process(const float deltaSeconds);
			bool Run();
			bool Resume();
			bool Stop();
			bool PushEvent(const cEvent &evt);
			int SetTimer(const string &name, const int timeMillis
				, const bool isLoop = false
				, const int syncId = -1);
			bool StopTimer(const int timerId);
			bool StopTick(const int tickId);
			void SetCodeTrace(const bool isCodeTrace);
			bool EnableNetworkSync(const bool enable);
			void ClearCodeTrace(const bool isTakeLast=false, const int removeSize=-1);
			void Clear();


		protected:
			bool ProcessEvent(const float deltaSeconds);
			bool ProcessDelayEvent(const float deltaSeconds);
			bool ProcessTimer(const float deltaSeconds);
			bool ExecuteInstruction(const float deltaSeconds, sRegister &reg);
			void CodeTraceLog(vector<ushort> &trace);
			void WriteTraceLog(const vector<ushort> &trace);
			bool IsAssignable(const VARTYPE srcVarType, const VARTYPE dstVarType);
			bool InitSyncTimer(const int syncId, const int timeOut);
			bool InitSyncFlow(const int syncId, const int pinCount);
			bool InitSyncOrderFlow(const int syncId, const int pinCount);
			bool CheckSync(const int syncId, const int pinIdx);
			void ClearNSync(const bool enable = false);


		public:
			enum class eState { 
				Stop  // waiting for start
				, Run // execute instruction
				, WaitCallback // wait until finish callback function
				, Wait // no operation state, wait until event trigger
			};

			int m_id; // unique id
			int m_parentId; // parent vmId
			eState m_state;
			string m_name;
			string m_scopeName; // execute command scopeName
			cInterpreter* m_itpr; // manage this vm interpreter reference
			sRegister m_reg;
			cSymbolTable m_symbTable;
			cIntermediateCode m_code;
			common::cCircularQueue2<cEvent> m_events;
			vector<cEvent> m_delayEvents;
			vector<sTimer> m_ticks;
			vector<sTimer> m_timers;
			vector<sSync> m_syncs;
			vector<int> m_stack; // simple integer stack (save return address)
			vector<iModule*> m_modules; // execute function module, reference

			// debugging
			bool m_isCodeTraceLog;
			vector<ushort> m_trace; // executed code index log (pair data)
									// 0-10,15-20,101-101
			bool m_isDebugging; // special purpose flag, dev debugging
			vector<ushort> m_trace2; // executed code index log (pair data)
									// 0-10,15-20,101-101

			
			// nsync, VM network synchronize
			struct sSymbol // network synchronize changed variable
			{
				string name; // symbol name
				float t; // sync time
				common::script::sVariable var; // compare variable
			};
			struct sNetworkSync // network synchronize variable
			{
				bool enable; // enable network synchronize?
				uint sync; // instant sync, 0x01:register, 0x02:instruction, 0x04:symbol
				bool regStreaming; // send register synchronize info?
				bool instStreaming; // send instruction streaming info?
				bool symbStreaming; // send symboltable synchronize info?
				float regTime; // register sync time, seconds unit
				float instTime; // instruction sync time, seconds unit
				float symbTime; // symboltable sync time, seconds unit
				map<string, sSymbol> chSymbols; // check change variable
												// key: variable name = scopeName + varname
			};
			sNetworkSync m_nsync;
			//~nsync
		};

	}
}
