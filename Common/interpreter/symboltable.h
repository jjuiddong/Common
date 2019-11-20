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

			template <class T>
			bool Set(const string &scopeName, const string &symbolName, const T &var);
			bool Set(const string &scopeName, const string &symbolName, const variant_t &var);

			template <class T>
			T Get(const string &scopeName, const string &symbolName);
			bool Get(const string &scopeName, const string &symbolName, OUT variant_t &out);

			bool IsExist(const string &scopeName, const string &symbolName);
			void Clear();

			static string MakeScopeName(const string &name, const int id);
			static std::pair<string,int> ParseScopeName(const string &scopeName);

			cSymbolTable& operator=(const cSymbolTable &rhs);


		public:
			map<string, map<string, variant_t>> m_symbols; // key: scopeName
														   // value: symbolName, value
		};


		//---------------------------------------------------------------------------------------
		// template function

		template <class T>
		inline bool cSymbolTable::Set(const string &scopeName, const string &symbolName, const T &var)
		{
			variant_t v((T)var);
			const bool r = Set(scopeName, symbolName, v);
			common::clearvariant(v);
			return r;
		}

		// template specialization (string)
		template <>
		inline bool cSymbolTable::Set<string>(const string &scopeName, const string &symbolName
			, const string &var)
		{
			variant_t v;
			v.vt = VT_BSTR;
			v.bstrVal = ::SysAllocString(common::str2wstr(var).c_str());

			// to avoid bstr memory move bug
			common::clearvariant(m_symbols[scopeName][symbolName]);
			m_symbols[scopeName][symbolName] = v;
			return true;
		}

		template <class T>
		inline T cSymbolTable::Get(const string &scopeName, const string &symbolName) 
		{
			variant_t var;
			if (!Get(scopeName, symbolName, var))
				return variant_t((T)0);
			return (T)var;
		}

		// template specialization (string)
		template <>
		inline string cSymbolTable::Get<string>(const string &scopeName, const string &symbolName) 
		{
			variant_t var;
			if (!Get(scopeName, symbolName, var))
				return "";
			return WStr128(var.bstrVal).str().c_str();
		}

	}
}
