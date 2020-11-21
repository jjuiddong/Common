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
			bool OneStep();
			bool Run();
			bool Break();
			bool Terminate();
			bool IsLoad();
			bool IsRun();
			void Clear();


		public:
			enum class eState {Stop, DebugWait, DebugStep, DebugRun };

			eState m_state;
			cInterpreter *m_interpreter; // reference
		};

	}
}
