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
			virtual ~cSymbolTable();

			bool Set(const StrId &scopeName, const StrId &symbolName
				, const variant_t &var);
			bool Get(const StrId &scopeName, const StrId &symbolName
				, OUT variant_t &out );
			bool IsExist(const StrId &scopeName, const StrId &symbolName);
			void Clear();


		public:
			map<StrId, map<StrId, variant_t>> m_symbols; // key: scopeName
														 // value: symbolName, value
			map<StrId, string> m_strTable; // key: scopeName::symbolName
										   // value: string 
										   // variant_t bstr type pointer management
		};

	}
}
