//
// 2019-10-29, jjuiddong
// virtual machine
//
#pragma once

#include "symboltable.h"


namespace common
{
	namespace script
	{

		class cVirtualMachine
		{
		public:
			cVirtualMachine();
			virtual ~cVirtualMachine();


		public:
			cSymbolTable m_symbTable;
		};

	}
}
