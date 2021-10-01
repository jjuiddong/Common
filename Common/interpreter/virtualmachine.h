//
// 2019-10-29, jjuiddong
// excute script intermediate code
//
// 2021-05-18
//	- add array, map type
//
#pragma once


namespace common
{
	namespace script
	{

		class cVirtualMachine
		{
		public:
			struct sRegister
			{
				uint idx; // instruction index (next execute instruction)
				uint exeIdx; // executed instruction index
				bool cmp; // compare output flag
				float tim; // delay timer (milliseconds unit)
				vector<variant_t> val; // register
				
				sRegister() { 
					val.resize(10);
				}
			};

			struct sTimer
			{
				int id; // unique timer id
				StrId name;
				float interval; // seconds unit
				float t; // decrease time, seconds unit
			};

			cVirtualMachine(const string &name);
			virtual ~cVirtualMachine();

			bool Init(const cIntermediateCode &code, iFunctionCallback *callback
				, void *arg = nullptr);
			bool Process(const float deltaSeconds);
			bool Run();
			bool Resume();
			bool Stop();
			bool PushEvent(const cEvent &evt);
			bool StopTimer(const int timerId);
			void SetCodeTrace(const bool isCodeTrace);
			void ClearCodeTrace(const bool isTakeLast=false);
			void Clear();


		protected:
			bool ProcessEvent(const float deltaSeconds);
			bool ProcessTimer(const float deltaSeconds);
			bool ExecuteInstruction(const float deltaSeconds, sRegister &reg);
			void CodeTraceLog(vector<ushort> &trace);
			void WriteTraceLog(const vector<ushort> &trace);


		public:
			enum class eState { 
				Stop  // waitting for start
				, Run // execute instruction
				, WaitCallback // wait until finish callback function
				, Wait // no operation state, wait until event trigger
			};

			int m_id; // unique id
			eState m_state;
			string m_name;
			sRegister m_reg;
			cSymbolTable m_symbTable;
			cIntermediateCode m_code;
			queue<cEvent> m_events;
			vector<sTimer> m_timers;
			vector<int> m_stack; // simple integer stack (save return address)
			iFunctionCallback *m_callback; // customize function interface
			void *m_callbackArgPtr; // callback function argument

			// debugging
			bool m_isCodeTraceLog;
			vector<ushort> m_trace; // executed code index log (pair data)
									// 0-10,15-20,101-101
			bool m_isDebugging; // special purpose flag, dev debugging
			vector<ushort> m_trace2; // executed code index log (pair data)
									// 0-10,15-20,101-101
		};

	}
}
