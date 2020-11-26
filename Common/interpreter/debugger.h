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
			bool Stop();
			bool Resume();
			bool OneStep();
			bool Break();
			bool IsLoad();
			bool IsRun();
			void Clear();


		public:
			enum class eState {
				Stop  // no load code
				, Wait // load code, wait operation
				, Step // one step execute instruction, and move DebugWait state
				, Run // run until break
			};

			eState m_state;
			cInterpreter *m_interpreter; // reference
		};

	}
}
