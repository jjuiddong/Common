//
// 2019-10-29, jjuiddong
// script interpreter main header
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

			bool Init(const StrPath &fileName, iFunctionCallback *callback
				, void *arg = nullptr);
			bool Process(const float deltaSeconds);
			bool ProcessOneStep(const float deltaSeconds);
			bool Run();
			bool Stop();
			bool DebugRun();
			bool DebugBreak();
			bool DebugCancel();
			bool DebugToRun();
			bool PushEvent(const cEvent &evt);
			bool IsRun() const;
			bool IsStop() const;
			bool IsDebug() const;
			void Clear();


		public:
			enum class eState {Stop, Run, Debug};

			eState m_state;
			StrPath m_fileName;
			cIntermediateCode m_code;
			queue<cEvent> m_events;
			vector<cVirtualMachine*> m_vms;
			iFunctionCallback *m_callback;
			void *m_callbackArgPtr;
		};

	}
}
