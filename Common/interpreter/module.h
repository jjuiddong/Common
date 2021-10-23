//
// 2021-10-20, jjuiddong
// interpreter function execute module
//
#pragma once


namespace common 
{
	namespace script
	{
		class cVirtualMachine;

		// module execute result
		enum class eModuleResult {
			None, // initial state, not defined
			NoHandler, // no handler 
			Done, // work done
			Wait, // working but not finished
		};

		interface iModule
		{
			virtual eModuleResult Execute(cVirtualMachine &vm
				, const string &scopeName
				, const string &funcName) = 0;

			virtual bool CloseModule(cVirtualMachine &vm) = 0;
		};
	}
}
