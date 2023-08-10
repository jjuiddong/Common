//
// 2019-11-04, jjuiddong
// interpreter debugger
//
#pragma once


namespace common
{
	namespace script
	{

		class cInterpreter;

		class cDebugger
		{
		public:
			cDebugger(cInterpreter *interpreter = nullptr);
			virtual ~cDebugger();

			bool Init(cInterpreter *interpreter);
			bool LoadIntermediateCode(const StrPath &fileName);
			bool Process(const float deltaSeconds);
			bool Run(const int vmId);
			bool StepRun(const int vmId);
			bool Stop(const int vmId);
			bool Resume(const int vmId);
			bool OneStep(const int vmId);
			bool Break(const int vmId);
			bool BreakPoint(const int vmId, const bool enable, const uint id);
			bool IsLoad(const int vmId);
			bool IsRun(const int vmId);
			bool IsDebug(const int vmId);
			bool IsBreak(const int vmId);
			void Clear();


		protected:
			int ProcessUntilNodeEnter(const float deltaSeconds);
			bool CheckBreakPoint();


		public:
			enum class eState {
				Stop  // no load code
				, Wait // load code, wait operation
				, Step // one step execute instruction, and move DebugWait state
				, Break // meet breakpoint, next frame change Wait state
				, Run // run until break
			};

			eState m_state;
			cInterpreter *m_interpreter; // reference
			float m_dt; // delta sconds
			set<uint> m_breakPoints;
		};

	}
}
