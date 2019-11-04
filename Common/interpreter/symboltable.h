//
// 2019-10-29, jjuiddong
// interpreter symboltable
//
#pragma once


namespace common
{
	namespace script
	{

		class cSymbolTable
		{
		public:
			cSymbolTable();
			cSymbolTable(const cSymbolTable &rhs);
			virtual ~cSymbolTable();

			bool Set(const string &scopeName, const string &symbolName
				, const variant_t &var);
			bool Get(const string &scopeName, const string &symbolName
				, OUT variant_t &out );
			bool IsExist(const string &scopeName, const string &symbolName);
			void Clear();

			static string MakeScopeName(const string &name, const int id);
			static std::pair<string,int> ParseScopeName(const string &scopeName);

			cSymbolTable& operator=(const cSymbolTable &rhs);


		public:
			map<string, map<string, variant_t>> m_symbols; // key: scopeName
														   // value: symbolName, value
		};

	}
}
