//
// 2019-10-30, jjuiddong
// script intermediate code
//
//
// ---------------------------------------------------------
// Sample
//
// # script intermediate code instruction sample
//
// main:
// nop
//
// Event_SeqWork:
// geti Event_SeqWork, Loading, val0
// seti GetFrontNode, node, val0
// call GetFrontNode
// geti GetFrontNode, front, val0
// seti ReqMove, Dest, val0
// call ReqMove
// geti ReqMove, Result, val0
// eqic val0, 0
// jnz jump1
// jmp jump2
// nop
//
// jump1:
// geti Event_Seqwork, Loading, val0
// seti ReqLoading, station, val0
// geti Event_Seqwork, lLayer, val1
// seti ReqLoading, lLayer, val1
// call ReqLoading
// nop
//
// jump2:
// geti ReqMove, Result, val0
// seti ErrorState, errorCode, val0
// call ErrorState
// nop
// ---------------------------------------------------------
//
#pragma once


namespace common
{
	namespace script
	{

		class cIntermediateCode
		{
		public:
			cIntermediateCode();
			cIntermediateCode(const cIntermediateCode &rhs);
			virtual ~cIntermediateCode();

			bool Read(const StrPath &fileName);
			bool Write(const StrPath &fileName);
			uint FindJumpAddress(const StrId &label);
			bool IsLoaded();
			void Clear();

			cIntermediateCode& operator=(const cIntermediateCode &rhs);


		protected:
			uint GetRegisterIndex(const string &regName);
			const char* GetRegisterName(const uint regIdx);


		public:
			StrPath m_fileName;
			vector<sInstruction> m_codes;
			cSymbolTable m_variables; // initial variable symboltable
			map<StrId, uint> m_jmpMap; //key: jump name, data: code line
			vector<std::pair<StrId, int>> m_timer1Events; // timer name, interval
			char m_regName[5]; // tricky code to optimize memory
		};

	}
}
