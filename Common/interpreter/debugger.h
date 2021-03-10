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
			bool Run();
			bool StepRun();
			bool Stop();
			bool Resume();
			bool OneStep();
			bool Break();
			bool BreakPoint(const bool enable, const uint id);
			bool IsLoad();
			bool IsRun();
			bool IsDebug();
			bool IsBreak();
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
