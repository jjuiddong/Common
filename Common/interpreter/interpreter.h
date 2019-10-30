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

			bool Init(const StrPath &fileName);
			bool Update(const float deltaSeconds);
			bool Run();
			bool Stop();
			bool PushEvent(const cEvent &evt);
			void Clear();


		public:
			enum class eState {Stop, Run};

			eState m_state;
			StrPath m_fileName;
			cIntermediateCode m_code;
			queue<cEvent> m_events;
			vector<cVirtualMachine*> m_vms;
		};

	}
}
