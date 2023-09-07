//
// 2023-09-06, jjuiddong
// function argument event
//	- execute new vm argument
//  - terminiate vm argument
//
#pragma once


namespace common
{
	namespace script
	{

		// argument event
		class cEventArg : public cEvent
		{
		public:
			cEventArg() : cEvent() {}
			cEventArg(const StrId& name, const cSymbolTable& symbTable = {})
				: cEvent(name)
				, m_symbTable(symbTable) { }

			virtual bool ProcessVM(cVirtualMachine& vm) override;


		public:
			cSymbolTable m_symbTable;
		};

	}
}

