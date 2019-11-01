//
// 2019-10-29, jjuiddong
// excute script virtual machine
//
#pragma once


namespace common
{
	namespace script
	{

		class cVirtualMachine
		{
		public:
			struct sRegister
			{
				uint idx; // excution command line index
				bool cmp; // compare output flag
				variant_t val[10]; // register
			};

			cVirtualMachine();
			virtual ~cVirtualMachine();

			bool Init(const cIntermediateCode &cmd, iFunctionCallback *callback
				, void *arg = nullptr);
			bool Update(const float deltaSeconds);
			bool Run();
			bool Stop();
			bool PushEvent(const cEvent &evt);
			void Clear();


		protected:
			bool Execute(sRegister &reg);


		public:
			enum class eState { Stop, Run, Wait };
			eState m_state;
			sRegister m_reg;
			cSymbolTable m_symbTable;
			cIntermediateCode m_code;
			iFunctionCallback *m_callback;
			void *m_callbackArgPtr;
		};

	}
}
