//
// 2019-10-29, jjuiddong
// virtual machine
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
				int idx;
				bool cmp;
				variant_t val[10];
			};

			cVirtualMachine();
			virtual ~cVirtualMachine();

			bool Init(const cIntermediateCode &code);
			bool Update(const float deltaSeconds);
			bool Run();
			bool Stop();
			void Clear();


		public:
			enum class eState { Stop, Run, Wait };

			eState m_state;
			sRegister m_reg;
			cSymbolTable m_symbTable;
			cIntermediateCode m_code;
		};

	}
}
