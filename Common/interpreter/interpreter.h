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

			bool Read(const StrPath &fileName);


		public:
			vector<cVirtualMachine*> m_vms;
		};

	}
}
