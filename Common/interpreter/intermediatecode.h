//
// 2019-10-30, jjuiddong
// script intermediate code
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
			virtual ~cIntermediateCode();

			bool Read(const StrPath &fileName);
			bool Write(const StrPath &fileName);
			bool IsLoaded();
			void Clear();


		protected:
			uint GetRegisterIndex(const string &regName);
			const char* GetRegisterName(const uint regIdx);


		public:
			vector<sCommandSet> m_cmds;
			map<StrId, uint> m_jmpMap; //key: jump name, data: code line
		};

	}
}
